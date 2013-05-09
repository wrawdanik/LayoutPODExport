/*
======================================================================
Common helper Library
Copyright 2002,  NewTek, Inc.

com_vector.c

Based on Ernie Wright's VecMath code. Please use VLEN, VSET3, VDOT, 
VADD, etc. defined in com_math.h for simple vector operations. The 
above are typeless macros.

Becareful of the lack of strong typing in C. Passing a float array when
the function requires a double array is an extremely common developer 
error.
====================================================================== */
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lwcomlib.h"
#include "com_vecmatquat.h"

/*
======================================================================
Single precision functions.
====================================================================== */
void LWVEC_setlen( LWFVector a, float d )
{
   float d0 = VLEN( a );

   if ( d0 != 0.0f ) VSCL( a, d / d0 );
}


void LWVEC_normalize( LWFVector a )
{
   LWVEC_setlen( a, 1.0f );
}

int LWVEC_equals( const LWFVector a, const LWFVector b )
{
	if(EQUALS_F(a[0], b[0]) && EQUALS_F(a[1], b[1]) && EQUALS_F(a[2], b[2]))
		return TRUE;
	else
		return FALSE;
}

void LWVEC_lerp( const LWFVector a, const LWFVector b, float t, LWFVector c )
{
   c[ 0 ] = a[ 0 ] + ( b[ 0 ] - a[ 0 ] ) * t;
   c[ 1 ] = a[ 1 ] + ( b[ 1 ] - a[ 1 ] ) * t;
   c[ 2 ] = a[ 2 ] + ( b[ 2 ] - a[ 2 ] ) * t;
}


void LWVEC_combine( const LWFVector a, const LWFVector b, LWFVector c, float sa, float sb )
{
   c[ 0 ] = sa * a[ 0 ] + sb * b[ 0 ];
   c[ 1 ] = sa * a[ 1 ] + sb * b[ 1 ];
   c[ 2 ] = sa * a[ 2 ] + sb * b[ 2 ];
}

void LWVEC_polynorm( const LWFVector v1, const LWFVector v2, const LWFVector vlast, LWFVector norm )
{
   LWFVector a, b;

   VSUB3(a, v2, v1);
   VSUB3(b, vlast, v1);
   VCROSS(norm, a, b);
   LWVEC_normalize( norm );
}


float LWVEC_dist( const LWFVector a, const LWFVector b )
{
   LWFVector c;

   VSUB3(c, a, b);
   return VLEN( c );
}


float LWVEC_angle( const LWFVector a, const LWFVector b )
{
   LWFVector na, nb;

   VCPY( na, a );
   LWVEC_normalize( na );
   VCPY( nb, b );
   LWVEC_normalize( nb );
   return ( float ) acos( VDOT( na, nb ));
}


void LWVEC_vec_hp( const LWFVector a, float *h, float *p )
{
   LWFVector n;

   VCPY( n, a );
   LWVEC_normalize( n );

   *p = ( float ) asin( -n[ 1 ] );
   if ( 1.0f - fabs( n[ 1 ] ) > EPSILON_F ) {
      *h = ( float )( acos( n[ 2 ] / cos( *p )));
      if ( n[ 0 ] < 0.0f )
         *h = ( float )( 2 * PI - *h );
   }
   else *h = 0.0f;
}


void LWVEC_hp_vec( float h, float p, LWFVector a )
{
   float
      ch = ( float ) cos( h ),
      sh = ( float ) sin( h ),
      cp = ( float ) cos( p ),
      sp = ( float ) sin( p );

   a[ 0 ] = sh * cp;
   a[ 1 ] = -sp;
   a[ 2 ] = ch * cp;
}

/*
======================================================================
Double precision functions.
====================================================================== */

void LWVEC_dsetlen( LWDVector a, double d )
{
   double d0 = VLEN( a );

   if ( d0 != 0.0 ) VSCL( a, d / d0 );
}


void LWVEC_dnormalize( LWDVector a )
{
   LWVEC_dsetlen( a, 1.0 );
}

int LWVEC_dequals( const LWDVector a, const LWDVector b )
{
	if(EQUALS_D(a[0], b[0]) && EQUALS_D(a[1], b[1]) && EQUALS_D(a[2], b[2]))
		return TRUE;
	else
		return FALSE;
}


void LWVEC_dlerp( const LWDVector a, const LWDVector b, double t, LWDVector c )
{
   c[ 0 ] = a[ 0 ] + ( b[ 0 ] - a[ 0 ] ) * t;
   c[ 1 ] = a[ 1 ] + ( b[ 1 ] - a[ 1 ] ) * t;
   c[ 2 ] = a[ 2 ] + ( b[ 2 ] - a[ 2 ] ) * t;
}


void LWVEC_dcombine( const LWDVector a, const LWDVector b, LWDVector c, double sa, double sb )
{
   c[ 0 ] = sa * a[ 0 ] + sb * b[ 0 ];
   c[ 1 ] = sa * a[ 1 ] + sb * b[ 1 ];
   c[ 2 ] = sa * a[ 2 ] + sb * b[ 2 ];
}


void LWVEC_dpolynorm( const LWDVector v1, const LWDVector v2, const LWDVector vlast, LWDVector norm )
{
   LWDVector a, b;

   VSUB3(a, v2, v1);
   VSUB3(b, vlast, v1);
   VCROSS(norm, a, b);
   LWVEC_dnormalize( norm );
}


double LWVEC_ddist( const LWDVector a, const LWDVector b )
{
   LWDVector c;

   VSUB3(c, a, b);
   return VLEN( c );
}


double LWVEC_dangle( const LWDVector a, const LWDVector b )
{
   LWDVector na, nb;

   VCPY( na, a );
   LWVEC_dnormalize( na );
   VCPY( nb, b );
   LWVEC_dnormalize( nb );
   return acos( VDOT( na, nb ));
}


void LWVEC_dvec_hp( const LWDVector a, double *h, double *p )
{
   LWDVector n;

   VCPY( n, a );
   LWVEC_dnormalize( n );

   *p = ( double ) asin( -n[ 1 ] );
   if ( 1.0 - fabs( n[ 1 ] ) > EPSILON_D ) {
      *h = acos( n[ 2 ] / cos( *p ));
      if ( n[ 0 ] < 0.0 )
         *h = 2 * PI - *h;
   }
   else *h = 0.0;
}


void LWVEC_dhp_vec( double h, double p, LWDVector a )
{
   double
      ch = cos( h ),
      sh = sin( h ),
      cp = cos( p ),
      sp = sin( p );

   a[ 0 ] = sh * cp;
   a[ 1 ] = -sp;
   a[ 2 ] = ch * cp;
}
