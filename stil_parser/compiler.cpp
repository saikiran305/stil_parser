#include "compiler.hpp"
#include "ast.hpp"
#include <algorithm>
#include <vector>

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
            cur_vec[x.name] = "X";
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
        
        bool compiler::operator()(const ast::timing &x)
        {
            for (auto const& s:x.wavetables)
                if (!(*this)(s)) return  false;
            return true;
            
        }
        
        bool compiler::operator()(const ast::wavetable &x)
        {
            wavetables[x.name] = x;
            for (auto const& s:x.sig_events)
                if (!(*this)(s)) return  false;
            std::sort(sampletimes.begin(),
                      sampletimes.end());
            wavetable_sampletimes[x.name] = sampletimes;
            sampletimes.clear();
            cur_wft = x.name;
            return true;
        }
        
        bool compiler::operator()(const ast::time_event &x)
        {
            if( !(std::find(sampletimes.begin(), sampletimes.end(), x.first) != sampletimes.end()))
                sampletimes.push_back(x.first);
            return  true;
        }
        
        bool compiler::operator()(const ast::sig_tim_event &x)
        {
            for (auto const& s:x.events)
                if (!(*this)(s)) return  false;
            return  true;
        }
        
        bool compiler::start(const ast::session &x)
        {
            fout.open(outfilename);
            std::cout << "Opening outfile " << outfilename << std::endl;
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
                for(auto j:v.second) std::cout << j << " ";
                std::cout << std::endl;
            }
            std::cout << "===================\n";
            
        }
        
        void compiler::print_wavetables() const {
            std::cout << "======Wavetables======\n";
            for(auto t: wavetables) {
                std::cout << "Wavetable: " << t.first
                << std::endl ;
                std::cout << "\t Period : " << t.second.period
                << std::endl;
                std::cout << "\t Signals : ";
                for (auto sig: t.second.sig_events)
                    std::cout << sig.name <<  " " ;
                std::cout << "\n===================\n";
                
            }
            for(auto t:wavetable_sampletimes) {
                std::cout << "Wavetable : " << t.first
                << std::endl;
                std::cout << "Sample Times : ";
                for (auto v:t.second) std::cout << v << " ";
                std::cout << std::endl;
            }
        }
        
        bool compiler::operator()(const ast::patburst &x) {
            return true;
        }
        
        bool compiler::operator()(const ast::patexec &x) {
            write_header();
            return true;
        }
        
        bool compiler::operator()(const ast::vec_stmt &x) {
            for (auto const& s:x)
                if (!(*this)(s)) return  false;
            return true;
        }
        
        
        bool compiler::operator()(const ast::cur_wft &x) {
            cur_wft = x;
            return true;
        }
        
        bool compiler::operator()(const ast::pattern &x) {
            for (auto const& s:x.pats)
                if (!(boost::apply_visitor(*this,s))) return  false;
            return true;
        }
        
        bool compiler::operator()(const ast::vec_data &x) {
            std::cout << "Executing vec data \n";
            /*
            if (find_group(x.name))
            {
                std::cout << "found group: "
                << x.name << std::endl;
                for (auto const& t : wavetable_sampletimes[cur_wft])
                {
                    for(int i = 0; i < groups[x.name].size(); ++i)
                    {
                        
                        auto sig = groups[x.name][i];
                        auto val_pair = wavetables[cur_wft].getvalue(sig, t, x.value[i]);
                        std::cout << "Result : "
                        << val_pair.first << " "
                        << val_pair.second
                        << std::endl;
                        if (val_pair.first)
                            cur_vec[sig] = val_pair.second;
                        write_vec(t);
                    }
                }
            }
             */
            return true;
        }
        
        void compiler::write_vec(int t) {
            fout << t << " ";
            for (auto const& elem:cur_vec)
                fout << elem.second;
        }
        
        void compiler::write_header() {
            for (auto const& elem:cur_vec)
                fout << elem.first << " ";
            fout << std::endl;
        }
        
        
        
        
        
        
        
        
        
    }
    
}
