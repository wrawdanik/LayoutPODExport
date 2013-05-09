/*
======================================================================
Common helper Library
Copyright 2002,  NewTek, Inc.

com_matrix.c

Based on Ernie Wright's VecMath code.
====================================================================== */
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lwcomlib.h"
#include "com_vecmatquat.h"

/* some matrix shorthand */

#define m00  m[ 0 ][ 0 ]
#define m01  m[ 0 ][ 1 ]
#define m02  m[ 0 ][ 2 ]
#define m03  m[ 0 ][ 3 ]
#define m10  m[ 1 ][ 0 ]
#define m11  m[ 1 ][ 1 ]
#define m12  m[ 1 ][ 2 ]
#define m13  m[ 1 ][ 3 ]
#define m20  m[ 2 ][ 0 ]
#define m21  m[ 2 ][ 1 ]
#define m22  m[ 2 ][ 2 ]
#define m23  m[ 2 ][ 3 ]
#define m30  m[ 3 ][ 0 ]
#define m31  m[ 3 ][ 1 ]
#define m32  m[ 3 ][ 2 ]
#define m33  m[ 3 ][ 3 ]

/*
======================================================================
Single precision functions.
====================================================================== */
void LWMAT_initm3( LWFMatrix3 m,
   float a1, float b1, float c1,
   float a2, float b2, float c2,
   float a3, float b3, float c3 )
{
   m00 = a1;  m01 = b1;  m02 = c1;
   m10 = a2;  m11 = b2;  m12 = c2;
   m20 = a3;  m21 = b3;  m22 = c3;
}


void LWMAT_initm4( LWFMatrix4 m,
   float a1, float b1, float c1, float d1,
   float a2, float b2, float c2, float d2,
   float a3, float b3, float c3, float d3,
   float a4, float b4, float c4, float d4 )
{
   m00 = a1;  m01 = b1;  m02 = c1;  m03 = d1;
   m10 = a2;  m11 = b2;  m12 = c2;  m13 = d2;
   m20 = a3;  m21 = b3;  m22 = c3;  m23 = d3;
   m30 = a4;  m31 = b4;  m32 = c4;  m33 = d4;
}


void LWMAT_identity3( LWFMatrix3 m )
{
   LWMAT_initm3( m,
      1.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 1.0f );
}


void LWMAT_identity4( LWFMatrix4 m )
{
   LWMAT_initm4( m,
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f );
}

int LWMAT_equals3( const LWFMatrix3 a, const LWFMatrix3 b )
{
	int i,j;
	for ( i = 0; i < 3; i++ )
		for ( j = 0; j < 3; j++ )
				if (!EQUALS_F(a[i][j], b[i][j])) 
					return FALSE;
	return TRUE;
}

int LWMAT_equals4( const LWFMatrix4 a, const LWFMatrix4 b )
{
	int i,j;
	for ( i = 0; i < 4; i++ )
		for ( j = 0; j < 4; j++ )
				if (!EQUALS_F(a[i][j], b[i][j])) 
					return FALSE;
	return TRUE;
}


void LWMAT_copym3( LWFMatrix3 to, const LWFMatrix3 from )
{
   memcpy( to, from, sizeof( LWFMatrix3 ));
}


void LWMAT_copym4( LWFMatrix4 to, const LWFMatrix4 from )
{
   memcpy( to, from, sizeof( LWFMatrix4 ));
}

void LWMAT_transform( const LWFVector a, const LWFMatrix3 m, LWFVector b )
{
   int i;

   for ( i = 0; i < 3; i++ )
      b[ i ] = a[ 0 ] * m[ 0 ][ i ]
             + a[ 1 ] * m[ 1 ][ i ]
             + a[ 2 ] * m[ 2 ][ i ];
}


void LWMAT_transformp( const LWFVector a, const LWFMatrix4 m, LWFVector b )
{
   float w;
   int i;

   for ( i = 0; i < 3; i++ )
      b[ i ] = a[ 0 ] * m[ 0 ][ i ]
             + a[ 1 ] * m[ 1 ][ i ]
             + a[ 2 ] * m[ 2 ][ i ] + m[ 3 ][ i ];

   w = a[ 0 ] * m[ 0 ][ 3 ]
     + a[ 1 ] * m[ 1 ][ 3 ]
     + a[ 2 ] * m[ 2 ][ 3 ] + m[ 3 ][ 3 ];

   if ( w != 0.0f ) {
      b[ 0 ] /= w;
      b[ 1 ] /= w;
      b[ 2 ] /= w;
   }
}


void LWMAT_matmul3( const LWFMatrix3 a, const LWFMatrix3 b, LWFMatrix3 c )
{
   int i, j, k;
   LWFMatrix3 m = { 0.0f };

   for ( i = 0; i < 3; i++ )
      for ( j = 0; j < 3; j++ )
         for ( k = 0; k < 3; k++ )
            m[ i ][ j ] += a[ i ][ k ] * b[ k ][ j ];

   memcpy( &c[ 0 ][ 0 ], &m[ 0 ][ 0 ], sizeof( LWFMatrix3 ));
}


void LWMAT_matmul4( const LWFMatrix4 a, const LWFMatrix4 b, LWFMatrix4 c )
{
   int i, j, k;
   LWFMatrix4 m = { 0.0f };

   for ( i = 0; i < 4; i++ )
      for ( j = 0; j < 4; j++ )
         for ( k = 0; k < 4; k++ )
            m[ i ][ j ] += a[ i ][ k ] * b[ k ][ j ];

   memcpy( &c[ 0 ][ 0 ], &m[ 0 ][ 0 ], sizeof( LWFMatrix4 ));
}


void LWMAT_scalem4( LWFMatrix4 a, float s )
{
   int i, j;

   for ( i = 0; i < 4; i++ )
      for ( j = 0; j < 4; j++ )
         a[ i ][ j ] *= s;
}


void LWMAT_scalem3( LWFMatrix3 a, float s )
{
   int i, j;

   for ( i = 0; i < 3; i++ )
      for ( j = 0; j < 3; j++ )
         a[ i ][ j ] *= s;
}


float det2( float a, float b, float c, float d )
{
    return a * d - b * c;
}


float LWMAT_det3( const LWFMatrix3 m )
{
   return   m00 * det2( m11, m21, m12, m22 )
          - m01 * det2( m10, m20, m12, m22 )
          + m02 * det2( m10, m20, m11, m21 );
}


float LWMAT_det4( const LWFMatrix4 m )
{
   LWFMatrix3 p;
   float a, b, c, d;

   LWMAT_initm3( p, m11, m21, m31, m12, m22, m32, m13, m23, m33 );  a = LWMAT_det3( p );
   LWMAT_initm3( p, m10, m20, m30, m12, m22, m32, m13, m23, m33 );  b = LWMAT_det3( p );
   LWMAT_initm3( p, m10, m20, m30, m11, m21, m31, m13, m23, m33 );  c = LWMAT_det3( p );
   LWMAT_initm3( p, m10, m20, m30, m11, m21, m31, m12, m22, m32 );  d = LWMAT_det3( p );

   return m00 * a - m01 * b + m02 * c - m03 * d;
}


void LWMAT_adjoint3( const LWFMatrix3 m, LWFMatrix3 adj )
{
   adj[ 0 ][ 0 ] =  det2( m11, m12, m21, m22 );
   adj[ 1 ][ 0 ] = -det2( m10, m12, m20, m22 );
   adj[ 2 ][ 0 ] =  det2( m10, m11, m20, m21 );
   adj[ 0 ][ 1 ] = -det2( m01, m02, m21, m22 );
   adj[ 1 ][ 1 ] =  det2( m00, m02, m20, m22 );
   adj[ 2 ][ 1 ] = -det2( m00, m01, m20, m21 );
   adj[ 0 ][ 2 ] =  det2( m01, m02, m11, m12 );
   adj[ 1 ][ 2 ] = -det2( m00, m02, m10, m12 );
   adj[ 2 ][ 2 ] =  det2( m00, m01, m10, m11 );
}


void LWMAT_adjoint4( const LWFMatrix4 m, LWFMatrix4 adj )
{
   LWFMatrix3 a;

   LWMAT_initm3( a, m11, m21, m31, m12, m22, m32, m13, m23, m33 );  adj[ 0 ][ 0 ] =  LWMAT_det3( a );
   LWMAT_initm3( a, m10, m20, m30, m12, m22, m32, m13, m23, m33 );  adj[ 1 ][ 0 ] = -LWMAT_det3( a );
   LWMAT_initm3( a, m10, m20, m30, m11, m21, m31, m13, m23, m33 );  adj[ 2 ][ 0 ] =  LWMAT_det3( a );
   LWMAT_initm3( a, m10, m20, m30, m11, m21, m31, m12, m22, m32 );  adj[ 3 ][ 0 ] = -LWMAT_det3( a );
   LWMAT_initm3( a, m01, m21, m31, m02, m22, m32, m03, m23, m33 );  adj[ 0 ][ 1 ] = -LWMAT_det3( a );
   LWMAT_initm3( a, m00, m20, m30, m02, m22, m32, m03, m23, m33 );  adj[ 1 ][ 1 ] =  LWMAT_det3( a );
   LWMAT_initm3( a, m00, m20, m30, m01, m21, m31, m03, m23, m33 );  adj[ 2 ][ 1 ] = -LWMAT_det3( a );
   LWMAT_initm3( a, m00, m20, m30, m01, m21, m31, m02, m22, m32 );  adj[ 3 ][ 1 ] =  LWMAT_det3( a );
   LWMAT_initm3( a, m01, m11, m31, m02, m12, m32, m03, m13, m33 );  adj[ 0 ][ 2 ] =  LWMAT_det3( a );
   LWMAT_initm3( a, m00, m10, m30, m02, m12, m32, m03, m13, m33 );  adj[ 1 ][ 2 ] = -LWMAT_det3( a );
   LWMAT_initm3( a, m00, m10, m30, m01, m11, m31, m03, m13, m33 );  adj[ 2 ][ 2 ] =  LWMAT_det3( a );
   LWMAT_initm3( a, m00, m10, m30, m01, m11, m31, m02, m12, m32 );  adj[ 3 ][ 2 ] = -LWMAT_det3( a );
   LWMAT_initm3( a, m01, m11, m21, m02, m12, m22, m03, m13, m23 );  adj[ 0 ][ 3 ] = -LWMAT_det3( a );
   LWMAT_initm3( a, m00, m10, m20, m02, m12, m22, m03, m13, m23 );  adj[ 1 ][ 3 ] =  LWMAT_det3( a );
   LWMAT_initm3( a, m00, m10, m20, m01, m11, m21, m03, m13, m23 );  adj[ 2 ][ 3 ] = -LWMAT_det3( a );
   LWMAT_initm3( a, m00, m10, m20, m01, m11, m21, m02, m12, m22 );  adj[ 3 ][ 3 ] =  LWMAT_det3( a );
}


void LWMAT_inverse3( const LWFMatrix3 m, LWFMatrix3 inv )
{
   float det;

   det = LWMAT_det3( m );
   if ( fabs( det ) < EPSILON_F ) return;

   LWMAT_adjoint3( m, inv );
   LWMAT_scalem3( inv, 1.0f / det );
}


void LWMAT_inverse4( const LWFMatrix4 m, LWFMatrix4 inv )
{
   float det;

   det = LWMAT_det4( m );
   if ( fabs( det ) < EPSILON_F ) return;

   LWMAT_adjoint4( m, inv );
   LWMAT_scalem4( inv, 1.0f / det );
}

/*
======================================================================
Double precision functions.
====================================================================== */

void LWMAT_dinitm3( LWDMatrix3 m,
   double a1, double b1, double c1,
   double a2, double b2, double c2,
   double a3, double b3, double c3 )
{
   m00 = a1;  m01 = b1;  m02 = c1;
   m10 = a2;  m11 = b2;  m12 = c2;
   m20 = a3;  m21 = b3;  m22 = c3;
}


void LWMAT_dinitm4( LWDMatrix4 m,
   double a1, double b1, double c1, double d1,
   double a2, double b2, double c2, double d2,
   double a3, double b3, double c3, double d3,
   double a4, double b4, double c4, double d4 )
{
   m00 = a1;  m01 = b1;  m02 = c1;  m03 = d1;
   m10 = a2;  m11 = b2;  m12 = c2;  m13 = d2;
   m20 = a3;  m21 = b3;  m22 = c3;  m23 = d3;
   m30 = a4;  m31 = b4;  m32 = c4;  m33 = d4;
}


void LWMAT_didentity3( LWDMatrix3 m )
{
   LWMAT_dinitm3( m,
      1.0, 0.0, 0.0,
      0.0, 1.0, 0.0,
      0.0, 0.0, 1.0 );
}


void LWMAT_didentity4( LWDMatrix4 m )
{
   LWMAT_dinitm4( m,
      1.0, 0.0, 0.0, 0.0,
      0.0, 1.0, 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 0.0, 1.0 );
}

int LWMAT_dequals3( const LWDMatrix3 a, const LWDMatrix3 b )
{
	int i,j;
	for ( i = 0; i < 3; i++ )
		for ( j = 0; j < 3; j++ )
				if (!EQUALS_D(a[i][j], b[i][j])) 
					return FALSE;
	return TRUE;
}

int LWMAT_dequals4( const LWDMatrix4 a, const LWDMatrix4 b )
{
	int i,j;
	for ( i = 0; i < 4; i++ )
		for ( j = 0; j < 4; j++ )
				if (!EQUALS_D(a[i][j], b[i][j])) 
					return FALSE;
	return TRUE;
}

void LWMAT_dcopym3( LWDMatrix3 to, const LWDMatrix3 from )
{
   memcpy( to, from, sizeof( LWDMatrix3 ));
}


void LWMAT_dcopym4( LWDMatrix4 to, const LWDMatrix4 from )
{
   memcpy( to, from, sizeof( LWDMatrix4 ));
}

void LWMAT_dtransform( const LWDVector a, const LWDMatrix3 m, LWDVector b )
{
   int i;

   for ( i = 0; i < 3; i++ )
      b[ i ] = a[ 0 ] * m[ 0 ][ i ]
             + a[ 1 ] * m[ 1 ][ i ]
             + a[ 2 ] * m[ 2 ][ i ];
}


void LWMAT_dtransformp( const LWDVector a, const LWDMatrix4 m, LWDVector b )
{
   double w;
   int i;

   for ( i = 0; i < 3; i++ )
      b[ i ] = a[ 0 ] * m[ 0 ][ i ]
             + a[ 1 ] * m[ 1 ][ i ]
             + a[ 2 ] * m[ 2 ][ i ] + m[ 3 ][ i ];

   w = a[ 0 ] * m[ 0 ][ 3 ]
     + a[ 1 ] * m[ 1 ][ 3 ]
     + a[ 2 ] * m[ 2 ][ 3 ] + m[ 3 ][ 3 ];

   if ( w != 0.0 ) {
      b[ 0 ] /= w;
      b[ 1 ] /= w;
      b[ 2 ] /= w;
   }
}


void LWMAT_dmatmul3( const LWDMatrix3 a, const LWDMatrix3 b, LWDMatrix3 c )
{
   int i, j, k;
   LWDMatrix3 m = { 0.0 };

   for ( i = 0; i < 3; i++ )
      for ( j = 0; j < 3; j++ )
         for ( k = 0; k < 3; k++ )
            m[ i ][ j ] += a[ i ][ k ] * b[ k ][ j ];

   memcpy( &c[ 0 ][ 0 ], &m[ 0 ][ 0 ], sizeof( LWDMatrix3 ));
}


void LWMAT_dmatmul4( const LWDMatrix4 a, const LWDMatrix4 b, LWDMatrix4 c )
{
   int i, j, k;
   LWDMatrix4 m = { 0.0 };

   for ( i = 0; i < 4; i++ )
      for ( j = 0; j < 4; j++ )
         for ( k = 0; k < 4; k++ )
            m[ i ][ j ] += a[ i ][ k ] * b[ k ][ j ];

   memcpy( &c[ 0 ][ 0 ], &m[ 0 ][ 0 ], sizeof( LWDMatrix4 ));
}


void LWMAT_dscalem4( LWDMatrix4 a, double s )
{
   int i, j;

   for ( i = 0; i < 4; i++ )
      for ( j = 0; j < 4; j++ )
         a[ i ][ j ] *= s;
}


void LWMAT_dscalem3( LWDMatrix3 a, double s )
{
   int i, j;

   for ( i = 0; i < 3; i++ )
      for ( j = 0; j < 3; j++ )
         a[ i ][ j ] *= s;
}


double LWMAT_ddet2( double a, double b, double c, double d )
{
    return a * d - b * c;
}


double LWMAT_ddet3( const LWDMatrix3 m )
{
   return   m00 * LWMAT_ddet2( m11, m21, m12, m22 )
          - m01 * LWMAT_ddet2( m10, m20, m12, m22 )
          + m02 * LWMAT_ddet2( m10, m20, m11, m21 );
}


double LWMAT_ddet4( const LWDMatrix4 m )
{
   LWDMatrix3 p;
   double a, b, c, d;

   LWMAT_dinitm3( p, m11, m21, m31, m12, m22, m32, m13, m23, m33 );  a = LWMAT_ddet3( p );
   LWMAT_dinitm3( p, m10, m20, m30, m12, m22, m32, m13, m23, m33 );  b = LWMAT_ddet3( p );
   LWMAT_dinitm3( p, m10, m20, m30, m11, m21, m31, m13, m23, m33 );  c = LWMAT_ddet3( p );
   LWMAT_dinitm3( p, m10, m20, m30, m11, m21, m31, m12, m22, m32 );  d = LWMAT_ddet3( p );

   return m00 * a - m01 * b + m02 * c - m03 * d;
}


void LWMAT_dadjoint3( const LWDMatrix3 m, LWDMatrix3 adj )
{
   adj[ 0 ][ 0 ] =  LWMAT_ddet2( m11, m12, m21, m22 );
   adj[ 1 ][ 0 ] = -LWMAT_ddet2( m10, m12, m20, m22 );
   adj[ 2 ][ 0 ] =  LWMAT_ddet2( m10, m11, m20, m21 );
   adj[ 0 ][ 1 ] = -LWMAT_ddet2( m01, m02, m21, m22 );
   adj[ 1 ][ 1 ] =  LWMAT_ddet2( m00, m02, m20, m22 );
   adj[ 2 ][ 1 ] = -LWMAT_ddet2( m00, m01, m20, m21 );
   adj[ 0 ][ 2 ] =  LWMAT_ddet2( m01, m02, m11, m12 );
   adj[ 1 ][ 2 ] = -LWMAT_ddet2( m00, m02, m10, m12 );
   adj[ 2 ][ 2 ] =  LWMAT_ddet2( m00, m01, m10, m11 );
}


void LWMAT_dadjoint4( const LWDMatrix4 m, LWDMatrix4 adj )
{
   LWDMatrix3 a;

   LWMAT_dinitm3( a, m11, m21, m31, m12, m22, m32, m13, m23, m33 );  adj[ 0 ][ 0 ] =  LWMAT_ddet3( a );
   LWMAT_dinitm3( a, m10, m20, m30, m12, m22, m32, m13, m23, m33 );  adj[ 1 ][ 0 ] = -LWMAT_ddet3( a );
   LWMAT_dinitm3( a, m10, m20, m30, m11, m21, m31, m13, m23, m33 );  adj[ 2 ][ 0 ] =  LWMAT_ddet3( a );
   LWMAT_dinitm3( a, m10, m20, m30, m11, m21, m31, m12, m22, m32 );  adj[ 3 ][ 0 ] = -LWMAT_ddet3( a );
   LWMAT_dinitm3( a, m01, m21, m31, m02, m22, m32, m03, m23, m33 );  adj[ 0 ][ 1 ] = -LWMAT_ddet3( a );
   LWMAT_dinitm3( a, m00, m20, m30, m02, m22, m32, m03, m23, m33 );  adj[ 1 ][ 1 ] =  LWMAT_ddet3( a );
   LWMAT_dinitm3( a, m00, m20, m30, m01, m21, m31, m03, m23, m33 );  adj[ 2 ][ 1 ] = -LWMAT_ddet3( a );
   LWMAT_dinitm3( a, m00, m20, m30, m01, m21, m31, m02, m22, m32 );  adj[ 3 ][ 1 ] =  LWMAT_ddet3( a );
   LWMAT_dinitm3( a, m01, m11, m31, m02, m12, m32, m03, m13, m33 );  adj[ 0 ][ 2 ] =  LWMAT_ddet3( a );
   LWMAT_dinitm3( a, m00, m10, m30, m02, m12, m32, m03, m13, m33 );  adj[ 1 ][ 2 ] = -LWMAT_ddet3( a );
   LWMAT_dinitm3( a, m00, m10, m30, m01, m11, m31, m03, m13, m33 );  adj[ 2 ][ 2 ] =  LWMAT_ddet3( a );
   LWMAT_dinitm3( a, m00, m10, m30, m01, m11, m31, m02, m12, m32 );  adj[ 3 ][ 2 ] = -LWMAT_ddet3( a );
   LWMAT_dinitm3( a, m01, m11, m21, m02, m12, m22, m03, m13, m23 );  adj[ 0 ][ 3 ] = -LWMAT_ddet3( a );
   LWMAT_dinitm3( a, m00, m10, m20, m02, m12, m22, m03, m13, m23 );  adj[ 1 ][ 3 ] =  LWMAT_ddet3( a );
   LWMAT_dinitm3( a, m00, m10, m20, m01, m11, m21, m03, m13, m23 );  adj[ 2 ][ 3 ] = -LWMAT_ddet3( a );
   LWMAT_dinitm3( a, m00, m10, m20, m01, m11, m21, m02, m12, m22 );  adj[ 3 ][ 3 ] =  LWMAT_ddet3( a );
}


void LWMAT_dinverse3( const LWDMatrix3 m, LWDMatrix3 inv )
{
   double det;

   det = LWMAT_ddet3( m );
   if ( fabs( det ) < EPSILON_D ) return;

   LWMAT_dadjoint3( m, inv );
   LWMAT_dscalem3( inv, 1.0 / det );
}


void LWMAT_dinverse4( const LWDMatrix4 m, LWDMatrix4 inv )
{
   double det;

   det = LWMAT_ddet4( m );
   if ( fabs( det ) < EPSILON_D ) return;

   LWMAT_dadjoint4( m, inv );
   LWMAT_dscalem4( inv, 1.0 / det );
}
