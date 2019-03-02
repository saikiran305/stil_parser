#ifndef VECGEN_HPP
#define VECGEN_HPP
#include "ast.hpp"
#include "error_handler.hpp"

#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>

namespace client {
    namespace patgen {
        namespace  x3 = boost::spirit::x3;
        
        struct sigtiming {
            std::vector<int> sampletimes;
            std::map<int, std::map<char, char> > vec_values;
            sigtiming() {}
            //            sigtiming(ast::sig_tim_event const& x);
            sigtiming(std::list<ast::time_event> const& events, std::string const& values);
            void add_events(std::list<ast::time_event> const& events, std::string const& values);
            void print();
            void get_value(int t, char value, bool& is_valid, char& out);
            
        };
        
        struct wavetable {
            int period;
            std::map<std::string,sigtiming> sigtimings;
        };
        
        
        class vecgen
        {
        public:
            typedef  bool result_type;
            typedef  std::function<
            void(x3::position_tagged, std::string const &)>
            error_handler_type;
            typedef  std::function<
            void(x3::position_tagged)>
            eh_type;
            
            
            template<typename ErrorHandler>
            vecgen(
                   ErrorHandler const& error_handler,
                   std::string const& outfilename
                   ) : error_handler(
                                     [&](x3::position_tagged pos, std::string const& msg)
                                     {error_handler(pos,msg);}
                                     ),
            outfilename(outfilename)
            {}
            
            //bool operator()(ast::nil) const {BOOST_ASSERT(0); return false;}
            bool operator()(ast::nil) const {return true;}
            bool operator()(ast::signals const& x)  ;
            bool operator()(ast::signal const& x)  ;
            bool operator()(ast::groups const& x);
            bool operator()(ast::group const& x);
            bool operator()(ast::timing const& x);
            bool operator()(ast::wavetable const& x);
            bool operator()(ast::time_event const& x);
            bool operator()(ast::sig_tim_event const& x);
            bool operator()(ast::patburst const& x);
            bool operator()(ast::patexec const& x);
            bool operator()(ast::pattern const& x);
            bool operator()(ast::cur_wft const& x);
            bool operator()(ast::vec_stmt const& x);
            bool operator()(ast::vec_data const& x);
            bool operator()(ast::procs const& x);
            bool operator()(ast::macros const& x);
            bool operator()(ast::macro_call const& x);
            bool operator()(ast::proc_call const& x);
            bool operator()(ast::cond_stmt const& x);
            bool operator()(ast::shift_stmt const& x);
            bool operator()(ast::annotation const& x);
            bool operator()(ast::scanstruct const& x);
            
            bool start(ast::session const& x) ;
            void print_signals() const;
            void print_groups() const;
            void print_wavetables() const;
            
        private:
            std::string outfilename;
            std::ofstream fout;
            bool vec_changed;
            
            
            std::map<std::string, ast::signal_type> signals;
            std::map<std::string, std::vector<std::string>> groups;
            std::map<std::string, wavetable > wavetables;
            std::map<std::string, std::vector<int> > wavetable_sampletimes;
            std::vector<int> sampletimes;
            std::map<std::string, ast::macro_proc_def> macros, procs;
            std::string cur_macro, cur_proc;
            int macro_proc, macro_proc_max_len;
            std::list<ast::vec_data> cur_args;
            std::string cur_wft, pre_wft;
            std::map<int, std::map<std::string, char> > cur_vec;
            std::map<std::string, char> pre_vec, work_vec;
            error_handler_type error_handler;
            
            void add_signal(ast::signal const& x) ;
            bool find_signal(std::string const& x);
            bool find_group(std::string const& x);
            void add_group(std::string const& x, std::string const& y);
            void add_sigtiming(std::string const& name, std::list<ast::time_event> const& events, std::string const& values);
            
            void write_header();
            void vec_proc(std::string const& sig, char val);
            void write_vec();
            void write_comment(std::string const& msg);
            int find_max_len(std::list<ast::vec_data> const& stmts);
            void vec_proc_wrap(std::string const& sig,char const& val);
            
        };
    }
}
#endif // VECGEN_HPP
