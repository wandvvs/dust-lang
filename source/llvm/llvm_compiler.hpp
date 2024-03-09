#pragma once

#include <llvm-16/llvm/IR/IRBuilder.h>
#include <llvm-16/llvm/IR/LLVMContext.h>
#include <llvm-16/llvm/IR/Value.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

#include "../token/token_buffer/token_buffer.hpp"

#include <unordered_map>

class LLVMCompiler
{
private:
    llvm::LLVMContext m_context;
    llvm::Module m_module;
    llvm::IRBuilder<> m_builder;
    TokenBuffer m_tokens_buffer;
    std::unordered_map<std::string, llvm::Value*> m_variables;
    size_t parse_pos = 0;

public:
    LLVMCompiler(const std::string& module_name, TokenBuffer tokens_buffer);

    void check_token_type(TokenType expected_type, const std::string& error_message) const;
    void generate_main_function();

    llvm::Value* process_exit();
    void process_let();
    void process_assign(); 

    llvm::Value* get_variable(const std::string& name) const;
    void set_variable(const std::string& name, llvm::Value* value);

    std::string get_llvm_ir_as_string() const;
    void print_ir() const;
    void verify_module();
};