#include "stil_def.hpp"
//#include "config.hpp"



namespace client {
  namespace parser {
    BOOST_SPIRIT_INSTANTIATE(stil_session_type, iterator_type, context_type);
  }
}
