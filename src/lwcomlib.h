/*
======================================================================
Common helper Library
Copyright 2002,  NewTek, Inc.

lwcomlib.h

Definitions and prototypes for the Common helper library.

Nelson Yu	19 Dec 2001
====================================================================== */

#ifndef LWCOMLIB_H
#define LWCOMLIB_H

#ifndef __cplusplus
	#ifndef FALSE
		#define FALSE 0
	#endif

	#ifndef TRUE
		#define TRUE !FALSE
	#endif
#else
	#ifndef FALSE
		#define FALSE false
	#endif

	#ifndef TRUE
		#define TRUE true
	#endif
#endif

#define MAX_STRING_SIZE 256
#define DEFAULT_CHANNELS 9

#endif
