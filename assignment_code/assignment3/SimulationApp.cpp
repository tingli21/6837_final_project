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

#include "SimpleSimulationNode.hpp"
#include "PendulumSystemNode.hpp"
#include "ClothSystemNode.hpp"


namespace GLOO {
SimulationApp::SimulationApp(const std::string& app_name,
                             glm::ivec2 window_size,
                             IntegratorType integrator_type,
                             float integration_step)
    : Application(app_name, window_size),
      integrator_type_(integrator_type),
      integration_step_(integration_step) {
  // TODO: use integrator type and step to create integrators; 
  // the lines below exist only to suppress compiler warnings.

  // one to grab root node
  SceneNode& root = scene_->GetRootNode();

  //COMMENT OUT THE ONE YOU WANT TO RUN

  //simple simulation (e 0.05)
  std::unique_ptr<SceneNode> simplenode = make_unique<SimpleSimulationNode>(integrator_type, integration_step);
  root.AddChild(std::move(simplenode));

  //pendulum simulation (e 0.05)
  std::unique_ptr<SceneNode> pendulumnode = make_unique<PendulumSystemNode>(integrator_type, integration_step);
  root.AddChild(std::move(pendulumnode));


  //cloth simulation (r 0.05)
  // std::unique_ptr<SceneNode> clothnode = make_unique<ClothSystemNode>(integrator_type, integration_step);
  // root.AddChild(std::move(clothnode));

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
