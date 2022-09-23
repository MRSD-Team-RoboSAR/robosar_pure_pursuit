// Created by Naren on 22/09/22
#ifndef RECIPROCAL_VELOCITY_OBSTACLE_H
#define RECIPROCAL_VELOCITY_OBSTACLE_H

#include <string>
#include <cmath>
#include <algorithm>
#include <queue>
#include <thread>
#include <mutex>

#include "robosar_messages/robosar_controller.h"
#include "lazy_traffic_agent.hpp"
// ROS stuff
#include <tf/tf.h>
#include <tf2_ros/transform_listener.h>
#include <ros/console.h>
#include <ros/ros.h>
#include <std_msgs/Bool.h>
#include <geometry_msgs/Twist.h>
#include <queue>

//A class that operates on 2D
#include "Vector2.h"
#include <queue>
#include <map>
#define NUM_VELOCITY_SAMPLES 5 //NUMBER OF SAMPLES PER EACH AGENT
#define MAX_SPEED 1.5 // Maximum speed of agent
#define AGENT_RADIUS 1.3 // Radius of agent
#define RADIUS_MULT_FACTOR 2 // since r1 + r2 = 2*r for RoboSAR, Define agent_radius and mult_factor for obstacle cone
#define TIME_STEP 1 //frequence at which controller runs ( 1/ timestep)
#define SAFETY_FACTOR 2 //The safety factor of the agent (weight for penalizing candidate velocities - the higher the safety factor, the less 'aggressive' an agent is)
#define MAX_NEIGHBORS 4 // Maximum number of neighbors to consider
#define MAX_NEIGH_DISTANCE 2.00 //Max distance among neighbors
using namespace std;
typedef pair<string, float> dist;

class RecVelocityObs {
    private:
        vector<dist> m_neighbors = vector<dist>(MAX_NEIGHBORS);
        map<string, Agent> m_agent_map;
        void computeNearestNeighbors(map<string, Agent> agent_map_, RVO::Vector2);
    public:

        RecVelocityObs(void){};
        ~RecVelocityObs(void){};
                                // int agent_id, RVO::Vector2 vel_pref, vector<RVO::Vector2>& velocity_vector, vector<RVO::Vector2>& position_vector, priority_queue< pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>> > neighbors
        void computeNewVelocity(int agent_id, RVO::Vector2 vel_pref, map<string,RVO::Vector2>& velocity_vector, map<string,RVO::Vector2>& position_vector);
        bool m_is_collision = false;
        RVO::Vector2 m_vel_curr;
        RVO::Vector2 m_vel_new;
        RVO::Vector2 m_pos_curr;
        float timeToCollision(const RVO::Vector2& p, const RVO::Vector2& v, const RVO::Vector2& p2, float radius, bool& collision);
};
#endif // RECIPROCAL_VELOCITY_OBSTACLE_H