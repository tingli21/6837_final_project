#include "SimulationApp.hpp"

#include "glm/gtx/string_cast.hpp"

#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/CameraComponent.hpp"
#include "gloo/components/LightComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/MeshLoader.hpp"
#include "gloo/lights/PointLight.hpp"
#include "gloo/lights/AmbientLight.hpp"
#include "gloo/cameras/ArcBallCameraNode.hpp"
#include "gloo/debug/AxisNode.hpp"

#include "Fluid.hpp"
#include "gloo/Image.hpp"
#include <string>
#include "Parameters.hpp"


namespace GLOO {
SimulationApp::SimulationApp(const std::string& app_name,
                             glm::ivec2 window_size)
    : Application(app_name, window_size) {
  // TODO: use integrator type and step to create integrators;
  // the lines below exist only to suppress compiler warnings.

  SceneNode& root = scene_->GetRootNode();
  std::cout << "hi" << std::endl;
  auto fluid = make_unique<Fluid>();
  root.AddChild(std::move(fluid));
  std::cout << "hi2" << std::endl;

  float add_amount = 0.3f * fmax(CELLS_X, CELLS_Y); //ADD_AMT_INIT=0.3f
  float cr = 0.f;
  float cg = 0.f;
  float cb = 0.f;

  fluid->add_U_y_force_at(0, 0, 10.f * 10); // FORCE_SCALE = 10.f
  fluid->add_U_x_force_at(0, 0, 10.f * 10);
  fluid->add_source_at(0, 0, add_amount);
  std::cout << "hi3" << std::endl;
  for (int i=0; i<24; i++){
    std::cout << "hi4" << std::endl;
    fluid->step();
    std::cout << "hi5" << std::endl;
    Image image(CELLS_X, CELLS_Y);
    for (size_t y = 0; y < CELLS_Y; y++) {
      for (size_t x = 0; x < CELLS_X; x++) {
        std::cout << "hi6" << std::endl;
        cr = fluid->S_at(y, x);
        cg = fluid->S_at(y, x);
        cb = fluid->S_at(y, x);
        glm::vec3 mycolor{cr, cg, cb};
        image.SetPixel(y, x, mycolor);
      }
    }
    image.SavePNG("frame"+ std::to_string(i) + ".png");
  }
}

void SimulationApp::SetupScene() {
  SceneNode& root = scene_->GetRootNode();

  std::unique_ptr<ArcBallCameraNode> camera_node = make_unique<ArcBallCameraNode>(45.f, 0.75f, 5.0f);
  scene_->ActivateCamera(camera_node->GetComponentPtr<CameraComponent>());
  root.AddChild(std::move(camera_node));

  root.AddChild(make_unique<AxisNode>('A'));

  std::shared_ptr<AmbientLight> ambient_light = std::make_shared<AmbientLight>();
  ambient_light->SetAmbientColor(glm::vec3(0.2f));
  root.CreateComponent<LightComponent>(ambient_light);

  std::shared_ptr<PointLight> point_light = std::make_shared<PointLight>();
  point_light->SetDiffuseColor(glm::vec3(0.8f, 0.8f, 0.8f));
  point_light->SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
  point_light->SetAttenuation(glm::vec3(1.0f, 0.09f, 0.032f));
  std::unique_ptr<SceneNode> point_light_node = make_unique<SceneNode>();
  point_light_node->CreateComponent<LightComponent>(point_light);
  point_light_node->GetTransform().SetPosition(glm::vec3(0.0f, 2.0f, 4.f));
  root.AddChild(std::move(point_light_node));
}
}  // namespace GLOO
