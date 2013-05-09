/*
======================================================================
vecmat.h

Basic vector and matrix functions.
====================================================================== */

#include <lwtypes.h>
#include "Vector3.h"
extern "C"
{
#include "com_transform.h"
#include "lay_transform.h"
#include "com_math.h"
}
#define vecangle(a,b) (float)acos(dot(a,b))    /* a and b must be unit vectors */

double dot( float *a, float  *b );
void cross( float *a, float *b, float *c );
void normalize( float *v );

namespace LayoutPODExport
{
	class SceneItem;
	class SceneInventory;
}



void LWMAT_getItemTransformKeyFrame(LayoutPODExport::SceneItem *item,LayoutPODExport::SceneInventory *scene, int frame, LWFMatrix4 M, MiniEngine::Vector3 &scale);
void LWMAT_getTransformKeyFrame(LayoutPODExport::SceneItem *item,LayoutPODExport::SceneInventory *scene, int frame,  LWFMatrix4 M, MiniEngine::Vector3 &scale,bool forceParent, bool skipParent);


void LWMAT_getItemTransformKeyFrameMatrix(LayoutPODExport::SceneItem *item,LayoutPODExport::SceneInventory *scene, int frame, LWFMatrix4 M);
void LWMAT_getTransformKeyFrameMatrix(LayoutPODExport::SceneItem *item,LayoutPODExport::SceneInventory *scene, int frame,  LWFMatrix4 M,bool forceParent, bool skipParent);

