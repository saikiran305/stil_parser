#include "vecgen.hpp"
#include "ast.hpp"
#include <algorithm>
#include <vector>
#include <string>
#include <ctype.h>
#include <set>
namespace client {
    namespace patgen {
        
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
            std::set<int> s( sampletimes.begin(), sampletimes.end() );
            sampletimes.assign( s.begin(), s.end() );
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
        
        bool vecgen::find_signal(std::string const &x){
            auto e = signals.find(x);
            if (e == signals.end()) return  false;
            return true;
        }
        
        bool vecgen::find_group(const std::string &x)
        {
            auto e = groups.find(x);
            if (e == groups.end()) return false;
            return true;
        }
        
        void vecgen::add_group(const std::string &x, const std::string &y)
        {
            groups[x].push_back(y);
        }
        void vecgen::add_signal(ast::signal const &x)
        {
            signals[x.name] = x.type;
            work_vec[x.name] = 'X';
        }
        bool vecgen::operator()(ast::signal const &x) {
            if (find_signal(x.name)) {
                error_handler(x, "Signal Redeclared: " +x.name);
                return false;
            }
            add_signal(x);
            return true;
        }
        bool vecgen::operator()(const ast::signals &x) {
            
            for (auto const& s:x)
            {
                if (!(*this)(s)) return  false;
            }
            return true;
        }
        bool vecgen::operator()(const ast::groups &x)
        {
            for (auto const& s:x)
            {
                if (!(*this)(s)) return  false;
            }
            
            return true;
        }
        
        bool vecgen::operator()(const ast::group &x)
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
        
        bool vecgen::operator()(const ast::timing &x)
        {
            for (auto const& s:x.wavetables)
                if (!(*this)(s)) return  false;
            return true;
            
        }
        
        void vecgen::add_sigtiming(std::string const& name, std::list<ast::time_event> const& events, std::string const& values)
        {
            if ( wavetables[cur_wft].sigtimings.find(name) == wavetables[cur_wft].sigtimings.end() ) {
                // not found
                wavetables[cur_wft].sigtimings[name] = sigtiming(events, values);
            } else {
                // found
                wavetables[cur_wft].sigtimings[name].add_events(events, values);
            }
        }
        
        bool vecgen::operator()(const ast::wavetable &x)
        {
            cur_wft = x.name;
            wavetables[x.name] = wavetable();
            wavetables[x.name].period = x.period;
            
            for (auto const& elem:x.sig_events) {
                for (auto const& s:elem.sig_event_list) {
                    
                    if (find_group(elem.name))
                    {
                        for (auto const& sig:groups[elem.name]) {
                            
                            add_sigtiming(sig, s.events, s.values);
                        }
                    }
                    else   {
                        
                        add_sigtiming(elem.name, s.events, s.values);
                        
                        
                    }
                    if (!(*this)(s)) return  false;
                }
            }
            std::sort(sampletimes.begin(),
                      sampletimes.end());
            wavetable_sampletimes[x.name] = sampletimes;
            sampletimes.clear();
            return true;
        }
        
        bool vecgen::operator()(const ast::time_event &x)
        {
            if( !(std::find(sampletimes.begin(), sampletimes.end(), x.first) != sampletimes.end()))
                sampletimes.push_back(x.first);
            return  true;
        }
        
        bool vecgen::operator()(const ast::sig_tim_event &x)
        {
            for (auto const& s:x.events)
                if (!(*this)(s)) return  false;
            return  true;
        }
        
        bool vecgen::start(const ast::session &x)
        {
            fout.open(outfilename);
            std::cout << "Opening outfile " << outfilename << std::endl;
            for (auto const& s:x)
            {
                
                if (!(boost::apply_visitor(*this,s))) return  false;
            }
            return true;
        }
        
        void vecgen::print_signals() const
        {
            std::cout << "=====Signals=======\n";
            for(auto v : signals) std::cout << v.first << ":" << v.second << std::endl;
            std::cout << "===================\n";
        }
        void vecgen::print_groups() const
        {
            std::cout << "=====Groups=======\n";
            for(auto v : groups) {
                std::cout << "Group: " << v.first  << ":" ;
                for(auto j:v.second) std::cout << j << " ";
                std::cout << std::endl;
            }
            std::cout << "===================\n";
            
        }
        
        void vecgen::print_wavetables() const {
            std::cout << "======Wavetables======\n";
            for(auto t: wavetables) {
                std::cout << "Wavetable: " << t.first
                << std::endl ;
                std::cout << "\t Period : " << t.second.period
                << std::endl;
                //                std::cout << "\t Signals : ";
                //                for (auto sig: t.second.sigtimings) {
                //                    std::cout << sig.first <<  " " ;
                //                    sig.second.print();
                //                }
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
        
        bool vecgen::operator()(const ast::patburst &x) {
            return true;
        }
        
        bool vecgen::operator()(const ast::patexec &x) {
            write_header();
            return true;
        }
        bool vecgen::operator()(const ast::annotation &x) {
            write_comment(x);
            return true;
        }
        bool vecgen::operator()(const ast::vec_stmt &x) {
            for (auto const& s:x.stmts)
                if (!(*this)(s)) return  false;
            write_comment("Vector");
            write_vec();
            return true;
        }
        
        
        bool vecgen::operator()(const ast::cur_wft &x) {
            cur_wft = x;
            sampletimes.clear();
            return true;
        }
        
        bool vecgen::operator()(const ast::pattern &x) {
            for (auto const& s:x.pats)
                if (!(boost::apply_visitor(*this,s))) return  false;
            return true;
        }
        
        void vecgen::vec_proc(std::string const& sig, char val)
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
                    cur_vec[t][sig] = out;
                    
                } else cur_vec[t].erase(sig);
            }
        }
        
        bool vecgen::operator()(const ast::vec_data &x) {
            
            if (find_group(x.name))
            {
                //std:: cout <<
                //"Group : " << x.name << std::endl;
                for(int i = 0; i < groups[x.name].size(); ++i)
                {
                    auto sig = groups[x.name][i];
                    vec_proc(sig, x.value.at(i));
                }
            } else vec_proc(x.name, x.value.at(0));
            //write_vec(t);
            return true;
        }
        void vecgen::write_comment(std::string const& msg)
        {
            
            fout << "//" << msg  << std::endl;
        }
        void vecgen::write_vec() {
            //std::sort(sampletimes.begin(),
            //        sampletimes.end());
            std::set<int> s( sampletimes.begin(), sampletimes.end() );
            sampletimes.assign( s.begin(), s.end() );
            for (auto const& t:sampletimes) {
                fout << t << " ";
                for (auto const& elem:work_vec) {
                    if (cur_vec[t][elem.first])
                        work_vec[elem.first] = cur_vec[t][elem.first];
                    fout << work_vec[elem.first];
                    //                    auto const& val = cur_vec[t][elem.first];
                    //                    if (val) {
                    //                        fout << val;
                    //                        work_vec[elem.first] = val;
                    //                    } else fout << work_vec[elem.first];
                    
                }
                fout << "\n";
            }
            
        }
        
        void vecgen::write_header() {
            for (auto const& elem:work_vec)
                fout << elem.first << " ";
            fout << std::endl;
        }
        
        bool vecgen::operator()(const ast::procs &x) {
            for (auto const& proc:x) {
                procs[proc.name] = proc;
            }
            return true;
        }
        
        bool vecgen::operator()(const ast::macros &x) {
            for (auto const& macro:x) {
                macros[macro.name] = macro;
            }
            return true;
        }
        
        bool vecgen::operator()(const ast::macro_call &x) {
            
            cur_macro = x.name;
            macro_proc = 1;
            macro_proc_max_len = find_max_len(x.stmts);
            cur_args = x.stmts;
            write_comment("Macro Call : " + cur_macro);
            for (auto const& s:macros[x.name].stmts)
                if (!(boost::apply_visitor(*this,s))) return  false;
            return true;
        }
        bool vecgen::operator()(const ast::proc_call &x) {
            cur_proc = x.name;
            macro_proc = 0;
            macro_proc_max_len = find_max_len(x.stmts);
            cur_args = x.stmts;
            write_comment("Proc Call : " + cur_proc);
            for (auto const& s:macros[x.name].stmts)
                if (!(boost::apply_visitor(*this,s))) return  false;
            return true;
            return true;
        }
        
        bool vecgen::operator()(const ast::cond_stmt &x) {
            for (auto const& s:x.stmts)
                if (!(*this)(s)) return  false;
            return true;
        }
        void vecgen::vec_proc_wrap(std::string const& sig, char const& val) {
            
            if (find_group(sig))
            {
                //for(int i = 0; i < groups[sig].size(); ++i)
                //{
                auto s = groups[sig][0];
                //std::cout << "s : " << s << " val : " << val << std::endl;
                vec_proc(s, val);
                //}
            } else vec_proc(sig, val);
            //write_vec(t);
        }
        
        bool vecgen::operator()(const ast::shift_stmt &x)
        {
            // Process non params first
            for (auto const& elem:x.stmts)
                if (elem.value != "#") vec_proc(elem.name, elem.value.at(0));
            for(int i = 0 ; i < macro_proc_max_len; ++i)
            {
                for (auto const& elem:cur_args) {
                    if (i < elem.value.length()) {
                        vec_proc_wrap(elem.name, elem.value.at(i));
                    }
                }
                write_vec();
            }
            
            return true;
        }
        
        int vecgen::find_max_len(std::list<ast::vec_data> const& stmts)
        {
            std::vector<int> lengths;
            for (auto const& vec:stmts) lengths.push_back(vec.value.length());
            return *std::max_element(lengths.begin(), lengths.end());;
        }
        
        
        
    }
    
}
