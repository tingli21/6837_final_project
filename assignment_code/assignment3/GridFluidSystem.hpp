#ifndef GRID_FLUID_SYSTEM_H_
#define GRID_FLUID_SYSTEM_H_

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "MarkerParticle.hpp"
#include "MacGrid.hpp"

namespace GLOO {
// template <class T>
// struct BBox{T x_min, x_max, y_min, y_max;};

class GridFluidSystem : public SceneNode {
public:
  GridFluidSystem(int size_x, int size_y, float step_size, int num_particles);
  void MakeMarkerParticles();
  void Update(double delta_time) override;
  void CountParticles(); //calclates the number of particles at each point of the grid

  int IndexOf(int i, int j) const {return size_x_ * j + i;}

private:
  int size_x_;
  int size_y_;
  int num_particles_;
  float step_size_;
  float water_density_;
  MacGrid* mac_grid_ptr;

  //list of vectors
  std::vector<MarkerParticle*> particle_count_; //number of particles in each grid cell n = size_x * size_y
  std::vector<MarkerParticle*> particles_; 

};
}  // namespace GLOO

#endif
