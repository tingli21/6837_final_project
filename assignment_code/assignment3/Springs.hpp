#ifndef SPRINGS_H_
#define SPRINGS_H_

#include <vector>
#include <stdexcept>

#include <glm/glm.hpp>

namespace GLOO {
struct Springs {
  // The state of a particle system: positions and velocities.
  float k;
  float rest_length;
  //two particles it connects to
  int start;
  int end;

};
}  // namespace GLOO

#endif
