#pragma once

#include <config.h>

#define Debug Serial1

#if ENABLE_DEBUG
#define DebugPrint(X...) Serial1.print(X)
#define DebugPrintLn(X...) Serial1.println(X)
#else
#define DebugPrint(X...)
#define DebugPrintLn(X...)
#endif
