#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/shaders/SimpleShader.hpp"
#include "gloo/InputManager.hpp"
#include "glm/gtx/string_cast.hpp"
#include "GridFluidSystem.hpp"
#include "MacGrid.hpp"
#include "MarkerParticle.hpp"


namespace GLOO {
GridFluidSystem::GridFluidSystem(int size_x, int size_y, float step_size, int num_particles){
  std::unique_ptr<SceneNode> gridnode = make_unique<MacGrid>(size_x, size_y);
  AddChild(std::move(gridnode));

  size_x_ = size_x;
  size_y_ = size_y;
  step_size_ = step_size;
  num_particles_ = num_particles;

  //render marker particles at the top
  float increment = float(size_x-1) / float(num_particles_);
  for (int i = 0; i < num_particles_; i++){
    float x_pos = 0.3*increment * i;
    auto particle = make_unique<MarkerParticle>(x_pos, 0.f, 0.f, 0.f);
    particles_.push_back(particle.get());
    AddChild(std::move(particle));
    // std::cout << particles_[i]->GetPosition()[0] << std::endl;
  }

}



void GridFluidSystem::Update(double delta_time){
  // for (int i=0l i<=int(delta_time/step_size_); i++){
  //   float step = step_size_ < delta_time ? step_size_ : delta_time;
  //
  // }
}

} // namespace GLOO
