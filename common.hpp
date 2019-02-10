#ifndef COMMON_HPP
#define COMMON_HPP
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/spirit/home/x3.hpp>

namespace client {
  namespace parser {
  namespace  x3 = boost::spirit::x3;

    using x3::raw;
    using x3::lexeme;
    using x3::alpha;
    using x3::alnum;
    using x3::lit;

    struct identifier_class;
    typedef x3::rule<identifier_class, std::string> identifier_type;
    x3::rule<class quote_class> const quote = "quote";
    identifier_type const identifier = "identifier";

    auto const identifier_def = raw[lexeme[(alpha | '_') >> *(alnum | '_')]];
    auto const quote_def = lit("'") | lit('"');
    BOOST_SPIRIT_DEFINE(identifier);
    BOOST_SPIRIT_DEFINE(quote)
  }

}

#endif // COMMON_HPP
