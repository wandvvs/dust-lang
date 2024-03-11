#include "llvm_compiler.hpp"
#include <cstddef>
#include <cstdlib>
#include <llvm-16/llvm/IR/BasicBlock.h>
#include <llvm-16/llvm/IR/Constant.h>
#include <llvm-16/llvm/IR/DerivedTypes.h>
#include <llvm-16/llvm/IR/Function.h>
#include <llvm-16/llvm/IR/Value.h>
#include <stdexcept>

LLVMCompiler::LLVMCompiler(const std::string& module_name, TokenBuffer tokens_buffer)
    : m_context(), m_module(module_name, m_context), m_builder(m_context), m_tokens_buffer(std::move(tokens_buffer))
    {
    }

void LLVMCompiler::process_extern_std()
{
    m_tokens_buffer.move_next();
    check_token_type(TokenType::SEMICOLON, "Expected ';' after extern");

    m_printf_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(m_context), {llvm::Type::getInt8PtrTy(m_context)}, true);
    m_printf_func = llvm::Function::Create(m_printf_type, llvm::Function::ExternalLinkage, "printf", m_module);   

    std_externed = true;
    m_tokens_buffer.move_next();
}

TokenType LLVMCompiler::current_type() const { return m_tokens_buffer.m_current.get_type(); }
std::string LLVMCompiler::current_value() const { return m_tokens_buffer.m_current.get_value(); }

void LLVMCompiler::check_token_type(TokenType expected_type, const std::string& error_message) const
{
    if(current_type() != expected_type)
    {
        std::cerr << "Syntax error. " << error_message << std::endl;

        exit(EXIT_FAILURE);
    }
}

llvm::Value* LLVMCompiler::get_variable(const std::string& name) const
{
    if (m_variables.count(name) == 0)
    {
        std::cerr << "Error: Variable '" << name << "' not defined." << std::endl;
        exit(EXIT_FAILURE);
    }

    return m_variables.at(name);
}

llvm::Value* LLVMCompiler::process_arithmetic(llvm::Value* left_value)
{
    while (current_type() == TokenType::PLUS || current_type() == TokenType::MINUS ||
           current_type() == TokenType::MUL || current_type() == TokenType::DIV) 
    {
        TokenType operation = current_type();
        m_tokens_buffer.move_next(); 
        llvm::Value* temp = process_expr(); 
        switch (operation) 
        {
            case TokenType::PLUS:
                left_value = m_builder.CreateAdd(left_value, temp, "add");
                break;
            case TokenType::MINUS:
                left_value = m_builder.CreateSub(left_value, temp, "sub");
                break;
            case TokenType::MUL:
                left_value = m_builder.CreateMul(left_value, temp, "mul");
                break;
            case TokenType::DIV:
                left_value = m_builder.CreateSDiv(left_value, temp, "div");
                break;
            default:
                std::cerr << "Unexpected operator." << std::endl;
                throw std::runtime_error("Unexpected operator.");
        }
    }
    return left_value;
}

void LLVMCompiler::process_assign() {
    if(m_constants.find(current_value()) != m_constants.end())
    {
        std::cerr << "Syntax error. Variable '" << current_value() << "' is constant and cannot be reassigned" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (m_variables.find(current_value()) == m_variables.end() && m_string_variables.find(current_value()) == m_string_variables.end()) 
    {
        std::cerr << "Syntax error. Undefined identifier" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string left_identifier_name = current_value();

    m_tokens_buffer.move_next();
    check_token_type(TokenType::ASSIGN, "Expected '=' after identifier");
    m_tokens_buffer.move_next();

    if(current_type() == TokenType::QOUTE)
    {
        m_tokens_buffer.move_next();
        std::string right_string_value = current_value();

        if(m_variables.find(left_identifier_name) != m_variables.end())
        {
            m_variables.erase(left_identifier_name);

            std::string string_literal = current_value();
            llvm::Value* str_ptr = m_builder.CreateGlobalStringPtr(string_literal);
            llvm::Value* value = str_ptr;
            m_tokens_buffer.move_next();
            check_token_type(TokenType::QOUTE, "Expected '\"' after string literal");

            m_string_variables[left_identifier_name] = value;
            m_tokens_buffer.move_next();

        }
        else
        {
            
            llvm::Value* str_ptr = m_string_variables[left_identifier_name];

            llvm::Constant* new_str_ptr = m_builder.CreateGlobalStringPtr(right_string_value);

            str_ptr->replaceAllUsesWith(new_str_ptr);

            m_string_variables[left_identifier_name] = new_str_ptr;
            m_tokens_buffer.move_next();
                    m_tokens_buffer.move_next();
        }
    }
    else if(current_type() == TokenType::IDENTIFIER && m_string_variables.find(left_identifier_name) != m_string_variables.end())
    {
        llvm::Value* str_ptr = m_string_variables[current_value()];
        llvm::Value* new_str_ptr = m_string_variables[left_identifier_name];

        str_ptr->replaceAllUsesWith(new_str_ptr);
        m_string_variables[current_value()] = new_str_ptr;

        m_tokens_buffer.move_next();
    }
    else
    {
    llvm::Value* right_value = process_expr();

    right_value = process_arithmetic(right_value);

    set_variable(left_identifier_name, right_value);
 
    }

   check_token_type(TokenType::SEMICOLON, "Expected for ';'");
    m_tokens_buffer.move_next();
}

void LLVMCompiler::set_variable(const std::string& name, llvm::Value* value) { m_variables[name] = value; }

llvm::Value* LLVMCompiler::process_exit()
{
    m_tokens_buffer.move_next();
    check_token_type(TokenType::LPAREN, "Expected for '(");
    m_tokens_buffer.move_next();

    if(current_type() == TokenType::IDENTIFIER && m_string_variables.find(current_value()) != m_string_variables.end())
    {
        std::cerr << "Syntax error. Exit argument can`t be string." << std::endl;
        exit(EXIT_FAILURE);
    }

    llvm::Value* return_value = process_expr();

    return_value = process_arithmetic(return_value);

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

void LLVMCompiler::process_writeln()
{
    m_tokens_buffer.move_next();
    check_token_type(TokenType::LPAREN, "Expected '(' after 'writeln'.");
    m_tokens_buffer.move_next();

    if(current_type() == TokenType::QOUTE)
    {
        m_tokens_buffer.move_next();

        check_token_type(TokenType::STRING_LITERAL, "Expected string literal after \"");
        std::string string_literal = m_tokens_buffer.m_current.get_value();

        m_tokens_buffer.move_next();

        check_token_type(TokenType::QOUTE, "Expected '\"' after string literal");
        m_tokens_buffer.move_next();
        check_token_type(TokenType::RPAREN, "Expected ')' after '\"'");

        llvm::Constant* format_str = m_builder.CreateGlobalStringPtr("%s\n");

        m_builder.CreateCall(m_printf_func, {format_str, m_builder.CreateGlobalStringPtr(string_literal)});
    }
    else if(current_type() == TokenType::INT_LITERAL)
    {
        llvm::Value* value = process_expr();

        value = process_arithmetic(value);


        llvm::Value* format_str = m_builder.CreateGlobalStringPtr("%d\n");

        m_builder.CreateCall(m_printf_func, {format_str, value});

    }
    else if(current_type() == TokenType::IDENTIFIER)
    {
        if (m_variables.find(current_value()) != m_variables.end()) 
        {
            std::string variable_name = current_value();
            llvm::Value* value = get_variable(variable_name);

            llvm::Value* format_str = m_builder.CreateGlobalStringPtr("%d\n");
            m_builder.CreateCall(m_printf_func, {format_str, value});
        }
        else if (m_string_variables.find(current_value()) != m_string_variables.end()) 
        {
            std::string variable_name = current_value();
            llvm::Value* value = m_string_variables[variable_name];

            llvm::Value* format_str = m_builder.CreateGlobalStringPtr("%s\n");
            m_builder.CreateCall(m_printf_func, {format_str, value});
        }
        else
        {
            std::cerr << "Syntax error. Undefined identifier" << std::endl;
            exit(EXIT_FAILURE);
        } 
    }
    m_tokens_buffer.move_next();
    if(current_value() == ")")
    {
        m_tokens_buffer.move_next();
    }
    check_token_type(TokenType::SEMICOLON, "Expected for ';'");
    m_tokens_buffer.move_next();
}


void LLVMCompiler::process_mut() {
    m_tokens_buffer.move_next();
    check_token_type(TokenType::IDENTIFIER, "Expected identifier after 'mut'");
    std::string variable_name = current_value();

    if (m_constants.find(variable_name) != m_constants.end()) {
        std::cerr << "Error: Variable '" << variable_name << "' is constant and cannot be reassigned." << std::endl;
        exit(EXIT_FAILURE);
    }

    m_tokens_buffer.move_next();
    check_token_type(TokenType::ASSIGN, "Expected '=' after identifier");
    m_tokens_buffer.move_next();

    llvm::Value* value;
    std::string right_variable_name = current_value();

    if(current_type() == TokenType::QOUTE)
    {
        m_tokens_buffer.move_next();
        check_token_type(TokenType::STRING_LITERAL, "Expected string literal after '\"'");

        std::string string_literal = current_value();
        llvm::Value* str_ptr = m_builder.CreateGlobalStringPtr(string_literal);
        value = str_ptr;
        m_tokens_buffer.move_next();
        check_token_type(TokenType::QOUTE, "Expected '\"' after string literal");

        m_string_variables[variable_name] = value;
        m_tokens_buffer.move_next();
    }
    else if (current_type() == TokenType::IDENTIFIER && m_string_variables.find(right_variable_name) != m_string_variables.end())
    {
        llvm::Value* str_ptr_a = m_string_variables[current_value()];
        m_string_variables[variable_name] = str_ptr_a;
        m_tokens_buffer.move_next();
    }
    else
    {
        value = process_expr();
        value = process_arithmetic(value);

        set_variable(variable_name, value);
    }
    check_token_type(TokenType::SEMICOLON, "Expected for ';'");
    m_tokens_buffer.move_next();
}

void LLVMCompiler::process_const()
{
    m_tokens_buffer.move_next();
    check_token_type(TokenType::IDENTIFIER, "Expected identifier after 'const'");
    std::string variable_name = current_value();

    if(m_variables.find(variable_name) != m_variables.end())
    {
        std::cerr << "Syntax error. Redifinition of '" << variable_name << "'" << std::endl;
        exit(EXIT_FAILURE);
    }


    m_tokens_buffer.move_next();
    check_token_type(TokenType::ASSIGN, "Expected '=' after identifier");
    m_tokens_buffer.move_next();

    llvm::Value* value = nullptr;
    std::string right_variable_name = current_value();

    if(current_type() == TokenType::QOUTE)
    {
        m_tokens_buffer.move_next();
        check_token_type(TokenType::STRING_LITERAL, "Expected string literal after '\"'");

        std::string string_literal = current_value();
        llvm::Value* str_ptr = m_builder.CreateGlobalStringPtr(string_literal);
        value = str_ptr;
        m_tokens_buffer.move_next();
        check_token_type(TokenType::QOUTE, "Expected '\"' after string literal");

        m_string_variables[variable_name] = value;
        m_tokens_buffer.move_next();
    }
    else if (current_type() == TokenType::IDENTIFIER && m_string_variables.find(right_variable_name) != m_string_variables.end())
    {
        llvm::Value* str_ptr_a = m_string_variables[current_value()];
        m_string_variables[variable_name] = str_ptr_a;
        m_tokens_buffer.move_next();
    }
    else
    {
        value = process_expr();
        set_variable(variable_name, value);
    }

    m_constants.insert(variable_name);
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
        if(current_type() == TokenType::EXTERN_STD)
        {
            process_extern_std();
        }
        if(current_type() == TokenType::WRITELN)
        {
            if(std_externed)
            {
                process_writeln();
            }
            else
            {
                std::cerr << "Syntax error. No finded extern 'std'" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        if(current_type() == TokenType::MUT)
        {
            process_mut();
        }
        if(current_type() == TokenType::CONST)
        {
            process_const();
        }
        if(current_type() == TokenType::IDENTIFIER)
        {
            process_assign();
        }
        parse_pos++;
    }

    m_builder.CreateRet(llvm::ConstantInt::get(m_builder.getInt64Ty(), 0));
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