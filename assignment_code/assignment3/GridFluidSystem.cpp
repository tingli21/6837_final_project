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
  auto gridnode = make_unique<MacGrid>(size_x, size_y);
  mac_grid_ptr = gridnode.get();
  AddChild(std::move(gridnode));

  size_x_ = size_x;
  size_y_ = size_y;
  step_size_ = step_size;
  num_particles_ = num_particles;

  //render marker particles at the top
  MakeMarkerParticles();

}

void GridFluidSystem::MakeMarkerParticles(){
  float increment = float(size_x_) / float(num_particles_-1);
  for (float i = 0.0; i < num_particles_; i++){
    float x_pos = increment * i;
    auto particle = make_unique<MarkerParticle>(x_pos, -0.5, 0.1f, -0.1f);
    particles_.push_back(particle.get());
    AddChild(std::move(particle));
  }

}

void GridFluidSystem::Update(double delta_time){
  mac_grid_ptr->UpdateCellTypes(particles_);

  for (int i=0; i<=int(delta_time/step_size_); i++){
    float step = step_size_ < delta_time ? step_size_ : delta_time;

    // advect all marker particles and ensure outside obstacles
    for (int n=0; n<particles_.size();n++) {
      particles_[n]->advect(step);
      float x = particles_[n]->GetPosition()[0];
      float y = particles_[n]->GetPosition()[1];

      if (mac_grid_ptr->GetCellType(x,y)==CellType::Solid) {
        std::cout << x << ", " << y << std::endl;

        //std::cout << particles_[n]->GetPosition()[0] << std::endl;
        particles_[n]->advect(-1.f*step);
        //std::cout << particles_[n]->GetPosition()[0] << std::endl;
      }
    }
  }
}

} // namespace GLOO
