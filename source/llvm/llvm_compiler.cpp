#include "llvm_compiler.hpp"
#include <cstddef>
#include <cstdlib>
#include <llvm-16/llvm/IR/BasicBlock.h>
#include <llvm-16/llvm/IR/DerivedTypes.h>
#include <llvm-16/llvm/IR/Function.h>
#include <llvm-16/llvm/IR/Value.h>
#include <stdexcept>

LLVMCompiler::LLVMCompiler(const std::string& module_name, TokenBuffer tokens_buffer)
    : m_context(), m_module(module_name, m_context), m_builder(m_context), m_tokens_buffer(std::move(tokens_buffer))
    {
    }

TokenType LLVMCompiler::current_type() const { return m_tokens_buffer.m_current.value().get_type(); }
std::string LLVMCompiler::current_value() const { return m_tokens_buffer.m_current.value().get_value().value(); }

void LLVMCompiler::check_token_type(TokenType expected_type, const std::string& error_message) const
{
    if(!m_tokens_buffer.m_current.has_value() || current_type() != expected_type)
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

void LLVMCompiler::process_assign() {
    if (m_variables.find(current_value()) == m_variables.end()) 
    {
        std::cerr << "Syntax error. Undefined identifier" << std::endl;
        throw std::runtime_error("Undefined identifier.");
    }

    std::string left_identifier_name = current_value();

    m_tokens_buffer.move_next();
    check_token_type(TokenType::ASSIGN, "Expected '=' after identifier");

    m_tokens_buffer.move_next();

    llvm::Value* right_value = process_expr();

    while (current_type() == TokenType::PLUS || current_type() == TokenType::MINUS ||
           current_type() == TokenType::MUL || current_type() == TokenType::DIV) 
    {
        TokenType operation = current_type();
        m_tokens_buffer.move_next(); 
        llvm::Value* temp = process_expr(); 
        switch (operation) 
        {
            case TokenType::PLUS:
                right_value = m_builder.CreateAdd(right_value, temp, "add");
                break;
            case TokenType::MINUS:
                right_value = m_builder.CreateSub(right_value, temp, "sub");
                break;
            case TokenType::MUL:
                right_value = m_builder.CreateMul(right_value, temp, "mul");
                break;
            case TokenType::DIV:
                right_value = m_builder.CreateSDiv(right_value, temp, "div");
                break;
            default:
                std::cerr << "Unexpected operator." << std::endl;
                throw std::runtime_error("Unexpected operator.");
        }
    }

    set_variable(left_identifier_name, right_value);
    check_token_type(TokenType::SEMICOLON, "Expected for ';'");
    m_tokens_buffer.move_next();
}



void LLVMCompiler::set_variable(const std::string& name, llvm::Value* value) { m_variables[name] = value; }

llvm::Value* LLVMCompiler::process_exit()
{
    m_tokens_buffer.move_next();
    check_token_type(TokenType::LPAREN, "Expected for '(");
    m_tokens_buffer.move_next();

    llvm::Value* return_value = process_expr();

    while (current_type() == TokenType::PLUS || current_type() == TokenType::MINUS ||
           current_type() == TokenType::MUL || current_type() == TokenType::DIV) 
    {
        TokenType operation = current_type();
        m_tokens_buffer.move_next(); 
        llvm::Value* temp = process_expr(); 
        switch (operation) 
        {
            case TokenType::PLUS:
                return_value = m_builder.CreateAdd(return_value, temp, "add");
                break;
            case TokenType::MINUS:
                return_value = m_builder.CreateSub(return_value, temp, "sub");
                break;
            case TokenType::MUL:
                return_value = m_builder.CreateMul(return_value, temp, "mul");
                break;
            case TokenType::DIV:
                return_value = m_builder.CreateSDiv(return_value, temp, "div");
                break;
            default:
                std::cerr << "Unexpected operator." << std::endl;
                throw std::runtime_error("Unexpected operator.");
        }
    }

    check_token_type(TokenType::RPAREN, "Expected ')'");

    m_tokens_buffer.move_next();
    check_token_type(TokenType::SEMICOLON, "Expected ';'");

    return return_value;
}

llvm::Value* LLVMCompiler::process_expr() {
    if (current_type() == TokenType::LPAREN) 
    {
        m_tokens_buffer.move_next();
        llvm::Value* value = process_expr();
        check_token_type(TokenType::RPAREN, "Expected ')' after expression in parentheses");
        m_tokens_buffer.move_next();
        return value;
    }

    llvm::Value* value = process_term(); 

    while (current_type() == TokenType::PLUS || current_type() == TokenType::MINUS) 
    {
        TokenType operation = current_type();
        m_tokens_buffer.move_next(); 

        llvm::Value* temp = process_term();         
        if (operation == TokenType::PLUS) 
        {
            value = m_builder.CreateAdd(value, temp, "add");
        } 
        else if (operation == TokenType::MINUS) 
        {
            value = m_builder.CreateSub(value, temp, "sub");
        }
    }

    return value;
}

llvm::Value* LLVMCompiler::process_term() {
    llvm::Value* value = process_factor(); 

    while (current_type() == TokenType::MUL || current_type() == TokenType::DIV) 
    {
        TokenType operation = current_type();
        m_tokens_buffer.move_next(); 

        llvm::Value* temp = process_factor(); 
        if (operation == TokenType::MUL) 
        {
            value = m_builder.CreateMul(value, temp, "mul");
        } 
        else if (operation == TokenType::DIV) 
        {
            value = m_builder.CreateSDiv(value, temp, "div");
        }
    }

    return value;
}

llvm::Value* LLVMCompiler::process_factor() 
{
    llvm::Value* value = nullptr;

    if (current_type() == TokenType::LPAREN) 
    {
        return process_expr();    
    }

    if (current_type() == TokenType::INT_LITERAL) 
    {
        value = m_builder.getInt64(std::stoi(current_value()));
        m_tokens_buffer.move_next();
    } 
    else if (current_type() == TokenType::IDENTIFIER) 
    {
        std::string assigned_variable_name = current_value();
        value = m_variables.at(assigned_variable_name);
        m_tokens_buffer.move_next();
    } 
    else 
    {
        std::cerr << "Unexpected token. Expected integer literal, identifier, or '('." << std::endl;
        exit(EXIT_FAILURE);
    }

    return value;
}

void LLVMCompiler::process_let() {
    m_tokens_buffer.move_next();
    check_token_type(TokenType::IDENTIFIER, "Expected identifier after 'let'");
    std::string variable_name = current_value();

    m_tokens_buffer.move_next();
    check_token_type(TokenType::ASSIGN, "Expected '=' after identifier");
    m_tokens_buffer.move_next();

    llvm::Value* value = process_expr(); 

    while (current_type() == TokenType::PLUS || current_type() == TokenType::MINUS ||
           current_type() == TokenType::MUL || current_type() == TokenType::DIV) 
    {
        TokenType operation = current_type();
        m_tokens_buffer.move_next(); 
        llvm::Value* temp = process_expr(); 
        switch (operation) 
        {
            case TokenType::PLUS:
                value = m_builder.CreateAdd(value, temp, "add");
                break;
            case TokenType::MINUS:
                value = m_builder.CreateSub(value, temp, "sub");
                break;
            case TokenType::MUL:
                value = m_builder.CreateMul(value, temp, "mul");
                break;
            case TokenType::DIV:
                value = m_builder.CreateSDiv(value, temp, "div");
                break;
            default:
                std::cerr << "Unexpected operator." << std::endl;
                throw std::runtime_error("Unexpected operator.");
        }
    }

    set_variable(variable_name, value);
    check_token_type(TokenType::SEMICOLON, "Expected for ';'");
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
        if(current_type() == TokenType::EXIT)
        {
            llvm::Value* return_value = process_exit();
            m_builder.CreateRet(return_value);
        }
        if(current_type() == TokenType::LET)
        {
            process_let();
        }

        if(current_type() == TokenType::IDENTIFIER)
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