/**
 *  @file
 *  @brief Implementation of FixpositionDriver class
 *
 * \verbatim
 *  ___    ___
 *  \  \  /  /
 *   \  \/  /   Fixposition AG
 *   /  /\  \   All right reserved.
 *  /__/  \__\
 * \endverbatim
 *
 */

/* SYSTEM / STL */
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>

/* PACKAGE */
#include <fixposition_driver_lib/converter/imu.hpp>
#include <fixposition_driver_lib/converter/llh.hpp>
#include <fixposition_driver_lib/converter/odometry.hpp>
#include <fixposition_driver_lib/converter/tf.hpp>
#include <fixposition_driver_lib/fixposition_driver.hpp>
#include <fixposition_driver_lib/helper.hpp>
#include <fixposition_driver_lib/parser.hpp>

#ifndef B460800
#define B460800 460800
#endif
#ifndef B500000
#define B500000 500000
#endif
#ifndef B921600
#define B921600 921600
#endif
#ifndef B1000000
#define B1000000 1000000
#endif

namespace fixposition {
FixpositionDriver::FixpositionDriver(const FixpositionDriverParams& params) : params_(params) {
    Connect();

    // static headers
    rawdmi_.head1 = 0xaa;
    rawdmi_.head2 = 0x44;
    rawdmi_.head3 = 0x13;
    rawdmi_.payloadLen = 20;
    rawdmi_.msgId = 2269;
    // these to be filled by each rosmsg
    rawdmi_.wno = 0;
    rawdmi_.tow = 0;
    rawdmi_.dmi1 = 0;
    rawdmi_.dmi2 = 0;
    rawdmi_.dmi3 = 0;
    rawdmi_.dmi4 = 0;
    rawdmi_.mask = 0;

    // initialize converters
    if (!InitializeConverters()) {
        std::cerr << "Could not initialize output converter!\n";
    }
}

FixpositionDriver::~FixpositionDriver() {
    if (client_fd_ != -1) {
        if (params_.fp_output.type == INPUT_TYPE::SERIAL) {
            tcsetattr(client_fd_, TCSANOW, &options_save_);
        }
        close(client_fd_);
    }
}

bool FixpositionDriver::Connect() {
    switch (params_.fp_output.type) {
        case INPUT_TYPE::TCP:
            return CreateTCPSocket();
            break;
        case INPUT_TYPE::SERIAL:
            return CreateSerialConnection();
            break;
        default:
            std::cerr << "Unknown connection type!\n";
            return false;
    }
}

void FixpositionDriver::WsCallback(const std::vector<int>& speeds) {
    if (speeds.size() == 1) {
        rawdmi_.dmi1 = speeds[0];
        rawdmi_.mask = (1 << 0) | (0 << 1) | (0 << 2) | (0 << 3);
    } else if (speeds.size() == 2) {
        rawdmi_.dmi1 = speeds[0];
        rawdmi_.dmi2 = speeds[1];
        rawdmi_.mask = (1 << 0) | (1 << 1) | (0 << 2) | (0 << 3) | (1 << 11);
    } else if (speeds.size() == 4) {
        rawdmi_.dmi1 = speeds[0];
        rawdmi_.dmi2 = speeds[1];
        rawdmi_.dmi3 = speeds[2];
        rawdmi_.dmi4 = speeds[3];
        rawdmi_.mask = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);
    } else {
        return;
    }

    // Calculate CRC
    const uint32_t checksum = nov_crc32((const uint8_t*)&rawdmi_, sizeof(rawdmi_));

    // Compose entire message
    uint8_t message[sizeof(rawdmi_) + sizeof(checksum)];
    memcpy(&message[0], &rawdmi_, sizeof(rawdmi_));
    memcpy(&message[sizeof(rawdmi_)], &checksum, sizeof(checksum));

    switch (params_.fp_output.type) {
        case INPUT_TYPE::TCP:
            send(this->client_fd_, &message[0], sizeof(message), MSG_DONTWAIT);
            break;
        case INPUT_TYPE::SERIAL:
            write(this->client_fd_, &message[0], sizeof(message));
            break;
        default:
            std::cerr << "Unknown connection type!\n";
            break;
    }
}

bool FixpositionDriver::InitializeConverters() {
    for (const auto& format : params_.fp_output.formats) {
        if (format == "ODOMETRY") {
            a_converters_["ODOMETRY"] = std::unique_ptr<OdometryConverter>(new OdometryConverter());
            a_converters_["TF"] = std::unique_ptr<TfConverter>(new TfConverter());
        } else if (format == "LLH") {
            a_converters_["LLH"] = std::unique_ptr<LlhConverter>(new LlhConverter());
        } else if (format == "RAWIMU") {
            a_converters_["RAWIMU"] = std::unique_ptr<ImuConverter>(new ImuConverter(false));
        } else if (format == "CORRIMU") {
            a_converters_["CORRIMU"] = std::unique_ptr<ImuConverter>(new ImuConverter(true));
        } else if (format == "TF") {
            if (a_converters_.find("TF") == a_converters_.end()) {
                a_converters_["TF"] = std::unique_ptr<TfConverter>(new TfConverter());
            }
        } else {
            std::cerr << "Unknown input format: " << format << "\n";
        }
    }
    return !a_converters_.empty();
}
bool FixpositionDriver::RunOnce() {
    if ((client_fd_ > 0) && (connection_status_ == 0) && ReadAndPublish()) {
        return true;
    } else {
        close(client_fd_);
        client_fd_ = -1;
        return false;
    }
}

bool FixpositionDriver::ReadAndPublish() {
    char readBuf[8192];

    ssize_t rv;
    if (params_.fp_output.type == INPUT_TYPE::TCP) {
        rv = recv(client_fd_, (void*)&readBuf, sizeof(readBuf), MSG_DONTWAIT);
    } else if (params_.fp_output.type == INPUT_TYPE::SERIAL) {
        rv = read(client_fd_, (void*)&readBuf, sizeof(readBuf));
    } else {
        rv = 0;
    }

    if (rv == 0) {
        std::cerr << "Connection closed.\n";
        return false;
    }

    if (rv < 0 && errno == EAGAIN) {
        /* no data for now, call back when the socket is readable */
        return true;
    }
    if (rv < 0) {
        std::cerr << "Connection error.\n";
        return false;
    }

    ssize_t start_id = 0;
    while (start_id < rv) {
        int msg_size = 0;
        // Nov B
        msg_size = IsNovMessage((uint8_t*)&readBuf[start_id], rv - start_id);
        if (msg_size > 0) {
            NovConvertAndPublish((uint8_t*)&readBuf[start_id], msg_size);
            start_id += msg_size;
            continue;
        }
        if (msg_size == 0) {
            // do nothing
        }
        if (msg_size < 0) {
            break;
        }

        // Nmea (incl. FP_A)
        msg_size = IsNmeaMessage(&readBuf[start_id], rv - start_id);
        if (msg_size > 0) {
            // NovConvertAndPublish(start, msg_size);
            std::string msg(&readBuf[start_id], msg_size);
            NmeaConvertAndPublish(msg);
            start_id += msg_size;
            continue;
        }
        if (msg_size == 0) {
            // do nothing
        }
        if (msg_size < 0) {
            break;
        }

        // No Match, increment by 1
        ++start_id;
    }

    return true;
}

void FixpositionDriver::NmeaConvertAndPublish(const std::string& msg) {
    // split the msg into tokens, removing the *XX checksum
    std::vector<std::string> tokens;
    std::size_t star_pos = msg.find_last_of("*");
    SplitMessage(tokens, msg.substr(1, star_pos - 1), ",");

    // if it doesn't start with FP then do nothing
    if (tokens.at(0) != "FP") {
        return;
    }

    // Get the header of the sentence
    const std::string header = tokens.at(1);

    // If we have a converter available, convert to ros. Currently supported are "FP", "LLH", "TF", "RAWIMU", "CORRIMU"
    if (a_converters_[header] != nullptr) {
        a_converters_[header]->ConvertTokens(tokens);
    }
}

void FixpositionDriver::NovConvertAndPublish(const uint8_t* msg, int size) {
    auto* header = reinterpret_cast<const Oem7MessageHeaderMem*>(msg);
    const auto msg_id = header->message_id;

    if (msg_id == static_cast<uint16_t>(MessageId::BESTGNSSPOS)) {
        for (auto& ob : bestgnsspos_obs_) {
            auto* payload = reinterpret_cast<const BESTGNSSPOSMem*>(msg + sizeof(Oem7MessageHeaderMem));
            ob(header, payload);
        }
    }
    // TODO add more msg types
}

bool FixpositionDriver::CreateTCPSocket() {
    struct sockaddr_in server_address;
    client_fd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (client_fd_ < 0) {
        std::cerr << "Error in client creation.\n";
        return false;
    } else {
        std::cout << "Client created.\n";
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(std::stoi(params_.fp_output.port));
    server_address.sin_addr.s_addr = inet_addr(params_.fp_output.ip.c_str());

    connection_status_ = connect(client_fd_, (struct sockaddr*)&server_address, sizeof server_address);

    if (connection_status_ != 0) {
        std::cerr << "Error on connection of TCP socket: " << strerror(errno) << "\n";
        return false;
    }
    return true;
}

bool FixpositionDriver::CreateSerialConnection() {
    client_fd_ = open(params_.fp_output.port.c_str(), O_RDWR | O_NOCTTY);

    struct termios options;
    speed_t speed;

    switch (params_.fp_output.baudrate) {
        case 9600:
            speed = B9600;
            break;

        case 38400:
            speed = B38400;
            break;

        case 57600:
            speed = B57600;
            break;

        case 115200:
            speed = B115200;
            break;

        case 230400:
            speed = B230400;
            break;

        case 460800:
            speed = B460800;
            break;

        case 500000:
            speed = B500000;
            break;

        case 921600:
            speed = B921600;
            break;

        case 1000000:
            speed = B1000000;
            break;

        default:
            speed = B115200;
            std::cerr << "Unsupported baudrate: " << params_.fp_output.baudrate
                      << "\n\tsupported examples:\n\t9600, "
                         "19200, "
                         "38400, "
                         "57600\t\n115200\n230400\n460800\n500000\n921600\n1000000\n";
    }

    if (client_fd_ == -1) {
        // Could not open the port.
        std::cerr << "Failed to open serial port " << strerror(errno) << "\n";
        return false;
    } else {
        // Get current serial port options:
        tcgetattr(client_fd_, &options);
        options_save_ = options;
        char speed_buf[10];
        snprintf(speed_buf, sizeof(speed_buf), "0%06o", (int)cfgetispeed(&options));

        options.c_iflag &= ~(IXOFF | IXON | ICRNL);
        options.c_oflag &= ~(OPOST | ONLCR);
        options.c_lflag &= ~(ISIG | ICANON | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE | IEXTEN);
        options.c_cc[VEOL] = 0;
        options.c_cc[VMIN] = 0;
        options.c_cc[VTIME] = 50;

        cfsetospeed(&options, speed); /* baud rate */
        tcsetattr(client_fd_, TCSANOW, &options);
        connection_status_ = 0;  // not used for serial, set to 0 (success)
        return true;
    }
}
}  // namespace fixposition
