#include <fstream>

#include "PendulumSystem.hpp"
#include "ParticleState.hpp"
#include "ParticleSystemBase.hpp"
#include "IntegratorBase.hpp"
#include "IntegratorType.hpp"
#include "IntegratorFactory.hpp"
#include "Springs.hpp"
#include "PendulumSystemNode.hpp"

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
PendulumSystemNode::PendulumSystemNode(IntegratorType integrator_type, float integration_step) {

    //initialize
    integrator_type_ = integrator_type;
    step_ = integration_step;

    //build up the Pendulum system
    system_ = PendulumSystem();
    system_.SetNumParticles(num_particles_);
    system_.FixPoint(0); //fixes the first point
    system_.SetK(10.f);
    system_.SetRestLength(0.3f);
    system_.SetDrag(0.5f);

    system_.AddSpring(.5f, .2, 0, 1);
    system_.AddSpring(.5f, .2, 1, 2);
    system_.AddSpring(.5f, .2, 2, 3); 

    //create initial particle state
    state_.positions.push_back(glm::vec3(.1f, .1f, 0.5f));
    state_.positions.push_back(glm::vec3(.2f, .2f, 0.5f));
    state_.positions.push_back(glm::vec3(.3f, .1f, 0.5f));
    state_.positions.push_back(glm::vec3(.4f, .2f, 0.5f));

    for (int i=0; i < num_particles_; i++){
        state_.velocities.push_back(glm::vec3(0.f, 0.f, 0.f)); 
    }

    // make shader and mesh
    shader_ = std::make_shared<SimpleShader>();
    sphere_mesh_ = PrimitiveFactory::CreateSphere(0.02f, 25, 25);
    spring_mesh_ = std::make_shared<VertexObject>();

    //create the springs
    std::unique_ptr<SceneNode> springs_node = make_unique<SceneNode>();
    springs_node->CreateComponent<ShadingComponent>(shader_); 

    std::unique_ptr<PositionArray> positions = make_unique<PositionArray>();
    for (int i=0; i< num_particles_; i++){
        positions->push_back(state_.positions[i]);
    }


    std::unique_ptr<IndexArray> indices = make_unique<IndexArray>();
    indices->push_back(0);
    indices->push_back(1);
    indices->push_back(1);
    indices->push_back(2);
    indices->push_back(2);
    indices->push_back(3);

    spring_mesh_->UpdatePositions(std::move(positions));
    spring_mesh_->UpdateIndices(std::move(indices));
    
    auto& rc = springs_node->CreateComponent<RenderingComponent>(spring_mesh_);
    rc.SetDrawMode(DrawMode::Lines);

    this->AddChild(std::move(springs_node)); 

    //setup a sphere scene node and spring scene node
    for (int i=0; i < num_particles_; i++){
        CreateParticle(state_.positions[i]);
    }
}

void PendulumSystemNode::CreateParticle(glm::vec3 pos) {
    std::unique_ptr<SceneNode> sphere_node = make_unique<SceneNode>();
    sphere_node->CreateComponent<ShadingComponent>(shader_); 
    sphere_node->CreateComponent<RenderingComponent>(sphere_mesh_);
    sphere_node->GetTransform().SetPosition(pos);
    particle_.push_back(sphere_node.get());
    this->AddChild(std::move(sphere_node)); 
}

void PendulumSystemNode::Update(double t) {

    // // make for loop so t/ integration_step + leftover time
    for (double j =0.0; j< t; j+= step_){
        std::unique_ptr<PositionArray> new_positions = make_unique<PositionArray>();

        std::unique_ptr<IntegratorBase<PendulumSystem, ParticleState> > i = IntegratorFactory::CreateIntegrator<PendulumSystem, ParticleState>(integrator_type_);
        ParticleState new_state_ = i->Integrate(system_, state_, 0.f, j); //TODO: what should start time be

        //override the old state with new state
        state_ = new_state_;
        printf("new size: %i\n", state_.positions.size());
        printf("particle_: %i\n", particle_.size());
        printf("num particles: %i\n", num_particles_);

        // //update the positions of particles and spring positions
        for (int k=0; k < num_particles_; k++){
            //update particle positions
            particle_[k]->GetTransform().SetPosition(state_.positions[k]);

            //update the spring's position array
            new_positions->push_back(state_.positions[k]);
        }
    
        spring_mesh_->UpdatePositions(std::move(new_positions)); 

    }

    // //when time = t
    std::unique_ptr<PositionArray> new_positions = make_unique<PositionArray>();
    std::unique_ptr<IntegratorBase<PendulumSystem, ParticleState> > i = IntegratorFactory::CreateIntegrator<PendulumSystem, ParticleState>(integrator_type_);
    ParticleState new_state_ = i->Integrate(system_, state_, 0.f, t);
    state_ = new_state_;
    for (int k=0; k < num_particles_; k++){
        //update particle positions
        particle_[k]->GetTransform().SetPosition(state_.positions[k]);

        //update the spring's position array
        new_positions->push_back(state_.positions[k]);
    }

    spring_mesh_->UpdatePositions(std::move(new_positions));

}

}  // namespace GLOO
