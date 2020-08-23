#pragma once

#ifdef __GNUC__
#define IXO_UNREACHABLE() __builtin_unreachable()
#else
#define IXO_UNREACHABLE() ((void)0)
#endif
