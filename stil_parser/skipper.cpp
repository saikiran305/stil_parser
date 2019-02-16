#include "skipper_def.hpp"

namespace client {
  namespace parser
  {
    BOOST_SPIRIT_INSTANTIATE(skipper_type, iterator_type, x3::unused_type)
  }
}
