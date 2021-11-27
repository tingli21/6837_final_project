#include "MarkerParticle.hpp"

#include <fstream>

#include "gloo/utils.hpp"
#include "gloo/MeshLoader.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/shaders/SimpleShader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "glm/gtx/string_cast.hpp"

namespace GLOO {

MarkerParticle::MarkerParticle(float pos_x, float pos_y, float vel_x, float vel_y) {
    // make shader and mesh
    shader_ = std::make_shared<SimpleShader>();
    sphere_mesh_ = PrimitiveFactory::CreateSphere(0.02f, 25, 25);
    glm::vec3 color(0.5f, 0.5f, 0.5f);
    material_ = std::make_shared<Material>(color, color, color, 0);

    pos_x_ = pos_x;
    pos_y_ = pos_y;

    vel_x_ = vel_x;
    vel_y_ = vel_y;

    Render();
}

MarkerParticle::MarkerParticle() {
    // make shader and mesh
    shader_ = std::make_shared<SimpleShader>();
    sphere_mesh_ = PrimitiveFactory::CreateSphere(0.02f, 25, 25);

    pos_x_ = 0.f;
    pos_y_ = 0.f;

    vel_x_ = 0.f;
    vel_y_ = 0.f;

    Render();
}

glm::vec3 MarkerParticle::GetPosition(){
    return glm::vec3(pos_x_, pos_y_, 0.f);
}

void MarkerParticle::Render(){
    //setup a sphere scene node and store pointer for later
    auto sphere_node = make_unique<SceneNode>();
    sphere_node->CreateComponent<ShadingComponent>(shader_);
    sphere_node->CreateComponent<RenderingComponent>(sphere_mesh_);
    sphere_node->GetTransform().SetPosition(GetPosition());
    sphere_node->CreateComponent<MaterialComponent>(material_);
    particle_ptr_ = sphere_node.get();
    AddChild(std::move(sphere_node));
}

void MarkerParticle::advect(float dt){
  pos_x_ += vel_x_ * dt;
  pos_y_ += vel_y_ * dt;
  particle_ptr_->GetTransform().SetPosition(GetPosition());
}

}  // namespace GLOO
