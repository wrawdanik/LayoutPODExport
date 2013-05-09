/*
======================================================================
Layout helper Library
Copyright 2002,  NewTek, Inc.

lay_transform.c

LW Transform helper functions.
====================================================================== */
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lwlaylib.h"
#include "com_transform.h"
#include "lay_transform.h"

/**
 * Constructs world-to-local space transformation matrix using the up, 
 * right, forward vectors (gives us orientation) and the world position.
 *
 * @param iteminfo	LWItemInfo pointer
 * @param id		Item
 * @param time		Evaluation time
 * @param M			Result in Matrix form
 */
void LWMAT_getWorldMatrix(LWItemInfo *iteminfo, 
						LWItemID id,
						LWTime time,
						LWFMatrix4 M)
{
	int i;
	LWDVector up, right, forward, wpos;

	iteminfo->param( id, LWIP_W_RIGHT,		time, right);
	iteminfo->param( id, LWIP_W_UP,			time, up);
	iteminfo->param( id, LWIP_W_FORWARD,	time, forward);
	iteminfo->param( id, LWIP_W_POSITION,	time, wpos);

	for(i=0; i < 3; i++)
	{
		M[0][i] = right[i];
		M[1][i] = up[i];
		M[2][i] = forward[i];
		M[3][i] = wpos[i];
	}

	M[0][3] = 0.0f;
	M[1][3] = 0.0f;
	M[2][3] = 0.0f;
	M[3][3] = 1.0f;
}

/**
 * Constructs world-to-local space rotation matrix using the up, 
 * right, forward vectors.
 *
 * @param iteminfo	LWItemInfo pointer
 * @param id		Item
 * @param time		Evaluation time
 * @param M			Result in Matrix form
 */
void LWMAT_getWorldRotMatrix(LWItemInfo *iteminfo, 
						LWItemID id,
						LWTime time,
						LWFMatrix4 M)
{
	int i;
	LWDVector up, right, forward;

	iteminfo->param( id, LWIP_W_RIGHT,		time, right);
	iteminfo->param( id, LWIP_W_UP,			time, up);
	iteminfo->param( id, LWIP_W_FORWARD,	time, forward);

	for(i=0; i < 3; i++)
	{
		M[0][i] = right[i];
		M[1][i] = up[i];
		M[2][i] = forward[i];
	}

	// zero out position
	VSET(M[3], 0.0f);

	M[0][3] = 0.0f;
	M[1][3] = 0.0f;
	M[2][3] = 0.0f;
	M[3][3] = 1.0f;
}

void LWMAT_getWorldScaleMatrix(LWItemInfo *iteminfo, 
						LWItemID id,
						LWTime t,
						LWFMatrix4 M)
{
	// NOT IMPLEMENTED
}

/**
 * Create a transformation matrix from item parameters.
 *
 * @param iteminfo	LWItemInfo pointer
 * @param id		Item
 * @param time		Evaluation time
 * @param M			Result in Matrix form
 */
void LWMAT_getItemTransform(LWItemInfo *iteminfo, 
				   LWItemID id, 
				   LWTime time, 
				   LWFMatrix4 M)
{
	LWFMatrix4 M2;
	LWDVector pos, rot, scale, piv, prot;

	// Initialize incoming matrix to identity since we will overwrite all values
	LWMAT_identity4( M );

	iteminfo->param( id, LWIP_POSITION,  time, pos   );
	iteminfo->param( id, LWIP_ROTATION,  time, rot   );
	iteminfo->param( id, LWIP_SCALING,   time, scale );
	iteminfo->param( id, LWIP_PIVOT,     time, piv   );
	iteminfo->param( id, LWIP_PIVOT_ROT, time, prot  );

	LWMAT_identity4( M2 );
	M2[ 3 ][ 0 ] = ( float ) -piv[ 0 ];
	M2[ 3 ][ 1 ] = ( float ) -piv[ 1 ];
	M2[ 3 ][ 2 ] = ( float ) -piv[ 2 ];
	LWMAT_matmul4( M, M2, M );

	LWMAT_identity4( M2 );
	M2[ 0 ][ 0 ] = ( float ) scale[ 0 ];
	M2[ 1 ][ 1 ] = ( float ) scale[ 1 ];
	M2[ 2 ][ 2 ] = ( float ) scale[ 2 ];
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

/**
 * Create a transformation matrix from item parameters.  Calls itself
 * recursively to account for parenting.
 *
 * @param iteminfo	LWItemInfo pointer
 * @param id		Item
 * @param time		Evaluation time
 * @param M			Result in Matrix form
 */
void LWMAT_getTransform(LWItemInfo *iteminfo, 
				   LWItemID id, 
				   LWTime time, 
				   LWFMatrix4 M)
{
	LWItemID parent;
	LWFMatrix4 M2;
	LWMAT_getItemTransform(iteminfo, id, time, M2);
	LWMAT_matmul4( M, M2, M );  

	if ( parent = iteminfo->parent( id ))
	 LWMAT_getTransform(iteminfo, parent, time, M );
}

/**
 * Create a rotation matrix from item parameters.  Calls itself
 * recursively to account for parenting.
 *
 * @param iteminfo	LWItemInfo pointer
 * @param id		Item
 * @param time		Evaluation time
 * @param M			Result in Matrix form
 */
void LWMAT_getRotations(	LWItemInfo *iteminfo, 
						LWItemID id, 
						LWTime time, 
						LWFMatrix4 M)
{
	LWFMatrix4 M2;
	LWDVector rot, scale, piv, prot;
	LWItemID parent;

	iteminfo->param( id, LWIP_ROTATION,  time, rot);
	iteminfo->param( id, LWIP_SCALING,   time, scale );
	iteminfo->param( id, LWIP_PIVOT,     time, piv   );
	iteminfo->param( id, LWIP_PIVOT_ROT, time, prot  );

	LWMAT_identity4( M2 );
	M2[ 3 ][ 0 ] = ( float ) -piv[ 0 ];
	M2[ 3 ][ 1 ] = ( float ) -piv[ 1 ];
	M2[ 3 ][ 2 ] = ( float ) -piv[ 2 ];
	LWMAT_matmul4( M, M2, M );

	LWMAT_identity4( M2 );
	LWMAT_getRotate(rot, M2 );
	LWMAT_matmul4( M, M2, M );

	LWMAT_identity4( M2 );
	LWMAT_getRotate(prot, M2 );
	LWMAT_matmul4( M, M2, M );

	if(parent = iteminfo->parent(id))
	{
	 LWMAT_getRotations(iteminfo, parent, time, M);
	}
}

/**
 * Gets the world-space position of an item.
 *
 * @param iteminfo	LWItemInfo pointer
 * @param id		Item
 * @param time		Evaluation time
 * @param wpos		Result
 */
void LWMAT_getWorldPos(LWItemInfo *iteminfo, LWItemID id, double time, LWDVector wpos)
{
	iteminfo->param(id, LWIP_W_POSITION, time, wpos);
}

/**
 * Co-ordinate space transformation: From World to Local space of item.
 *
 * @param iteminfo	LWItemInfo pointer
 * @param id		Item
 * @param time		Evaluation time
 * @param wpos		Incoming World position
 * @param localPos	Localized position
 */
void LWMAT_getLocalPos(LWItemInfo *iteminfo, LWItemID local, double time, LWDVector wpos, LWDVector localPos)
{
	LWFVector temp, tempPos;
	LWFMatrix4 m, inv;

	if(iteminfo->parent(local))
	{
		// Construct matrix(based on hierarchy of object) and invert
		LWMAT_identity4(m);
		LWMAT_getTransform(iteminfo, iteminfo->parent(local), time, m);
		LWMAT_inverse4(m, inv);

		// Transform the world position of the target into the local co-ordinate space.
		VCPY(temp, wpos);
		LWMAT_transformp(temp, inv, tempPos);
		VCPY(localPos, tempPos);
	}
	else
	{
		// No need to transform world co-ordinates into local if there is no hierarchy
		VCPY(localPos, wpos);
	}
}

/**
 * Create a scale-only matrix from item parameters.  Calls itself
 * recursively to account for parenting.
 *
 * @param iteminfo	LWItemInfo pointer
 * @param id		Item
 * @param time		Evaluation time
 * @param M			Result in Matrix form
 */
void LWMAT_getScales(LWItemInfo *iteminfo, 
				   LWItemID id, 
				   LWTime time, 
				   LWFMatrix4 M)
{
  LWFMatrix4 M2;
  LWDVector scale;
  LWItemID parent;

  iteminfo->param(id, LWIP_SCALING, time, scale);

  LWMAT_identity4( M2 );
  M2[ 0 ][ 0 ] = ( float ) scale[ 0 ];
  M2[ 1 ][ 1 ] = ( float ) scale[ 1 ];
  M2[ 2 ][ 2 ] = ( float ) scale[ 2 ];
  LWMAT_matmul4( M, M2, M );

  if (parent = iteminfo->parent(id))
     LWMAT_getScales(iteminfo, parent, time, M );
}

/**
 * Gets the world-space rotation of an item.
 *
 * @param iteminfo	LWItemInfo pointer
 * @param id		Item
 * @param time		Evaluation time
 * @param wrot		Result in Euler form
 * @param m			Result in Matrix form
 */
void LWMAT_getWorldRot(LWItemInfo *iteminfo, LWItemID id, double time, LWDVector wrot, LWFMatrix4 m)
{
	VSET3(wrot, 0.0, 0.0, 0.0);
	if(iteminfo->parent(id))
	{
		LWMAT_identity4(m);
		LWMAT_getRotations(iteminfo, id, time, m);
		LWMAT_getEuler(m, wrot);
	}
	else
	{
		// No parent, just take the current rotation
		iteminfo->param(id, LWIP_ROTATION, time, wrot);
		LWMAT_getRotate(wrot, m);
	}
}

/**
 * Co-ordinate space transformation: From World to Local space of item.
 *
 * @param iteminfo	LWItemInfo pointer
 * @param id		Item
 * @param time		Evaluation time
 * @param wrot		Incoming world-space rotation
 * @param localRot	Result in Euler form   
 */
void LWMAT_getLocalRot(LWItemInfo *iteminfo, LWItemID local, double time, LWDVector wrot, LWDVector localRot)
{
	LWFMatrix4 m;
	VSET3(localRot, 0.0, 0.0, 0.0);

	if(iteminfo->parent(local))
	{
		LWMAT_identity4(m);
		LWMAT_getRotate(wrot, m);
		LWMAT_getLocalRot2(iteminfo, local, time, wrot, m, localRot);
	}
	else
	{
		VCPY(localRot, wrot);
	}
}

/**
 * Co-ordinate space transformation: From World to Local space of item.
 *
 * @param iteminfo	LWItemInfo pointer
 * @param id		Item
 * @param time		Evaluation time
 * @param wrot		Incoming world-space rotation
 * @param localRot	Result in Euler form   
 * @param m			Result in Matrix form
 */
void LWMAT_getLocalRot2(LWItemInfo *iteminfo, LWItemID local, double time, LWDVector wrot, LWFMatrix4 m, LWDVector localRot)
{
	LWFMatrix4 m2, inv, mResult;
	VSET3(localRot, 0.0, 0.0, 0.0);

	if(iteminfo->parent(local))
	{
		// Target's ROTATION in CHILD-SPACE
		LWMAT_identity4(m2);
		LWMAT_identity4(inv);
		LWMAT_identity4(mResult);
		LWMAT_getRotations(iteminfo, iteminfo->parent(local), time, m2);
		LWMAT_inverse4(m2, inv);
		LWMAT_matmul4(m, inv, mResult);
		LWMAT_getEuler(mResult, localRot);
	}
	else
	{
		VCPY(localRot, wrot);
	}
}

/**
 * Gets the world-space scale of an item.
 *
 * @param mathf		LWFVecMathFuncs
 * @param iteminfo	LWItemInfo pointer
 * @param id		Item
 * @param time		Evaluation time
 * @param wscale	Result
 */
void LWMAT_getWorldScale(LWItemInfo *iteminfo, LWItemID id, double time, LWDVector wscale)
{
	LWFMatrix4 m;
	VSET3(wscale, 1.0, 1.0, 1.0);

	if(iteminfo->parent(id))
	{
		LWMAT_identity4(m);
		LWMAT_getScales(iteminfo, id, time, m);
		LWMAT_getScale(m, wscale);
	}
	else
	{
		// No parent == world scale is the local scale
		iteminfo->param(id, LWIP_SCALING, time, wscale);
	}
}

/**
 * Co-ordinate space transformation: From World to Local space of item.
 *
 * @param iteminfo	LWItemInfo pointer
 * @param id		Item
 * @param time		Evaluation time
 * @param wscale	Incoming world-space scale
 * @param localScale Scale in local-space
 */
void LWMAT_getLocalScale(LWItemInfo *iteminfo, LWItemID local, double time, LWDVector wscale, LWDVector localScale)
{
	LWFMatrix4 m, m2, inv, mResult;
	VSET3(localScale, 0.0, 0.0, 0.0);

	if(iteminfo->parent(local))
	{
		// World Scale in local space
		LWMAT_identity4(m2);
		LWMAT_identity4(inv);
		LWMAT_identity4(mResult);
		LWMAT_getScales(iteminfo, iteminfo->parent(local), time, m2);
		LWMAT_inverse4(m2, inv);
		LWMAT_matmul4(m, inv, mResult);
		LWMAT_getScale(m, wscale);
	}
	else
	{
		VCPY(localScale, wscale);
	}
}

/**
 * Helper function. Gets World-Space position of item X and transforms it to 
 * the co-ordinate space of item Y.
 *
 * @param iteminfo	LWItemInfo pointer
 * @param global	World Item
 * @param local		Local Item
 * @param time		Evaluation time
 * @param newPos	Localized position
 */
void LWMAT_getNewPosition(LWItemInfo *iteminfo, LWItemID global, LWItemID local, double time, LWDVector newPos)
{
	LWDVector wpos;
	LWMAT_getWorldPos(iteminfo, global, time, wpos);
	LWMAT_getLocalPos(iteminfo, local, time, wpos, newPos);
}


/**
 * Helper function. Gets World-Space orientation of item X and transforms it to 
 * the co-ordinate space of item Y.
 *
 * @param iteminfo	LWItemInfo pointer
 * @param target	World Item
 * @param local		Local Item
 * @param time		Evaluation time
 * @param newRot	Localized rotation
 */
void LWMAT_getNewOrientation(LWItemInfo *iteminfo, LWItemID target, LWItemID local, double time, LWDVector newRot)
{
	LWFMatrix4 m;
	LWDVector wrot;
	LWMAT_identity4(m);
	LWMAT_getWorldRot(iteminfo, target, time, wrot, m);
	LWMAT_getLocalRot2(iteminfo, local, time, wrot, m, newRot);
}

/**
 * Helper function. Gets World-Space scale of item X and transform it to Y co-ordinate space.
 *
 * @param iteminfo	LWItemInfo pointer
 * @param target	World Item
 * @param local		Local Item
 * @param time		Evaluation time
 * @param newRot	Localized rotation
 */
void LWMAT_getNewScale(LWItemInfo *iteminfo, LWItemID target, LWItemID local, double time, LWDVector newScale)
{
	LWDVector wscale;
	LWMAT_getWorldScale(iteminfo, target, time, wscale);
	LWMAT_getLocalScale(iteminfo, target, time, wscale, newScale);
}
