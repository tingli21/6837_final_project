#ifndef PENDULUM_SYSTEM_NODE_H_
#define PENDULUM_SYSTEM_NODE_H_

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "PendulumSystem.hpp"
#include "ParticleState.hpp"
#include "IntegratorType.hpp"

#include <string>
#include <vector>

namespace GLOO {
class PendulumSystemNode : public SceneNode {
 public:
  PendulumSystemNode(IntegratorType integrator_type, float integration_step);
  void CreateParticle(glm::vec3 pos);
  void CreateSpringSpec();
  void Update(double t);
  void CreateSpring(int i);

 private:
    IntegratorType integrator_type_;
    float step_;

    //state, system
    PendulumSystem system_;
    ParticleState state_;

    //shaders, mesh
    std::shared_ptr<VertexObject> sphere_mesh_;
    std::shared_ptr<VertexObject> spring_mesh_;
    std::shared_ptr<ShaderProgram> shader_;
    std::vector<SceneNode*> particle_; //list of particles/spheres

    //particles
    int num_particles_ = 4;
    float mass_ = 1.0; //TODO: check
    Springs spring_spec_;


    std::vector<SceneNode*> springs_;
    std::vector<SceneNode*> particles_;
};
}  // namespace GLOO

#endif
