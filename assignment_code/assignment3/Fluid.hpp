#ifndef FLUID_H_
#define FLUID_H_

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "gloo/components/MaterialComponent.hpp"
// #include "Solver.hpp"
#include "Parameters.hpp"
#include <vector>


namespace GLOO {
class Fluid : public SceneNode {
private:
  // velocity grids
  std::vector<float> U0_y; //(num_cells, 0.f);
  std::vector<float> U0_x; //(num_cells, 0.f);
  std::vector<float> U1_y; //(num_cells, 0.f);
  std::vector<float> U1_x; //(num_cells, 0.f);

  // scalar grids - density values
  std::vector<float> S0; //(num_cells, 0.f);
  std::vector<float> S1; //(num_cells, 0.f);

  void swap_grids();

public:
  Fluid();
  void step();

  // setters
  void add_U_y_force_at(int y, int x, float force);
  void add_U_x_force_at(int y, int x, float force);
  void add_source_at(int y, int x, float source);

  // getters
  float Uy_at(int y, int x);
  float Ux_at(int y, int x);
  float S_at(int y, int x);

  // from solver
  void v_step(std::vector<float> U1_y, std::vector<float> U1_x, std::vector<float> U0_y, std::vector<float> U0_x);
  void s_step(std::vector<float> S1, std::vector<float> S0, std::vector<float> U_y, std::vector<float> U_x);

  void negate_field(std::vector<float> field){
    for (int i=0; i<num_cells; i++) {field[i] = -field[i];}
  }

  void set_boundary_values(std::vector<float> field, int key);

  void add_force(std::vector<float> field, std::vector<float> force, int key){
    for (int y = 1; y < CELLS_Y - 1; y++) {
        for (int x = 1; x < CELLS_X - 1; x++) {
            field[IndexOf(y, x)] += force[IndexOf(y, x)];
        }
    }
    set_boundary_values(field, key);
  }

  float lin_interp(float y, float x, std::vector<float> field){
    int yfloor = floor(y);
    int xfloor = floor(x);

    float ydiff = (y - 0.5f) - (float) yfloor;
    float xdiff = (x - 0.5f) - (float) xfloor;

    float tl = field[IndexOf(yfloor, xfloor)];
    float bl = field[IndexOf(yfloor + 1, xfloor)];
    float tr = field[IndexOf(yfloor, xfloor + 1)];
    float br = field[IndexOf(yfloor + 1, xfloor + 1)];

    float vl = (1.0f - ydiff) * tl + ydiff * bl;
    float vr = (1.0f - ydiff) * tr + ydiff * br;

    return (1.0f - xdiff) * vl + xdiff * vr;
  }

  void transport(std::vector<float> S1, std::vector<float> S0, std::vector<float> U_y, std::vector<float> U_x, int key){
    for (int y = 1; y < CELLS_Y - 1; y++) {
        for (int x = 1; x < CELLS_X - 1; x++) {
            // trace particle
            float y0 = ((float) y + 0.5f) - DT * U_y[IndexOf(y, x)];
            float x0 = ((float) x + 0.5f) - DT * U_x[IndexOf(y, x)];

            y0 = fmax(1.0f, fmin(((float) CELLS_Y) - 2.0f, y0));
            x0 = fmax(1.0f, fmin(((float) CELLS_X) - 2.0f, x0));

            S1[IndexOf(y, x)] = lin_interp(y0, x0, S0);
        }
    }
    set_boundary_values(S1, key);
  }

  void lin_solve(std::vector<float> S1, std::vector<float> S0, float a, float b, int key) {
    for (int i = 0; i < NUM_ITER; i++) {
        for (int y = 1; y < CELLS_Y - 1; y++) {
            for (int x = 1; x < CELLS_X - 1; x++) {
                S1[IndexOf(y, x)] = (S0[IndexOf(y, x)]
                        + a * (S1[IndexOf(y + 1, x)] + S1[IndexOf(y - 1, x)]
                             + S1[IndexOf(y, x + 1)] + S1[IndexOf(y, x - 1)])) / b;
            }
        }
        set_boundary_values(S1, key);
    }
  }

  void diffuse(std::vector<float> S1, std::vector<float> S0, float diff, int key) {
    float a = DT * diff * num_cells;
    lin_solve(S1, S0, a, 1.0f + 4.0f * a, key);
  }

  void project(std::vector<float> U1_y, std::vector<float> U1_x, std::vector<float> U0_y, std::vector<float> U0_x) {
      // construct initial guess for the solution
      std::vector<float> S(num_cells, 0.f);

      // compute the divergence of the velocity field
      std::vector<float> divergence(num_cells, 0.f);
      for (int y = 1; y < CELLS_Y - 1; y++) {
          for (int x = 1; x < CELLS_X - 1; x++) {
              divergence[IndexOf(y, x)] = U0_y[IndexOf(y + 1, x)] - U0_y[IndexOf(y - 1, x)]
                                      + U0_x[IndexOf(y, x + 1)] - U0_x[IndexOf(y, x - 1)];
          }
      }
      set_boundary_values(divergence, 0);

      // solve the Poisson equation
      negate_field(divergence);
      lin_solve(S, divergence, 1.0f, 4.0f, 0);

      // subtract the gradient from the previous solution
      for (int y = 1; y < CELLS_Y - 1; y++) {
          for (int x = 1; x < CELLS_X - 1; x++) {
              U1_y[IndexOf(y, x)] = U0_y[IndexOf(y, x)] - (S[IndexOf(y + 1, x)] - S[IndexOf(y - 1, x)]) / 2.0f;
              U1_x[IndexOf(y, x)] = U0_x[IndexOf(y, x)] - (S[IndexOf(y, x + 1)] - S[IndexOf(y, x - 1)]) / 2.0f;
          }
      }
      set_boundary_values(U1_y, 1);
      set_boundary_values(U1_x, 2);
  }

  void dissipate(std::vector<float> S1, std::vector<float> S0) {
      for (int i = 0; i < num_cells; i++) {
          S1[i] = S0[i] / (1.0f + DT * DISSIPATION);
      }
  }

  float curl(int y, int x, std::vector<float> U_y, std::vector<float> U_x) {
    return (U_y[IndexOf(y, x + 1)] - U_y[IndexOf(y, x - 1)]
            - U_x[IndexOf(y + 1, x)] + U_x[IndexOf(y - 1, x)]) / 2.0f;
  }

};
}  // namespace GLOO

#endif
