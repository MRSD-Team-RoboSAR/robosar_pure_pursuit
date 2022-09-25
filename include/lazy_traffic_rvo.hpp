// Created by Naren on 22/09/22
#ifndef LAZY_TRAFFIC_RVO_H
#define LAZY_TRAFFIC_RVO_H

// #include <string>
// #include <cmath>
// #include <algorithm>
// #include <queue>
// #include <thread>
// #include <mutex>

// #include "robosar_messages/robosar_controller.h"
//#include "lazy_traffic_agent.hpp"
// // ROS stuff
// #include <tf/tf.h>
// #include <tf2_ros/transform_listener.h>
// #include <ros/console.h>
// #include <ros/ros.h>
// #include <std_msgs/Bool.h>
// #include <geometry_msgs/Twist.h>
// #include <queue>

//A class that operates on 2D
#include "Vector2.h"
#include <queue>
#include <map>
#include <unordered_map>


#define NUM_VELOCITY_SAMPLES 5 //NUMBER OF SAMPLES PER EACH AGENT
#define MAX_SPEED 1.5 // Maximum speed of agent
#define AGENT_RADIUS 1.3 // Radius of agent
#define RADIUS_MULT_FACTOR 2 // since r1 + r2 = 2*r for RoboSAR, Define agent_radius and mult_factor for obstacle cone
#define TIME_STEP 1 //frequence at which controller runs ( 1/ timestep)
#define SAFETY_FACTOR 2 //The safety factor of the agent (weight for penalizing candidate velocities - the higher the safety factor, the less 'aggressive' an agent is)

using namespace std;
typedef pair<string, float> AgentDistPair;

float rvoTimeToCollision(const Vector2& ego_position, const Vector2& vel_a_to_b, const Vector2& neighbor_position, float obstacle_radius, bool& is_in_collision) {
    RVO::Vector2 minkowski_diff = neighbor_position - ego_position;
    float sq_diam = obstacle_radius * obstacle_radius;
    float time;

    float discr = -sqrt(det(vel_a_to_b, minkowski_diff)) + sq_diam * absSq(vel_a_to_b);
    if (discr > 0) 
    {
      if (is_in_collision) 
      {
        time = (vel_a_to_b* minkowski_diff + sqrt(discr)) / absSq(vel_a_to_b);
        if (time < 0) {
          time = -INFINITY;
        }
      } 
      else 
      {
        time = (vel_a_to_b* minkowski_diff - sqrt(discr)) / absSq(vel_a_to_b);
        if (time < 0) 
        {
          time = INFINITY;
        }
      }
    } 
    else 
    {
      if (is_in_collision) 
      {
        time = -INFINITY;
      } 
      else 
      {
        time = INFINITY;
      }
    } 
    return time;
}



RVO::Vector2 rvoComputeNewVelocity(bool& is_collision, std::vector<AgentDistPair> neighbors, string myName, unordered_map<string, Agent> agent_map) {

    RVO::Vector2 vel_cand;
    RVO::Vector2 vel_computed;
    float min_penalty = INFINITY;
    RVO::Vector2 vel_pref = agent_map[myName].preferred_velocity_;
    RVO::Vector2 pos_curr(agent_map[myName].current_pose_.transform.translation.x,agent_map[myName].current_pose_.transform.translation.y);
    RVO::Vector2 vel_curr = agent_map[myName].current_velocity_;
    for(int i=0;i<NUM_VELOCITY_SAMPLES;i++)
    {

        if(i==0) 
        {
            vel_cand = vel_pref;
        }
        else 
        {
            do 
            {
                vel_cand = RVO::Vector2(2.0f*rand() - RAND_MAX, 2.0f*rand() - RAND_MAX);
            } while(absSq(vel_cand) > sqrt((float) RAND_MAX));
            vel_cand *= (MAX_SPEED / RAND_MAX);
        }

        float dist_to_pref_vel ;
        if(is_collision)
        {
            dist_to_pref_vel = 0;
        }
        else
        {
            dist_to_pref_vel = abs(vel_cand - vel_pref);
        }
        float min_t_to_collision = INFINITY;
        for(auto n: neighbors)
        {
            // If neighbor is an obstacle, agent_Radius, position and other attributes would change
            // Change code accordingly
            float t_to_collision;
            RVO::Vector2 vel_a_to_b;
            RVO::Vector2 vel_b = agent_map[n.first].current_velocity_;
            vel_a_to_b = 2*vel_cand - vel_curr - vel_b;
            RVO::Vector2 neigh_pos(agent_map[n.first].current_pose_.transform.translation.x, agent_map[n.first].current_pose_.transform.translation.y);
            float time = RecVelocityObs::timeToCollision(pos_curr, vel_a_to_b, neigh_pos, RADIUS_MULT_FACTOR*AGENT_RADIUS, is_collision);
            if(is_collision) 
            {
                t_to_collision = -ceil(time / TIME_STEP);
                t_to_collision -= absSq(vel_cand) / (MAX_SPEED*MAX_SPEED);

            }
            else
            {
                t_to_collision = time;
            }
            if(t_to_collision<min_t_to_collision)
            {
                min_t_to_collision = t_to_collision;
                if(SAFETY_FACTOR / min_t_to_collision + dist_to_pref_vel>= min_penalty)
                    break;
            }
        }
        float penalty = SAFETY_FACTOR / min_t_to_collision + dist_to_pref_vel;
        if(penalty < min_penalty)
        {
            min_penalty = penalty;
            vel_computed = vel_cand;
        }

    }
    return vel_computed;
}


#endif // LAZY_TRAFFIC_RVO_H