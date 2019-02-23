#include "compiler.hpp"
#include "ast.hpp"
#include <algorithm>
#include <vector>
#include <string>
#include <ctype.h>
#include <set>
namespace client {
    namespace vecgen {
        
        void sigtiming::add_events(std::list<ast::time_event> const& events, std::string const& values)
        {
            for (auto const& event:events)
            {
                sampletimes.push_back(event.first);
                for (int i = 0; i < values.size(); ++i) {
                    if (values.size() == event.second.size())
                        vec_values[event.first][values.at(i)] = event.second.at(i);
                    else vec_values[event.first][values.at(i)] = event.second.at(0);
                }
                
            }
        }
        
        sigtiming::sigtiming(ast::sig_tim_event const& x)
        {
            std::cout << "Construct with sig_tim_event \n";
            add_events(x.events, x.values);
            std::sort(sampletimes.begin(),
                      sampletimes.end());
        }
        
        sigtiming::sigtiming(std::list<ast::time_event> const& events, std::string const& values)
        {
            add_events(events, values);
        }
        
        
        void sigtiming::print()
        {
            std::cout << "Sample Times: ";
            for (auto const& item:vec_values)
                std::cout << item.first << " ";
            std::cout << std::endl;
            for (auto const& item:vec_values) {
                std::cout << "time: " << item.first << "ns :: ";
                for (auto const& val:item.second)
                    std::cout << val.first << " : "
                    << val.second << ", ";
                std::cout << std::endl;
            }
        }
        
        void sigtiming::get_value(int t, char value, bool& is_valid, char& out) {
            if (std::binary_search(sampletimes.begin(), sampletimes.end(), t))
            {
                if ( vec_values[t].find(value) == vec_values[t].end() ) {
                    // not found
                    is_valid = false;
                } else {
                    // found
                    is_valid = true;
                    out = vec_values[t][value];
                    //std:: cout << vec_values[t][value];
                }
                
            }
            else {
                is_valid = false;
            }
        }
        
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
            work_vec[x.name] = 'X';
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
        
        void compiler::add_sigtiming(std::string const& name, std::list<ast::time_event> const& events, std::string const& values)
        {
            if ( wavetables[cur_wft].sigtimings.find(name) == wavetables[cur_wft].sigtimings.end() ) {
                // not found
                wavetables[cur_wft].sigtimings[name] = sigtiming(events, values);
            } else {
                // found
                wavetables[cur_wft].sigtimings[name].add_events(events, values);
            }
        }
        
        bool compiler::operator()(const ast::wavetable &x)
        {
            cur_wft = x.name;
            wavetables[x.name] = wavetable();
            wavetables[x.name].period = x.period;
            
            std::cout << "Wavetable : " << x.name << std::endl;
            
            for (auto const& s:x.sig_events) {
                
                if (find_group(s.name))
                {
                    for (auto const& sig:groups[s.name]) {
                        
                        add_sigtiming(sig, s.events, s.values);
                        //wavetables[x.name].sigtimings[sig] = sigtiming(s.events, s.values);
                        std::cout << "Signal : " << sig << std::endl;
                        wavetables[x.name].sigtimings[sig].print();
                    }
                }
                else   {
                    std::cout << "Adding "
                    << s.name << " to table : " << x.name
                    << std::endl;
                    
                    add_sigtiming(s.name, s.events, s.values);
                    
                    //wavetables[x.name].sigtimings[s.name] = sigtiming(s);
                    wavetables[x.name].sigtimings[s.name].print();
                }
                if (!(*this)(s)) return  false;
            }
            std::sort(sampletimes.begin(),
                      sampletimes.end());
            wavetable_sampletimes[x.name] = sampletimes;
            sampletimes.clear();
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
                //for (auto sig: t.second.sig_events)
                //  std::cout << sig.name <<  " " ;
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
            write_vec();
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
        
        void compiler::vec_proc(std::string const& sig, char val)
        {
            bool valid = false;
            char out;
            pre_vec[sig] = val;
            for (auto const& t : wavetable_sampletimes[cur_wft])
            {
                wavetables[cur_wft].sigtimings[sig].get_value(t, val, valid, out);
                if (valid)
                {
                    sampletimes.push_back(t);
                    cur_vec[t][sig] = val;
                }
            }
        }
        
        bool compiler::operator()(const ast::vec_data &x) {
            
            if (find_group(x.name))
            {
                for(int i = 0; i < groups[x.name].size(); ++i)
                {
                    auto sig = groups[x.name][i];
                    vec_proc(sig, x.value.at(i));
                }
            } else vec_proc(x.name, x.value.at(0));
            //write_vec(t);
            return true;
        }
        
        void compiler::write_vec() {
            //std::sort(sampletimes.begin(),
              //        sampletimes.end());
            std::set<int> s( sampletimes.begin(), sampletimes.end() );
            sampletimes.assign( s.begin(), s.end() );
            for (auto const& t:sampletimes) {
                fout << t << " ";
                for (auto const& elem:work_vec) {
                    auto const& val = cur_vec[t][elem.first];
                    if (val) {
                        fout << val;
                        work_vec[elem.first] = val;
                    } else fout << work_vec[elem.first];
                    
                }
                fout << std::endl;
            }
            
        }
        
        void compiler::write_header() {
            for (auto const& elem:work_vec)
                fout << elem.first << " ";
            fout << std::endl;
        }
        
        bool compiler::operator()(const ast::procs &x) { 
            return true;
        }
        
        bool compiler::operator()(const ast::macros &x) { 
            return true;
        }
        
        bool compiler::operator()(const ast::macro_call &x) {
            return true;
        }
        bool compiler::operator()(const ast::proc_call &x) {
            return true;
        }
        
        bool compiler::operator()(const ast::cond_stmt &x) { 
            return true;
        }
        
        
        
        
    }
    
}
