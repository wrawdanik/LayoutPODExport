/*
======================================================================
Common helper Library
Copyright 2002,  NewTek, Inc.

com_transform.c

Common transformation functions.

NOTE: Some are for educational purposes only as the functionality is 
redundant.
====================================================================== */
#include <lwrender.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lwcomlib.h"
#include "com_vecmatquat.h"
#include "com_transform.h"

/*
======================================================================
LWMAT_getRotate()

Fill the upper left 3x3 submatrix based on H, P, B angles.

        ch   0  -sh           1    0    0           cb   sb   0
  Ry =  0    1    0     Rx =  0   cp   sp     Rz = -sb   cb   0
        sh   0   ch           0  -sp   cp           0    0    1

  M = Rz Rx Ry

The matrix rows will correspond to LW's right, up and forward vectors
after the same rotations.  Called by get_transform().
====================================================================== */
void LWMAT_getRotate(double hpb[], 
				LWFMatrix4 M )
{
  LWFMatrix3 Rx, Ry, Rz, R;
  float sh, ch, sp, cp, sb, cb;
  int i, j;

  sh = ( float ) sin( hpb[ 0 ] );
  ch = ( float ) cos( hpb[ 0 ] );
  sp = ( float ) sin( hpb[ 1 ] );
  cp = ( float ) cos( hpb[ 1 ] );
  sb = ( float ) sin( hpb[ 2 ] );
  cb = ( float ) cos( hpb[ 2 ] );

  LWMAT_initm3( Ry, ch, 0.0f, -sh, 0.0f, 1.0f, 0.0f, sh, 0.0f, ch );
  LWMAT_initm3( Rx, 1.0f, 0.0f, 0.0f, 0.0f, cp, sp, 0.0f, -sp, cp );
  LWMAT_initm3( Rz, cb, sb, 0.0f, -sb, cb, 0.0f, 0.0f, 0.0f, 1.0f );

	LWMAT_matmul3( Rz, Rx, R );
	LWMAT_matmul3( R, Ry, R );

  for ( i = 0; i < 3; i++ )
     for ( j = 0; j < 3; j++ )
        M[ i ][ j ] = R[ i ][ j ];
}

void LWMAT_getRotateZXY(double hpb[], 
				LWFMatrix4 M )
{
  LWFMatrix3 Rx, Ry, Rz, R;
  float sh, ch, sp, cp, sb, cb;
  int i, j;

  sh = ( float ) sin( hpb[ 0 ] );
  ch = ( float ) cos( hpb[ 0 ] );
  sp = ( float ) sin( hpb[ 1 ] );
  cp = ( float ) cos( hpb[ 1 ] );
  sb = ( float ) sin( hpb[ 2 ] );
  cb = ( float ) cos( hpb[ 2 ] );

  LWMAT_initm3( Ry, ch, 0.0f, -sh, 0.0f, 1.0f, 0.0f, sh, 0.0f, ch );
  LWMAT_initm3( Rx, 1.0f, 0.0f, 0.0f, 0.0f, cp, sp, 0.0f, -sp, cp );
  LWMAT_initm3( Rz, cb, sb, 0.0f, -sb, cb, 0.0f, 0.0f, 0.0f, 1.0f );

	LWMAT_matmul3( Ry, Rx, R );
	LWMAT_matmul3( R, Rz, R );

  for ( i = 0; i < 3; i++ )
     for ( j = 0; j < 3; j++ )
        M[ i ][ j ] = R[ i ][ j ];
}


void LWMAT_getTranslation(LWFMatrix4 m, LWDVector newPos)
{
	int i;
	for(i=0; i < 3; i++)
		newPos[i] = m[3][i];
}

/**
 * Extracts Euler angles from a 4x4 Matrix
 *
 * @param m			4x4 Matrix
 * @param newRot	Euler angle result
 */
void LWMAT_getEuler(LWFMatrix4 m, LWDVector newRot)
{
	int i,j;
	LWFVector rotations;
	LWFMatrix3 subMatrix;

	for(i=0; i < 3; i++)
		for(j=0; j < 3; j++)
		{
			subMatrix[i][j] = m[i][j];
		}
	LWMAT_getYXZrotations(rotations,subMatrix);
	for ( i = 0; i < 3; i++ )
	{
		newRot[i] = rotations[i];
	}
}

void LWMAT_getYXZrotations(float v[3], const LWFMatrix3 m)
{
	// Pitch
	double cp;
	double s,c;

	// X
	if(m[2][1] > 1.0f)
	{
		v[1] = (float)asin(-1.0);
	}
	else if(m[2][1] < -1.0f)
	{
		v[1] = (float)asin(1.0);
	}
	else
	{
		v[1] = asin(-1.0f*m[2][1]);
	}
	cp = cos(v[1]);

    if(!NEAR_ZERO_D(cp))
    {
		// Y
		s = m[2][0]/cp;
		c = m[2][2]/cp;
		v[0] = atan2(s,c);

		// Z
		s = m[0][1]/cp;
		c = m[1][1]/cp;
		v[2] = atan2(s,c);
    }
    else
    {
		v[0] = 0.0f;
		v[2] = atan2(m[0][2], m[1][2]);
		if(NEAR_ZERO_D(v[1]+1.57080))
		{
			v[0] = -v[0];
			v[1] = -v[1];
			v[2] = -v[2];
		}
    }
}

/**
 * Extracts scale vector from a 4x4 Matrix
 *
 * @param m			4x4 Matrix
 * @param newRot	Result
 */
void LWMAT_getScale(LWFMatrix4 m, LWDVector newScale)
{
	int i,j;

	for(i=0; i < 3; i++)
		for(j=0; j < 3; j++)
		{
			if(i == j)
				newScale[i] = m[i][j];
		}	
}

