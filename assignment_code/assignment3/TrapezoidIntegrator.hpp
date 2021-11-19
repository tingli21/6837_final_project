#ifndef TRAPEZOID_INTEGRATOR_H_
#define TRAPEZOID_INTEGRATOR_H_

#include "IntegratorBase.hpp"

namespace GLOO {
template <class TSystem, class TState>
class TrapezoidIntegrator : public IntegratorBase<TSystem, TState> {
  TState Integrate(const TSystem& system,
                   const TState& state,
                   float start_time,
                   float dt) const override {
    // TODO: Here we are returning the state at time t (which is NOT what we
    // want). Please replace the line below by the state at time t + dt using
    // forward Euler integration.
    // TState = X()
    // TSystem = F()

    TState f0 = system.ComputeTimeDerivative(state, start_time);
    TState f1 = system.ComputeTimeDerivative(state + dt*f0, start_time + dt);
    return state + float(dt/2.0)*(f0+f1);;
  }
};
}  // namespace GLOO

#endif
