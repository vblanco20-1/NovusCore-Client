#include <glm/glm.hpp>

typedef uint32_t uint;

namespace AngelScriptIntegration {

#define USING_VEC(x)\
  using x##2; \
  using x##3; \
  using x##4;

USING_VEC(glm::vec)
USING_VEC(glm::bvec)
USING_VEC(glm::ivec)
USING_VEC(glm::uvec)
USING_VEC(glm::dvec)

#undef USING_VEC

} // namespace AngelScriptIntegration
