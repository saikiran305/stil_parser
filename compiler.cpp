#include "compiler.hpp"
#include "ast.hpp"

namespace client {
namespace vecgen {
bool compiler::find_signal(std::string const &x){
        auto e = signals.find(x);
    if (e == signals.end()) return  false;
    return true;
}

bool compiler::find_group(const std::string &x)
{
    auto e = groups.find(x);
    if (e == groups.end()) return false;
    return true;
}

void compiler::add_group(const std::string &x, const std::string &y)
{
    groups[x].push_back(y);
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
bool compiler::operator()(const ast::groups &x)
{
    for (auto const& s:x)
    {
        if (!(*this)(s)) return  false;
    }

    return true;
}

bool compiler::operator()(const ast::group &x)
{
    for (auto const& s:x.items)
    {
        if (!find_group(s)) {

            if (!find_signal(s))
            {
                error_handler(x, "Signal Undeclared: " +s);
                 return false;
            }
            add_group(x.name,s);
        }
        else
        {
            for (auto j:groups[s]) add_group(x.name, j);
        }
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
    std::cout << "=====Signals=======\n";
  for(auto v : signals) std::cout << v.first << ":" << v.second << std::endl;
    std::cout << "===================\n";
}
void compiler::print_groups() const
{
    std::cout << "=====Groups=======\n";
    for(auto v : groups) {
        std::cout << "Group: " << v.first  << ":" ;
        for(auto j:v.second) std::cout << j ;
        std::cout << std::endl;
    }
    std::cout << "===================\n";

}

}

}
