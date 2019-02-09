#ifndef SKIPPER_DEF_HPP
#define SKIPPER_DEF_HPP

#include "config.hpp"
#include "skipper.hpp"
#include <boost/spirit/home/x3.hpp>

namespace client
{
  namespace parser
  {
    namespace {
    namespace x3 = boost::spirit::x3;
      skipper_type const skipper = "skipper";
      auto const skipper_def =
    x3::ascii::space
    | (x3::lit("/*") > x3::seek[x3::lit("*/")])
    | (x3::lit("//") > x3::seek[x3::eol | x3::eoi])

    ;
    }
    BOOST_SPIRIT_DEFINE(skipper)
    const skipper_type& getSkipper()
    {
      return skipper;
    }

  }
}

#endif // SKIPPER_DEF_HPP
