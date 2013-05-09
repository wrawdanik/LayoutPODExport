/*
Common helper Library
Copyright 2002,  NewTek, Inc.
*/
#ifndef LWCOM_TRANSFORMS_H
#define LWCOM_TRANSFORMS_H

#include "com_vecmatquat.h"

void LWMAT_getRotate(double hpb[], LWFMatrix4 M );
void LWMAT_getRotateZXY(double hpb[], LWFMatrix4 M );
void LWMAT_getTranslation(LWFMatrix4 m, LWDVector newRot);
void LWMAT_getEuler(LWFMatrix4 m, LWDVector newRot);
void LWMAT_getYXZrotations(float v[3], const LWFMatrix3 m);
void LWMAT_getScale(LWFMatrix4 m, LWDVector newScale);

#endif
