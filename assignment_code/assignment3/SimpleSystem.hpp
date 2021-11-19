#ifndef SIMPLE_SYSTEM_H
#define SIMPLE_SYSTEM_H

#include "ParticleSystemBase.hpp"
#include "ParticleState.hpp"

namespace GLOO {
class SimpleSystem: public ParticleSystemBase {
  public:
  ParticleState ComputeTimeDerivative(const ParticleState& state,float time) const{
    //Implement the simple example
    //TODO: loop through everything in position and pushback the altered position
    // return velocity as a copy of positions

    ParticleState new_state = ParticleState();
    std::vector<glm::vec3> position = state.positions;

    std::vector<glm::vec3> pos(1);
    std::vector<glm::vec3> vel(1);

    new_state.positions = pos;
    new_state.velocities = vel;

    new_state.positions[0] = glm::vec3(-position[0][1], position[0][0], 0.f);
    new_state.velocities[0] = glm::vec3(0.f, 0.f, 0.f);

    return new_state;
  }

};
}  // namespace GLOO

#endif
