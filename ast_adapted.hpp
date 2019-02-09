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
#endif // AST_ADAPTED_HPP
