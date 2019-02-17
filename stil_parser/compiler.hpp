#ifndef COMPILER_HPP
#define COMPILER_HPP
#include "ast.hpp"
#include "error_handler.hpp"

#include <map>
#include <vector>
#include <iostream>
#include <fstream>

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
                     ErrorHandler const& error_handler,
                     std::string const& outfilename
                     ) : error_handler(
            [&](x3::position_tagged pos, std::string const& msg)
                                       {error_handler(pos,msg);}
                                       ),
            outfilename(outfilename)
            {}
            
            bool operator()(ast::nil) const {BOOST_ASSERT(0); return false;}
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
            
            bool start(ast::session const& x) ;
            void print_signals() const;
            void print_groups() const;
            void print_wavetables() const;
            
        private:
            std::string outfilename;
            std::ofstream fout;
            std::map<std::string, ast::signal_type> signals;
            std::map<std::string, std::vector<std::string>> groups;
            std::map<std::string, ast::wavetable> wavetables;
            std::map<std::string, std::vector<int> > wavetable_sampletimes;
            std::vector<int> sampletimes;
            std::string cur_wft;
            std::map<std::string, std::string> cur_vec;
            error_handler_type error_handler;
            
            void add_signal(ast::signal const& x) ;
            bool find_signal(std::string const& x);
            bool find_group(std::string const& x);
            void add_group(std::string const& x, std::string const& y);
            
            void write_header();
            void write_vec(int t);
            
        };
    }
}
#endif // COMPILER_HPP
