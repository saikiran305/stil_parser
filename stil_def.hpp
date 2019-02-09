#ifndef STIL_DEF_HPP
#define STIL_DEF_HPP

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>
#include "ast.hpp"
#include "ast_adapted.hpp"
#include "stil.hpp"
#include "common.hpp"
#include "error_handler.hpp"
#include "config.hpp"

namespace client {
namespace parser {

using x3::raw;
using x3::lexeme;
using x3::lit;
using namespace x3::ascii;

x3::symbols<char> keywords;
x3::symbols<ast::signal_type> signal_type;

// Helper to add keywords once
void add_keywords() {
    keywords.add
  ("STIL")
  ("Signals")
  ("SignalGroups")
  ("In")
  ("Out")
  ("InOut")
  ("Supply")
  ("Pseudo")
  ;

    signal_type.add
  ("In",client::ast::in)
  ("Out", client::ast::out)
  ("InOut", client::ast::inout)
  ("Supply", client::ast::supply)
  ("Pseudo", client::ast::pseudo)
  ;

}

struct stil_session_class;
struct signal_item_class;
struct group_item_class;

x3::rule<class version_class, std::string> const version = "version";

//Session
x3::rule<class session_class, ast::session> const session = "session";
x3::rule<class block_class, ast::block> const block = "block";

//Signals
x3::rule<class signal_name_class, std::string> const signal_name = "signal_name";
x3::rule<signal_item_class, ast::signal> const signal_item = "signal_item";
x3::rule<class signals, ast::signals> const signals = "signals";

//Group
x3::rule<class group_name_class, std::string> const group_name = "group_name";
x3::rule<group_item_class, ast::group> const group_item = "group_item";
x3::rule<class groups, ast::groups> const groups = "groups";


x3::rule<stil_session_class, ast::stil_ast> const stil_session = "stil_session";

//Grammar Start
auto const version_def =
        lit("STIL")
    > +(x3::digit) > x3::char_(".") > +(x3::digit)
    >  lit(";");

// Signal Grammar
auto const signal_name_def =
  !keywords >> identifier >> x3::char_("[") > +(x3::digit) > x3::char_("]")
  | !keywords >> identifier
  ;

auto const signal_item_def = signal_name > signal_type > lit(";");
auto const signals_def = lit("Signals")
  > "{"
  >> *signal_item
  > "}";

//Groups Grammar
auto const group_name_def = !keywords >> identifier;
auto const group_item_def = group_name >
        '='
        >
        group_name % '+'
        > ';'
        ;
auto const groups_def = lit("SignalGroups")
        > "{"
        >> *group_item
        > "}";

auto const block_def =
        //x3::eps
        signals
       | groups
                ;
auto const session_def = *block;
auto const stil_session_def = version
        > session;

BOOST_SPIRIT_DEFINE(
        version,
        signal_name,
        signal_item,
        signals,
        group_name,
        group_item,
        groups,
        block,
        session,
        stil_session
        )
struct stil_session_class : error_handler_base, x3::annotate_on_success {};
struct signal_item_class : x3::annotate_on_success {};
struct group_item_class : x3::annotate_on_success {};
}
}

namespace client
{
  parser::stil_session_type const& stil_session()
  {
    parser::add_keywords();
    return parser::stil_session;
  }
}

#endif // STIL_DEF_HPP
