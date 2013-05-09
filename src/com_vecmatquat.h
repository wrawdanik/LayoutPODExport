/*
Common helper Library
Copyright 2002,  NewTek, Inc.
*/

#ifndef LWCOM_VECMATHQUAT__H
#define LWCOM_VECMATHQUAT__H

#include "com_math.h"

/*
======================================================================
Single precision functions.
====================================================================== 
*/

void   LWVEC_setlen   ( LWFVector a, float d );
void   LWVEC_normalize( LWFVector a );
void   LWVEC_lerp     ( const LWFVector a, const LWFVector b, float t, LWFVector c );
void   LWVEC_combine  ( const LWFVector a, const LWFVector b, LWFVector c, float sa, float sb );
void   LWVEC_polynorm ( const LWFVector v1, const LWFVector v2, const LWFVector vlast, LWFVector norm );
float  LWVEC_dist     ( const LWFVector a, const LWFVector b );
float  LWVEC_angle    ( const LWFVector a, const LWFVector b );
void   LWVEC_vec_hp   ( const LWFVector a, float *h, float *p );
void   LWVEC_hp_vec   ( float h, float p, LWFVector a );


// Matrix helpers
void   LWMAT_initv    ( LWFVector a, float x, float y, float z );
void   LWMAT_initm3   ( LWFMatrix3 m, float, float, float, float,
                         float, float, float, float, float );
void   LWMAT_initm4   ( LWFMatrix4 m, float, float, float, float,
                         float, float, float, float, float, float,
                         float, float, float, float, float, float );
void   LWMAT_identity3( LWFMatrix3 m );
void   LWMAT_identity4( LWFMatrix4 m );
int	   LWMAT_equals3	( const LWFMatrix3 a, const LWFMatrix3 b );
int	   LWMAT_equals4	( const LWFMatrix4 a, const LWFMatrix4 b );
void   LWMAT_copym3   ( LWFMatrix3 to, const LWFMatrix3 from );
void   LWMAT_copym4   ( LWFMatrix4 to, const LWFMatrix4 from );
void   LWMAT_transform( const LWFVector a, const LWFMatrix3 m, LWFVector b );
void   LWMAT_transformp( const LWFVector a, const LWFMatrix4 m, LWFVector b );
void   LWMAT_matmul3  ( const LWFMatrix3 a, const LWFMatrix3 b, LWFMatrix3 c );
void   LWMAT_matmul4  ( const LWFMatrix4 a, const LWFMatrix4 b, LWFMatrix4 c );
void   LWMAT_scalem4  ( LWFMatrix4 a, float s );
void   LWMAT_scalem3  ( LWFMatrix3 a, float s );
float  LWMAT_det2     ( float a, float b, float c, float d );
float  LWMAT_det3     ( const LWFMatrix3 m );
float  LWMAT_det4     ( const LWFMatrix4 m );
void   LWMAT_adjoint3 ( const LWFMatrix3 m, LWFMatrix3 adj );
void   LWMAT_adjoint4 ( const LWFMatrix4 m, LWFMatrix4 adj );
void   LWMAT_inverse3 ( const LWFMatrix3 m, LWFMatrix3 inv );
void   LWMAT_inverse4 ( const LWFMatrix4 m, LWFMatrix4 inv );

// Quaternion helpers
void LWQUAT_qnormalize(LWFQuat q, LWFQuat qq);
void LWQUAT_qmul(LWFQuat qL, LWFQuat qR, LWFQuat qq);
void LWQUAT_qinverse(LWFQuat q, LWFQuat qq);
void LWQUAT_quat_to_mat(LWFQuat q, LWFMatrix3 mat);
void LWQUAT_mat_to_quat(LWFMatrix3 mat, LWFQuat q);	
void LWQUAT_QFromAngAxis(float ang, LWFVector axis, LWFQuat q);
void LWQUAT_AngAxisFromQ(LWFQuat q, float *ang, LWFVector axis);
float LWQUAT_RelAngAxis(LWFQuat p, LWFQuat q, LWFVector axis);
void LWQUAT_applyMatrix(LWFMatrix4 M, LWFQuat Q, LWFQuat qu);

/*
======================================================================
Double precision functions.
====================================================================== 
*/

void   LWVEC_dsetlen   ( LWDVector a, double d );
void   LWVEC_dnormalize( LWDVector a );
void   LWVEC_dlerp     ( const LWDVector a, const LWDVector b, double t, LWDVector c );
void   LWVEC_dcombine  ( const LWDVector a, const LWDVector b, LWDVector c, double sa, double sb );
void   LWVEC_dpolynorm ( const LWDVector v1, const LWDVector v2, const LWDVector vlast, LWDVector norm );
double LWVEC_ddist     ( const LWDVector a, const LWDVector b );
double LWVEC_dangle    ( const LWDVector a, const LWDVector b );
void   LWVEC_dvec_hp   ( const LWDVector a, double *h, double *p );
void   LWVEC_dhp_vec   ( double h, double p, LWDVector a );

void   LWMAT_dinitm3   ( LWDMatrix3 m, double, double, double, double,
                             double, double, double, double, double );
void   LWMAT_dinitm4   ( LWDMatrix4 m, double, double, double, double,
                             double, double, double, double, double, double,
                             double, double, double, double, double, double );
void   LWMAT_didentity3( LWDMatrix3 m );
void   LWMAT_didentity4( LWDMatrix4 m );
int	   LWMAT_dequals3	( const LWDMatrix3 a, const LWDMatrix3 b );
int	   LWMAT_dequals4( const LWDMatrix4 a, const LWDMatrix4 b );
void   LWMAT_dcopym3   ( LWDMatrix3 to, const LWDMatrix3 from );
void   LWMAT_dcopym4   ( LWDMatrix4 to, const LWDMatrix4 from );
void   LWMAT_dtransform( const LWDVector a, const LWDMatrix3 m, LWDVector b );
void   LWMAT_dtransformp( const LWDVector a, const LWDMatrix4 m, LWDVector b );
void   LWMAT_dmatmul3  ( const LWDMatrix3 a, const LWDMatrix3 b, LWDMatrix3 c );
void   LWMAT_dmatmul4  ( const LWDMatrix4 a, const LWDMatrix4 b, LWDMatrix4 c );
void   LWMAT_dscalem4  ( LWDMatrix4 a, double s );
void   LWMAT_dscalem3  ( LWDMatrix3 a, double s );
double LWMAT_ddet2     ( double a, double b, double c, double d );
double LWMAT_ddet3     ( const LWDMatrix3 m );
double LWMAT_ddet4     ( const LWDMatrix4 m );
void   LWMAT_dadjoint3 ( const LWDMatrix3 m, LWDMatrix3 adj );
void   LWMAT_dadjoint4 ( const LWDMatrix4 m, LWDMatrix4 adj );
void   LWMAT_dinverse3 ( const LWDMatrix3 m, LWDMatrix3 inv );
void   LWMAT_dinverse4 ( const LWDMatrix4 m, LWDMatrix4 inv );

#endif
