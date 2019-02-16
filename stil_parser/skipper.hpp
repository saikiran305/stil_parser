#ifndef SKIPPER_HPP
#define SKIPPER_HPP

#include <boost/spirit/home/x3.hpp>



namespace client {
  namespace x3 = boost::spirit::x3;
  namespace parser {


    // struct skipper_class;
    using skipper_type = x3::rule<struct skipper_class, const x3::unused_type>;
    BOOST_SPIRIT_DECLARE(skipper_type)

    const skipper_type& getSkipper();

  }
  parser::skipper_type const& skipper();
}


#endif // SKIPPER_HPP
