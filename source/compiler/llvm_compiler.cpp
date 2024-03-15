#include "llvm_compiler.hpp"
#include <cstdlib>
#include <llvm-16/llvm/ADT/APFloat.h>
#include <llvm-16/llvm/IR/Constant.h>
#include <llvm-16/llvm/IR/Constants.h>
#include <llvm-16/llvm/IR/GlobalVariable.h>
#include <llvm-16/llvm/IR/Type.h>
#include <llvm-16/llvm/IR/Value.h>
#include <llvm-16/llvm/Support/Casting.h>

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
                left_value = m_builder.CreateFAdd(left_value, temp, "add");
                break;
            case TokenType::MINUS:
                left_value = m_builder.CreateFSub(left_value, temp, "sub");
                break;
            case TokenType::MUL:
                left_value = m_builder.CreateFMul(left_value, temp, "mul");
                break;
            case TokenType::DIV:
                left_value = m_builder.CreateFDiv(left_value, temp, "div");
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
    if (m_variables.find(current_value()) == m_variables.end() && m_string_variables.find(current_value()) == m_string_variables.end() && m_boolean_variables.find(current_value()) == m_boolean_variables.end()) 
    {
        std::cerr << "Syntax error. Undefined identifier" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string left_identifier_name = current_value();

    m_tokens_buffer.move_next();
    check_token_type(TokenType::ASSIGN, "Expected '=' after identifier");
    m_tokens_buffer.move_next();

    if(current_type() == TokenType::CHECK)
    {
        process_check(left_identifier_name);
    }
    else if(current_type() == TokenType::QOUTE)
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
        else if(m_boolean_variables.find(left_identifier_name) != m_variables.end())
        {
            m_boolean_variables.erase(left_identifier_name);

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
    else if(current_type() == TokenType::TRUE || current_type() == TokenType::FALSE)
    {
        if(m_variables.find(left_identifier_name) != m_variables.end()) 
        {
            m_variables.erase(left_identifier_name);

            std::string current_bool = current_value();
            llvm::Value* str_ptr = m_builder.CreateGlobalStringPtr(current_bool);
            llvm::Value* value = str_ptr;

            m_boolean_variables[left_identifier_name] = value;

            m_tokens_buffer.move_next();
        }
        else if(m_boolean_variables.find(left_identifier_name) != m_boolean_variables.end())
        {
            m_boolean_variables.erase(left_identifier_name);

            std::string current_bool = current_value();
            llvm::Value* str_ptr = m_builder.CreateGlobalStringPtr(current_bool);
            llvm::Value* value = str_ptr;

            m_boolean_variables[left_identifier_name] = value;

            m_tokens_buffer.move_next();
        }
        else if(m_string_variables.find(left_identifier_name) != m_boolean_variables.end())
        {
            m_string_variables.erase(left_identifier_name);

            std::string current_bool = current_value();
            llvm::Value* str_ptr = m_builder.CreateGlobalStringPtr(current_bool);
            llvm::Value* value = str_ptr;

            m_boolean_variables[left_identifier_name] = value;

            m_tokens_buffer.move_next();
        }
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

    llvm::Type* int64_type = llvm::Type::getInt64Ty(m_context);
    return_value = m_builder.CreateFPToSI(return_value, int64_type, "castToInt64");

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
            value = m_builder.CreateFAdd(value, temp, "add");
        } 
        else if (operation == TokenType::MINUS) 
        {
            value = m_builder.CreateFSub(value, temp, "sub");
        }
    }

    return value;
}

llvm::Value* LLVMCompiler::process_term() {
    llvm::Value* value = process_factor(); 

    while (current_type() == TokenType::MUL || current_type() == TokenType::DIV) {
        TokenType operation = current_type();
        m_tokens_buffer.move_next(); 

        llvm::Value* temp = process_factor(); 
        if (operation == TokenType::MUL) 
        {
            value = m_builder.CreateFMul(value, temp, "mul");
        } 
        else if (operation == TokenType::DIV) 
        {
            value = m_builder.CreateFDiv(value, temp, "div");
        }
    }

    return value;
}

llvm::Value* LLVMCompiler::process_factor() {
    llvm::Value* value = nullptr;

    if (current_type() == TokenType::LPAREN) {
        return process_expr();    
    }
    else if (current_type() == TokenType::INT_LITERAL || current_type() == TokenType::FLOAT_LITERAL) 
    {
        value = llvm::ConstantFP::get(m_builder.getDoubleTy(), std::stod(current_value()));
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
    else if(current_type() == TokenType::INT_LITERAL || current_type() == TokenType::FLOAT_LITERAL || current_type() == TokenType::LPAREN)
    {
        llvm::Value* format_str = nullptr;
        if(current_type() == TokenType::INT_LITERAL)
        {
            format_str = m_builder.CreateGlobalStringPtr("%.0f\n");
        }
        else if(current_type() == TokenType::FLOAT_LITERAL)
        {
            format_str = m_builder.CreateGlobalStringPtr("%.6f\n");
        }
        else
        {
            format_str = m_builder.CreateGlobalStringPtr("%.6f\n");
        }
        llvm::Value* value = process_expr();

        value = process_arithmetic(value);

        m_builder.CreateCall(m_printf_func, {format_str, value});

    }
    else if(current_type() == TokenType::TRUE || current_type() == TokenType::FALSE)
    {
        llvm::Value* format_str = nullptr;
        std::string bool_string = current_type() == TokenType::TRUE ? "true" : "false";
        format_str = m_builder.CreateGlobalStringPtr("%s\n");
        llvm::Value* bool_str_ptr = m_builder.CreateGlobalStringPtr(bool_string);
        m_builder.CreateCall(m_printf_func, {format_str, bool_str_ptr});

        m_tokens_buffer.move_next();
    }
    else if(current_type() == TokenType::IDENTIFIER)
    {
        if (m_variables.find(current_value()) != m_variables.end()) 
        {
            std::string variable_name = current_value();

            llvm::Value* format_str = nullptr;
            llvm::Value* value = get_variable(variable_name);
            llvm::Type* value_type = value->getType();

            if(value_type->isFloatingPointTy())
            {
                format_str = m_builder.CreateGlobalStringPtr("%lf\n"); 
            }
            else
            {
                format_str = m_builder.CreateGlobalStringPtr("%d\n");
            }

            m_builder.CreateCall(m_printf_func, {format_str, value});
        }
        else if(m_boolean_variables.find(current_value()) != m_boolean_variables.end())
        {
            std::string variable_name = current_value();
            llvm::Value* value = m_boolean_variables[variable_name];

            llvm::Value* format_str = m_builder.CreateGlobalStringPtr("%s\n");
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

void LLVMCompiler::process_check(const std::string& left_variable_name)
{
    m_tokens_buffer.move_next();

    llvm::Value* left_expression = nullptr;
    llvm::Value* right_expression = nullptr;

    if(m_variables.find(left_variable_name) != m_variables.end())
    {
        m_variables.erase(left_variable_name);
    }
    else if(m_string_variables.find(left_variable_name) != m_string_variables.end())
    {
        m_string_variables.erase(left_variable_name);
    }
    else if(m_boolean_variables.find(left_variable_name) != m_boolean_variables.end())
    {
        m_boolean_variables.erase(left_variable_name);
    }
    if(current_type() == TokenType::FLOAT_LITERAL || current_type() == TokenType::INT_LITERAL || current_type() == TokenType::LPAREN
    || m_variables.find(current_value()) != m_variables.end())
    {
        if(current_type() == TokenType::IDENTIFIER && m_variables.find(current_value()) == m_variables.end())
        {
            std::cerr << "Undefined identifier: " << current_value() << std::endl;
            exit(EXIT_FAILURE);
        }

        left_expression = process_expr();
        left_expression = process_arithmetic(left_expression);

        if(current_type() == TokenType::EQUAL || current_type() == TokenType::NOT_EQUAL)
        {
            TokenType equal_or_not_equal = current_type();
            m_tokens_buffer.move_next();
            if(current_type() == TokenType::IDENTIFIER && m_variables.find(current_value()) == m_variables.end())
            {
                std::cerr << "Undefined data type: " << current_value() << std::endl;
                exit(EXIT_FAILURE);
 
            }
            if(current_type() == TokenType::FLOAT_LITERAL || current_type() == TokenType::INT_LITERAL || current_type() == TokenType::IDENTIFIER || current_type() == TokenType::LPAREN)
            {
                right_expression = process_expr();
                right_expression = process_arithmetic(right_expression);

                llvm::APFloat left_expression_value = llvm::cast<llvm::ConstantFP>(left_expression)->getValueAPF();
                llvm::APFloat right_expression_value = llvm::cast<llvm::ConstantFP>(right_expression)->getValueAPF();

                bool is_values_equal = left_expression_value.bitwiseIsEqual(right_expression_value);

                std::string bool_string = equal_or_not_equal == TokenType::EQUAL ? is_values_equal ? "true" : "false"
                    : !is_values_equal ? "true" : "false";

                llvm::Value* bool_str_ptr = m_builder.CreateGlobalStringPtr(std::move(bool_string));

                m_boolean_variables[left_variable_name] = bool_str_ptr;
            }
            else
            {
                std::cerr << "Syntax error. Expected for float or literal identifier or literal after '=='" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        else if(current_type() == TokenType::LESS || current_type() == TokenType::MORE)
        {
            TokenType less_or_more = current_type();
            m_tokens_buffer.move_next();

            if(current_type() == TokenType::FLOAT_LITERAL || current_type() == TokenType::INT_LITERAL || current_type() == TokenType::IDENTIFIER)
            {
                right_expression = process_expr();
                right_expression = process_arithmetic(right_expression);

                llvm::APFloat left_expression_value = llvm::cast<llvm::ConstantFP>(left_expression)->getValueAPF();
                llvm::APFloat right_expression_value = llvm::cast<llvm::ConstantFP>(right_expression)->getValueAPF();

                bool is_less_or_more = less_or_more == TokenType::LESS ? left_expression_value.compare(right_expression_value) == llvm::APFloat::cmpLessThan
                    : left_expression_value.compare(right_expression_value) == llvm::APFloat::cmpGreaterThan;

                std::string bool_string = is_less_or_more ? "true" : "false";
                llvm::Value* bool_str_ptr = m_builder.CreateGlobalStringPtr(std::move(bool_string));

                m_boolean_variables[left_variable_name] = bool_str_ptr;
            }
        }
        else
        {
            std::cerr << "Unexpected logical operator '" << current_value() << "'" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else if (m_string_variables.find(current_value()) != m_string_variables.end() && current_type() == TokenType::IDENTIFIER) 
    {
        left_expression = m_string_variables[current_value()];

        m_tokens_buffer.move_next();

        if (current_type() == TokenType::EQUAL || current_type() == TokenType::NOT_EQUAL) {
            m_tokens_buffer.move_next();

            if (current_type() == TokenType::QOUTE) {
                m_tokens_buffer.move_next();

                std::string right_string_literal = current_value();
                m_tokens_buffer.move_next();
                m_tokens_buffer.move_next();

                right_expression = m_builder.CreateGlobalStringPtr(std::move(right_string_literal));

                if (llvm::GlobalVariable* global_left_string = llvm::dyn_cast<llvm::GlobalVariable>(left_expression)) 
                {
                    if (llvm::GlobalVariable* global_right_string = llvm::dyn_cast<llvm::GlobalVariable>(right_expression)) 
                    {
                        llvm::ConstantDataSequential* global_left_string_data = llvm::dyn_cast<llvm::ConstantDataSequential>(global_left_string->getInitializer());
                        llvm::ConstantDataSequential* global_right_string_data = llvm::dyn_cast<llvm::ConstantDataSequential>(global_right_string->getInitializer());

                        if (global_left_string_data && global_right_string_data) 
                        {
                            bool is_strings_equal = global_left_string_data->getRawDataValues() == global_right_string_data->getRawDataValues();

                            std::string bool_string = is_strings_equal ? "true" : "false";
                            llvm::Value* bool_str_ptr = m_builder.CreateGlobalStringPtr(std::move(bool_string));

                            m_boolean_variables[left_variable_name] = bool_str_ptr;

                        }
                    }
                }
            }
            else if(current_type() == TokenType::IDENTIFIER && m_string_variables.find(current_value()) != m_string_variables.end())
            {
                right_expression = m_string_variables[current_value()];

                if (llvm::GlobalVariable* global_left_string = llvm::dyn_cast<llvm::GlobalVariable>(left_expression)) 
                {
                    if (llvm::GlobalVariable* global_right_string = llvm::dyn_cast<llvm::GlobalVariable>(right_expression)) 
                    {
                        llvm::ConstantDataSequential* global_left_string_data = llvm::dyn_cast<llvm::ConstantDataSequential>(global_left_string->getInitializer());
                        llvm::ConstantDataSequential* global_right_string_data = llvm::dyn_cast<llvm::ConstantDataSequential>(global_right_string->getInitializer());

                        if (global_left_string_data && global_right_string_data) 
                        {
                            bool is_strings_equal = global_left_string_data->getRawDataValues() == global_right_string_data->getRawDataValues();

                            std::string bool_string = is_strings_equal ? "true" : "false";
                            llvm::Value* bool_str_ptr = m_builder.CreateGlobalStringPtr(std::move(bool_string));

                            m_boolean_variables[left_variable_name] = bool_str_ptr;

                        }
                    }
                }

                m_tokens_buffer.move_next();
            }
            else
            {
                std::cerr << "Syntax error. You can equal only string literals or string variables." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }

    else if(current_type() == TokenType::QOUTE)
    {
        m_tokens_buffer.move_next();
        std::string left_string_literal = current_value();
        m_tokens_buffer.move_next();
        m_tokens_buffer.move_next();

        if(current_type() == TokenType::EQUAL || current_type() == TokenType::NOT_EQUAL)
        {
            m_tokens_buffer.move_next();

            if(current_type() == TokenType::QOUTE)
            {
                m_tokens_buffer.move_next();
                std::string right_string_literal = current_value();
                m_tokens_buffer.move_next();

                bool strings_equal = left_string_literal == right_string_literal;

                std::string bool_string = strings_equal ? "true" : "false";
                llvm::Value* bool_str_ptr = m_builder.CreateGlobalStringPtr(std::move(bool_string));

                m_boolean_variables[left_variable_name] = bool_str_ptr;
            }
            else if (current_type() == TokenType::IDENTIFIER && m_string_variables.find(current_value()) != m_string_variables.end()) 
            {
                right_expression = m_string_variables[current_value()];  
                left_expression = m_builder.CreateGlobalStringPtr(left_string_literal);

                if (llvm::GlobalVariable* global_right_string = llvm::dyn_cast<llvm::GlobalVariable>(right_expression)) 
                {
                    if (llvm::GlobalVariable* global_left_string = llvm::dyn_cast<llvm::GlobalVariable>(left_expression)) 
                    {
                        llvm::ConstantDataSequential* global_right_string_data = llvm::dyn_cast<llvm::ConstantDataSequential>(global_right_string->getInitializer());
                        llvm::ConstantDataSequential* global_left_string_data = llvm::dyn_cast<llvm::ConstantDataSequential>(global_left_string->getInitializer());

                        if (global_right_string_data && global_left_string_data) 
                        {
                            bool is_strings_equal = global_right_string_data->getRawDataValues() == global_left_string_data->getRawDataValues();

                            std::string bool_string = is_strings_equal ? "true" : "false";
                            llvm::Value* bool_str_ptr = m_builder.CreateGlobalStringPtr(std::move(bool_string));

                            m_boolean_variables[left_variable_name] = bool_str_ptr;
                        }
                        
                    }
                }
            }
           m_tokens_buffer.move_next();
        }
        else
        {
            std::cerr << "Unexpected logical operator '" << current_value() << "'" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else if(current_type() == TokenType::TRUE || current_type() == TokenType::FALSE || m_boolean_variables.find(current_value()) != m_boolean_variables.end())
    {
        std::string left_string_literal = current_value();

        m_tokens_buffer.move_next();

        if(current_type() == TokenType::EQUAL || current_type() == TokenType::NOT_EQUAL)
        {
            m_tokens_buffer.move_next();

            if(current_type() == TokenType::TRUE || current_type() == TokenType::FALSE)
            {
                std::string right_string_literal = current_value();
                m_tokens_buffer.move_next();

                bool strings_equal = left_string_literal == right_string_literal;

                std::string bool_string = strings_equal ? "true" : "false";
                llvm::Value* bool_str_ptr = m_builder.CreateGlobalStringPtr(std::move(bool_string));

                m_boolean_variables[left_variable_name] = bool_str_ptr;
            }
            else if(current_type() == TokenType::IDENTIFIER && m_boolean_variables.find(current_value()) != m_boolean_variables.end())
            {
                right_expression = m_boolean_variables[current_value()];  
                left_expression = m_builder.CreateGlobalStringPtr(left_string_literal);

                if (llvm::GlobalVariable* global_right_string = llvm::dyn_cast<llvm::GlobalVariable>(right_expression)) 
                {
                    if (llvm::GlobalVariable* global_left_string = llvm::dyn_cast<llvm::GlobalVariable>(left_expression)) 
                    {
                        llvm::ConstantDataSequential* global_right_string_data = llvm::dyn_cast<llvm::ConstantDataSequential>(global_right_string->getInitializer());
                        llvm::ConstantDataSequential* global_left_string_data = llvm::dyn_cast<llvm::ConstantDataSequential>(global_left_string->getInitializer());

                        if (global_right_string_data && global_left_string_data) 
                        {
                            bool is_strings_equal = global_right_string_data->getRawDataValues() == global_left_string_data->getRawDataValues();

                            std::string bool_string = is_strings_equal ? "true" : "false";
                            llvm::Value* bool_str_ptr = m_builder.CreateGlobalStringPtr(std::move(bool_string));

                            m_boolean_variables[left_variable_name] = bool_str_ptr;
                            m_tokens_buffer.move_next();
                        }
                        
                    }
                }
            }

        }
    }
    else
    {
        std::cerr << "Unexpected identifier or symbol after '?'" << std::endl;
        exit(EXIT_FAILURE);
    }
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

    if(current_type() == TokenType::CHECK)
    {
        process_check(std::move(variable_name));
    }
    else if(current_type() == TokenType::QOUTE)
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
    else if(current_type() == TokenType::IDENTIFIER && m_boolean_variables.find(right_variable_name) != m_boolean_variables.end())
    {
        llvm::Value* str_ptr_a = m_boolean_variables[current_value()];
        m_boolean_variables[variable_name] = str_ptr_a;
        m_tokens_buffer.move_next();
    }
    else if (current_type() == TokenType::TRUE || current_type() == TokenType::FALSE)
    {
        std::string bool_string = current_type() == TokenType::TRUE ? "true" : "false";
        llvm::Value* bool_str_ptr = m_builder.CreateGlobalStringPtr(bool_string);

        m_boolean_variables[variable_name] = bool_str_ptr;

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
    else if(current_type() == TokenType::CHECK)
    {
        process_check(std::move(variable_name));
    }
    else if (current_type() == TokenType::IDENTIFIER && m_string_variables.find(right_variable_name) != m_string_variables.end())
    {
        llvm::Value* str_ptr_a = m_string_variables[current_value()];
        m_string_variables[variable_name] = str_ptr_a;
        m_tokens_buffer.move_next();
    }
    else if (current_type() == TokenType::TRUE || current_type() == TokenType::FALSE)
    {
        std::string bool_string = current_type() == TokenType::TRUE ? "true" : "false";
        llvm::Value* bool_str_ptr = m_builder.CreateGlobalStringPtr(bool_string);

        m_boolean_variables[variable_name] = bool_str_ptr;

        m_tokens_buffer.move_next();
    }
    else
    {
        value = process_expr();
        value = process_arithmetic(value);

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

    for (; parse_pos < m_tokens_buffer.m_tokens.size(); ++parse_pos) 
    {
        switch (current_type()) 
        {
            case TokenType::EXIT:
            {
                m_builder.CreateRet(process_exit());
                break;
            }
            case TokenType::EXTERN_STD:
            {
                process_extern_std();
                break;
            }
            case TokenType::WRITELN:
            {
                std_externed ? process_writeln() : (std::cerr << "Syntax error. No 'std' extern found" << std::endl, exit(EXIT_FAILURE));
                break;
            }
            case TokenType::MUT:
            {
                process_mut();
                break;
            }
            case TokenType::CONST:
            {
                process_const();
                break;
            }
            case TokenType::IDENTIFIER:
            {
                process_assign();
                break;
            }
            default:
                break;
        }
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