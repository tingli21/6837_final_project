#include <fstream>

#include "SimpleSimulationNode.hpp"
#include "ParticleState.hpp"
#include "ParticleSystemBase.hpp"
#include "SimpleSystem.hpp"
#include "IntegratorBase.hpp"
#include "IntegratorType.hpp"
#include "IntegratorFactory.hpp"

#include "gloo/utils.hpp"
#include "gloo/MeshLoader.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/shaders/SimpleShader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "glm/gtx/string_cast.hpp"

namespace GLOO {
SimpleSimulationNode::SimpleSimulationNode(IntegratorType integrator_type, float integration_step) {

    //initialize
    integrator_type_ = integrator_type;
    step_ = integration_step;

    // make shader and mesh
    shader_ = std::make_shared<SimpleShader>();
    sphere_mesh_ = PrimitiveFactory::CreateSphere(0.02f, 25, 25);

    //create initial particle state
    state_.positions.push_back(glm::vec3(1.f, 0.5f, 1.f));
    state_.velocities.push_back(glm::vec3(1.f, 2.f, 3.f)); //this doesn't really matter

    //setup a sphere scene node and store pointer for later
    std::unique_ptr<SceneNode> sphere_node = make_unique<SceneNode>();
    sphere_node->CreateComponent<ShadingComponent>(shader_); 
    sphere_node->CreateComponent<RenderingComponent>(sphere_mesh_);
    sphere_node->GetTransform().SetPosition(state_.positions[0]);
    particle_.push_back(sphere_node.get()); 
    this->AddChild(std::move(sphere_node));

    //setup a stationary particle to show decay
    std::unique_ptr<SceneNode> sphere_node2 = make_unique<SceneNode>();
    sphere_node2->CreateComponent<ShadingComponent>(shader_); 
    sphere_node2->CreateComponent<RenderingComponent>(sphere_mesh_);
    sphere_node2->GetTransform().SetPosition(state_.positions[0]);
    this->AddChild(std::move(sphere_node2));
}

void SimpleSimulationNode::Update(double t) {
    // make for loop so t/ integration_step + leftover time
    // integrator factory returns the integrator 
    for (double j =0.0; j< t; j+= step_){
        std::unique_ptr<IntegratorBase<SimpleSystem, ParticleState> > i = IntegratorFactory::CreateIntegrator<SimpleSystem, ParticleState>(integrator_type_);
        ParticleState new_state_ = i->Integrate(system_, state_, 0.f, j);

        //override the old state with new state
        state_ = new_state_;

        //TODO: update the positions of everything
        particle_[0]->GetTransform().SetPosition(state_.positions[0]);
    }

    //when time = t
    std::unique_ptr<IntegratorBase<SimpleSystem, ParticleState> > i = IntegratorFactory::CreateIntegrator<SimpleSystem, ParticleState>(integrator_type_);
    ParticleState new_state_ = i->Integrate(system_, state_, 0.f, t);
    state_ = new_state_;
    particle_[0]->GetTransform().SetPosition(state_.positions[0]);

}
}  // namespace GLOO
