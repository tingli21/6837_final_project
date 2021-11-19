#ifndef RK4_INTEGRATOR_H_
#define RK4_INTEGRATOR_H_

#include "IntegratorBase.hpp"

namespace GLOO {
template <class TSystem, class TState>
class RK4Integrator : public IntegratorBase<TSystem, TState> {
  TState Integrate(const TSystem& system,
                   const TState& state,
                   float start_time,
                   float dt) const override {
    
    TState K1 = system.ComputeTimeDerivative(state, start_time);
    TState K2 = system.ComputeTimeDerivative(state + (dt/2)*K1, start_time + dt/2);
    TState K3 = system.ComputeTimeDerivative(state + (dt/2)*K2, start_time + dt/2);
    TState K4 = system.ComputeTimeDerivative(state + (dt)*K3, start_time + dt);

    return state + (dt/6)*(K1 + 2*K2 + 2*K3 + K4);
  }
};
}  // namespace GLOO

#endif
