#ifndef CLOTH_SYSTEM_NODE_H_
#define CLOTH_SYSTEM_NODE_H_

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "ClothSystem.hpp"
#include "ParticleState.hpp"
#include "IntegratorType.hpp"

#include <string>
#include <vector>

namespace GLOO {
class ClothSystemNode : public SceneNode {
 public:
  ClothSystemNode(IntegratorType integrator_type, float integration_step);
  void CreateParticle(glm::vec3 pos);
  void Update(double t);
  void PlotOriginalPoints();
  int IndexOf(int row, int col);

 private:
    IntegratorType integrator_type_;
    float step_;
    bool prev_released1 = true; //resets the cloth
    bool prev_released2 = true; //toggles wind

    //state, system
    ClothSystem system_;
    ParticleState state_;

    //shaders, mesh
    std::shared_ptr<VertexObject> sphere_mesh_;
    std::shared_ptr<VertexObject> spring_mesh_;
    std::shared_ptr<ShaderProgram> shader_;
    std::vector<SceneNode*> particle_; //list of particles/spheres

    //particles
    int num_particles_;
    float mass_ = 1.0; //TODO: check
    Springs spring_spec_;

    //cloth dimensions
    int num_rows = 10;
    int num_cols = 10;

    std::vector<SceneNode*> springs_;
    std::vector<SceneNode*> particles_;
};
}  // namespace GLOO

#endif
