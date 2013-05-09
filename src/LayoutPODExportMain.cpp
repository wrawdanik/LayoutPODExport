#include "LayoutPODExportMain.h"

#include <QtGui>
#include <lwpanel.h>
#include <lwdisplay.h>
#include <lwmotion.h>
#include "Database.h"
#include <Windows.h>
#include "MainDialog.h"
#include "Vector3.h"
#include "Quat.h"


using namespace LayoutPODExport;


static MainDialog *dialog=0;

XCALL_( int ) Activate( int version, GlobalFunc *global, LWLayoutGeneric *local,
   void *serverData )
{



  // if ( version != LWLAYOUTGENERIC_VERSION )
    //  return AFUNC_BADVERSION;

	if(dialog)
	{
		dialog->raise();
		dialog->setFocus();
		return 	AFUNC_OK;
	}




	HostDisplayInfo *hdi;
   hdi = (HostDisplayInfo*)global( LWHOSTDISPLAYINFO_GLOBAL, GFUSE_TRANSIENT );


	EnableWindow((HWND)hdi->window,0);

	
	LayoutPODExportMain	pMain;

		pMain.layoutGeneric=local;

	pMain.itemInfo = (LWItemInfo*)global( LWITEMINFO_GLOBAL, GFUSE_TRANSIENT );
	if(!pMain.itemInfo)
		return AFUNC_BADGLOBAL;	

	pMain.sceneInfo=(LWSceneInfo*)global( LWSCENEINFO_GLOBAL, GFUSE_TRANSIENT );
	if ( !pMain.sceneInfo ) 
		return AFUNC_BADGLOBAL;

	pMain.objectInfo=(LWObjectInfo*)global( LWOBJECTINFO_GLOBAL, GFUSE_TRANSIENT );
	if ( !pMain.objectInfo ) 
		return AFUNC_BADGLOBAL;

	pMain.cameraInfo=(LWCameraInfo*)global( LWCAMERAINFO_GLOBAL, GFUSE_TRANSIENT );
	if ( !pMain.cameraInfo ) 
		return AFUNC_BADGLOBAL;

	pMain.lightInfo=(LWLightInfo*)global( LWLIGHTINFO_GLOBAL, GFUSE_TRANSIENT );
	if ( !pMain.lightInfo ) 
		return AFUNC_BADGLOBAL;

	pMain.interfaceInfo=(LWInterfaceInfo*)global( LWINTERFACEINFO_GLOBAL, GFUSE_TRANSIENT );
	if ( !pMain.interfaceInfo ) 
		return AFUNC_BADGLOBAL;

	pMain.objectFunc=(LWObjectFuncs*)global( LWOBJECTFUNCS_GLOBAL, GFUSE_TRANSIENT );
	if ( !pMain.objectFunc ) 
		return AFUNC_BADGLOBAL;

	pMain.boneInfo=(LWBoneInfo *)global( LWBONEINFO_GLOBAL, GFUSE_TRANSIENT );
	if ( !pMain.boneInfo ) 
		return AFUNC_BADGLOBAL;



	pMain.globalFunc=global;

	

    

	int argc=0;
	char *argv;

	QApplication app(argc, &argv);


	dialog= new MainDialog(&pMain);
	dialog->exec();




	//pMain.editOp->done( pMain.editOp->state, EDERR_NONE, EDSELM_FORCEPOLS );

	delete dialog;
	dialog=0;
	EnableWindow((HWND)hdi->window,1);
	return AFUNC_OK;	
}

// Item motion handler stuff



typedef struct st_instance {
	LWItemID	id;					// bone id as item id
	void		*dbp;
	
} AINSTANCE;


XCALL_( static LWInstance )
Create( void *priv, LWItemID item, LWError *err )
{
	char chunkName[] = "LayoutPODExportMotion";
	
	AINSTANCE *inst;

	int *p;
	LWGlobalPool *memfunc;
	LWMemChunk mem;
	if ( inst = (AINSTANCE *)calloc( 1, sizeof( AINSTANCE ))) 
	{
		GlobalFunc *glob=(GlobalFunc*)priv;
		inst->id=item;
		inst->dbp=0;
		
		if(priv)
		{
			memfunc = (LWGlobalPool *)glob( LWGLOBALPOOL_RENDER_GLOBAL, GFUSE_TRANSIENT );
			if(memfunc)
			{	
				mem = memfunc->find( chunkName );
				if ( !mem )
					mem = memfunc->create( chunkName, sizeof( int* ));		
				p = ( int * ) mem;

				inst->id=item;
				inst->dbp=p;
			}
		}
   }

   if(!inst || !inst->dbp)
	*err = "Couldn't allocate instance data.";

   return inst;
}




XCALL_( static void )
Destroy( void *inst )
{
   free( inst );
}


/*
======================================================================
Copy()

Handler callback.  Copy instance data.
====================================================================== */

XCALL_( static LWError )
Copy( void *to, void *from )
{
	AINSTANCE *inTo=(AINSTANCE*)to;
	AINSTANCE *inFrom=(AINSTANCE*)from;

	*inTo = *inFrom;
   return NULL;
}


/*
======================================================================
Load()

Handler callback.  Read instance data.
====================================================================== */

XCALL_( static LWError )
Load( void *inst, const LWLoadState *ls )
{

   return NULL;
}


/*
======================================================================
Save()

Handler callback.  Write instance data.
====================================================================== */

XCALL_( static LWError )
Save( void *inst, const LWSaveState *ss )
{

   return NULL;
}


/*
======================================================================
Describe()

Handler callback.  Write a short, human-readable string describing
the instance data.
====================================================================== */

XCALL_( static const char * )
Describe( void *inst )
{
   static char desc[ 256 ];

   AINSTANCE * rin=(AINSTANCE *)inst;

   sprintf( desc, "LayoutPODExportMotion item=%x", rin->id );
   return desc;
}


/*
======================================================================
Flags()

Handler callback.
====================================================================== */

XCALL_( static unsigned int )
Flags( LWInstance )
{
   return LWIMF_AFTERIK;
}


// this is where we record motion ( custom plugins set to afterIK won't get counted)

XCALL_( static void )
Evaluate( void *inst, const LWItemMotionAccess *access )
{
	AINSTANCE * ain=(AINSTANCE *)inst;
	int *dbp;
	dbp=(int*)ain->dbp;
	SceneInventory *scene=(SceneInventory*)*(dbp);
	if(scene==0)
		return;

	int firstFrame=scene->getFirstFrame();

	LWDVector position3;
	LWDVector rotation3;
	LWDVector scale3;


	SceneItem *item=scene->getItem(access->item);	

	access->getParam(LWIP_POSITION,access->time,position3);
	access->getParam(LWIP_ROTATION,access->time,rotation3);
	access->getParam(LWIP_SCALING,access->time,scale3);

	KeyFrame &keyFrame=item->keyFramesOther[access->frame+1-firstFrame];

	keyFrame.frame=access->frame;
	keyFrame.time=(float)access->time;
	keyFrame.position.x=position3[0];
	keyFrame.position.y=position3[1];
	keyFrame.position.z=position3[2];

	keyFrame.scale.x=scale3[0];
	keyFrame.scale.y=scale3[1];
	keyFrame.scale.z=scale3[2];

	keyFrame.orient.x=rotation3[0];
	keyFrame.orient.y=rotation3[1];
	keyFrame.orient.z=rotation3[2];
	
}

XCALL_( static int )
Handler( long version, GlobalFunc *global, LWItemMotionHandler *local,
   void *serverData)
{
   if ( version != LWITEMMOTION_VERSION ) return AFUNC_BADVERSION;

   local->inst->priv	= global;
   local->inst->create  = Create;
   local->inst->destroy = Destroy;
   local->inst->load    = Load;
   local->inst->save    = Save;
   local->inst->copy    = Copy;
   local->inst->descln  = Describe;
   local->evaluate      = Evaluate;
   local->flags         = Flags;

   return AFUNC_OK;
}


extern "C"
{
	ServerRecord      ServerDesc[] = 
	{
		{(const char*)LWLAYOUTGENERIC_CLASS, (const char*)"LayoutPODExport", (ActivateFunc*)Activate},
		{ LWITEMMOTION_HCLASS, "LayoutPODExportMotion", (ActivateFunc*)Handler },
		{ NULL }
	};

}