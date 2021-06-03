#pragma once

/* Mask printf and fprintf for CMD check */
#include <Rinternals.h>
#define printf Rprintf

#ifdef __cplusplus
extern "C" {
#endif
  void Rprintf2(FILE * stream, const char *format, ...);
#ifdef __cplusplus
}
#endif

#define fprintf Rprintf2
#undef stderr
#define stderr NULL
