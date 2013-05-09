#include "SceneItemOptions.h"
#include "LayoutPODExportMain.h"

#include <qfile>

using namespace LayoutPODExport;




SceneItemOptions::SceneItemOptions()
{
	//saveOptions(mainData);

	scaleGeometryValue=1.0f;
	scaleUvValue=1.0f;
	vertexFormat=FORMAT_FLOAT;
	geometryOptions=INDEXED_LIST;
	exportItem=true;
	hasBones=false;
	exportMatrix=false;
	exportItemUserOverride=false;
	exportAnimation=false;
	exportSkeletalAnimation=false;
	excludeLeafNodes=true;
	excludeNoWeightBones=true;
	collapseScene=COLLAPSE_ALL;
	includeRestFrame=false;

	exportVertexOptions[EXPORT_VERTEX_NORMAL]=true;
	exportVertexOptions[EXPORT_VERTEX_COLOR_DIFFUSE]=true;
	exportVertexOptions[EXPORT_VERTEX_UV0]=true;
	exportVertexOptions[EXPORT_VERTEX_UV1]=true;
	exportVertexOptions[EXPORT_VERTEX_UV2]=true;
	exportVertexOptions[EXPORT_VERTEX_UV3]=true;
	exportVertexOptions[EXPORT_VERTEX_UV4]=true;
	exportVertexOptions[EXPORT_VERTEX_UV5]=true;
	exportVertexOptions[EXPORT_VERTEX_UV6]=true;
	exportVertexOptions[EXPORT_VERTEX_UV7]=true;
	exportVertexOptions[EXPORT_VERTEX_WEIGHTS]=true;
	exportVertexOptions[EXPORT_VERTEX_GENERATE_TANGENT]=false;

	for(int i=0;i<VERTEX_OPTIONS_LAST;i++)
	{
		isVertexChannel[i]=false;
	}
	isVertexChannel[EXPORT_VERTEX_GENERATE_TANGENT]=true;
	maxBones=0;

	triangleSort=PVRT_NONE;
}



