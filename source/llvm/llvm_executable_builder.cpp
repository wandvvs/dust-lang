#include "llvm_executable_builder.hpp"

LLVMExecutableBuilder::LLVMExecutableBuilder(std::string llvm_ir, std::string output_file)
    : m_llvm_ir(std::move(llvm_ir)), m_output_file(std::move(output_file)) 
    {
    }

void LLVMExecutableBuilder::write_llvm_ir_to_file() const
{

    std::fstream out(m_output_file, std::ios::out);

    if(!out.is_open())
    {
        throw std::runtime_error("Failed to open output file for writing.");
    }

    out << m_llvm_ir;
}

void LLVMExecutableBuilder::convert_ir_to_object() const
{
    std::string command = "llc -filetype=obj " + m_output_file + " -o " + m_output_file + ".o";
    int result = system(command.c_str());

    if(result != 0)
    {
        throw std::runtime_error("Failed convert LLVM IR to object file.");
    }
}

void LLVMExecutableBuilder::link_objects() const
{
    std::string command = "clang " + m_output_file + ".o -o " + m_output_file;
    int result = system(command.c_str());

    if(result != 0)
    {
        throw std::runtime_error("Failed to link objects");
    }
}

void LLVMExecutableBuilder::build_executable() const
{
    write_llvm_ir_to_file();
    convert_ir_to_object();
    link_objects();
}