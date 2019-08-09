#ifndef __MINERUTILS_H
#define __MINERUTILS_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

// Endian swapping routines
uint32_t BSWAP32(uint32_t data);
void SwapBuffer32(void *data, int chunks);

// ASCII <-> binary conversion routines
int ASCIIHexToBinary(void * rawstr, const char * asciistr, size_t len);
void BinaryToASCIIHex(char * asciistr, const void * rawstr, size_t len);

// File reading routine
size_t LoadTextFile(char **Output, char *Filename);

// Difficulty conversion & validity testing routines
void CreateTargetFromDiff(uint32_t *FullTarget, double Diff);
bool FullTest(const uint32_t *Hash, const uint32_t *FullTarget);

// Time routines

#ifdef __linux__

#define TIME_TYPE	struct timespec

#else

#define TIME_TYPE	clock_t

#endif

TIME_TYPE MinerGetCurTime(void);
double SecondsElapsed(TIME_TYPE Start, TIME_TYPE End);


void sleep(uint32_t ms);
bool  atomic_load(bool *ptr);
void  atomic_store(bool *ptr, bool val);
int32_t strcasecmp( char *s1,  char *v2);
extern uint32_t BSWAP32(uint32_t x);
#endif
