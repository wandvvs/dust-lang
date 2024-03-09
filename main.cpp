#include "source/lexer/lexer.hpp"
#include "source/llvm/llvm_compiler.hpp"
#include "source/token/token.hpp"
#include "source/token/token_buffer/token_buffer.hpp"
#include "source/llvm/llvm_executable_builder.hpp"
#include <sstream>

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
    std::cout << "[+] Tokenized." << std::endl;

    TokenBuffer buffer(std::move(tokens));
    std::cout << "[+] Buffered." << std::endl;

    LLVMCompiler compiler("dust_prog", std::move(buffer));
    compiler.generate_main_function();
    std::cout << "[+] Compiled." << std::endl;

    compiler.verify_module();
    std::cout << "[+] Compile verifed" << std::endl;

    LLVMExecutableBuilder exec(compiler.get_llvm_ir_as_string(), "test");
    exec.build_executable();
    std::cout << "[+] Generated executable." << std::endl;

    return 0;
}