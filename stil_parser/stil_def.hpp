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
        using x3::int_;
        using namespace x3::ascii;
        
        x3::symbols<char> keywords;
        x3::symbols<ast::signal_type> signal_type;
        x3::symbols<ast::scan_sig_type_t> scan_signal_type;
        
        // Helper to add keywords once
        void add_keywords() {
            keywords.add
            ("STIL")
            ("Signals")
            ("SignalGroups")
            ("Timing")
            ("Period")
            ("WaveformTable")
            ("Waveforms")
            ("PatternBurst")
            ("PatList")
            ("PatternExec")
            ("Pattern")
            ("Vector")
            ("Condition")
            ("MacroDefs")
            ("Procedurs")
            ("Call")
            ("Proc")
            ("In")
            ("Out")
            ("InOut")
            ("Supply")
            ("Pseudo")
            ("ScanIn")
            ("ScanOut")
            ;
            
            signal_type.add
            ("In",client::ast::in)
            ("Out", client::ast::out)
            ("InOut", client::ast::inout)
            ("Supply", client::ast::supply)
            ("Pseudo", client::ast::pseudo)
            ;
            
            scan_signal_type.add
            ("ScanIn", client::ast::scan_in)
            ("ScanOut", client::ast::scan_out)
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
        x3::rule<class sig_stmt_class, ast::sig_stmt> const sig_stmt = "sig_stmt";
        x3::rule<class signal_name_class, std::string> const signal_name = "signal_name";
        x3::rule<signal_item_class, ast::signal> const signal_item = "signal_item";
        x3::rule<class signals, ast::signals> const signals = "signals";
        
        //Group
        x3::rule<class group_name_class, std::string> const group_name = "group_name";
        x3::rule<group_item_class, ast::group> const group_item = "group_item";
        x3::rule<class groups, ast::groups> const groups = "groups";
        
        //Timing
        x3::rule<class eu_class, int> const eu = "eu";
        x3::rule<class wfc_class, char> const wfc = "wfc";
        x3::rule<class tim_event_class, ast::time_event>const tim_event = "tim_event";
        x3::rule<class sig_time_event_class , ast::sig_tim_event> const sig_time_event = "sig_time_event";
        x3::rule<class wavetable_class, ast::wavetable> const wavetable = "wavetable";
        x3::rule<class timing_class, ast::timing> const timing = "timing";
        
        //PatternExec, PatternBurst
        x3::rule<class patburst_class, ast::patburst> const patburst = "patburst";
        x3::rule<class patexec_class, ast::patexec> const patexec = "patexec";
        
        
        
        //Macro Def & Procedures
        x3::rule<class macros_class, ast::macros> const macros = "macrodefs";
        x3::rule<class procs_class, ast::procs> const procs = "procs";
        x3::rule<class macro_class, ast::macro_proc_def> const macro = "macro";
        x3::rule<class proc_class, ast::macro_proc_def> const proc = "proc";
        
        x3::rule<class macro_call_class, ast::macro_call> const macro_call = "macro_call";
        x3::rule<class proc_call_class, ast::proc_call> const proc_call = "proc_call";
        
        //Pattern
        x3::rule<class pattern_class, ast::pattern> const pattern = "pattern";
        x3::rule<class vec_data_class, ast::vec_data> const vec_data = "vec_data";
        x3::rule<class vec_stmt_class, ast::vec_stmt> const vec_stmt = "vec_stmt";
        x3::rule<class cond_stmt_class, ast::cond_stmt> const cond_stmt = "cond_stmt";
        x3::rule<class pat_stmt_class, ast::pat_stmt> const pat_stmt = "pat_stmt";
        
        //session
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
        
        auto const sig_stmt_def =
        "{"
        > scan_signal_type
        > x3::int_ > ";"
        > "}"
        ;
        auto const signal_item_def =
        signal_name > signal_type
        >> -(sig_stmt  - lit(";"))
        >> -(lit(";"))
        ;
        
        
        
        
        auto const signals_def = lit("Signals")
        > "{"
        >> *signal_item
        > "}";
        
        //Groups Grammar
        auto const group_name_def = !keywords >> identifier;
        auto const group_item_def = group_name >>
        '='
        >>
        (group_name % '+')
        >> (lit(";") | x3::omit[sig_stmt])
        //>> -(x3::omit["{" >> scan_signal_type >> int_ >> ";" >> "}"])
        //> ';'
        ;
        auto const groups_def = lit("SignalGroups")
        > "{"
        >> *group_item
        > "}";
        // Timing and Wavefromtable grammar
        auto const eu_def =
        //quote >>
        int_ >> lit("ns")
        //>> quote
        ;
        auto const wfc_def = x3::alnum;
        /*
         | x3::digit
         | char_('#')
         | char_('%')
         ;*/
        
        auto const tim_event_def = eu >> (char_ % '/') >> ";";
        auto const sig_time_event_def =
        identifier
        > "{"
        >> +(wfc)
        >> "{"
        >> +(tim_event_def)
        >> "}"
        >> "}"
        ;
        auto const wavetable_def = lit("WaveformTable")
        > identifier
        > "{"
        > lit("Period")
        >> eu > ";"
        >> lit("Waveforms")
        > "{"
        >> +(sig_time_event)
        > "}"
        > "}"
        ;
        auto const timing_def = lit("Timing")
        > "{"
        >> *(wavetable)
        >> "}"
        ;
        
        
        
        //Pattern Burst/Exec
        auto patburst_def = lit("PatternBurst")
        >> identifier
        >> "{"
        >> lit("PatList") >> "{"
        >> *(identifier > ";")
        >> "}" >> "}";
        
        auto patexec_def = lit("PatternExec")
        > "{"
        > -("Timing" > identifier > ";")
        > "PatternBurst" > identifier > ";"
        > "}"
        ;
        
        
        //Pattern
        auto const vec_data_def =
        identifier  > "=" > +(wfc) > ';'
        ;
        auto const vec_stmt_def =
        (lit("V") | lit("Vector"))
        > "{"
        > +(vec_data)
        > "}"
        ;
        auto const cond_stmt_def =
        (lit("C") | lit("Condition"))
        > "{"
        > +(vec_data)
        > "}"
        ;
        auto const pat_stmt_def =
        vec_stmt
        | "W" > identifier > ";"
        | cond_stmt
        | macro_call
        | proc_call
        ;
        auto const pattern_def = lit("Pattern")
        > identifier
        > "{"
        > +(pat_stmt)
        > "}"
        ;
        
        //Macros & Procs
        auto macro_def = identifier
        > "{"
        >> *(pat_stmt)
        >> "}"
        ;
        
        auto proc_def = identifier
        > "{"
        >> *(pat_stmt)
        >> "}"
        ;
        
        auto macro_call_def = lit("Macro")
        > identifier
        >> -("{" >> *(pat_stmt) >> "}")
        | ';'
        ;
        
        auto proc_call_def = lit("Call")
        > identifier
        >> -("{" >> *(pat_stmt) >> "}")
        | ';'
        ;
        
        auto macros_def = lit("MacroDefs")
        > "{"
        >> *(macro)
        >> "}"
        ;
        
        auto procs_def = lit("Procedures")
        > "{"
        >> *(proc)
        > "}"
        ;
        
        auto const block_def =
        //x3::eps
        signals
        | groups
        | timing
        | macros
        | procs
        | patburst
        | patexec
        | pattern
        ;
        auto const session_def = *block;
        auto const stil_session_def = version
        > session;
        
        BOOST_SPIRIT_DEFINE(
                            version,
                            signal_name,
                            sig_stmt,
                            signal_item,
                            signals,
                            group_name,
                            group_item,
                            groups,
                            eu,
                            wfc,
                            tim_event,
                            sig_time_event,
                            wavetable,
                            timing,
                            patburst,
                            patexec,
                            pattern,
                            vec_data,
                            vec_stmt,
                            cond_stmt,
                            pat_stmt,
                            macro,
                            proc,
                            macros,
                            procs,
                            macro_call,
                            proc_call,
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
