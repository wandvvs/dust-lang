#include "llvm_compiler.hpp"
#include <cstddef>
#include <llvm-16/llvm/IR/BasicBlock.h>
#include <llvm-16/llvm/IR/DerivedTypes.h>
#include <llvm-16/llvm/IR/Function.h>
#include <llvm-16/llvm/IR/Value.h>
#include <stdexcept>

LLVMCompiler::LLVMCompiler(const std::string& module_name, TokenBuffer tokens_buffer)
    : m_context(), m_module(module_name, m_context), m_builder(m_context), m_tokens_buffer(std::move(tokens_buffer))
    {
    }

void LLVMCompiler::check_token_type(TokenType expected_type, const std::string& error_message) const
{
    if(!m_tokens_buffer.m_current.has_value() || m_tokens_buffer.m_current.value().get_type() != expected_type)
    {
        std::cerr << "Syntax error. " << error_message << std::endl;

        throw std::runtime_error("Syntax error");
    }
}

llvm::Value* LLVMCompiler::get_variable(const std::string& name) const
{
    if (m_variables.count(name) == 0)
    {
        std::cerr << "Error: Variable '" << name << "' not defined." << std::endl;
        throw std::runtime_error("Variable not defined.");
    }

    return m_variables.at(name);
}

void LLVMCompiler::set_variable(const std::string& name, llvm::Value* value) { m_variables[name] = value; }

llvm::Value* LLVMCompiler::process_exit()
{
    m_tokens_buffer.move_next();
    check_token_type(TokenType::LPAREN, "Expected for '(");
    m_tokens_buffer.move_next();

    llvm::Value* return_value = nullptr;

    if(m_tokens_buffer.m_current.value().get_type() == TokenType::INT_LITERAL)
    {
        return_value = m_builder.getInt64(std::stoi(m_tokens_buffer.m_current.value().get_value().value()));
    }
    else if (m_tokens_buffer.m_current.value().get_type() == TokenType::IDENTIFIER)
    {
        return_value = get_variable(m_tokens_buffer.m_current.value().get_value().value());
    }
    else
    {
        std::cerr << "Syntax error. Expected integer literal or variable." << std::endl;
        throw std::runtime_error("Syntax error.");
    }

    m_tokens_buffer.move_next();
    check_token_type(TokenType::RPAREN, "Expected ')'");

    m_tokens_buffer.move_next();
    check_token_type(TokenType::SEMICOLON, "Expected ';'");

    return return_value;
}

void LLVMCompiler::process_let()
{
    m_tokens_buffer.move_next();
    check_token_type(TokenType::IDENTIFIER, "Expected identifier after 'let'");
    std::string variable_name = m_tokens_buffer.m_current.value().get_value().value();

    m_tokens_buffer.move_next();
    check_token_type(TokenType::ASSIGN, "Expected '=' after identifier");
    m_tokens_buffer.move_next();

    llvm::Value* value = nullptr;

    if(m_tokens_buffer.m_current.value().get_type() == TokenType::INT_LITERAL)
    {
        value = m_builder.getInt64(std::stoi(m_tokens_buffer.m_current.value().get_value().value()));
    }
    else if (m_tokens_buffer.m_current.value().get_type() == TokenType::IDENTIFIER)
    {
        std::string assigned_variable_name = m_tokens_buffer.m_current.value().get_value().value();
        value = m_variables.at(assigned_variable_name);
    }
    else
    {
        std::cerr << "Syntax error. Expected integer literal or variable." << std::endl;
        throw std::runtime_error("Syntax error.");
    }

    set_variable(variable_name, value);

    m_tokens_buffer.move_next();
    check_token_type(TokenType::SEMICOLON, "Expected for ';'");
    m_tokens_buffer.move_next();
}

void LLVMCompiler::process_assign()
{
    if(m_variables.find(m_tokens_buffer.m_current.value().get_value().value()) == m_variables.end())
    {
        std::cerr << "Syntax error. Undefined identifier" << std::endl;
        throw std::runtime_error("Undefined identifier.");
    }

    std::string left_identifier_name = m_tokens_buffer.m_current.value().get_value().value();

    m_tokens_buffer.move_next();
    check_token_type(TokenType::ASSIGN, "Expected '=' after identifier");

    m_tokens_buffer.move_next();
    llvm::Value* right_value = nullptr;

    if(m_tokens_buffer.m_current.value().get_type() == TokenType::INT_LITERAL)
    {
        right_value = m_builder.getInt64(std::stoi(m_tokens_buffer.m_current.value().get_value().value()));
        set_variable(left_identifier_name, right_value);
    }
    else if (m_tokens_buffer.m_current.value().get_type() == TokenType::IDENTIFIER)
    {
        std::string right_identifier_name = m_tokens_buffer.m_current.value().get_value().value();
        right_value = m_variables.at(right_identifier_name);
    }
    else
    {
        check_token_type(TokenType::IDENTIFIER, "Expected identifier or interger literal before '='");
    }
    set_variable(left_identifier_name, right_value);

    m_tokens_buffer.move_next();
    check_token_type(TokenType::SEMICOLON, "Expected for ';");
    m_tokens_buffer.move_next();
}

void LLVMCompiler::generate_main_function()
{
    llvm::FunctionType* func_type = llvm::FunctionType::get(m_builder.getInt64Ty(), false);
    llvm::Function* main_func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, "main", m_module);

    llvm::BasicBlock* entry = llvm::BasicBlock::Create(m_context, "entrypoint", main_func);
    m_builder.SetInsertPoint(entry);

    while(parse_pos <= m_tokens_buffer.m_tokens.size())
    {
        if(m_tokens_buffer.m_current.value().get_type() == TokenType::EXIT)
        {
            llvm::Value* return_value = process_exit();
            m_builder.CreateRet(return_value);
        }
        if(m_tokens_buffer.m_current.value().get_type() == TokenType::LET)
        {
            process_let();
        }

        if(m_tokens_buffer.m_current.value().get_type() == TokenType::IDENTIFIER)
        {
            process_assign();
        }
        parse_pos++;
    }
}

std::string LLVMCompiler::get_llvm_ir_as_string() const
{
    std::string IRString;
    llvm::raw_string_ostream OS(IRString);
    m_module.print(OS, nullptr);
    return OS.str();
}

void LLVMCompiler::print_ir() const
{
    llvm::outs() << "Generated LLVM IR:\n";
    m_module.print(llvm::outs(), nullptr);
}
 
void LLVMCompiler::verify_module() { llvm::verifyModule(m_module); }