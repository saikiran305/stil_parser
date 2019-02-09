#include "compiler.hpp"
#include "ast.hpp"

namespace client {
namespace vecgen {
bool compiler::find_signal(std::string const &x){
        auto e = signals.find(x);
    if (e == signals.end()) return  false;
    return true;
}
void compiler::add_signal(ast::signal const &x)
{
    signals[x.name] = x.type;
}
bool compiler::operator()(ast::signal const &x) {
        if (find_signal(x.name)) {
        error_handler(x, "Signal Redeclared: " +x.name);
        return false;
    }
    add_signal(x);
    return true;
}
bool compiler::operator()(const ast::signals &x) {

        for (auto const& s:x)
    {
        if (!(*this)(s)) return  false;
    }
    return true;
}
bool compiler::start(const ast::session &x)
{
    for (auto const& s:x)
        {

            if (!(boost::apply_visitor(*this,s))) return  false;
        }
    return true;
}

void compiler::print_signals() const
{
  for(auto v : signals) std::cout << v.first << ":" << v.second << std::endl;
}
}
}
