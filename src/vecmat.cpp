/*
======================================================================
vecmat.c

Basic vector and matrix functions.
====================================================================== */

#include <math.h>
#include "vecmat.h"
#include "Database.h"

using namespace LayoutPODExport;

double dot( float *a, float  *b  )
{
   return a[ 0 ] * b[ 0 ] + a[ 1 ] * b[ 1 ] + a[ 2 ] * b[ 2 ];
}


void cross( float *a, float *b, float *c )
{
   c[ 0 ] = a[ 1 ] * b[ 2 ] - a[ 2 ] * b[ 1 ];
   c[ 1 ] = a[ 2 ] * b[ 0 ] - a[ 0 ] * b[ 2 ];
   c[ 2 ] = a[ 0 ] * b[ 1 ] - a[ 1 ] * b[ 0 ];
}


void normalize( float *v )
{
   float r;

   r = ( float ) sqrt( dot( v, v ));
   if ( r > 0 ) {
      v[ 0 ] /= r;
      v[ 1 ] /= r;
      v[ 2 ] /= r;
   }
}

// separate instance of Matrix style POD animation 

void LWMAT_getItemTransformKeyFrameMatrix(SceneItem *item,SceneInventory *scene, int frame,  LWFMatrix4 M)
{

	LWFMatrix4 M2;
	LWDVector pos, rot, scale, piv, prot;

	LWMAT_identity4( M );

	KeyFrame &keyFrame=item->keyFramesOther[frame];

	pos[0]=keyFrame.position.x; pos[1]=keyFrame.position.y; pos[2]=-keyFrame.position.z;
	rot[0]=-keyFrame.orient.x; rot[1]=-keyFrame.orient.y; rot[2]=keyFrame.orient.z;
	scale[0]=keyFrame.scale.x; scale[1]=keyFrame.scale.y; scale[2]=keyFrame.scale.z;
	piv[0]=item->pivotPos.x; piv[1]=item->pivotPos.y; piv[2]=-item->pivotPos.z;
	prot[0]=-item->pivotOrient.x; prot[1]=-item->pivotOrient.y; prot[2]=item->pivotOrient.z;	

	if((scene->getLwMain()->itemInfo->flags(item->itemId)&LWITEMF_GOAL_ORIENT))
	{
		SceneItem *goalItem=scene->getItem(scene->getLwMain()->itemInfo->goal(item->itemId));
		if(goalItem!=0)
		{
			LWFMatrix4 goalM;
			LWMAT_identity4( goalM );
			//Compute world rotation matrix for goal
			LWMAT_getTransformKeyFrameMatrix(goalItem,scene,frame,goalM,true,false);

			LWFMatrix4 parentM;	
			LWMAT_identity4( parentM );			

			//Compute world rotation matrix for parent
			if ( item->parent!=0)
				LWMAT_getTransformKeyFrameMatrix(item->parent,scene,frame,parentM,true,false);

			LWFMatrix4 inverseM;	
			LWMAT_inverse4(parentM, inverseM);
			LWFMatrix4 resultM;
			LWMAT_matmul4(goalM, inverseM, resultM);
			LWMAT_getEuler(resultM, rot);
		}
	}

	LWMAT_identity4( M2 );
	M2[ 3 ][ 0 ] = ( float ) -piv[ 0 ];
	M2[ 3 ][ 1 ] = ( float ) -piv[ 1 ];
	M2[ 3 ][ 2 ] = ( float ) -piv[ 2 ];
	LWMAT_matmul4( M, M2, M );

	LWMAT_identity4( M2 );
	M2[ 0 ][ 0 ] = ( float ) scale[0];
	M2[ 1 ][ 1 ] = ( float ) scale[1];
	M2[ 2 ][ 2 ] = ( float ) scale[2];
	LWMAT_matmul4( M, M2, M );

	LWMAT_identity4( M2 );
	LWMAT_getRotate(rot, M2 );
	LWMAT_matmul4( M, M2, M );

	LWMAT_identity4( M2 );
	LWMAT_getRotate(prot, M2 );
	LWMAT_matmul4( M, M2, M );

	LWMAT_identity4( M2 );
	M2[ 3 ][ 0 ] = ( float ) pos[ 0 ];
	M2[ 3 ][ 1 ] = ( float ) pos[ 1 ];
	M2[ 3 ][ 2 ] = ( float ) pos[ 2 ];
	LWMAT_matmul4( M, M2, M );
}


void LWMAT_getTransformKeyFrameMatrix(SceneItem *item,SceneInventory *scene, int frame,  LWFMatrix4 M, bool forceParent, bool skipParent)
{
	
	LWFMatrix4 M2;
	LWMAT_getItemTransformKeyFrameMatrix(item,scene,frame,M2);
	LWMAT_matmul4( M, M2, M );  

	if ( item->parent!=0 && !skipParent)
	{
		if(forceParent || item->parent->collapsed[0])
			LWMAT_getTransformKeyFrameMatrix(item->parent,scene,frame,M,forceParent,false);
	}
		
}

void LWMAT_getItemTransformKeyFrame(SceneItem *item,SceneInventory *scene, int frame,  LWFMatrix4 M,MiniEngine::Vector3 &inScale)
{

	LWFMatrix4 M2;
	LWDVector pos, rot, scale, piv, prot;

	// Initialize incoming matrix to identity since we will overwrite all values
	LWMAT_identity4( M );

	KeyFrame &keyFrame=item->keyFramesOther[frame];

	pos[0]=keyFrame.position.x; pos[1]=keyFrame.position.y; pos[2]=keyFrame.position.z;
	rot[0]=keyFrame.orient.x; rot[1]=keyFrame.orient.y; rot[2]=keyFrame.orient.z;
	scale[0]=keyFrame.scale.x; scale[1]=keyFrame.scale.y; scale[2]=keyFrame.scale.z;
	piv[0]=item->pivotPos.x; piv[1]=item->pivotPos.y; piv[2]=item->pivotPos.z;
	prot[0]=item->pivotOrient.x; prot[1]=item->pivotOrient.y; prot[2]=item->pivotOrient.z;	


	
	if((scene->getLwMain()->itemInfo->flags(item->itemId)&LWITEMF_GOAL_ORIENT))
	{
		SceneItem *goalItem=scene->getItem(scene->getLwMain()->itemInfo->goal(item->itemId));
		if(goalItem!=0)
		{
			LWFMatrix4 goalM;
			LWMAT_identity4( goalM );
			//Compute world rotation matrix for goal
			LWMAT_getTransformKeyFrameMatrix(goalItem,scene,frame,goalM,true,false);

			LWFMatrix4 parentM;	
			LWMAT_identity4( parentM );			

			//Compute world rotation matrix for parent
			if ( item->parent!=0)
				LWMAT_getTransformKeyFrameMatrix(item->parent,scene,frame,parentM,true,false);

			LWFMatrix4 inverseM;	
			LWMAT_inverse4(parentM, inverseM);
			LWFMatrix4 resultM;
			LWMAT_matmul4(goalM, inverseM, resultM);
			LWMAT_getEuler(resultM, rot);
			rot[0]*=-1.0f;
			rot[1]*=-1.0f;
		}
	}


	LWMAT_identity4( M2 );
	M2[ 3 ][ 0 ] = ( float ) -piv[ 0 ];
	M2[ 3 ][ 1 ] = ( float ) -piv[ 1 ];
	M2[ 3 ][ 2 ] = ( float ) -piv[ 2 ];
	LWMAT_matmul4( M, M2, M );

	MiniEngine::Vector3 tmpScale(scale[0],scale[1],scale[2]);

	inScale*=tmpScale;

	LWMAT_identity4( M2 );
	LWMAT_getRotate(rot, M2 );
	LWMAT_matmul4( M, M2, M );

	LWMAT_identity4( M2 );
	LWMAT_getRotate(prot, M2 );
	LWMAT_matmul4( M, M2, M );

	LWMAT_identity4( M2 );
	M2[ 3 ][ 0 ] = ( float ) pos[ 0 ];
	M2[ 3 ][ 1 ] = ( float ) pos[ 1 ];
	M2[ 3 ][ 2 ] = ( float ) pos[ 2 ];
	LWMAT_matmul4( M, M2, M );
}


void LWMAT_getTransformKeyFrame(SceneItem *item,SceneInventory *scene, int frame,  LWFMatrix4 M, MiniEngine::Vector3 &scale, bool forceParent, bool skipParent)
{
	
	LWFMatrix4 M2;
	LWMAT_getItemTransformKeyFrame(item,scene,frame,M2,scale);
	LWMAT_matmul4( M, M2, M );  

	if ( item->parent!=0 && !skipParent)
	{
		if(forceParent || item->parent->collapsed[0])
			LWMAT_getTransformKeyFrame(item->parent,scene,frame,M,scale,forceParent,false );
	}
		
}