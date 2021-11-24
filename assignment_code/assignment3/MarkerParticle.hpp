#ifndef MARKER_PARTICLE_H_
#define MARKER_PARTICLE_H_

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "gloo/components/MaterialComponent.hpp"

namespace GLOO {

enum class State { Solid, Air, Liquid };

class MarkerParticle : public SceneNode {
  public:
    MarkerParticle();
    MarkerParticle(float pos_x, float pos_y, float vel_x, float vel_y);

    glm::vec3 GetPosition();
    void Render();

    void advect(float dt);

  private:
    float pos_x_;
    float pos_y_;
    float vel_x_;
    float vel_y_;

    std::shared_ptr<VertexObject> sphere_mesh_;
    std::shared_ptr<ShaderProgram> shader_;
    std::shared_ptr<Material> material_;

    State state_;
};
}  // namespace GLOO

#endif
