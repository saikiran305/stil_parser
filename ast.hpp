#ifndef AST_HPP
#define AST_HPP

#include <boost/config/warning_disable.hpp>
#include <boost/variant/recursive_variant.hpp>

#include <boost/fusion/include/io.hpp>
#include <boost/optional.hpp>

#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <string>
#include <list>
#include <iostream>

namespace client {
    namespace ast {

    namespace x3 = boost::spirit::x3;

    struct nil {};

    // STIL SIGNALS Begin
    enum signal_type
      {
       in,
       out,
       inout,
       supply,
       pseudo
      };
    enum optoken
    {
        op_plus,
        op_minus
    };

    struct signal : x3::position_tagged {
      signal(
         std::string const& name = "",
         signal_type const& type = in
         )
    : name(name) , type(type) {}
      std::string name;
      signal_type type;

    };


    struct signals : std::list<signal> {};
    //  STIL Signals End
    // Groups
    struct group : x3::position_tagged {
/*
        group() {}
      group(std::string const& name , std::vector<std::string> const& signals)
    :name(name), signals(signals) {}
*/
        //typedef std::string value_type;
      std::string name;
      std::list<std::string> items;
    };
    struct groups : std::list<group> {};

    // Block
    struct block : x3::variant <
            nil,
            signals,
            groups
            >
    {
        using base_type::base_type;
        using base_type::operator=;
    };
    // session
    struct session : std::list<block> {};


    struct stil_ast : x3::position_tagged
    {
        std::string version;
        session  blocks;
    };

    inline std::ostream& operator<<(std::ostream& out, nil const& var)
    {
      out << "Signal: " << "nil" << std::endl;
      return out;
    }

    inline std::ostream& operator<<(std::ostream& out, signal const& var)
    {
      out << "Signal: " << var.name <<":"<< var.type << std::endl;
      return out;
    }
    inline std::ostream& operator<<(std::ostream& out, signals const& var)
    {
      for (auto v: var) out << v ;
      return out;
    }
    inline std::ostream& operator<<(std::ostream& out, group const& var)
        {
          out << "Group: " << var.name <<":";
          //for (auto v: var.signals) out << v;
          //out << "\n";
          return out;
        }

    inline std::ostream& operator<<(std::ostream& out, groups const& var)
        {
          for (auto v: var) out << v ;
          return out;
        }

    struct Visitor {
        void operator()(signals const& var)
        {
            std::cout << "Printing Signals \n";
            for (auto v: var) std::cout << v;
        }
        void operator()(signal const& var)
        {
            std::cout << "Printing Signal \n";
            std::cout << var;
        }
        void operator()(nil const& var)
        {
            std::cout << "nil";
        }

    };
}
}
#endif // AST_HPP
