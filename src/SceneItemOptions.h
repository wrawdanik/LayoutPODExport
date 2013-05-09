#ifndef SCENE_ITEM_OPTIONS_H
#define SCENE_ITEM_OPTIONS_H

#include <qcolor>
#include <qstring>

namespace LayoutPODExport
{
class LayoutPODExportMain;

	class SceneItemOptions
	{
	public:

		enum VERTEX_OPTIONS
		{
			EXPORT_VERTEX_NORMAL,			
			EXPORT_VERTEX_COLOR_DIFFUSE,
			EXPORT_VERTEX_UV0,
			EXPORT_VERTEX_UV1,
			EXPORT_VERTEX_UV2,
			EXPORT_VERTEX_UV3,
			EXPORT_VERTEX_UV4,
			EXPORT_VERTEX_UV5,
			EXPORT_VERTEX_UV6,
			EXPORT_VERTEX_UV7,
			EXPORT_VERTEX_WEIGHTS,
			EXPORT_VERTEX_GENERATE_TANGENT,
			VERTEX_OPTIONS_LAST
		};


		enum GEOMETRY_OPTIONS
		{	
			INDEXED_LIST,
			STRIP
		};

		enum VERTEX_FORMAT
		{
			FORMAT_FLOAT,
			FORMAT_SHORT
		};

		enum COLLAPSE_SCENE
		{
			COLLAPSE_NONE,
			COLLAPSE_ALL,
			COLLAPSE_TOP_ONLY
		};

		enum TRIANGLE_SORT
		{
			PVRT_NONE,
			PVRT_LIST,
			PVRT_STRIP
		};

		float		scaleGeometryValue;					
		float		scaleUvValue;					
		bool		exportVertexOptions[VERTEX_OPTIONS_LAST];		
		bool		isVertexChannel[VERTEX_OPTIONS_LAST];
		int			maxUVMapsPerPoint;		
		GEOMETRY_OPTIONS	geometryOptions;		// store
		VERTEX_FORMAT	vertexFormat;				// store
		COLLAPSE_SCENE	collapseScene;				// store
		bool		exportItem;						// global on/off for exporting
		bool		exportItemUserOverride;			// set to true once on/off switch has been modified by the user ( to prevent silent override which could come from SceneInventory::verifyError())
		bool		exportAnimation;				// store
		bool		exportSkeletalAnimation;
		bool		exportMatrix;					// store
		bool		excludeLeafNodes;				// store
		bool		excludeNoWeightBones;			// store
		TRIANGLE_SORT			triangleSort;
		bool		includeRestFrame;
		bool		hasBones;
		int			maxBones;
		QString		errorMsg;


		SceneItemOptions();
	};

};



#endif