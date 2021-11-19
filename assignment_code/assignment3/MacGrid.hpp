#ifndef MAC_GRID_H_
#define MAC_GRID_H_

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "gloo/components/MaterialComponent.hpp"


namespace GLOO {
template <class T>
struct BBox{T x_min, x_max, y_min, y_max;};

class MacGrid : public SceneNode {
public:
  MacGrid(int size_x, int size_y);
  void PlotLineSegment(glm::vec3 p1, glm::vec3 p2);
  // int IndexOf(int i, int j);
  int IndexOf(int i, int j) const {return size_x_ * j + i;}

  // getter functions
  float vel_x(int i, int j) const {return velocities[IndexOf(i,j)] + velocities[IndexOf(i+1,j)]/2.f;}
  float vel_y(int i, int j) const {return velocities[IndexOf(i,j)] + velocities[IndexOf(i,j+1)]/2.f;}

  // interpolate velocity somewhere inside a cell
  float interpolate(float x, float y);

private:
  int size_x_;
  int size_y_;

  std::vector<SceneNode*> grid_vertices;
  std::shared_ptr<VertexObject> sphere_mesh_;
  std::shared_ptr<ShaderProgram> shader_;
  std::shared_ptr<Material> material;

  std::vector<float> velocities; // n = (size_x)(size_y)
  std::vector<float> pressures; // n = (size_x-1)*(size_y-1)
};
}  // namespace GLOO

#endif
