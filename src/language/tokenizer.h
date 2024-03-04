#pragma once

#include "../core/vector.h"

#define TOKENIZER_VECTOR_DEFAULT_CAPACITY 1024

cabor_vector tokenize(const char* buffer, size_t size);
