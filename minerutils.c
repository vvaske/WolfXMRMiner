#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "minerutils.h"

// Parameter len is bytes in rawstr, therefore, asciistr must have
// at least (len << 1) + 1 bytes allocated, the last for the NULL
void BinaryToASCIIHex(char * asciistr, const void * rawstr, size_t len)
{
	for(int i = 0, j = 0; i < len; ++i)
	{
		asciistr[j++] = "0123456789abcdef"[((uint8_t *)rawstr)[i] >> 4];
		asciistr[j++] = "0123456789abcdef"[((uint8_t *)rawstr)[i] & 0x0F];
	}
		
	asciistr[len << 1] = 0x00;
}

// Parameter len is the size in bytes of asciistr, meaning rawstr
// must have (len >> 1) bytes allocated
// Maybe asciistr just NULL terminated?
// Returns length of rawstr in bytes
int ASCIIHexToBinary(void * rawstr, const char * asciistr, size_t len)
{
	for(int i = 0, j = 0; i < len; ++i)
	{
		char tmp = asciistr[i];
		if(tmp < 'A') tmp -= '0';
		else if(tmp < 'a') tmp = (tmp - 'A') + 10;
		else tmp = (tmp - 'a') + 10;
		
		if(i & 1) ((uint8_t *)rawstr)[j++] |= tmp & 0x0F;
		else ((uint8_t *)rawstr)[j] = tmp << 4;
	}
	
	return(len >> 1);
}

uint32_t BSWAP32(uint32_t x)
{
	#ifdef __GNUC__
	return(__builtin_bswap32(x));
	#else
	return _byteswap_ulong(x);
		/*
		((x << 24) & 0xff000000) |
		((x << 8) & 0x00ff0000) |
		((x >> 8) & 0x0000ff00) |
		((x >> 24) & 0x000000ff); */
	#endif
}

// Parameter chunks is the number of 32-bit chunks in the buffer
// Buffer length MUST be divisible by 4 bytes, or last chunk
// should be omitted (for our uses, this should never happen)
void SwapBuffer32(void *data, int chunks)
{
	for(int i = 0; i < chunks; ++i) ((uint32_t *)data)[i] = BSWAP32(((uint32_t *)data)[i]);
}

// Pointer returned in Output must be freed
size_t LoadTextFile(char **Output, char *Filename)
{
	size_t len;
	FILE *kernel = fopen(Filename, "rb");
	
	fseek(kernel, 0, SEEK_END);
	len = ftell(kernel);
	fseek(kernel, 0, SEEK_SET);
	
	*Output = (char *)malloc(sizeof(char) * (len + 2));
	len = fread(*Output, sizeof(char), len, kernel);
	Output[0][len] = 0x00;		// NULL terminator
	fclose(kernel);
	
	return(len);
}

bool FullTest(const uint32_t *Hash, const uint32_t *FullTarget)
{
	bool ret;
	
	for(int i = 7; i >= 0; --i)
	{
		ret = Hash[i] <= FullTarget[i];
		if(Hash[i] != FullTarget[i]) break;
	}
	
	return(ret);
}

void CreateTargetFromDiff(uint32_t *FullTarget, double Diff)
{
	int i;
	uint64_t tmp;
	
	for(i = 6; i > 0 && Diff > 1.0; --i) Diff /= 4294967296.0;
	
	tmp = 4294901760.0 / Diff;
	
	if(!tmp && i == 6)
	{
		memset(FullTarget, 0xFF, 32);
	}
	else
	{
		memset(FullTarget, 0x00, 32);
		
		FullTarget[i] = (uint32_t)tmp;
		FullTarget[i + 1] = (uint32_t)(tmp >> 32);
	}
}

#ifdef __linux__

TIME_TYPE MinerGetCurTime(void)
{
	TIME_TYPE CurTime;
	clock_gettime(CLOCK_REALTIME, &CurTime);
	return(CurTime);
}

double SecondsElapsed(TIME_TYPE Start, TIME_TYPE End)
{
	double NanosecondsElapsed = 1e9 * (double)(End.tv_sec - Start.tv_sec) + (double)(End.tv_nsec - Start.tv_nsec);
	return(NanosecondsElapsed * 1e-9);
}

#else

TIME_TYPE MinerGetCurTime(void)
{
	return(clock());
}

double SecondsElapsed(TIME_TYPE Start, TIME_TYPE End)
{
	return((double)(End - Start) / CLOCKS_PER_SEC);
}



#endif

#ifdef __linux__

void Sleep(uint32_t ms)
{
	struct timespec t;
	t.tv_sec = ms / 1000;
	t.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&t, NULL);
}

#endif

#include "synchapi.h"
void sleep(uint32_t ms)
{
	Sleep(ms);
};

bool  atomic_load(bool *ptr)
{
	return *ptr;
};

void  atomic_store(bool *ptr, bool val)
{
	*ptr = val;
};

int32_t strcasecmp(char *s1, char *s2)
{
	while (*s1 && *s2)
	{
		if (toupper(*s1) == toupper(*s2))
		{
			s1++; s2++;
		}
		else
			break;
	}
	return (s1[0]-s2[0]);
};
