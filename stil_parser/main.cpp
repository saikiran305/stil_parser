#include <iostream>
#include "ast.hpp"
#include "stil.hpp"
#include "error_handler.hpp"
#include "config.hpp"
#include "skipper.hpp"
#include "compiler.hpp"
#include <fstream>
#include <boost/algorithm/string.hpp>

int main(int argc, char **argv)
{
    char const* filename;
    if (argc > 1)
    {
        filename = argv[1];
    }
    else
    {
        std::cerr << "Error: No input file provided." << std::endl;
    }
    std::ifstream in(filename, std::ios_base::in);
    std::cout << "Opening file:" << filename << std::endl;
    
    if (!in)
    {
        std::cerr << "Error: Could not open input file: "
        << filename
        << std::endl;
        return 1;
    }
    std::string source;
    in.unsetf(std::ios::skipws);
    std::copy(
              std::istream_iterator<char>(in),
              std::istream_iterator<char>(),
              std::back_inserter(source));
    
    using client::parser::iterator_type;
    iterator_type iter = source.begin();
    iterator_type end = source.end();
    
    using boost::spirit::x3::with;
    using client::parser::error_handler_type;
    error_handler_type error_handler(iter, end, std::cerr);
    client::ast::stil_ast ast;
    
    std::string outfilename = filename;
    boost::replace_all(outfilename, ".stil", "_parsed.stil");
    client::vecgen::compiler compiler(error_handler, outfilename);
    
    auto const parser =
    with<client::parser::error_handler_tag>(std::ref(error_handler))
    [
     client::stil_session()
     ];
    const auto& skipper = client::parser::getSkipper();
    //auto const skipper = client::skipper();
    
    using boost::spirit::x3::ascii::space;
    bool r = phrase_parse(iter, end, parser,skipper, ast);
    
    if (r && iter == end)
    {
        //client::ast::Visitor v;
        std::cout << "\nParsing succeeded \n";
        std::cout << "STIL version : "
        << ast.version << std::endl;
        
        //for (auto block:ast.blocks) boost::apply_visitor(v,block);
        
        if (compiler.start(ast.blocks)) {
            //compiler.print_signals();
            compiler.print_groups();
            compiler.print_wavetables();
        }
        return 0;
    }
    else {
        std::cout << "Parsing failed \n";
        return 1;
    }
    
}

