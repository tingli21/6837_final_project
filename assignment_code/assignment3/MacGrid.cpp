#include "MacGrid.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/shaders/SimpleShader.hpp"
#include "gloo/InputManager.hpp"
#include "glm/gtx/string_cast.hpp"


namespace GLOO {
MacGrid::MacGrid(int size_x, int size_y){
  size_x_ = size_x;
  size_y_ = size_y;

  sphere_mesh_ = PrimitiveFactory::CreateSphere(0.015f, 25, 25);
  shader_ = std::make_shared<PhongShader>();
  glm::vec3 color(1.f, 1.f, 1.f);
  glm::vec3 color_border(1.f, 0.f, 0.f);
  material = std::make_shared<Material>(color, color, color, 0);
  material_border = std::make_shared<Material>(color_border, color_border, color_border, 0);

  InitCellTypes();

  for (int j=0; j<size_x_; j++){
    for (int i=0; i<size_y_; i++){
      auto pt = make_unique<SceneNode>();
      SceneNode *node_ptr = pt.get();
      AddChild(std::move(pt));
      grid_vertices.push_back(node_ptr);

      glm::vec3 pos(0.3*j, -0.3*i, 0);

      grid_vertices[IndexOf(i,j)]->GetTransform().SetPosition(pos);
      grid_vertices[IndexOf(i,j)]->CreateComponent<ShadingComponent>(shader_);
      grid_vertices[IndexOf(i,j)]->CreateComponent<RenderingComponent>(sphere_mesh_);

      // make border red, and other nodes white
      if (j==0 || j==size_x_-1 || i==size_y_-1){
        grid_vertices[IndexOf(i,j)]->CreateComponent<MaterialComponent>(material_border);
      }
      else {
        grid_vertices[IndexOf(i,j)]->CreateComponent<MaterialComponent>(material);
      }
    }
  }

  // draw line segments between grid points
  for (int j=0; j<size_x_-1; j++){
    for (int i=0; i<size_y_; i++){
      glm::vec3 pos1(0.3*j, -0.3*i, 0);
      glm::vec3 pos2(0.3*(j+1), -0.3*i, 0);

      PlotLineSegment(pos1,pos2);
    }
  }

  for (int j=0; j<size_x_; j++){
    for (int i=0; i<size_y_-1; i++){
      glm::vec3 pos1(0.3*j, -0.3*i, 0);
      glm::vec3 pos2(0.3*j, -0.3*(i+1), 0);

      PlotLineSegment(pos1,pos2);
    }
  }

}

float MacGrid::interpolate(float x, float y){
  int i=floor(x);
  int j=floor(y);

  float val_00 = velocities[IndexOf(i,j)];
  float val_10 = velocities[IndexOf(i+1,j)];
  float val_01 = velocities[IndexOf(i,j+1)];
  float val_11 = velocities[IndexOf(i+1,j+1)];

  // interpolate x
  float val_0 = (1-(x-i))*val_00 + (x-i)*val_10;
  float val_1 = (1-(x-i))*val_01 + (x-i)*val_11;

  // interpolate y
  float val = (1-(y-j))*val_0 + (y-j)*val_1;
  return val;
}

void MacGrid::PlotLineSegment(glm::vec3 p1, glm::vec3 p2){
  auto line = std::make_shared<VertexObject>();

  auto positions = make_unique<PositionArray>();

  positions->push_back(p1);
  positions->push_back(p2);

  auto indices = make_unique<IndexArray>();
  indices->push_back(0);
  indices->push_back(1);

  line->UpdatePositions(std::move(positions));
  line->UpdateIndices(std::move(indices));

  auto shader = std::make_shared<SimpleShader>();

  auto line_node = make_unique<SceneNode>();
  line_node->CreateComponent<ShadingComponent>(shader);

  auto& rc = line_node->CreateComponent<RenderingComponent>(line);
  rc.SetDrawMode(DrawMode::Lines);

  AddChild(std::move(line_node));
}

void MacGrid::InitCellTypes(){
  // Set all cells to solid
  for (int j=0; j<size_x_; j++){
    for (int i=0; i<size_y_; i++){
      cell_types.push_back(CellType::Solid);
    }
  }
  // Set center cells to air
  for (int j=0; j<size_x_-1; j++){
    for (int i=1; i<size_y_-1; i++){
      cell_types[IndexOf(i,j)] = CellType::Air;
    }
  }
}


} // namespace GLOO
