#ifndef STIL_HPP
#define STIL_HPP

#include <boost/spirit/home/x3.hpp>
#include "ast.hpp"
#include "error_handler.hpp"

namespace client
{
  namespace x3 = boost::spirit::x3;
  namespace parser
  {
    struct stil_session_class ;
    typedef x3::rule<stil_session_class, ast::stil_ast> stil_session_type;
    typedef stil_session_type::id stil_session_id;
    BOOST_SPIRIT_DECLARE(stil_session_type);
  }
  parser::stil_session_type const& stil_session();
}


#endif // STIL_HPP
