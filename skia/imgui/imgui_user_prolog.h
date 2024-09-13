#pragma once

#include "imzero_hooks.h"
#include "imzero_extensions.h"

#ifdef IMZERO_DEBUG_BUILD
//---- Define assertion handler. Defaults to calling assert().
// If your macro uses multiple statements, make sure is enclosed in a 'do { .. } while (0)' block so it can be used as a single statement.
//#define IM_ASSERT(_EXPR)  MyAssert(_EXPR)
//#define IM_ASSERT(_EXPR)  ((void)(_EXPR))     // Disable asserts
#define IM_ASSERT(_EXPR) ((_EXPR) ? (void)(0) : imzeroAssert(__func__,__FILE__,__LINE__))
#endif