// Created by Indraneel and Naren on 19/09/22
#ifndef LAZY_TRAFFIC_CONTROLLER_H
#define LAZY_TRAFFIC_CONTROLLER_H

#include <string>
#include <cmath>
#include <algorithm>
#include <queue>
#include <thread>
#include <ros/console.h>
#include <ros/ros.h>
#include <std_msgs/Bool.h>
#include <geometry_msgs/Twist.h>

class LazyTrafficController {

public:

    LazyTrafficController(void);
    ~LazyTrafficController(void);


private:

    void RunController(void);
    void statusCallback(const std_msgs::Bool &status_msg);
    std::set<std::string> getFleetStatusInfo(void);
    void initialiseAgentMap(std::set<std::string> active_agents);
    void computeVelocities(const ros::TimerEvent&);

    typedef struct Agent {
        std::string name;
        ros::Publisher pub_vel_;
        std::string robot_frame_id_;
    } Agent_s;

    // std::set<Agent_s> agent_set_;
    std::map<std::string, Agent_s> agent_map_;
    std::set<std::string> active_agents;

    std::thread traffic_controller_thread_;
    bool controller_active_;
    ros::Subscriber status_subscriber_;
    ros::NodeHandle nh_;
    bool fleet_status_outdated_;
    ros::ServiceClient status_client; 
    ros::Timer controller_timer;
    std::string map_frame_id_;
    double controller_period_s;

};
#endif // LAZY_TRAFFIC_CONTROLLER_H