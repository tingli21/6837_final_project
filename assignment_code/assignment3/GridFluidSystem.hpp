#ifndef GRID_FLUID_SYSTEM_H_
#define GRID_FLUID_SYSTEM_H_

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "MarkerParticle.hpp"

namespace GLOO {
// template <class T>
// struct BBox{T x_min, x_max, y_min, y_max;};

class GridFluidSystem : public SceneNode {
public:
  GridFluidSystem(int size_x, int size_y, float step_size, int num_particles);
  void Update(double delta_time) override;

  int IndexOf(int i, int j) const {return size_x_ * j + i;}

private:
  int size_x_;
  int size_y_;
  int num_particles_;
  float step_size_;

  //list of vectors
  std::vector<MarkerParticle*> particles_;


  // std::vector<SceneNode*> grid_vertices;
  // std::shared_ptr<VertexObject> sphere_mesh_;
  // std::shared_ptr<ShaderProgram> shader_;
  // std::shared_ptr<Material> material;

  // std::vector<float> velocities; // n = (size_x)(size_y)
  // std::vector<float> pressures; // n = (size_x-1)*(size_y-1)
};
}  // namespace GLOO

#endif
