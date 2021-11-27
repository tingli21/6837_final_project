#ifndef MAC_GRID_H_
#define MAC_GRID_H_

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "MarkerParticle.hpp"


namespace GLOO {
template <class T>
struct BBox{T x_min, x_max, y_min, y_max;};
enum class CellType { Solid, Air, Liquid };

class MacGrid : public SceneNode {
public:
  MacGrid(int size_x, int size_y);

  void PlotLineSegment(glm::vec3 p1, glm::vec3 p2);
  int IndexOf(int i, int j) const {return size_x_ * j + i;}

  // cell type functions
  void InitCellTypes();
  CellType GetCellType(int i, int j) const{
    i = glm::clamp(i, 0, size_x_-1);
    j = glm::clamp(j, 0, size_y_-1);
    return cell_types[IndexOf(i,j)];
  }
  void SetCellType(int i, int j, CellType cell_type){cell_types[IndexOf(i,j)] = cell_type;}
  void UpdateCellTypes(std::vector<MarkerParticle*> particles);

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
  std::shared_ptr<Material> material_border;

  std::vector<float> velocities; // n = (size_x)(size_y)
  std::vector<float> pressures; // n = (size_x-1)*(size_y-1)
  std::vector<CellType> cell_types; // n = size_x * size_y
};
}  // namespace GLOO

#endif
