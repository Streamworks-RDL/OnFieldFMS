#ifndef __VERBOSE_PRINT_H__
#define __VERBOSE_PRINT_H__

#include <stdio.h>

#ifdef VERBOSE
#define vprint(...) printf(__VA_ARGS__);
#else
#define vprint(...)
#endif

#endif
