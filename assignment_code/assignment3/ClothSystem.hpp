#ifndef CLOTH_SYSTEM_H
#define CLOTH_SYSTEM_H

#include "ParticleSystemBase.hpp"
#include "ParticleState.hpp"
#include "Springs.hpp"
#include "gloo/utils.hpp"

namespace GLOO {
class ClothSystem : public ParticleSystemBase {
  public:

    //springs
    std::vector<Springs> springs_ = {};
    float k = 1.0;
    float rest_length = 0.2;

    //particles
    float mass_ = 1.0;
    int num_particles_ = 0;

    //forces
    glm::vec3 g_vector = glm::vec3(0.f, -1.f, 0.f); //TODO: UPDATE
    float d = 1.0; // drag
    bool wind_on = false;

    std::vector<int> fixed_points; 

  void SetNumParticles(int i){
    num_particles_ = i;
  }

  void SetK(float i){
    k = i;
  }

  void SetRestLength(int i){
    rest_length = i;
  }

  void SetDrag(float i){
    d = i;
  }

  void FixPoint(int i){
    fixed_points.push_back(i);
  }

  void ToggleWind(){
      wind_on = !wind_on;
  }

  void AddSpring(int k, int rest_length, int start, int end ){
    //creates the new spring
    Springs new_spring;
    new_spring.k = k;
    new_spring.rest_length = rest_length;
    new_spring.start = start;
    new_spring.end = end;

    springs_.push_back(new_spring);
  }

  ParticleState ComputeTimeDerivative(const ParticleState& state, float time) const{
    int num_particles = state.positions.size(); 

    //wind force
    std::vector<glm::vec3> wind;
    for (int i=0; i< num_particles; i++){
      glm::vec3 w_force;
      if(wind_on){
        float r2 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/100.0));
        w_force = glm::vec3(0.f, 0.f, 50.0-r2);
      }
      else{
        w_force = glm::vec3(0.f, 0.f, 0.f);
      }
        wind.push_back(w_force);
    }
    
    // calculate gravity force = m*g
    std::vector<glm::vec3> gravity;
    float g = 9.81;

    for (int i=0; i< num_particles; i++){
        glm::vec3 g_force = mass_ * g * g_vector;
        gravity.push_back(g_force);
    }

    //viscous drag force = -k*x'
    std::vector<glm::vec3> drag;
    for (int i=0; i< num_particles; i++){
        glm::vec3 d_force = -d * state.velocities[i];
        drag.push_back(d_force);
    }

    std::vector<glm::vec3> springforce;

    for (int i = 0; i< springs_.size(); i++){
      springforce.push_back(glm::vec3(0.f, 0.f, 0.f));
    }

    for (Springs s : springs_){
      int start = s.start;
      int end = s.end;

      glm::vec3 d = state.positions[start] - state.positions[end];
      float mag_d = glm::length(d);

      glm::vec3 s_force = -k * (mag_d-rest_length)*(d/mag_d);
      springforce[start] = glm::vec3(springforce[start][0] + s_force[0], springforce[start][1] + s_force[1], springforce[start][2] + s_force[2]);
      springforce[end] = glm::vec3(springforce[end][0] - s_force[0], springforce[end][1] - s_force[1], springforce[end][2] - s_force[2]);
    }

    //calculate the acceleration = f/m
    std::vector<glm::vec3> acceleration_;
    for (int i=0; i< num_particles; i++){
        glm::vec3 total = gravity[i] + drag[i] + springforce[i] + wind[i];
        acceleration_.push_back(total/mass_); //HELP 
    }

    // new position = old position + velocity
    // new velocity = old velocity + acceleration


    std::vector<glm::vec3> new_position;
    std::vector<glm::vec3> new_velocity;

    for (int i=0; i< num_particles; i++){
        //if point is fixed, velocity and acceleration =  0
        if (std::find(fixed_points.begin(), fixed_points.end(), i) != fixed_points.end()) { 
          new_position.push_back(glm::vec3(0.f, 0.f, 0.f));
          new_velocity.push_back(glm::vec3(0.f, 0.f, 0.f));
        }
        else{
          new_position.push_back(state.velocities[i]);
          new_velocity.push_back(acceleration_[i]);
        }
    }
    //print new_position.size() and new_velocity.size() sizes
    // printf("position: %i\n", new_position.size());
    // printf("velocity: %i\n", new_velocity.size());
    // printf("num particles: %i\n", num_particles);
    // printf("num particles: %i\n", num_particles_);

    //return the new state
    ParticleState new_state = ParticleState();
    new_state.positions = new_position;
    new_state.velocities = new_velocity;
    return new_state;
  }

};

}  // namespace GLOO

#endif
