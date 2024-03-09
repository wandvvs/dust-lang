#pragma once

#include <stdexcept>
#include <string>
#include <fstream>

class LLVMExecutableBuilder
{
private:
    std::string m_llvm_ir;
    std::string m_output_file;

    void write_llvm_ir_to_file() const;
    void convert_ir_to_object() const;
    void link_objects() const;

public:
    LLVMExecutableBuilder(std::string llvm_ir, std::string output_file);

    void build_executable() const;
};