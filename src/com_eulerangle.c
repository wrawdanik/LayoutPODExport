/*
======================================================================
Common helper Library
Copyright 2002,  NewTek, Inc.

com_eulerangle.c

Based on Ken Shoemake work in 1993.

Convert Euler angles to/from matrix or quaternions.
Support for 24 angle schemes.
====================================================================== */

#include "lwcomlib.h"
#include "com_math.h"
#include "com_vecmatquat.h"
#include "com_eulerangle.h"
#include <math.h>	
#include <float.h>

static int X = 0;
static int Y = 1;
static int Z = 2;
static int W = 3;

#define EulHPB		  EulOrdYXZr 

/* Right-handed, for column vectors */

LWEUL_init(LWEulerAngles ea, float ai, float aj, float ah, int order)
{
	EULSET4(ea, ai, aj, ah, order);
}

/* Construct quaternion from Euler angles (in radians). */
void LWEUL_toQuat(LWEulerAngles ea, LWFQuat qu)
{
    float a[3], ti, tj, th, ci, cj, ch, si, sj, sh, cc, cs, sc, ss;
    int i,j,k,h,n,s,f;
    EulGetOrd(((unsigned)ea[W]),i,j,k,h,n,s,f);
    if (f==EulFrmR) {float t = ea[X]; ea[X] = ea[Z]; ea[Z] = t;}
    if (n==EulParOdd) ea[Y] = -ea[Y];
    ti = ea[X]*0.5f; tj = ea[Y]*0.5f; th = ea[Z]*0.5f;
    ci = cos(ti);  cj = cos(tj);  ch = cos(th);
    si = sin(ti);  sj = sin(tj);  sh = sin(th);
    cc = ci*ch; cs = ci*sh; sc = si*ch; ss = si*sh;
    if (s==EulRepYes) {
	a[i] = cj*(cs + sc);	/* Could speed up with */
	a[j] = sj*(cc + ss);	/* trig identities. */
	a[k] = sj*(cs - sc);
	qu[W] = cj*(cc - ss);
    } else {
	a[i] = cj*sc - sj*cs;
	a[j] = cj*ss + sj*cc;
	a[k] = cj*cs - sj*sc;
	qu[W] = cj*cc + sj*ss;
    }
    if (n==EulParOdd) a[j] = -a[j];
    qu[X] = a[X]; qu[Y] = a[Y]; qu[Z] = a[Z];
}

/* Construct matrix from Euler angles (in radians). */
void LWEUL_toMatrix(LWEulerAngles ea, LWFMatrix4 M)
{
    float ti, tj, th, ci, cj, ch, si, sj, sh, cc, cs, sc, ss;
    int i,j,k,h,n,s,f;
    EulGetOrd(((unsigned)ea[W]),i,j,k,h,n,s,f);
    if (f==EulFrmR) {float t = ea[X]; ea[X] = ea[Z]; ea[Z] = t;}
    if (n==EulParOdd) {ea[X] = -ea[X]; ea[Y] = -ea[Y]; ea[Z] = -ea[Z];}
    ti = ea[X];	  tj = ea[Y];	th = ea[Z];
    ci = cos(ti); cj = cos(tj); ch = cos(th);
    si = sin(ti); sj = sin(tj); sh = sin(th);
    cc = ci*ch; cs = ci*sh; sc = si*ch; ss = si*sh;
    if (s==EulRepYes) {
	M[i][i] = cj;	  M[i][j] =  sj*si;    M[i][k] =  sj*ci;
	M[j][i] = sj*sh;  M[j][j] = -cj*ss+cc; M[j][k] = -cj*cs-sc;
	M[k][i] = -sj*ch; M[k][j] =  cj*sc+cs; M[k][k] =  cj*cc-ss;
    } else {
	M[i][i] = cj*ch; M[i][j] = sj*sc-cs; M[i][k] = sj*cc+ss;
	M[j][i] = cj*sh; M[j][j] = sj*ss+cc; M[j][k] = sj*cs-sc;
	M[k][i] = -sj;	 M[k][j] = cj*si;    M[k][k] = cj*ci;
    }
    M[W][X]=M[W][Y]=M[W][Z]=M[X][W]=M[Y][W]=M[Z][W]=0.0f; M[W][W]=1.0f;
}

/* Convert matrix to Euler angles (in radians). */
void LWEUL_fromMatrix(LWFMatrix4 M, int order, LWEulerAngles ea)
{
    int i,j,k,h,n,s,f;
    EulGetOrd(order,i,j,k,h,n,s,f);
    if (s==EulRepYes) {
	float sy = sqrt(M[i][j]*M[i][j] + M[i][k]*M[i][k]);
	if (sy > 16.0f*FLT_EPSILON) {
	    ea[X] = atan2(M[i][j], M[i][k]);
	    ea[Y] = atan2(sy, M[i][i]);
	    ea[Z] = atan2(M[j][i], -M[k][i]);
	} else {
	    ea[X] = atan2(-M[j][k], M[j][j]);
	    ea[Y] = atan2(sy, M[i][i]);
	    ea[Z] = 0;
	}
    } else {
	float cy = sqrt(M[i][i]*M[i][i] + M[j][i]*M[j][i]);
	if (cy > 16.0f*FLT_EPSILON) {
	    ea[X] = atan2(M[k][j], M[k][k]);
	    ea[Y] = atan2(-M[k][i], cy);
	    ea[Z] = atan2(M[j][i], M[i][i]);
	} else {
	    ea[X] = atan2(-M[j][k], M[j][j]);
	    ea[Y] = atan2(-M[k][i], cy);
	    ea[Z] = 0;
	}
    }
    if (n==EulParOdd) {ea[X] = -ea[X]; ea[Y] = - ea[Y]; ea[Z] = -ea[Z];}
    if (f==EulFrmR) {float t = ea[X]; ea[X] = ea[Z]; ea[Z] = t;}
    ea[W] = order;
}

/* Convert quaternion to Euler angles (in radians). */
void LWEUL_fromQuat(LWFQuat q, int order, LWEulerAngles ea)
{
    LWFMatrix4 M;
    float Nq = q[X]*q[X]+q[Y]*q[Y]+q[Z]*q[Z]+q[W]*q[W];
    float s = (Nq > 0.0) ? (2.0 / Nq) : 0.0;
    float xs = q[X]*s,	  ys = q[Y]*s,	 zs = q[Z]*s;
    float wx = q[W]*xs,	  wy = q[W]*ys,	 wz = q[W]*zs;
    float xx = q[X]*xs,	  xy = q[X]*ys,	 xz = q[X]*zs;
    float yy = q[Y]*ys,	  yz = q[Y]*zs,	 zz = q[Z]*zs;
    M[X][X] = 1.0 - (yy + zz); M[X][Y] = xy - wz; M[X][Z] = xz + wy;
    M[Y][X] = xy + wz; M[Y][Y] = 1.0 - (xx + zz); M[Y][Z] = yz - wx;
    M[Z][X] = xz - wy; M[Z][Y] = yz + wx; M[Z][Z] = 1.0 - (xx + yy);
    M[W][X]=M[W][Y]=M[W][Z]=M[X][W]=M[Y][W]=M[Z][W]=0.0; M[W][W]=1.0;
    LWEUL_fromMatrix(M, order, ea);
}

// ----------------------------------------------------------------------------
/* Rotate vector about axis by angle */ 
void LWEUL_applyAngleAxis(float a, LWFVector axis, LWFVector v)
{
	LWFMatrix4 R;
	LWFQuat cam,out;
	LWEulerAngles outAngs;
	float	s;

	QSET(cam, 0.0f);
	s=sin(RADIANS(a)/2.0);
 	out[X] = (float)(axis[0]*s);
 	out[Y] = (float)(axis[1]*s);
 	out[Z] = (float)(axis[2]*s);
	out[W] = (float)cos(RADIANS(a)/2.0);
	LWEUL_fromQuat(out, EulOrdXYZs, outAngs);
	LWEUL_toMatrix(outAngs, R);
	cam[X] = (float)v[0];
	cam[Y] = (float)v[1];
	cam[Z] = (float)v[2];
	LWQUAT_applyMatrix(R,cam, out);
	v[0] = out[X];
	v[1] = out[Y];
	v[2] = out[Z];
}

/* Rotate vector about axis by angle */ 
void LWEUL_applyHPB(double hpb[3],LWFVector v)
{
	LWFMatrix4 R;
	LWFQuat cam,out;
	LWEulerAngles inAngs;
	QSET(cam, 0.0f);
	EULSET4(cam, 0,0,0,EulHPB);
	inAngs[X] = (float)hpb[0]; //*RADperDEG;
	inAngs[Y] = (float)hpb[1]; //*RADperDEG;
	inAngs[Z] = (float)hpb[2]; //*RADperDEG;
	LWEUL_toMatrix(inAngs, R);
	cam[X] = (float)v[0];
	cam[Y] = (float)v[1];
	cam[Z] = (float)v[2];
	LWQUAT_applyMatrix(R,cam, out);
	v[0] = out[X];
	v[1] = out[Y];
	v[2] = out[Z];
}

/*  */ 
void LWEUL_applyInverseHPB(double hpb[3],LWFVector v)
{
	LWFMatrix4 R;
	LWFQuat cam,out;
	LWEulerAngles inAngs;
	QSET(cam, 0.0f);
	EULSET4(inAngs, 0,0,0,EulHPB);
	inAngs[X] = (float)hpb[0]; //*RADperDEG;
	inAngs[Y] = (float)hpb[1]; //*RADperDEG;
	inAngs[Z] = (float)hpb[2]; //*RADperDEG;
	LWEUL_toQuat(inAngs, out);
	out[X] = -out[X];	 // = sin(-a/2)
	out[Y] = -out[Y];
	out[Z] = -out[Z];
	LWEUL_fromQuat(out, EulOrdXYZs, inAngs);
	LWEUL_toMatrix(inAngs, R);
	cam[X] = (float)v[0];
	cam[Y] = (float)v[1];
	cam[Z] = (float)v[2];
	LWQUAT_applyMatrix(R,cam, out);
	v[0] = out[X];
	v[1] = out[Y];
	v[2] = out[Z];
}


/* A simple attempt to get camera/light direction vector from layout's H,P,B settings */ 
void LWEUL_getDirection(double hpb[3], LWFVector v)
{
	LWEulerAngles inAngs;
	LWFMatrix4 R;
	LWFQuat cam,out;
	QSET(cam, 0.0f);
	EULSET4(inAngs, 0,0,0,EulHPB);
	inAngs[X] = (float)hpb[0];
	inAngs[Y] = (float)hpb[1];
	inAngs[Z] = (float)hpb[2];
	cam[X] = (float)v[0];
	cam[Y] = (float)v[1];
	cam[Z] = (float)v[2];
	LWEUL_toMatrix(inAngs, R);
	LWQUAT_applyMatrix(R,cam, out);
	v[0] = out[X];
	v[1] = out[Y];
	v[2] = out[Z];
}

/* Get orientation quaternion from layout's H,P,B settings */
void LWEUL_HPBtoQuat(double hpb[3], LWFQuat out)
{
	LWEulerAngles inAngs;
	EULSET4(inAngs,0,0,0,EulHPB);
	inAngs[X] = (float)hpb[0];
	inAngs[Y] = (float)hpb[1];
	inAngs[Z] = (float)hpb[2];
	LWEUL_toQuat(inAngs, out);
}

/* Fill v[] with axis (from origin), return angle for quaternion style rotation */
void LWEUL_getAngleAxis(double hpb[3], float* ang, LWFVector v)
{
	LWFQuat out;
	float	angle, s;
	LWEulerAngles inAngs;
	EULSET4(inAngs, 0,0,0,EulHPB);
	inAngs[X] = (float)hpb[0];
	inAngs[Y] = (float)hpb[1];
	inAngs[Z] = (float)hpb[2];
	LWEUL_toQuat(inAngs, out);
	angle = acos(out[W]);
	s=sin(angle);
	angle *= RADIANS(2.0f);
	if(s)		 /* Is this valid???!!! */
	{
		v[0] = out[X]/s;
		v[1] = out[Y]/s;
		v[2] = out[Z]/s;
	}
	else if(!angle)
	{
		VSET(v, 0.0f);
	}
	*ang = angle;
}


/* Convert Body-fixed HPB rotation to X,Y,Z Space-fixed rotations */
void LWEUL_HPBtoXYZ(double hpb[3],double xyz[3])
{
	LWFMatrix4 R;
	LWEulerAngles outAngs, inAngs;
	EULSET4(inAngs, 0,0,0,EulHPB);
	inAngs[X] = (float)(RADIANS(hpb[0]));
	inAngs[Y] = (float)(RADIANS(hpb[1]));
	inAngs[Z] = (float)(RADIANS(hpb[2]));
	LWEUL_toMatrix(inAngs, R);
	LWEUL_fromMatrix(R, EulOrdXYZs,outAngs);
	xyz[0] = DEGREES(-outAngs[X]);	/* For some reason, modeler rotations need to be negated??*/
	xyz[1] = DEGREES(-outAngs[Y]);
	xyz[2] = DEGREES(-outAngs[Z]);
}

/* Convert Body-fixed HPB rotation to ZXY Space-fixed rotations */
void LWEUL_HPBtoZXY(double hpb[3],double zxy[3])
{
	LWFMatrix4 R;
	LWEulerAngles outAngs, inAngs;
	EULSET4(inAngs, 0,0,0,EulHPB);
	inAngs[X] = (float)(RADIANS(hpb[0]));
	inAngs[Y] = (float)(RADIANS(hpb[1]));
	inAngs[Z] = (float)(RADIANS(hpb[2]));
	LWEUL_toMatrix(inAngs, R);
	LWEUL_fromMatrix(R, EulOrdZXYs,outAngs);
	zxy[0] = DEGREES(-outAngs[X]);	/* For some reason, modeler rotations need to be negated??*/
	zxy[1] = DEGREES(-outAngs[Y]);
	zxy[2] = DEGREES(-outAngs[Z]);
}

void LWEUL_ZXYtoHPB(double zxy[3], double hpb[3])
{
	LWFMatrix4 R;
	LWEulerAngles outAngs, inAngs;
	EULSET4(inAngs, 0,0,0,EulOrdZXYr);
	inAngs[X] = (float)(RADIANS(zxy[0]));
	inAngs[Y] = (float)(RADIANS(zxy[1]));
	inAngs[Z] = (float)(RADIANS(zxy[2]));
	LWEUL_toMatrix(inAngs, R);
	LWEUL_fromMatrix(R, EulHPB,outAngs);
	hpb[0] = DEGREES(-outAngs[X]);	/* For some reason, modeler rotations need to be negated??*/
	hpb[1] = DEGREES(-outAngs[Y]);
	hpb[2] = DEGREES(-outAngs[Z]);
}


void LWEUL_AngleAxistoXYZ(float a, LWFVector v, double xyz[3])
{
	LWFQuat out;
	LWEulerAngles outAngs;
	float	s;
	s=sin(RADIANS(a)/2.0f);
 	out[X] = (float)(v[0]*s);
 	out[Y] = (float)(v[1]*s);
 	out[Z] = (float)(v[2]*s);
	out[W] = (float)cos(RADIANS(a)/2.0f);
	LWEUL_fromQuat(out, EulOrdXYZs,outAngs);
	xyz[0] = DEGREES(-outAngs[X]);	/* For some reason, modeler rotations need to be negated??*/
	xyz[1] = DEGREES(-outAngs[Y]);
	xyz[2] = DEGREES(-outAngs[Z]);
}

void LWEUL_XYZtoHP(LWFVector v, double hp[2])
{
    float hyp;
	float x = v[X];
	float y = v[Y];
	float z = v[Z];
	double* h = &hp[0];
	double* p = &hp[1];

    if (x == 0.0f && z == 0.0f) {
        *h = 0.0f;
        if (y != 0.0f)
            *p = (y < 0.0f) ? HALFPI : -HALFPI;
        else
            *p = 0.0f;
    }
    else {
        if (z == 0.0f)
            *h = (x < 0.0f) ? -HALFPI : HALFPI;
        else if (z < 0.0f)
            *h = atan(x / z) - PI;
        else
            *h = atan(x / z);
        hyp = sqrt(x * x + z * z);
        if (hyp == 0.0f)
            *p = (y < 0.0f) ? HALFPI : -HALFPI;
        else
            *p = -atan(y / hyp);
    }
}
