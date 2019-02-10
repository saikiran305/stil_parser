#ifndef AST_ADAPTED_HPP
#define AST_ADAPTED_HPP
#include "ast.hpp"
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(client::ast::signal,
    name,type
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
#endif // AST_ADAPTED_HPP
