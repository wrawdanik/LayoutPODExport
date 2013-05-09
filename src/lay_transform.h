/*
Layout helper Library
Copyright 2002,  NewTek, Inc.
*/
#ifndef LWLAY_TRANSFORMS_H
#define LWLAY_TRANSFORMS_H


#include <lwrender.h>
#include "com_vecmatquat.h"


// Not complete - UNTESTED
void LWMAT_getWorldMatrix(LWItemInfo *iteminfo, 
						LWItemID id,
						LWTime t,
						LWFMatrix4 M);

void LWMAT_getWorldRotMatrix(LWItemInfo *iteminfo, 
						LWItemID id,
						LWTime t,
						LWFMatrix4 M);

void LWMAT_getWorldScaleMatrix(LWItemInfo *iteminfo, 
						LWItemID id,
						LWTime t,
						LWFMatrix4 M);

void LWMAT_getItemTransform(LWItemInfo *iteminfo, 
				   LWItemID id, 
				   LWTime time, 
				   LWFMatrix4 M);

void LWMAT_getTransform(LWItemInfo *iteminfo, 
				   LWItemID id, 
				   LWTime t, 
				   LWFMatrix4 M);

void LWMAT_getRotations(LWItemInfo *iteminfo, 
						LWItemID id, 
						LWTime t, 
						LWFMatrix4 M);

void LWMAT_getScales(LWItemInfo *iteminfo, 
				   LWItemID id, 
				   LWTime t, 
				   LWFMatrix4 M);

void LWMAT_getWorldPos(LWItemInfo *iteminfo, LWItemID id, double time, LWDVector wpos);
void LWMAT_getLocalPos(LWItemInfo *iteminfo, LWItemID local, double time, LWDVector wpos, LWDVector localPos);

void LWMAT_getWorldRot(LWItemInfo *iteminfo, LWItemID id, double time, LWDVector wrot, LWFMatrix4 m);
void LWMAT_getLocalRot(LWItemInfo *iteminfo, LWItemID local, double time, LWDVector wrot, LWDVector localRot);
void LWMAT_getLocalRot2(LWItemInfo *iteminfo, LWItemID local, double time, LWDVector wrot, LWFMatrix4 m, LWDVector localRot);

void LWMAT_getWorldScale(LWItemInfo *iteminfo, LWItemID id, double time, LWDVector wscale);
void LWMAT_getLocalScale(LWItemInfo *iteminfo, LWItemID local, double time, LWDVector wscale, LWDVector localScale);

void LWMAT_getNewPosition(LWItemInfo *iteminfo, LWItemID global, LWItemID local, double time, LWDVector newPos);
void LWMAT_getNewOrientation(LWItemInfo *iteminfo, LWItemID target, LWItemID local, double time, LWDVector newRot);
void LWMAT_getNewScale(LWItemInfo *iteminfo, LWItemID target, LWItemID local, double time, LWDVector newScale);

#endif
