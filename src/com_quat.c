/*
======================================================================
Common helper Library
Copyright 2002,  NewTek, Inc.

com_quat.c

Based on:

 Quaternion operations -- 
 	From "Quaternion Calculus and Fast Animation",
  	by Ken Shoemake,  in notes for SIGGRAPH 1987 Course # 10, 
    "Computer Animation: 3-D Motion Specification and Control".

====================================================================== */
#include "lwcomlib.h"
#include "com_math.h"
#include "com_vecmatquat.h"
#include <math.h>
#include <string.h>

static const int X = 0;
static const int Y = 1;
static const int Z = 2;
static const int W = 3;
// static int nxt[3] = {Y,Z,X};
static int nxt[3] = {1,2,0};

/* qnorm: normalize a quaternion */
void LWQUAT_qnormalize(LWFQuat q, LWFQuat qq) 
{
	float l,c;
	l =  sqrt(q[X]*q[X] + q[Y]*q[Y] + q[Z]*q[Z] + q[W]*q[W]);
	if (l==0) 
	{
		qq[W] = 1.0;	
		qq[Y] = qq[Z] = qq[X] = 0.0;
	}
	else 
	{	
		c = 1.0/l;
		qq[X] = q[X]*c;
		qq[Y] = q[Y]*c;
		qq[Z] = q[Z]*c;
		qq[W] = q[W]*c;
		}
	}

/*  qmul: Compute quaternion product qq = qL * qR. */
void LWQUAT_qmul(LWFQuat qL, LWFQuat qR, LWFQuat qq) 
{
	float qp[4];
	qp[W] = qL[W]*qR[W] - qL[X]*qR[X] - qL[Y]*qR[Y] - qL[Z]*qR[Z];
	qp[X] = qL[W]*qR[X] + qL[X]*qR[W] + qL[Y]*qR[Z] - qL[Z]*qR[Y];
	qp[Y] = qL[W]*qR[Y] + qL[Y]*qR[W] + qL[Z]*qR[X] - qL[X]*qR[Z];
	qp[Z] = qL[W]*qR[Z] + qL[Z]*qR[W] + qL[X]*qR[Y] - qL[Y]*qR[X];
	memcpy(qq,qp,sizeof(LWFQuat));
}


/* qinv:  Form multiplicative inverse of q */
void LWQUAT_qinverse(LWFQuat q, LWFQuat qq) 
{
	float l,norminv;
	l =	 (q[X]*q[X] + q[Y]*q[Y] + q[Z]*q[Z] + q[W]*q[W]);
	if (l==0.0) l = 1.0;
	norminv = 1.0/l;
	qq[X] = -q[X] * norminv;
	qq[Y] = -q[Y] * norminv;
	qq[Z] = -q[Z] * norminv;
	qq[W] = q[W] * norminv;
}

/* quat_to_mat: Convert quaternion to 3x3 rotation matrix;
   Quaternion need not be unit magnitude.  When it always is,
   this routine can be simplified */
void LWQUAT_quat_to_mat(LWFQuat q, LWFMatrix3 mat) 	
{
	float s,xs,ys,zs,wx,wy,wz,xx,xy,xz,yy,yz,zz,den;

	/* For unit q, just set s = 2.0; or or set xs = q[X] + q[X], etc */
	den =  (q[X]*q[X] + q[Y]*q[Y] + q[Z]*q[Z] + q[W]*q[W]);
	if (den==0.0) {  s = 1.0; }
	else s = 2.0/den;

	xs = q[X] * s;   ys = q[Y] * s;   zs = q[Z] * s;
	wx = q[W] * xs;  wy = q[W] * ys; wz = q[W] * zs;
	xx = q[X] * xs;  xy = q[X] * ys; xz = q[X] * zs;
	yy = q[Y] * ys;  yz = q[Y] * zs; zz = q[Z] * zs;

	mat[X][X] = 1.0 - (yy +zz);
	mat[X][Y] = xy - wz;
	mat[X][Z] = xz + wy;

	mat[Y][X] = xy + wz;
	mat[Y][Y] = 1.0 - (xx +zz);
	mat[Y][Z] = yz - wx;

	mat[Z][X] = xz - wy;
	mat[Z][Y] = yz + wx;
	mat[Z][Z] = 1.0 - (xx + yy);
}

/* mat_to_quat: Convert 3x3 rotation matrix to unit quaternion */

void LWQUAT_mat_to_quat(LWFMatrix3 mat, LWFQuat q)	
{
	float tr,s;
	int i,j,k;
	tr = mat[X][X] + mat[Y][Y] + mat[Z][Z];
	if (tr > 0.0) 
	{
		s = sqrt(tr + 1.0);
		q[W] = s * 0.5;
		s = 0.5 / s;
		q[X] = (mat[Z][Y] - mat[Y][Z]) * s;
		q[Y] = (mat[X][Z] - mat[Z][X]) * s;
		q[Z] = (mat[Y][X] - mat[X][Y]) * s;
	}
	else 
	{
		i = X;
		if (mat[Y][Y] > mat[X][X]) i = Y;
		if (mat[Z][Z] > mat[i][i]) i = Z;
		j = nxt[i];
		k = nxt[j];
		s = sqrt( (mat[i][i] - (mat[j][j]+mat[k][k])) + 1.0 );
		q[i] = s * 0.5;
		if (s!=0.0)	s = 0.5 /s;
		q[W] = (mat[k][j] - mat[j][k]) * s;
		q[j] = (mat[j][i] + mat[i][j]) * s;
		q[k] = (mat[k][i] + mat[i][k]) * s;
	}
}

/* --- Compute equivalent quaternion from [angle,axis] representation */
void LWQUAT_QFromAngAxis(float ang, LWFVector axis, LWFQuat q) 
{
	int i;
	float omega,s;
	omega = ang*0.5;
	s = sin(omega);
	for (i=0; i<3; i++) q[i] = s*axis[i];
	q[3] = cos(omega);
}

/* --- Convert quaternion to equivalent [angle,axis] representation */

void LWQUAT_AngAxisFromQ(LWFQuat q, float *ang, LWFVector axis) 
{
	int i;
	float omega,s,qn[4];
	LWQUAT_qnormalize(q,qn);
	omega = acos(qn[3]);
	*ang = 2.0*omega;
	s = sin(omega);
	if (fabs(s)>.000001) for (i=0; i<3; i++) axis[i]  = qn[i]/s;
}

/* ---------------------------------------------
	Compute [angle,axis] corresponding to the rotation from
    p to q.  Function returns angle, sets axis.  
	--------------------------- */

float LWQUAT_RelAngAxis(LWFQuat p, LWFQuat q, LWFVector axis) 
{
	float pinv[4],r[4],ang;
	LWQUAT_qinverse(p,pinv);
	LWQUAT_qmul(pinv,q,r);
	LWQUAT_AngAxisFromQ(r,&ang,axis);
	return(ang);
}

void LWQUAT_applyMatrix(LWFMatrix4 M, LWFQuat Q, LWFQuat qu)	 /* Uses Q as a regular old 3-vector */
{
	QSET(qu, 0.0f);
	qu[X] = M[0][0]*Q[X] + M[0][1]*Q[Y] + M[0][2]*Q[Z] + M[0][3]*Q[W] ;
	qu[Y] = M[1][0]*Q[X] + M[1][1]*Q[Y] + M[1][2]*Q[Z] + M[1][3]*Q[W] ;
	qu[Z] = M[2][0]*Q[X] + M[2][1]*Q[Y] + M[2][2]*Q[Z] + M[2][3]*Q[W] ;
	qu[W] = M[3][0]*Q[X] + M[3][1]*Q[Y] + M[3][2]*Q[Z] + M[3][3]*Q[W] ;
}

/* -- Convert Euler angles to equivalent Quaternion, for 
  x=Roll, y=Pitch, z= Yaw.  Note: there are many different conventions
  for Euler angles. */
/*
void LWQUAT_EulerToQuat(float x, float y, float z, float q[4]) {
	float m[4][3], rm[3][3];
	InitTMatrix(m);
	RotateX(m, x);
	RotateY(m, y);
	RotateZ(m, z);
	mat_to_quat(m,q);
	}
*/
