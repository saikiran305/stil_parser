#ifndef COMPILER_HPP
#define COMPILER_HPP
#include "ast.hpp"
#include "error_handler.hpp"

#include <map>
#include <vector>

namespace client {
namespace vecgen {
namespace  x3 = boost::spirit::x3;

    class compiler
    {
    public:
        typedef  bool result_type;
        typedef  std::function<
        void(x3::position_tagged, std::string const &)>
        error_handler_type;

        template<typename ErrorHandler>
        compiler(
                ErrorHandler const& error_handler
                ) : error_handler(
                        [&](x3::position_tagged pos, std::string const& msg)
                        {error_handler(pos,msg);}
                        )
    {}

    bool operator()(ast::nil) const {BOOST_ASSERT(0); return false;}
    bool operator()(ast::signals const& x)  ;
    bool operator()(ast::signal const& x)  ;
    bool start(ast::session const& x) ;
    void print_signals() const;

    private:
    std::map<std::string, ast::signal_type> signals;
    error_handler_type error_handler;

    void add_signal(ast::signal const& x) ;
    bool find_signal(std::string const& x);
    };
}
}
#endif // COMPILER_HPP
