#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <boost/spirit/home/x3.hpp>
#include "error_handler.hpp"
#include "skipper.hpp"
namespace client { namespace parser
  {
    typedef std::string::const_iterator iterator_type;
    // typedef x3::phrase_parse_context<x3::ascii::space_type>::type phrase_context_type;
    typedef x3::phrase_parse_context<skipper_type>::type phrase_context_type;
    typedef error_handler<iterator_type> error_handler_type;

    typedef x3::context<
      error_handler_tag
      ,std::reference_wrapper<error_handler_type> const
      ,phrase_context_type>
    context_type;
  }

}

#endif // CONFIG_HPP
