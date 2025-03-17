#pragma once

#include "ir.h"
#include "codegen.h"

cabor_x64_assembly* cabor_compile(const char* code, const char* filename);
void cabor_write_asmbl_to_file(const char* filename, cabor_x64_assembly* asmbl);

