#ifndef LAYOUT_OGRE_EXPORT_MAIN_H
#define LAYOUT_OGRE_EXPORT_MAIN_H

#include <lwserver.h>
#include <lwcmdseq.h>
#include <lwhost.h>
#include <lwgeneric.h>
#include <lwsurf.h>
#include <lwserver.h>
#include <lwhost.h>
#include <lwrender.h>

namespace LayoutPODExport
{
    class LayoutPODExportMain
    {
    
	public:


		LWLayoutGeneric *layoutGeneric;
		LWItemInfo	*itemInfo;
		LWSceneInfo *sceneInfo;
		LWObjectInfo *objectInfo;
		LWCameraInfo *cameraInfo;
		LWLightInfo	 *lightInfo;	
		LWObjectFuncs *objectFunc;
		LWBoneInfo	*boneInfo;
		LWInterfaceInfo *interfaceInfo;
		GlobalFunc	*globalFunc;
    };
}

#endif