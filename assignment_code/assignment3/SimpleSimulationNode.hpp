#ifndef SIMULATION_NODE_H_
#define SIMULATION_NODE_H_

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "SimpleSystem.hpp"
#include "ParticleState.hpp"
#include "IntegratorType.hpp"

#include <string>
#include <vector>

namespace GLOO {
class SimpleSimulationNode : public SceneNode {
 public:
  SimpleSimulationNode(IntegratorType integrator_type, float integration_step);
  void Update(double t);

 private:
    IntegratorType integrator_type_;
    float step_;

    std::vector<SceneNode*> particles_;

    //state, system
    SimpleSystem system_;
    ParticleState state_;

    //shaders, mesh
    std::shared_ptr<VertexObject> sphere_mesh_;
    std::shared_ptr<ShaderProgram> shader_;
    std::vector<SceneNode*> particle_;


};
}  // namespace GLOO

#endif
