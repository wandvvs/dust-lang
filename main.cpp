#include "source/token/token.hpp"
#include "source/lexer/lexer.hpp"
#include "source/compiler/llvm_compiler.hpp"
#include "source/compiler/llvm_executable_builder.hpp"
#include "source/token/token_buffer/token_buffer.hpp"

int main(int argc, char** argv) 
{
    if(argc != 2)
    {
        std::cerr << "[-] Incorrect usage." << std::endl;
        std::cerr << "[-] Correct usage: ./dust-lang [input.dust]" << std::endl;

        exit(EXIT_FAILURE);
    }

    std::string source;
    {
        std::fstream file_input(argv[1], std::ios::in);
        std::stringstream stream_input;

        stream_input << file_input.rdbuf();
        source = stream_input.str();
    }

    Lexer lexer(std::move(source));
    std::vector<Token> tokens = lexer.tokenize();

    LLVMCompiler compiler("dust_prog", std::move(TokenBuffer(std::move(tokens))));
    compiler.generate();
    compiler.verify_module();

    LLVMExecutableBuilder exec(compiler.get_llvm_ir_as_string(), "out");
    exec.build_executable();

    return 0;
}