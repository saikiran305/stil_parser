#ifndef AST_ADAPTED_HPP
#define AST_ADAPTED_HPP
#include "ast.hpp"
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(client::ast::sig_stmt,sig_type, cnt )
BOOST_FUSION_ADAPT_STRUCT(client::ast::signal,
    name,type, scan_sig_type
        )
BOOST_FUSION_ADAPT_STRUCT(client::ast::group,
                          name, items
                          )

BOOST_FUSION_ADAPT_STRUCT(client::ast::stil_ast,
                          version, blocks
                          )
//BOOST_FUSION_ADAPT_STRUCT(client::ast::time_event, time, values)
BOOST_FUSION_ADAPT_STRUCT(client::ast::sig_tim_event, name, values, events)
BOOST_FUSION_ADAPT_STRUCT(client::ast::wavetable, name, period, sig_events)
BOOST_FUSION_ADAPT_STRUCT(client::ast::timing,
                          wavetables
                          )
BOOST_FUSION_ADAPT_STRUCT(client::ast::patburst, name, patlist)
BOOST_FUSION_ADAPT_STRUCT(client::ast::patexec, timingblock, burst)
BOOST_FUSION_ADAPT_STRUCT(client::ast::vec_data, name, value)
BOOST_FUSION_ADAPT_STRUCT(client::ast::pattern, name, pats)
#endif // AST_ADAPTED_HPP