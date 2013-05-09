


// Basic algorithm
//
// Every UV map gets assigned a special bit within 32 bit mask ( which means there can be max 32 UV maps )
// Polygons are scanned and each polygon's points are evaluated against all UV maps - anytime an UV map if found for a point, this UVs map
// bit id is then added to the polygon overall UV bit mask.

// For each UV map there is a submesh instance created with additional single "global" submesh for polygons which have points sharing multiple UVs.

// All polygons are scanned and if a polys UV bit mask contains more than a single entry, this poly is assigned to the "global"  submesh and its UV mask
// is or'ed with the "global" submesh mask. 
// If a polygon's bit mask contains only one entry, this polygon is assigned to the submesh corresponding to that particular bit 

// The end effet is that polygons which contains points mapped to multiple UVs are separated into one submesh and all polys which contain points with just one UV
// map are stored in its own submesh corresponding to that UV map.






#ifndef DATABASE_H
#define DATABASE_H


#include <qvector>
#include <qstring>
#include <qhash>
#include <qhash>
#include <qpair>
#include <QTextStream>

#include "OGLESTools.h"
#include <QSet>
#include <bitset>

#include "LayoutPODExportMain.h"
#include "SceneItemOptions.h"
#include "Vector3.h"
#include "Quat.h"


namespace LayoutPODExport
{
	
	class Database;

	class GenericMap
	{
	public:
		QString name;
		const char *cName;
		LWID  typeId;
	};

	class ColorMap : public GenericMap
	{	
	public:
	};

	class WeightMap : public GenericMap
	{	
	public:
	};

	class UvMap : public GenericMap
	{	
	public:
	};

	class Surface : public GenericMap
	{
	public:
		LWSurfaceID sId;
		double		smoothAngle;		// smoothing angle for this surface
	};

	class KeyFrame
	{
	public:

		float time;
		int	  frame;		
		MiniEngine::Vector3 position;		
		MiniEngine::Vector3 scale;
		MiniEngine::Vector3	orient;
		MiniEngine::Quat	quatOrient;
		LWFMatrix4		matrix;
	};

	class SceneItem
	{
	public:

		LWItemID  itemId; 
		bool	hasMesh;
		SceneItem  *parent;
		SPODNode  *podNode;
		bool		hasMeshConnection;
		int			podNodeIndex;
		QList<SceneItem*> children;
		QList<KeyFrame>	keyFrames;
		QList<KeyFrame>	keyFramesOther;
		QString		name;
		QString filePath;
		WeightMap *weightMap;
		MiniEngine::Vector3 pivotPos;
		MiniEngine::Vector3 pivotOrient;
		MiniEngine::Vector3 restPos;
		MiniEngine::Vector3	restOrient;
		LWItemType    lwType;
		Database	*database;
		SceneItemOptions options;
		bool	collapsed[2];
	};



	class SceneInventory
	{
	public:
		SceneInventory(LayoutPODExportMain *main);
		~SceneInventory();

		bool reloadScene();

		QList<SceneItem*>&	getTopLevelItems() 	{ return topChildren;}
		QList<SceneItem*>&	getMeshItems() 	{ return meshObjects;}
		QString	getItemName(SceneItem* item) const;	
		SceneItem*	getItem(LWItemID item);

		inline LayoutPODExportMain * getLwMain() { return lwMain;}

		inline int getUvMapCount() const 
		{
			return uvMaps.size();
		}

		inline int getWeightMapCount() const 
		{
			return weightMaps.size();
		}

		inline int getColorMapCount() const 
		{
			return colorMaps.size();
		}

		inline int getFirstFrame() const  { return 	(int) firstFrame;}
		inline int getLastFrame() const  { return 	(int) lastFrame;}
		inline int getFrameStep() const { return (int) frameStep;}

		inline void setFrameRange(int firstFrame, int lastFrame)
		{
			this->firstFrame=firstFrame;
			this->lastFrame=lastFrame;
			numFrames=(lastFrame-firstFrame+1)/frameStep;
		}

		QString getSceneFileName();

		bool serializeToPODFile(QString fileName);

		inline QList<UvMap*>& getUvMaps()  { return uvMaps;}
		inline QList<ColorMap*>& getColorMaps()  { return colorMaps;}
		inline QList<WeightMap*>& getWeightMaps()  { return weightMaps;}

		inline QList<SceneItem*>	getAllItems() { return itemLookup.values();}

		inline QHash<QString, Surface*>& getSurfaceMap() { return mapSurfaces;}

		inline void updateGeneratedFrames(int val) 
		{ 
			if(val>actualGeneratedFrames)
			actualGeneratedFrames=val;
		}

		inline SceneItemOptions&	getGlobalOptions() { return globalOptions;}

		void verifyErrors();
		void verifyObjectsIntegrity();

		void collapseAll(int index,bool excludeLeafNodes,bool excludeNoWeightBones, bool exportSkeletal);
		void collapseAllButTop(int index, bool excludeLeafNodes,bool excludeNoWeightBones, bool exportSkeletal);
		void collapseNone(int index,bool excludeLeafNodes,bool excludeNoWeightBones, bool exportSkeletal);
		void makeUserCollapsePermament();

	protected:
		
		static void checkVector3(MiniEngine::Vector3 &out,float value=0, float tolerance=std::numeric_limits<float>::epsilon());
		static void checkQuat(MiniEngine::Quat &out);
		static bool checkIfEqualVector(const MiniEngine::Vector3 &first,const MiniEngine::Vector3 &second,float tolerance=std::numeric_limits<float>::epsilon());
		static bool checkIfEqualQuat(const MiniEngine::Quat &first,const MiniEngine::Quat &second,float tolerance=std::numeric_limits<float>::epsilon());
		
		static void transformWithParent(KeyFrame &parent, KeyFrame &current);

		void serializeSubmeshVerticesPOD(SPODMesh *podMesh,QSet<int>  &sPoints, SceneItem *item, bool needsRestFrame );
		void createAnimation(SPODNode *node ,SceneItem *item);
		bool checkIfRestPositionNeeded();
		void createAnimationMatrix(SPODNode *node ,SceneItem *item,bool needsRestFrame);
		void updateAllKeyFrames();
		void updateKeyFrameFromParent(SceneItem *item,int frame, KeyFrame &finalFrame);

		char * createPODString( QString& string);
		void initUvMaps();
		void initColorWeightMaps();
		void initSurfaces();
		void createSurface(LWSurfaceID id,LWSurfaceFuncs *surff);

		void createItems(LWItemType);
		void createBoneItems();
		SceneItem* createSceneItem(LWItemID item);
		void createRelationships();
		void checkMeshConnection();
		void checkMeshConnectionItem(SceneItem *item);
		void checkMeshConnectionBone(SceneItem *item);
		void getBoneInfo(SceneItem *object);
		void getObjectInfo(SceneItem *object);
		void clear();
		void createCameraNode(SPODNode *node);
		void createPODNodes(CPVRTModelPOD *scene, bool needsRestFrame);
		void updateBoundingBox();
		void calculateNumberNodes(int &meshNodes, int &otherNodes);
		void sortGeometry(SPODMesh *mesh);
		void reasignBoneBatches(SPODMesh *mesh, SceneItem *item);

		int writeKeyFrameVector(MiniEngine::Vector3 &data,float *animPtr, float snapTo);
		int writeKeyFrameQuat(MiniEngine::Quat &data,float *animPtr);

		QList<SceneItem*> topChildren;
		QList<SceneItem*> meshObjects;
		QList<SceneItem*> lightObjects;
		QList<SceneItem*> cameraObjects;
		QList<SceneItem*> nullObjects;
		QList<SceneItem*> boneObjects;
		LayoutPODExportMain *lwMain;
		QHash<LWItemID, SceneItem*> itemLookup;

		QList<UvMap*>		uvMaps;
		QList<ColorMap*>	colorMaps;			// all available color maps
		QList<WeightMap*>	weightMaps;
		QHash<QString,WeightMap*> mapWeigths;
		QList<Surface*>	surfaces;
		QHash<QString, Surface*>	mapSurfaces;
		int		actualGeneratedFrames;

		LWFrame	numFrames;			
		LWFrame	firstFrame;			
		LWFrame lastFrame;
		LWFrame frameStep;

		MiniEngine::Vector3	mSceneMaxBox;
		MiniEngine::Vector3 mSceneMinBox;

		SceneItemOptions globalOptions;

	};

	class PointSimple
	{

	public:
		LWPntID	id;
		unsigned short numPolys;
		LWPolID  *polyId;	
	};


	class Point
	{
	public:
		LWPntID  id;					// lightwave point id				
		float   normal[3];			// vertex normal
		int		index;			// sequential index which will be used to store point offsets as they are written into XML file 
		int		uvMapCount;
		int		colorMapCount;
		int		weightMapCount;
	};


	class Polygon
	{

	public:
		LWPolID		id;				// polygon id
		int         numPnts;		// number of points
		Point		pPoints[3];		// the actual points attached to this polygon after "internal" unwelding
		float		normal[3];		// polygon normal
		Surface		*surface;
	};


	class Database
	{
	public:

		enum INVALID_GEOMETRY
		{
			DEGENERATED=0,
			NON_TRIANGLE,
			OTHER,
			LAST_INVALID
		};




		PointSimple			*points;
		Polygon				*polys;
		QList<UvMap*>		uvMaps;				// all available uvMaps 
		QList<ColorMap*>	colorMaps;			// all available color maps
		QList<WeightMap*>	weightMaps;		// all available weight maps
		QSet<UvMap*>		uvMapsSet;
		QSet<ColorMap*>		colorMapsSet;			// all available color maps
		QSet<WeightMap*>	weightMapsSet;
		int			numPolys;				// total number of polygons
		int			numPoints;				// total number of points
		int			numPolysScaned;			// total number of polygons scanned
		int			countInvalid[LAST_INVALID];		
		QHash<int,PointSimple*>	mapPoints;
		QHash<int,Polygon*>	mapPolys;
		int tmpCurrIndex;
		int		indexCount;
		int		maxWeigthsPerPoint;
		int		numPointsWithoutUv;
		int		numPointsWithoutColor;
		int		numPointsWithoutWeight;
		SceneInventory  *inventory;
		SceneItem	*sceneItem;
		LayoutPODExportMain  *lwMain;
		LWMeshInfo *meshInfo;
		bool		hasErrors;

		Database(LayoutPODExportMain *lwMain, SceneInventory *iv, SceneItem *item);

		~Database();

		LayoutPODExportMain* getLightwaveMain()	{ return lwMain;}

		
		void update();
		

		static int store_point( Database *db, LWPntID id );
		static int store_polygon( Database *db,LWPolID id  );



	protected:

		void scanGeometry();
		void updatePointsPerPoly();
		void calculateNormalsAndIndexing();
		
		bool hasDiscUv(LWPntID point, LWPolID polygon);

		bool allocateStorage();
		void freeStorage();


	};



}



#endif