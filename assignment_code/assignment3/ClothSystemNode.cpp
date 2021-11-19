#include <fstream>

#include "ClothSystem.hpp"
#include "ParticleState.hpp"
#include "ParticleSystemBase.hpp"
#include "IntegratorBase.hpp"
#include "IntegratorType.hpp"
#include "IntegratorFactory.hpp"
#include "Springs.hpp"
#include "ClothSystemNode.hpp"

#include "gloo/utils.hpp"
#include "gloo/MeshLoader.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/shaders/SimpleShader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/InputManager.hpp"
#include "glm/gtx/string_cast.hpp"

namespace GLOO {
ClothSystemNode::ClothSystemNode(IntegratorType integrator_type, float integration_step) {

    //initialize
    integrator_type_ = integrator_type;
    step_ = integration_step;
    num_particles_ = num_cols * num_rows;

    //build up the Pendulum system
    system_ = ClothSystem();
    system_.SetNumParticles(num_particles_);

    for (int i = 0; i < num_cols; i++){
        system_.FixPoint(i);
    }
    // system_.FixPoint(0); //fixes the first point
    // system_.FixPoint(1); 
    // system_.FixPoint(num_cols-2);
    // system_.FixPoint(num_cols-1);

    system_.SetK(25.f);
    system_.SetRestLength(0.5f);
    system_.SetDrag(5.f);

    std::unique_ptr<IndexArray> indices = make_unique<IndexArray>();

    //add horizontal structural springs to the system
    for (int r = 0; r < num_rows; r++){
        for (int c = 0; c < num_cols- 1; c++){
            // connects c and c+1
            int start = IndexOf(r,c);
            int end = IndexOf(r,c+1);
            system_.AddSpring(.5f, .2, start, end);

            indices->push_back(start);
            indices->push_back(end);
        }
    }

    //add vertical structural springs to the system
    for (int r = 0; r < num_rows-1; r++){
        for (int c = 0; c < num_cols; c++){
            // connects r and r+1
            int start = IndexOf(r,c);
            int end = IndexOf(r+1,c);
            system_.AddSpring(.5f, .2, start, end);

            indices->push_back(start);
            indices->push_back(end);
        }
    }

    //add shear structural springs y=-x
    for (int r = 0; r < num_rows - 1; r++){
        for (int c = 0; c < num_cols - 1; c++){
            // connects (r,c) and (r+1, c+1)
            int start = IndexOf(r,c);
            int end = IndexOf(r+1,c+1);
            system_.AddSpring(.5f, .2, start, end);

            indices->push_back(start);
            indices->push_back(end);
        }
    }

    //add shear structural springs y=x
    for (int r = 1; r < num_rows; r++){
        for (int c = 0; c < num_cols - 1; c++){
            // connects (r,c) and (r-1, c+1)
            int start = IndexOf(r,c);
            int end = IndexOf(r-1,c+1);
            system_.AddSpring(.5f, .2, start, end);

            indices->push_back(start);
            indices->push_back(end);
        }
    }

    //add horizontal flex springs 
    for (int r = 0; r < num_rows; r++){
        for (int c = 0; c < num_cols - 2; c++){
            // connects (r,c) and (r, c+2)
            int start = IndexOf(r,c);
            int end = IndexOf(r,c+2);
            system_.AddSpring(.5f, .2, start, end);

            indices->push_back(start);
            indices->push_back(end);
        }
    }

    //add vertical flex springs
    for (int r = 0; r < num_rows - 2; r++){
        for (int c = 0; c < num_cols; c++){
            // connects (r,c) and (r+2, c)
            int start = IndexOf(r,c);
            int end = IndexOf(r+2,c);
            system_.AddSpring(.5f, .2, start, end);

            indices->push_back(start);
            indices->push_back(end);
        }
    }

    //create initial particle state
    for (int r = 0; r < num_rows; r++){
        for (int c = 0; c < num_cols; c++){
            state_.positions.push_back(glm::vec3(c, -r, 0.5f));
        }
    }

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

int ClothSystemNode::IndexOf(int row, int col) {
    //returns the index of the particle at position (row, col)
    return num_cols*row + col; 
}

void ClothSystemNode::CreateParticle(glm::vec3 pos) {
    std::unique_ptr<SceneNode> sphere_node = make_unique<SceneNode>();
    sphere_node->CreateComponent<ShadingComponent>(shader_); 
    sphere_node->CreateComponent<RenderingComponent>(sphere_mesh_);
    sphere_node->GetTransform().SetPosition(pos);
    particle_.push_back(sphere_node.get());
    this->AddChild(std::move(sphere_node)); 
}

void ClothSystemNode::PlotOriginalPoints(){

    ParticleState new_state;

    //reset positions
    for (int r = 0; r < num_rows; r++){
        for (int c = 0; c < num_cols; c++){
            new_state.positions.push_back(glm::vec3(c, -r, 0.5f));
        }
    }

    //reset velocities
    for (int i=0; i < num_particles_; i++){
        new_state.velocities.push_back(glm::vec3(0.f, 0.f, 0.f)); 
    }
    state_ = new_state;

    std::unique_ptr<PositionArray> new_positions = make_unique<PositionArray>();
    for (int k=0; k < num_particles_; k++){
        //update particle positions
        particle_[k]->GetTransform().SetPosition(state_.positions[k]);

        //update the spring's position array
        new_positions->push_back(state_.positions[k]);
    }
    
    spring_mesh_->UpdatePositions(std::move(new_positions)); //TODO: SEG FAULT
}

void ClothSystemNode::Update(double t) {

    if (InputManager::GetInstance().IsKeyPressed('R')) {
        if (prev_released1) {
            //reset the scene
            PlotOriginalPoints();
        
        }
        prev_released1 = false;
    }
    else {
    prev_released1 = true;
    }

    if (InputManager::GetInstance().IsKeyPressed('W')) {
        if (prev_released2) {
            //reset the scene
            system_.ToggleWind();
        
        }
        prev_released2 = false;
    }
    else {
    prev_released2 = true;
    }

    //make for loop so t/ integration_step + leftover time
    for (double j =0.0; j< t; j+= step_){
        std::unique_ptr<PositionArray> new_positions = make_unique<PositionArray>();

        std::unique_ptr<IntegratorBase<ClothSystem, ParticleState> > i = IntegratorFactory::CreateIntegrator<ClothSystem, ParticleState>(integrator_type_);
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
    
        spring_mesh_->UpdatePositions(std::move(new_positions)); //TODO: SEG FAULT

    }

    // //when time = t
    std::unique_ptr<PositionArray> new_positions = make_unique<PositionArray>();
    std::unique_ptr<IntegratorBase<ClothSystem, ParticleState> > i = IntegratorFactory::CreateIntegrator<ClothSystem, ParticleState>(integrator_type_);
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
