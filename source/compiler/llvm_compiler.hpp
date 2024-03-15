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
#include <cstddef>
#include <stdexcept>
#include <unordered_set>

class LLVMCompiler
{
private:
    llvm::LLVMContext m_context;
    llvm::Module m_module;
    llvm::IRBuilder<> m_builder;
    TokenBuffer m_tokens_buffer;
    size_t parse_pos = 0;

    TokenType current_type() const;
    std::string current_value() const;

    std::unordered_map<std::string, llvm::Value*> m_variables;
    std::unordered_map<std::string, llvm::Value*> m_string_variables;
    std::unordered_map<std::string, llvm::Value*> m_boolean_variables;
    std::unordered_set<std::string> m_constants;

    llvm::Value* process_exit();
    void process_writeln();

    void process_use_io();

    llvm::Value* process_expr();
    llvm::Value* process_term();
    llvm::Value* process_factor();
    llvm::Value* process_arithmetic(llvm::Value* left_value);

    llvm::Function* m_printf_func;
    llvm::FunctionType* m_printf_type;

    void process_check(const std::string& left_variable_name);
    void process_global_variables(llvm::Value* left_expression, llvm::Value* right_expression, std::string left_variable_name, TokenType prev);

    void process_mut();
    void process_const();
    void process_assign(); 

    llvm::Value* get_variable(const std::string& name) const;
    void set_variable(const std::string& name, llvm::Value* value);

    void check_token_type(TokenType expected_type, const std::string& error_message) const;

    bool use_io = false;

public:
    LLVMCompiler(const std::string& module_name, TokenBuffer tokens_buffer);

    void generate();
    void verify_module();

    std::string get_llvm_ir_as_string() const;
};