/*
======================================================================
Common helper Library
Copyright 2002,  NewTek, Inc.

com_math.c

Simple math/number manipulation functions.
====================================================================== */
#include "com_math.h"
#include <limits.h>

const LWFVector LWVEC_ZERO_F = {0.0f, 0.0f, 0.0f};
const LWFVector LWVEC_ONES_F = {1.0f, 1.0f, 1.0f};

const LWDVector LWVEC_ZERO_D = {0.0, 0.0, 0.0};
const LWDVector LWVEC_ONES_D = {1.0, 1.0, 1.0};


const LWFMatrix3 LWMAT_IDENT3_F = {1.0f, 0.0f, 0.0f, 
								 0.0f, 1.0f, 0.0f, 
								 0.0f, 0.0f, 1.0f};
const LWFMatrix4 LWMAT_IDENT4_F = {1.0f, 0.0f, 0.0f, 0.0f,
								 0.0f, 1.0f, 0.0f, 0.0f,
								 0.0f, 0.0f, 1.0f, 0.0f,
								 0.0f, 0.0f, 0.0f, 1.0f};
const LWDMatrix3 LWMAT_IDENT3_D = {1.0, 0.0, 0.0, 
								 0.0, 1.0, 0.0, 
								 0.0, 0.0, 1.0};
const LWDMatrix4 LWMAT_IDENT4_D = {1.0, 0.0, 0.0, 0.0,
								 0.0, 1.0, 0.0, 0.0,
								 0.0, 0.0, 1.0, 0.0,
								 0.0, 0.0, 0.0, 1.0};

int FLTBYTE( float v)
{
	int t = (int)((v)*(float)UCHAR_MAX + 0.5f);
	return CLAMP(t,0,UCHAR_MAX);
}

int FLTSHORT( float v)
{
	int t = (int)((v)*(float)USHRT_MAX + 0.5f);
	return CLAMP(t,0,USHRT_MAX);
}

int FLTLONG( float v)
{
	int t = (int)((v)*(float)INT_MAX + 0.5f);
	return CLAMP(t,0,INT_MAX);
}

float BYTEFLOAT(unsigned char b)
{
	float t = ((float)b)/(float)UCHAR_MAX;
	return CLAMP(t, 0.0f, 1.0f);
}

float SHORTFLOAT(unsigned short s)
{
	float t = ((float)s)/(float)USHRT_MAX;
	return CLAMP(t, 0.0f, 1.0f);
}

float LONGFLOAT(unsigned long l)
{
	float t = ((float)l)/(float)INT_MAX;
	return CLAMP(t, 0.0f, 1.0f);
}
