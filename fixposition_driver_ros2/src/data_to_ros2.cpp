/**
 *  @file
 *  @brief Convert Data classes to ROS2 msgs
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

/* PACKAGE */
#include <fixposition_driver_ros2/data_to_ros2.hpp>

namespace fixposition {

void FpToRosMsg(const fixposition::OdometryData& data, rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        nav_msgs::msg::Odometry msg;
        
        // Populate message
        if (data.stamp.tow == 0.0 && data.stamp.wno == 0) {
            msg.header.stamp = rclcpp::Clock().now();
        } else {
            msg.header.stamp = GpsTimeToMsgTime(data.stamp);
        }
        msg.header.frame_id = data.frame_id;
        msg.child_frame_id = data.child_frame_id;

        PoseWithCovDataToMsg(data.pose, msg.pose);
        TwistWithCovDataToMsg(data.twist, msg.twist);

        // Publish message
        pub->publish(msg);
    }
}

void FpToRosMsg(const ImuData& data, rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        sensor_msgs::msg::Imu msg;
        
        // Populate message
        if (data.stamp.tow == 0.0 && data.stamp.wno == 0) {
            msg.header.stamp = rclcpp::Clock().now();
        } else {
            msg.header.stamp = GpsTimeToMsgTime(data.stamp);
        }
        msg.header.frame_id = data.frame_id;

        tf2::toMsg(data.linear_acceleration, msg.linear_acceleration);
        tf2::toMsg(data.angular_velocity, msg.angular_velocity);

        // Publish message
        pub->publish(msg);
    }
}


void FpToRosMsg(const FP_IMUBIAS& data, rclcpp::Publisher<fixposition_driver_ros2::msg::IMUBIAS>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::IMUBIAS msg;

        // Populate message
        if (data.stamp.tow == 0.0 && data.stamp.wno == 0) {
            msg.header.stamp = rclcpp::Clock().now();
        } else {
            msg.header.stamp = GpsTimeToMsgTime(data.stamp);
        }
        
        msg.header.frame_id = data.frame_id;
        msg.fusion_imu = data.fusion_imu;
        msg.imu_status = data.imu_status;
        msg.imu_noise = data.imu_noise;
        msg.imu_conv = data.imu_conv;
        tf2::toMsg(data.bias_acc, msg.bias_acc);
        tf2::toMsg(data.bias_gyr, msg.bias_gyr);
        tf2::toMsg(data.bias_cov_acc, msg.bias_cov_acc);
        tf2::toMsg(data.bias_cov_gyr, msg.bias_cov_gyr);

        // Publish message
        pub->publish(msg);
    }
}

void FpToRosMsg(const FP_GNSSANT& data, rclcpp::Publisher<fixposition_driver_ros2::msg::GNSSANT>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::GNSSANT msg;
        
        // Populate message
        if (data.stamp.tow == 0.0 && data.stamp.wno == 0) {
            msg.header.stamp = rclcpp::Clock().now();
        } else {
            msg.header.stamp = GpsTimeToMsgTime(data.stamp);
        }

        msg.gnss1_state = data.gnss1_state;
        msg.gnss1_power = data.gnss1_power;
        msg.gnss1_age = data.gnss1_age;
        msg.gnss2_state = data.gnss2_state;
        msg.gnss2_power = data.gnss2_power;
        msg.gnss2_age = data.gnss2_age;
        
        // Publish message
        pub->publish(msg);
    }
}

void FpToRosMsg(const FP_GNSSCORR& data, rclcpp::Publisher<fixposition_driver_ros2::msg::GNSSCORR>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::GNSSCORR msg;
        
        // Populate message
        if (data.stamp.tow == 0.0 && data.stamp.wno == 0) {
            msg.header.stamp = rclcpp::Clock().now();
        } else {
            msg.header.stamp = GpsTimeToMsgTime(data.stamp);
        }

        msg.gnss1_fix = data.gnss1_fix;
        msg.gnss1_nsig_l1 = data.gnss1_nsig_l1;
        msg.gnss1_nsig_l2 = data.gnss1_nsig_l2;
        msg.gnss2_fix = data.gnss2_fix;
        msg.gnss2_nsig_l1 = data.gnss2_nsig_l1;
        msg.gnss2_nsig_l2 = data.gnss2_nsig_l2;

        msg.corr_latency = data.corr_latency;
        msg.corr_update_rate = data.corr_update_rate;
        msg.corr_data_rate = data.corr_data_rate;
        msg.corr_msg_rate = data.corr_msg_rate;

        msg.sta_id = data.sta_id;
        tf2::toMsg(data.sta_llh, msg.sta_llh);
        msg.sta_dist = data.sta_dist;

        // Publish message
        pub->publish(msg);
    }
}

void FpToRosMsg(const FP_LLH& data, rclcpp::Publisher<fixposition_driver_ros2::msg::LLH>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::LLH msg;

        // Populate message
        if (data.stamp.tow == 0.0 && data.stamp.wno == 0) {
            msg.header.stamp = rclcpp::Clock().now();
        } else {
            msg.header.stamp = GpsTimeToMsgTime(data.stamp);
        }

        tf2::toMsg(data.llh, msg.position);
        Eigen::Map<Eigen::Matrix3d> cov_map = Eigen::Map<Eigen::Matrix3d>(msg.covariance.data());
        cov_map = data.cov;

        // Publish message
        pub->publish(msg);
    }
}

void FpToRosMsg(const FP_ODOMENU& data, rclcpp::Publisher<fixposition_driver_ros2::msg::ODOMENU>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::ODOMENU msg;

        // Populate message
        if (data.odom.stamp.tow == 0.0 && data.odom.stamp.wno == 0) {
            msg.header.stamp = rclcpp::Clock().now();
        } else {
            msg.header.stamp = GpsTimeToMsgTime(data.odom.stamp);
        }
        
        msg.header.frame_id = data.odom.frame_id;
        msg.pose_frame = data.odom.child_frame_id;
        msg.kin_frame = data.odom.child_frame_id;

        PoseWithCovDataToMsg(data.odom.pose, msg.pose);
        TwistWithCovDataToMsg(data.odom.twist, msg.velocity);
        tf2::toMsg(data.acceleration, msg.acceleration);
        msg.fusion_status = data.fusion_status;
        msg.imu_bias_status = data.imu_bias_status;
        msg.gnss1_status = data.gnss1_status;
        msg.gnss2_status = data.gnss2_status;
        msg.wheelspeed_status = data.wheelspeed_status;

        // Publish message
        pub->publish(msg);
    }
}

void FpToRosMsg(const FP_ODOMETRY& data, rclcpp::Publisher<fixposition_driver_ros2::msg::ODOMETRY>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::ODOMETRY msg;

        // Populate message
        if (data.odom.stamp.tow == 0.0 && data.odom.stamp.wno == 0) {
            msg.header.stamp = rclcpp::Clock().now();
        } else {
            msg.header.stamp = GpsTimeToMsgTime(data.odom.stamp);
        }
        
        msg.header.frame_id = data.odom.frame_id;
        msg.pose_frame = data.odom.child_frame_id;
        msg.kin_frame = data.odom.child_frame_id;

        PoseWithCovDataToMsg(data.odom.pose, msg.pose);
        TwistWithCovDataToMsg(data.odom.twist, msg.velocity);
        tf2::toMsg(data.acceleration, msg.acceleration);
        msg.fusion_status = data.fusion_status;
        msg.imu_bias_status = data.imu_bias_status;
        msg.gnss1_status = data.gnss1_status;
        msg.gnss2_status = data.gnss2_status;
        msg.wheelspeed_status = data.wheelspeed_status;
        msg.version = data.version;
        
        // Publish message
        pub->publish(msg);
    }
}

void FpToRosMsg(const FP_ODOMSH& data, rclcpp::Publisher<fixposition_driver_ros2::msg::ODOMSH>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::ODOMSH msg;

        // Populate message
        if (data.odom.stamp.tow == 0.0 && data.odom.stamp.wno == 0) {
            msg.header.stamp = rclcpp::Clock().now();
        } else {
            msg.header.stamp = GpsTimeToMsgTime(data.odom.stamp);
        }
        
        msg.header.frame_id = data.odom.frame_id;
        msg.pose_frame = data.odom.child_frame_id;
        msg.kin_frame = data.odom.child_frame_id;

        PoseWithCovDataToMsg(data.odom.pose, msg.pose);
        TwistWithCovDataToMsg(data.odom.twist, msg.velocity);
        tf2::toMsg(data.acceleration, msg.acceleration);
        msg.fusion_status = data.fusion_status;
        msg.imu_bias_status = data.imu_bias_status;
        msg.gnss1_status = data.gnss1_status;
        msg.gnss2_status = data.gnss2_status;
        msg.wheelspeed_status = data.wheelspeed_status;

        // Publish message
        pub->publish(msg);
    }
}

void FpToRosMsg(const FP_ODOMSTATUS& data, rclcpp::Publisher<fixposition_driver_ros2::msg::ODOMSTATUS>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::ODOMSTATUS msg;

        // Populate message
        if (data.stamp.tow == 0.0 && data.stamp.wno == 0) {
            msg.header.stamp = rclcpp::Clock().now();
        } else {
            msg.header.stamp = GpsTimeToMsgTime(data.stamp);
        }

        msg.init_status = data.init_status;
        msg.fusion_imu = data.fusion_imu;
        msg.fusion_gnss1 = data.fusion_gnss1;
        msg.fusion_gnss2 = data.fusion_gnss2;
        msg.fusion_corr = data.fusion_corr;
        msg.fusion_cam1 = data.fusion_cam1;
        msg.fusion_ws = data.fusion_ws;
        msg.fusion_markers = data.fusion_markers;
        msg.imu_status = data.imu_status;
        msg.imu_noise = data.imu_noise;
        msg.imu_conv = data.imu_conv;
        msg.gnss1_status = data.gnss1_status;
        msg.gnss2_status = data.gnss2_status;
        msg.baseline_status = data.baseline_status;
        msg.corr_status = data.corr_status;
        msg.cam1_status = data.cam1_status;
        msg.ws_status = data.ws_status;
        msg.ws_conv = data.ws_conv;
        msg.markers_status = data.markers_status;
        msg.markers_conv = data.markers_conv;

        // Publish message
        pub->publish(msg);
    }
}

void FpToRosMsg(const FP_TEXT& data, rclcpp::Publisher<fixposition_driver_ros2::msg::TEXT>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::TEXT msg;

        // Populate message
        msg.level = data.level;
        msg.text = data.text;

        // Publish message
        pub->publish(msg);
    }
}

void FpToRosMsg(const FP_TP& data, rclcpp::Publisher<fixposition_driver_ros2::msg::TP>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::TP msg;

        // Populate message
        msg.tp_name = data.tp_name;
        msg.timebase = data.timebase;
        msg.timeref = data.timeref;
        msg.tp_tow_sec = data.tp_tow_sec;
        msg.tp_tow_psec = data.tp_tow_psec;
        msg.gps_leaps = data.gps_leaps;

        // Publish message
        pub->publish(msg);
    }
}

void FpToRosMsg(const FP_EOE& data, rclcpp::Publisher<fixposition_driver_ros2::msg::EOE>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::EOE msg;

        // Populate message
        if (data.stamp.tow == 0.0 && data.stamp.wno == 0) {
            msg.header.stamp = rclcpp::Clock().now();
        } else {
            msg.header.stamp = GpsTimeToMsgTime(data.stamp);
        }
        msg.epoch = data.epoch;

        // Publish message
        pub->publish(msg);
    }
}

void FpToRosMsg(const GP_GGA& data, rclcpp::Publisher<fixposition_driver_ros2::msg::GPGGA>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::GPGGA msg;

        // Populate message
        msg.time = data.time_str;
        msg.latitude = data.llh(0);
        msg.lat_ns = data.lat_ns;
        msg.longitude = data.llh(1);
        msg.lon_ew = data.lon_ew;
        msg.quality = data.quality;
        msg.num_sv = data.num_sv;
        msg.hdop = data.hdop;
        msg.alt = data.llh(2);
        msg.alt_unit = data.alt_unit;
        msg.diff_age = data.diff_age;
        msg.diff_sta = data.diff_sta;
        msg.sentence = data.sentence;

        // Publish message
        pub->publish(msg);
    }
}

void FpToRosMsg(const GP_GLL& data, rclcpp::Publisher<fixposition_driver_ros2::msg::GPGLL>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::GPGLL msg;

        // Populate message
        msg.latitude = data.latlon(0);
        msg.lat_ns = data.lat_ns;
        msg.longitude = data.latlon(1);
        msg.lon_ew = data.lon_ew;
        msg.time = data.time_str;
        msg.status = data.status;
        msg.mode = data.mode;

        // Publish message
        pub->publish(msg);
    }
}

void FpToRosMsg(const GN_GSA& data, rclcpp::Publisher<fixposition_driver_ros2::msg::GNGSA>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::GNGSA msg;

        // Populate message
        msg.mode_op = data.mode_op;
        msg.mode_nav = data.mode_nav;
        
        for (unsigned int i = 0; i < data.ids.size(); i++) {
           msg.ids.push_back(data.ids.at(i));
        }
        
        msg.pdop = data.pdop;
        msg.hdop = data.hdop;
        msg.vdop = data.vdop;
        msg.gnss_id = data.gnss_id;

        // Publish message
        pub->publish(msg);
    }
}

void FpToRosMsg(const GP_GST& data, rclcpp::Publisher<fixposition_driver_ros2::msg::GPGST>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::GPGST msg;

        // Populate message
        msg.time = data.time_str;
        msg.rms_range = data.rms_range;
        msg.std_major = data.std_major;
        msg.std_minor = data.std_minor;
        msg.angle_major = data.angle_major;
        msg.std_lat = data.std_lat;
        msg.std_lon = data.std_lon;
        msg.std_alt = data.std_alt;
        
        // Publish message
        pub->publish(msg);
    }
}

void FpToRosMsg(const GX_GSV& data, rclcpp::Publisher<fixposition_driver_ros2::msg::GXGSV>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::GXGSV msg;

        // Populate message
        msg.sentences = data.sentences;
        msg.sent_num = data.sent_num;
        msg.num_sats = data.num_sats;
        
        for (unsigned int i = 0; i < data.sat_id.size(); i++) {
            msg.sat_id.push_back(data.sat_id.at(i));
            msg.elev.push_back(data.elev.at(i));
            msg.azim.push_back(data.azim.at(i));
            msg.cno.push_back(data.cno.at(i));
        }

        msg.signal_id = data.signal_id;

        // Publish message
        pub->publish(msg);
    }
}


void FpToRosMsg(const GP_HDT& data, rclcpp::Publisher<fixposition_driver_ros2::msg::GPHDT>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::GPHDT msg;

        // Populate message
        msg.heading = data.heading;
        msg.true_ind = data.true_ind;

        // Publish message
        pub->publish(msg);
    }
}

void FpToRosMsg(const GP_RMC& data, rclcpp::Publisher<fixposition_driver_ros2::msg::GPRMC>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::GPRMC msg;

        // Populate message
        msg.time = data.time_str;
        msg.status = data.status;
        msg.latitude = data.latlon(0);
        msg.lat_ns = data.lat_ns;
        msg.longitude = data.latlon(1);
        msg.lon_ew = data.lon_ew;
        msg.speed = data.speed;
        msg.course = data.course;
        msg.date = data.date_str;
        msg.mode = data.mode;

        // Publish message
        pub->publish(msg);
    }
}

void FpToRosMsg(const GP_VTG& data, rclcpp::Publisher<fixposition_driver_ros2::msg::GPVTG>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::GPVTG msg;

        // Populate message
        msg.cog_true = data.cog_true;
        msg.cog_ref_t = data.cog_ref_t;
        msg.cog_mag = data.cog_mag;
        msg.cog_ref_m = data.cog_ref_m;
        msg.sog_knot = data.sog_knot;
        msg.sog_unit_n = data.sog_unit_n;
        msg.sog_kph = data.sog_kph;
        msg.sog_unit_k = data.sog_unit_k;
        msg.mode = data.mode;

        // Publish message
        pub->publish(msg);
    }
}

void FpToRosMsg(const GP_ZDA& data, rclcpp::Publisher<fixposition_driver_ros2::msg::GPZDA>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::GPZDA msg;

        // ROS Header
        if (data.stamp.tow == 0.0 && data.stamp.wno == 0) {
            msg.header.stamp = rclcpp::Clock().now();
        } else {
            msg.header.stamp = GpsTimeToMsgTime(data.stamp);
        }
        msg.header.frame_id = "FP_POI";

        // Populate message
        msg.time = data.time_str;
        msg.date = data.date_str;
        msg.local_hr = data.local_hr;
        msg.local_min = data.local_min;

        // Publish message
        pub->publish(msg);
    }
}

void TfDataToMsg(const TfData& data, geometry_msgs::msg::TransformStamped& msg) {
    msg.header.frame_id = data.frame_id;
    msg.child_frame_id = data.child_frame_id;

    if (data.stamp.tow == 0.0 && data.stamp.wno == 0) {
        msg.header.stamp = rclcpp::Clock().now();
    } else {
        msg.header.stamp = GpsTimeToMsgTime(data.stamp);
    }

    msg.transform.rotation = tf2::toMsg(data.rotation);
    tf2::toMsg(data.translation, msg.transform.translation);
}

void NavSatFixDataToMsg(const NavSatFixData& data, sensor_msgs::msg::NavSatFix& msg) {
    if (data.stamp.tow == 0.0 && data.stamp.wno == 0) {
        msg.header.stamp = rclcpp::Clock().now();
    } else {
        msg.header.stamp = GpsTimeToMsgTime(data.stamp);
    }
    msg.header.frame_id = data.frame_id;
    msg.status.status = data.status.status;
    msg.status.service = data.status.service;
    msg.latitude = data.latitude;
    msg.longitude = data.longitude;
    msg.altitude = data.altitude;

    Eigen::Map<Eigen::Matrix<double, 3, 3>> cov_map =
        Eigen::Map<Eigen::Matrix<double, 3, 3>>(msg.position_covariance.data());
    cov_map = data.cov;

    msg.position_covariance_type = data.position_covariance_type;
}

void PoseWithCovDataToMsg(const PoseWithCovData& data, geometry_msgs::msg::PoseWithCovariance& msg) {
    msg.pose.position = tf2::toMsg(data.position);
    msg.pose.orientation = tf2::toMsg(data.orientation);

    Eigen::Map<Eigen::Matrix<double, 6, 6>> cov_map = Eigen::Map<Eigen::Matrix<double, 6, 6>>(msg.covariance.data());
    cov_map = data.cov;
}

void TwistWithCovDataToMsg(const fixposition::TwistWithCovData& data, geometry_msgs::msg::TwistWithCovariance& msg) {
    tf2::toMsg(data.linear, msg.twist.linear);
    tf2::toMsg(data.angular, msg.twist.angular);

    Eigen::Map<Eigen::Matrix<double, 6, 6>> cov_map = Eigen::Map<Eigen::Matrix<double, 6, 6>>(msg.covariance.data());
    cov_map = data.cov;
}

void OdometryDataToTf(const FP_ODOMETRY& data, std::shared_ptr<tf2_ros::TransformBroadcaster> pub) {
    if (data.fusion_status > 0) {
        // Ensure Fusion orientation is a valid quaternion
        if (data.odom.pose.orientation.vec().isZero() && (data.odom.pose.orientation.w() == 0)) {
            return;
        }

        // Populate message
        geometry_msgs::msg::TransformStamped msg;
        OdomToTf(data.odom, msg);

        // Publish message
        pub->sendTransform(msg);
    }
}

void OdomToTf(const OdometryData& data, geometry_msgs::msg::TransformStamped& tf) {
    // Populate message
    tf.header.frame_id = data.frame_id;
    tf.child_frame_id = data.child_frame_id;

    if (data.stamp.tow == 0.0 && data.stamp.wno == 0) {
        tf.header.stamp = rclcpp::Clock().now();
    } else {
        tf.header.stamp = GpsTimeToMsgTime(data.stamp);
    }

    tf.transform.rotation = tf2::toMsg(data.pose.orientation);
    tf2::toMsg(data.pose.position, tf.transform.translation);
}

void PublishNav2Tf(const std::map<std::string, std::shared_ptr<geometry_msgs::msg::TransformStamped>>& tf_map, std::shared_ptr<tf2_ros::StaticTransformBroadcaster> static_br_, std::shared_ptr<tf2_ros::TransformBroadcaster> br_) {
    if (tf_map.at("ECEFENU0") && tf_map.at("POIPOISH") && tf_map.at("ECEFPOISH") && tf_map.at("ENU0POI")) {
        // Publish FP_ECEF -> map
        tf_map.at("ECEFENU0")->child_frame_id = "map";
        static_br_->sendTransform(*tf_map.at("ECEFENU0"));

        // Compute FP_ENU0 -> FP_POISH
        // Extract translation and rotation from ECEFENU0
        geometry_msgs::msg::Vector3 trans_ecef_enu0 = tf_map.at("ECEFENU0")->transform.translation;
        geometry_msgs::msg::Quaternion rot_ecef_enu0 = tf_map.at("ECEFENU0")->transform.rotation;
        Eigen::Vector3d t_ecef_enu0_;
        t_ecef_enu0_ << trans_ecef_enu0.x, trans_ecef_enu0.y, trans_ecef_enu0.z;
        Eigen::Quaterniond q_ecef_enu0_(rot_ecef_enu0.w, rot_ecef_enu0.x, rot_ecef_enu0.y, rot_ecef_enu0.z);

        // Extract translation and rotation from ECEFPOISH
        geometry_msgs::msg::Vector3 trans_ecef_poish = tf_map.at("ECEFPOISH")->transform.translation;
        geometry_msgs::msg::Quaternion rot_ecef_poish = tf_map.at("ECEFPOISH")->transform.rotation;
        Eigen::Vector3d t_ecef_poish;
        t_ecef_poish << trans_ecef_poish.x, trans_ecef_poish.y, trans_ecef_poish.z;
        Eigen::Quaterniond q_ecef_poish(rot_ecef_poish.w, rot_ecef_poish.x, rot_ecef_poish.y, rot_ecef_poish.z);

        // Compute the ENU transformation
        const Eigen::Vector3d t_enu0_poish = fixposition::TfEnuEcef(t_ecef_poish, fixposition::TfWgs84LlhEcef(t_ecef_enu0_));
        const Eigen::Quaterniond q_enu0_poish = q_ecef_enu0_.inverse() * q_ecef_poish;

        // Create tf2::Transform tf_ENU0POISH
        tf2::Transform tf_ENU0POISH;
        tf_ENU0POISH.setOrigin(tf2::Vector3(t_enu0_poish.x(), t_enu0_poish.y(), t_enu0_poish.z()));
        tf2::Quaternion tf_q_enu0_poish(q_enu0_poish.x(), q_enu0_poish.y(), q_enu0_poish.z(), q_enu0_poish.w());
        tf_ENU0POISH.setRotation(tf_q_enu0_poish);

        // Publish map -> odom
        // Multiply the transforms
        tf2::Transform tf_ENU0POI;
        tf2::fromMsg(tf_map.at("ENU0POI")->transform, tf_ENU0POI);
        tf2::Transform tf_combined = tf_ENU0POI * tf_ENU0POISH.inverse();

        // Create a new TransformStamped message
        geometry_msgs::msg::TransformStamped tf_map_odom;
        tf_map_odom.header.stamp = rclcpp::Clock().now();
        tf_map_odom.header.frame_id = "map";
        tf_map_odom.child_frame_id = "odom";
        tf_map_odom.transform = tf2::toMsg(tf_combined);
        br_->sendTransform(tf_map_odom);

        // Publish odom -> base_link
        geometry_msgs::msg::TransformStamped tf_odom_base;
        tf_odom_base.header.stamp = rclcpp::Clock().now();
        tf_odom_base.header.frame_id = "odom";
        tf_odom_base.child_frame_id = "base_link";
        tf_odom_base.transform = tf2::toMsg(tf_ENU0POISH);

        // Send the transform
        br_->sendTransform(tf_odom_base);
    }
}

void OdomToNavSatFix(const fixposition::FP_ODOMETRY& data, rclcpp::Publisher<sensor_msgs::msg::NavSatFix>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        sensor_msgs::msg::NavSatFix msg;
    
        // Populate message header
        if (data.odom.stamp.tow == 0.0 && data.odom.stamp.wno == 0) {
            msg.header.stamp = rclcpp::Clock().now();
        } else {
            msg.header.stamp = GpsTimeToMsgTime(data.odom.stamp);
        }
        msg.header.frame_id = data.odom.child_frame_id;
        
        // Populate LLH position
        Eigen::Map<Eigen::Matrix<double, 3, 3>> cov_map =
            Eigen::Map<Eigen::Matrix<double, 3, 3>>(msg.position_covariance.data());
        
        if (data.odom.pose.position.isZero()) {
            msg.latitude  = 0;
            msg.longitude = 0;
            msg.altitude  = 0;
            msg.position_covariance_type = 0;
            cov_map = Eigen::Matrix3d::Zero();
        } else {
            const Eigen::Vector3d llh_pos = TfWgs84LlhEcef(data.odom.pose.position);
            msg.latitude  = RadToDeg(llh_pos(0));
            msg.longitude = RadToDeg(llh_pos(1));
            msg.altitude  = llh_pos(2);

            // Populate LLH covariance
            const Eigen::Matrix3d p_cov_e = data.odom.pose.cov.topLeftCorner(3, 3);
            const Eigen::Matrix3d C_l_e = RotEnuEcef(data.odom.pose.position);
            const Eigen::Matrix3d p_cov_l = C_l_e * p_cov_e * C_l_e.transpose();
            cov_map = p_cov_l;
            msg.position_covariance_type = 3;
        }

        // Populate LLH status
        int status_flag = std::max(data.gnss1_status, data.gnss2_status);

        if (status_flag < static_cast<int8_t>(GnssStatus::FIX_TYPE_S2D)) {
            msg.status.status = static_cast<int8_t>(NavSatStatusData::Status::STATUS_NO_FIX);
            msg.status.service = static_cast<uint16_t>(NavSatStatusData::Service::SERVICE_NONE);

        } else if (status_flag >= static_cast<int8_t>(GnssStatus::FIX_TYPE_S2D) || status_flag < static_cast<int8_t>(GnssStatus::FIX_TYPE_RTK_FLOAT)) {
            msg.status.status = static_cast<int8_t>(NavSatStatusData::Status::STATUS_FIX);
            msg.status.service = static_cast<uint16_t>(NavSatStatusData::Service::SERVICE_ALL);

        } else if (status_flag >= static_cast<int8_t>(GnssStatus::FIX_TYPE_RTK_FLOAT)) {
            msg.status.status = static_cast<int8_t>(NavSatStatusData::Status::STATUS_GBAS_FIX);
            msg.status.service = static_cast<uint16_t>(NavSatStatusData::Service::SERVICE_ALL);

        } else {
            msg.status.status = static_cast<int8_t>(NavSatStatusData::Status::STATUS_NO_FIX);
            msg.status.service = static_cast<uint16_t>(NavSatStatusData::Service::SERVICE_NONE);
        }

        // Publish message
        pub->publish(msg);
    }
}

void OdomToImuMsg(const fixposition::FP_ODOMETRY& data, rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        sensor_msgs::msg::Imu msg;
    
        // Populate message
        if (data.odom.stamp.tow == 0.0 && data.odom.stamp.wno == 0) {
            msg.header.stamp = rclcpp::Clock().now();
        } else {
            msg.header.stamp = GpsTimeToMsgTime(data.odom.stamp);
        }
        
        msg.header.frame_id = data.odom.frame_id;
        tf2::toMsg(data.acceleration, msg.linear_acceleration);
        tf2::toMsg(data.odom.twist.angular, msg.angular_velocity);

        // Publish message
        pub->publish(msg);
    }
}

void OdomToYprMsg(const OdometryData& data, rclcpp::Publisher<geometry_msgs::msg::Vector3Stamped>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        geometry_msgs::msg::Vector3Stamped msg;

        // Populate message
        if (data.stamp.tow == 0.0 && data.stamp.wno == 0) {
            msg.header.stamp = rclcpp::Clock().now();
        } else {
            msg.header.stamp = GpsTimeToMsgTime(data.stamp);
        }
        msg.header.frame_id = "FP_ENU";

        // Euler angle wrt. ENU frame in the order of Yaw Pitch Roll
        Eigen::Vector3d enu_euler = RotToEul(data.pose.orientation.toRotationMatrix());
        msg.vector.set__x(enu_euler.x());
        msg.vector.set__y(enu_euler.y());
        msg.vector.set__z(enu_euler.z());

        // Publish message
        pub->publish(msg);
    }
}

void JumpWarningMsg(std::shared_ptr<rclcpp::Node> node, const times::GpsTime& stamp, const Eigen::Vector3d& pos_diff, 
                    const Eigen::MatrixXd& prev_cov, rclcpp::Publisher<fixposition_driver_ros2::msg::COVWARN>::SharedPtr pub) {
    if (pub->get_subscription_count() > 0) {
        // Create message
        fixposition_driver_ros2::msg::COVWARN msg;

        // Populate message
        if (stamp.tow == 0.0 && stamp.wno == 0) {
            msg.header.stamp = rclcpp::Clock().now();
        } else {
            msg.header.stamp = GpsTimeToMsgTime(stamp);
        }

        std::stringstream warn_msg;
        warn_msg << "Position jump detected! The change in position is greater than the estimated covariances. "
                 << "Position difference: [" << pos_diff[0] << ", " << pos_diff[1] << ", " << pos_diff[2] << "], "
                 << "Covariances: [" << prev_cov(0,0) << ", " << prev_cov(1,1) << ", " << prev_cov(2,2) << "]";

        RCLCPP_WARN(node->get_logger(), "%s", warn_msg.str().c_str());
        tf2::toMsg(pos_diff, msg.jump);
        tf2::toMsg(Eigen::Vector3d(prev_cov(0,0),prev_cov(1,1),prev_cov(2,2)), msg.covariance);

        // Publish message
        pub->publish(msg);
    }
}

}  // namespace fixposition
