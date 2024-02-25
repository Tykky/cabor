#pragma once

#include "../cabor_defines.h"
#include "../logging/logging.h"
#include <stdlib.h>
#include <assert.h>

#ifndef NDEBUG
#define CABOR_ASSERT(eval, reason) assert(eval && reason)
#else
#define CABOR_ASSERT(eval, reason)
#endif

#ifdef CABOR_ENABLE_BREAK_ON_RUNTIME_ERROR
#define CABOR_DEBUG_BREAK assert(0)
#else
#define CABOR_DEBUG_BREAK
#endif

#define CABOR_RUNTIME_ERROR(reason) CABOR_LOG_ERR_F("%s:%d in '%s': %s", __FILE__, __LINE__, __func__, reason); CABOR_DEBUG_BREAK; CABOR_EXIT(1)
#define CABOR_TEST_ERROR(reason) CABOR_LOG_ERR_F("-- %s:%d in '%s': %s", __FILE__, __LINE__, __func__, reason); CABOR_DEBUG_BREAK;
#define CABOR_EXIT(value) exit(value)
