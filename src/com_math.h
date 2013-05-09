/*
Common helper Library
Copyright 2002,  NewTek, Inc.
*/
#ifndef LWCOM_MATH_H
#define LWCOM_MATH_H

// Define LW_PLUGIN for plugins, otherwise the types will redefined.
#ifdef LW_PLUGIN
	#include <lwtypes.h>
	#include <lwmath.h>
#else
	typedef float		 LWFVector[3];
	typedef double		 LWDVector[3];

	// copied from lwmath.h directly - should be the same

	#ifndef PI
		#define PI     (3.14159265358979324)
	#endif
	#ifndef TWOPI
		#define TWOPI  (2.0 * PI)
	#endif
	#ifndef HALFPI
		#define HALFPI (0.5 * PI)
	#endif

	#define VSET(a,x)       ((a)[0]=(x), (a)[1]=(x), (a)[2]=(x))
	#define VCLR(a)         VSET(a,0.0)
	#define VCPY(a,b)       ((a)[0] =(b)[0], (a)[1] =(b)[1], (a)[2] =(b)[2])
	#define VSCL(a,x)       ((a)[0]*= (x),   (a)[1]*= (x),   (a)[2]*= (x))
	#define VADD(a,b)       ((a)[0]+=(b)[0], (a)[1]+=(b)[1], (a)[2]+=(b)[2])
	#define VSUB(a,b)       ((a)[0]-=(b)[0], (a)[1]-=(b)[1], (a)[2]-=(b)[2])
	#define VADDS(a,b,x)    ((a)[0]+=(b)[0]*(x), (a)[1]+=(b)[1]*(x), (a)[2]+=(b)[2]*(x))
	#define VSCL3(r,a,x)    ((r)[0]=(a)[0]*(x),    (r)[1]=(a)[1]*(x),    (r)[2]=(a)[2]*(x))
	#define VADD3(r,a,b)    ((r)[0]=(a)[0]+(b)[0], (r)[1]=(a)[1]+(b)[1], (r)[2]=(a)[2]+(b)[2])
	#define VSUB3(r,a,b)    ((r)[0]=(a)[0]-(b)[0], (r)[1]=(a)[1]-(b)[1], (r)[2]=(a)[2]-(b)[2])
	#define VADDS3(r,a,b,x) ((r)[0]=(a)[0]+(b)[0]*(x), (r)[1]=(a)[1]+(b)[1]*(x), (r)[2]=(a)[2]+(b)[2]*(x))
	#define VDOT(a,b)       ((a)[0]*(b)[0] + (a)[1]*(b)[1] + (a)[2]*(b)[2])
	#define VLEN(a)         sqrt(VDOT(a,a))
	#define VCROSS(r,a,b)   ((r)[0] = (a)[1]*(b)[2] - (a)[2]*(b)[1],\
				 (r)[1] = (a)[2]*(b)[0] - (a)[0]*(b)[2],\
				 (r)[2] = (a)[0]*(b)[1] - (a)[1]*(b)[0])
	#define VMUL3(r,a,b) ((r)[0]=(a)[0]*(b)[0], (r)[1]=(a)[1]*(b)[1], (r)[2]=(a)[2]*(b)[2])

	#ifndef	ABS
	#define ABS(a) ((a < 0) ? (-(a)) : (a))
	#endif

	#ifndef	MAX
	#define MAX(a,b) ((a) > (b) ? (a) : (b))
	#endif

	#ifndef	MIN
	#define MIN(a,b) ((a) < (b) ? (a) : (b))
	#endif

	#define CLAMP(a,b,c) (((a) < (b)) ? (b) : (((a) > (c)) ? (c) : (a)))
	#define SWAP(a,b)	{ a^=b; b^=a; a^=b; }

	#define RADIANS(deg)		((deg)*0.017453292519943295769236907684886)
	#define DEGREES(rad)		((rad)*57.2957795130823208767981548141052)
#endif

#define VSET3(a,x,y,z)		((a)[0]=(x), (a)[1]=(y), (a)[2]=(z))
#define VLEN_SQUARED(a)		(VDOT(a,a))
#define VNEG(a)				((a)[0] = -(a)[0], (a)[1]=-(a)[1], (a)[2]=-(a)[2])

typedef float		 LWFQuat[4];
typedef double		 LWDQuat[4];

#define QSET(a,x)			((a)[0]=(x), (a)[1]=(x), (a)[2]=(x), (a)[3]=(x))
#define QSET4(a,x,y,z,w)	((a)[0]=(x), (a)[1]=(y), (a)[2]=(z), (a)[3]=(w))

typedef float LWFMatrix3[ 3 ][ 3 ];
typedef float LWFMatrix4[ 4 ][ 4 ];
typedef double LWDMatrix3[ 3 ][ 3 ];
typedef double LWDMatrix4[ 4 ][ 4 ];

#define EPSILON_F  1e-6
#define EPSILON_D  1e-15

#define SQR(x)	((x) * (x))
#define NEAR_ZERO_F(x)	(ABS(x) < EPSILON_F)
#define NEAR_ZERO_D(x)	(ABS(x) < EPSILON_D)
#define EQUALS_F(x,y)		(NEAR_ZERO_F((x) - (y)))
#define EQUALS_D(x,y)		(NEAR_ZERO_D((x) - (y)))

extern const LWFVector LWVEC_ZERO_F;
extern const LWFVector LWVEC_ONES_F;
extern const LWDVector LWVEC_ZERO_D;
extern const LWDVector LWVEC_ONES_D;

extern const LWFMatrix3 LWMAT_IDENT3_F;
extern const LWFMatrix4 LWMAT_IDENT4_F;
extern const LWDMatrix3 LWMAT_IDENT3_D;
extern const LWDMatrix4 LWMAT_IDENT4_D;


int FLTBYTE(float v);
int FLTSHORT(float v);
int FLTLONG( float v);

float BYTEFLOAT(unsigned char b);
float SHORTFLOAT(unsigned short s);
float LONGFLOAT(unsigned long l);

#endif
