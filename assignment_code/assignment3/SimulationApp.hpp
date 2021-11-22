#ifndef SIMULATION_APP_H_
#define SIMULATION_APP_H_

#include "gloo/Application.hpp"

#include "IntegratorType.hpp"
#include "MarkerParticle.hpp"

namespace GLOO {
class SimulationApp : public Application {
 public:
  SimulationApp(const std::string& app_name,
                glm::ivec2 window_size,
                IntegratorType integrator_type,
                float integration_step);
  void SetupScene() override;

 private:
  IntegratorType integrator_type_;
  float integration_step_;

  float num_particles_ = 20;

  int size_x_ = 5;
  int size_y_ = 5;

  //list of vectors 
  std::vector<MarkerParticle> particles_;
};
}  // namespace GLOO

#endif
