
#include "Database.h"
#include "LayoutPODExportMain.h"
#include <math.h>
#include <string.h>
#include <assert.h>
#include "vecmat.h"
#include "Matrix3.h"
#include "MathUtils.h"
#include "PVRTGeometry.h"
#include "PVRTBoneBatch.h"
#include <QStack.h>
#include "vecmat.h"

using namespace LayoutPODExport;


// object is exported as single - it is based on texture maps
// if it has more than one texture map - the textures 





SceneInventory::SceneInventory(LayoutPODExportMain *main)
{
	lwMain=main;
}
SceneInventory::~SceneInventory()
{
	clear();
}



SceneItem*	SceneInventory::getItem(LWItemID itemId)
{
	if(itemLookup.contains(itemId))
		return itemLookup.value(itemId);
	return 0;
}


QString	SceneInventory::getItemName(SceneItem* item) const
{
	QString name;

	if(item!=0)
	{
		const char* c_name=lwMain->itemInfo->name(item->itemId);
		if(!c_name)
			return false;
		name.append(c_name);
	}
	return name;
}

void SceneInventory::clear()
{
	QMutableHashIterator<LWItemID, SceneItem*> i(itemLookup);
	while (i.hasNext()) 
	{
		i.next();		
		SceneItem *item=i.value();
		i.remove();
		delete item;
	}

	for(int i=0;i<uvMaps.size();i++)
	{
		delete uvMaps[i];
	}
	uvMaps.clear();

	for(int i=0;i<colorMaps.size();i++)
	{
		delete colorMaps[i];
	}
	colorMaps.clear();

	for(int i=0;i<weightMaps.size();i++)
	{
		delete weightMaps[i];
	}
	for(int i=0;i<surfaces.size();i++)
	{
		delete surfaces[i];
	}

	surfaces.clear();
}

QString SceneInventory::getSceneFileName()
{
	QString fileName;

	if(lwMain->sceneInfo->filename!=0)
	{
		fileName.append(lwMain->sceneInfo->filename);
	}

	return fileName;
}

bool SceneInventory::reloadScene()
{
	clear();

	firstFrame=lwMain->interfaceInfo->previewStart;
	lastFrame=lwMain->interfaceInfo->previewEnd;
	frameStep=lwMain->interfaceInfo->previewStep;


	if(!frameStep)
		frameStep=1;
	numFrames=(lastFrame-firstFrame+1)/frameStep;
	
	initUvMaps();
	initColorWeightMaps();
	initSurfaces();

	//scan all items 

	createItems(LWI_OBJECT);
	createItems(LWI_LIGHT);
	createItems(LWI_CAMERA);
	createBoneItems();	


	createRelationships();

	for(int i=0;i<meshObjects.size();i++)
	{
		getObjectInfo(meshObjects[i]);
	}

	checkMeshConnection();
	verifyErrors();
	verifyObjectsIntegrity();
	actualGeneratedFrames=0;
	
	switch(globalOptions.collapseScene)
	{
	case	SceneItemOptions::COLLAPSE_NONE:
		collapseNone(0,globalOptions.excludeLeafNodes,globalOptions.excludeNoWeightBones,globalOptions.exportSkeletalAnimation);
		break;
	case	SceneItemOptions::COLLAPSE_ALL:
		collapseAll(0,globalOptions.excludeLeafNodes,globalOptions.excludeNoWeightBones,globalOptions.exportSkeletalAnimation);
		break;
	case	SceneItemOptions::COLLAPSE_TOP_ONLY:
		collapseAllButTop(0,globalOptions.excludeLeafNodes,globalOptions.excludeNoWeightBones,globalOptions.exportSkeletalAnimation);
		break;
	}

	return true;
}


void	SceneInventory::createRelationships()
{
	QHash<LWItemID, SceneItem*>::iterator current=itemLookup.begin();
	while (current != itemLookup.end()) 
	{
		SceneItem *item=current.value();
		LWItemID parentId=lwMain->itemInfo->parent( current.key());
		if(parentId!=LWITEM_NULL)
		{
			SceneItem *parent=itemLookup[parentId];
			if(parent)
			{
				parent->children.push_back(item);
				item->parent=parent;
			}
		}
     ++current;
	}

	// update pivot stuff

}


void  SceneInventory::createBoneItems()
{
	for(int i=0;i<meshObjects.size();i++)
	{
		LWItemID id = lwMain->itemInfo->first( LWI_BONE, meshObjects[i]->itemId);

		while ( id ) 
		{
			createSceneItem(id);
			id =lwMain->itemInfo->next( id );
		}	
	}

	for(int i=0;i<nullObjects.size();i++)
	{
		LWItemID id = lwMain->itemInfo->first( LWI_BONE, nullObjects[i]->itemId );
		while ( id ) 
		{
			createSceneItem(id);
			id =lwMain->itemInfo->next( id );
		}	
	}
}

void  SceneInventory::createItems(LWItemType type)
{

	LWItemID id = lwMain->itemInfo->first( type, NULL );
	while ( id ) 
	{
		createSceneItem(id);
		id =lwMain->itemInfo->next( id );
	}	
}

SceneItem* SceneInventory::createSceneItem(LWItemID item)
{
	if(item==LWITEM_NULL)
		return 0;

	SceneItem *sceneItem;

	if(itemLookup.contains(item))
	{
		return 0;
	}
	else
	{
		sceneItem=new SceneItem;

		for(int i=0;i<numFrames+1;i++)
		{
			sceneItem->keyFrames.push_back(KeyFrame());
			sceneItem->keyFramesOther.push_back(KeyFrame());
		}

		sceneItem->weightMap=0;
		sceneItem->hasMeshConnection=false;
		sceneItem->hasMesh=false;	
		sceneItem->podNode=0;
		sceneItem->collapsed[0]=false;
		sceneItem->collapsed[1]=false;
		if(lwMain->itemInfo->parent( item )==LWITEM_NULL)
			topChildren.push_back(sceneItem);
		if(lwMain->itemInfo->type(item)==LWI_OBJECT)
		{
			LWMeshInfoID  mesh=lwMain->objectInfo->meshInfo( item, 1 );
			if(mesh)
			{
				meshObjects.push_back(sceneItem);
				sceneItem->hasMesh=true;
			}
			else
				nullObjects.push_back(sceneItem);
		}
		sceneItem->itemId=item;
		sceneItem->lwType=lwMain->itemInfo->type(item);
		sceneItem->database=0;
		sceneItem->name.append(lwMain->itemInfo->name(item));
		sceneItem->parent=0;
		using namespace MiniEngine;
		LWDVector vec;
		LWDVector vecr;
		lwMain->itemInfo->param(sceneItem->itemId, LWIP_PIVOT, 0, vec );
		lwMain->itemInfo->param(sceneItem->itemId, LWIP_PIVOT_ROT, 0, vecr);
		sceneItem->pivotOrient.x=vecr[0];
		sceneItem->pivotOrient.y=vecr[1];
		sceneItem->pivotOrient.z=vecr[2];
		sceneItem->pivotPos.x=vec[0];sceneItem->pivotPos.y=vec[1];sceneItem->pivotPos.z=vec[2];
		sceneItem->keyFramesOther[0].position=Vector3::Zero;
		sceneItem->keyFramesOther[0].scale=Vector3::One;
		sceneItem->keyFramesOther[0].orient=Vector3::Zero;
		itemLookup.insert(item,sceneItem);
		switch(sceneItem->lwType)
		{
			case LWI_BONE:		
				getBoneInfo(sceneItem);
				boneObjects.push_back(sceneItem);
				break;
			case LWI_CAMERA:
				cameraObjects.push_back(sceneItem);
				break;
			case LWI_LIGHT:
				lightObjects.push_back(sceneItem);
				break;
		}
	}	
	
	return sceneItem;
}

void SceneInventory::checkMeshConnection()
{
	 QList<SceneItem*>	&allItems=getAllItems();

	 for(int i=0;i<meshObjects.size();i++)
	 {
		checkMeshConnectionBone(meshObjects[i]);
		checkMeshConnectionItem(meshObjects[i]);
	 }
}

void SceneInventory::checkMeshConnectionItem(SceneItem *item)
{

	SceneItem *current=item;

	while(current)
	{
		current->hasMeshConnection=true;
		current=current->parent;
	}
}

void SceneInventory::checkMeshConnectionBone(SceneItem *item)
{
	if(item->lwType==LWI_BONE)
		item->hasMeshConnection=true;	

	for(int i=0;i<item->children.size();i++)
	{
		checkMeshConnectionBone(item->children[i]);
	}
}

void SceneInventory::reasignBoneBatches(SPODMesh *mesh, SceneItem *item)
{

	 QList<SceneItem*>	allItems=getAllItems();

	int *cBone= mesh->sBoneBatches.pnBatches;

	

	for(int i=0; i<mesh->sBoneBatches.nBatchCnt;i++)
	{
		for(int j=0;j<mesh->sBoneBatches.pnBatchBoneCnt[i];j++)
		{
			int offset=*cBone;			
				
			bool assigned=false;

			WeightMap* cWeight=item->database->weightMaps[offset];
			for(int z=0;z<allItems.size();z++)
			{
				SceneItem *cItem=allItems[z];
				if(cItem->weightMap==cWeight)
				{
					if(cItem->podNode!=0 && cItem->podNodeIndex!=-1)
					{
						*cBone=cItem->podNodeIndex;
						assigned=true;
					}
				}
			}
			if(assigned==false)
				int bla=20;
			cBone++;
		}
	}

}

void SceneInventory::getBoneInfo(SceneItem *object)
{
	const char *weightMapName=lwMain->boneInfo->weightMap(object->itemId);
	if(weightMapName!=0)
	{
		QString name;
		name.append(weightMapName);
		object->weightMap=mapWeigths[name];
	}
	LWDVector vec;
	lwMain->boneInfo->restParam(object->itemId, LWIP_POSITION,vec );
	object->keyFramesOther[0].position.x=vec[0];
	object->keyFramesOther[0].position.y=vec[1];
	object->keyFramesOther[0].position.z=vec[2];
	lwMain->boneInfo->restParam(object->itemId, LWIP_ROTATION, vec);
	object->keyFramesOther[0].orient.x=vec[0];
	object->keyFramesOther[0].orient.y=vec[1];
	object->keyFramesOther[0].orient.z=vec[2];

}

void SceneInventory::getObjectInfo(SceneItem *object)
{
	object->database=new Database(lwMain, this,object);
	object->database->update();
	object->filePath.append(lwMain->objectInfo->filename(object->itemId));	


	if(object->hasMesh)
	{
		if(object->database->indexCount>0)
		{
			for(int i=0;i<boneObjects.size();i++)
			{
				if(object->database->weightMapsSet.contains(boneObjects[i]->weightMap))
				{
					object->options.hasBones=true;
					globalOptions.hasBones=true;
					break;
				}
			}

			object->options.isVertexChannel[SceneItemOptions::EXPORT_VERTEX_NORMAL]=true;
			object->options.isVertexChannel[SceneItemOptions::EXPORT_VERTEX_NORMAL]=true;
			object->options.isVertexChannel[SceneItemOptions::EXPORT_VERTEX_COLOR_DIFFUSE]=(object->database->uvMapsSet.size()>0);
			for(int i=0;i<object->database->uvMapsSet.size();i++)
			{
				object->options.isVertexChannel[SceneItemOptions::EXPORT_VERTEX_UV0+i]=true;
			}
			object->options.isVertexChannel[SceneItemOptions::EXPORT_VERTEX_WEIGHTS]=(object->database->weightMapsSet.size()>0);
			if(!object->options.hasBones)
				object->options.exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_WEIGHTS]=false;
		}
	}


	

}

void SceneInventory::verifyObjectsIntegrity()
{
	for(int i=0;i<meshObjects.size();i++)
	{
		SceneItem *object=meshObjects[i];


		// verify that object has uvs and normals

		bool foundUvNormal=false;

		for(int i=SceneItemOptions::EXPORT_VERTEX_UV0;i<SceneItemOptions::EXPORT_VERTEX_WEIGHTS;i++)
		{
			if(object->options.exportVertexOptions[i] && object->options.isVertexChannel[i])
			{
				foundUvNormal=true;
			}
		}
		if(foundUvNormal)
		{
			if(!object->options.exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_NORMAL] || !globalOptions.exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_GENERATE_TANGENT])
				foundUvNormal=false;			
		}
		object->options.exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_GENERATE_TANGENT]=foundUvNormal;
	}
}

void SceneInventory::verifyErrors()
{
	
		
	for(int i=0;i<meshObjects.size();i++)
	{
		SceneItem *object=meshObjects[i];

		object->database->hasErrors=false;

		if(object->database->indexCount==0)
			object->database->hasErrors=true;
		else
		{

			for(int i=0;i<Database::LAST_INVALID;i++)
			{
				if(object->database->countInvalid[i]>0)
				{
					object->database->hasErrors=true;
					break;
				}
			}
			if(!object->database->hasErrors)
			{
				if(object->database->uvMaps.size()>0 && object->database->numPointsWithoutUv>0)
				{
					for(int j=SceneItemOptions::EXPORT_VERTEX_UV0;j<SceneItemOptions::EXPORT_VERTEX_WEIGHTS;j++)
					{
						if(object->options.isVertexChannel[j] && object->options.exportVertexOptions[j])
						{
							object->database->hasErrors=true;
							break;
						}
					}

				}
				if(object->database->colorMaps.size()>0 && object->database->numPointsWithoutColor>0 && object->options.isVertexChannel[SceneItemOptions::EXPORT_VERTEX_COLOR_DIFFUSE] &&
				object->options.exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_COLOR_DIFFUSE])	
					object->database->hasErrors=true;
				if(object->database->weightMaps.size()>0 && object->database->numPointsWithoutWeight>0 && object->options.isVertexChannel[SceneItemOptions::EXPORT_VERTEX_WEIGHTS] &&
				object->options.exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_WEIGHTS])		
					object->database->hasErrors=true;
				
			}
		}
		if(!object->options.exportItemUserOverride)
		{
			if(object->database->hasErrors)
				object->options.exportItem=false;
			else
				object->options.exportItem=true;
		}
	}

}


void SceneInventory::initUvMaps()
{
	LWObjectFuncs *objfunc=(LWObjectFuncs *)lwMain->globalFunc( LWOBJECTFUNCS_GLOBAL, GFUSE_TRANSIENT );
	int numUVMaps=objfunc->numVMaps( LWVMAP_TXUV);

	for(int i=0;i<numUVMaps;i++)
	{
		UvMap *uvMap=new UvMap;
		uvMap->name.append(objfunc->vmapName( LWVMAP_TXUV,i ));
		uvMap->cName=objfunc->vmapName( LWVMAP_TXUV,i );
		uvMap->typeId=LWVMAP_TXUV;
		uvMaps.append(uvMap);
	}	
	
}



void SceneInventory::initColorWeightMaps()
{
	LWObjectFuncs *objfunc=(LWObjectFuncs *)lwMain->globalFunc( LWOBJECTFUNCS_GLOBAL, GFUSE_TRANSIENT );
	int numMaps=objfunc->numVMaps( LWVMAP_RGB);

	for(int i=0;i<numMaps;i++)
	{
		ColorMap *cMap=new ColorMap;
		cMap->name.append(objfunc->vmapName( LWVMAP_RGB,i ));
		cMap->cName=objfunc->vmapName( LWVMAP_RGB,i );
		cMap->typeId=LWVMAP_RGB;
		colorMaps.append(cMap);
	}		

	numMaps=objfunc->numVMaps( LWVMAP_RGBA);

	for(int i=0;i<numMaps;i++)
	{
		ColorMap *cMap=new ColorMap;
		cMap->name.append(objfunc->vmapName( LWVMAP_RGBA,i ));
		cMap->cName=objfunc->vmapName( LWVMAP_RGBA,i );
		cMap->typeId=LWVMAP_RGBA;
		colorMaps.append(cMap);
	}	

	numMaps=objfunc->numVMaps( LWVMAP_WGHT);

	for(int i=0;i<numMaps;i++)
	{
		WeightMap *wMap=new WeightMap;
		wMap->name.append(objfunc->vmapName( LWVMAP_WGHT,i ));
		wMap->cName=objfunc->vmapName( LWVMAP_WGHT,i );
		wMap->typeId=LWVMAP_WGHT;
		weightMaps.append(wMap);
		QString name;
		name.append(wMap->cName);
		mapWeigths[name]=wMap;
	}

}


void SceneInventory::initSurfaces()
{

	LWSurfaceFuncs *surff = (LWSurfaceFuncs *)lwMain->globalFunc( LWSURFACEFUNCS_GLOBAL, GFUSE_TRANSIENT );

	if(!surff)
		return ;

	LWSurfaceID id = surff->first();
	while ( id )
	{
		createSurface(id,surff);
		id = surff->next( id );
	}
}

void SceneInventory::createSurface(LWSurfaceID id,LWSurfaceFuncs *surff)
{
	Surface *nSurf=new Surface;

	nSurf->name.append(surff->name(id));
	nSurf->sId=id;

	double *values=surff->getFlt(id,SURF_SMAN );			// get smoothing angle
	if(values)
		nSurf->smoothAngle=values[0];				// first value

	surfaces.append(nSurf);

	mapSurfaces.insert(nSurf->name,nSurf);
}


void SceneInventory::calculateNumberNodes(int &meshNodes, int &otherNodes)
{

	QList<SceneItem*> &allItems=getAllItems();

	for(int i=0;i<allItems.size();i++)
	{
		SceneItem *item=allItems[i];
		if(item->hasMesh && item->options.exportItem)
			meshNodes++;
		else if(!item->collapsed[0])
			otherNodes++;
	}

}

bool SceneInventory::serializeToPODFile(QString fileName)
{

	QByteArray fName = fileName.toLatin1();    
	const char *c_fileName = fName.data(); 

	// create POD file.

	if(globalOptions.hasBones)
	{
		
	}



	CPVRTModelPOD	*podScene=new CPVRTModelPOD();
	podScene->nFlags=0;				// will do float
	podScene->pfColourBackground[0]=0;
	podScene->pfColourBackground[1]=0;
	podScene->pfColourBackground[2]=0;

	podScene->pfColourAmbient[0]=0.3f;
	podScene->pfColourAmbient[1]=0.3f;
	podScene->pfColourAmbient[2]=0.3f;

	// just one fake camera

	podScene->nNumCamera=1;	
	podScene->pCamera=new SPODCamera[1];	
	podScene->pCamera[0].nIdxTarget=-1;
	podScene->pCamera[0].fFOV=0.610865f;
	podScene->pCamera[0].fNear=1.0f;
	podScene->pCamera[0].fFar=1000.0f;
	podScene->pCamera[0].pfAnimFOV=0;
		

	podScene->nNumLight=0;
	podScene->pLight=0;	

	podScene->nNumMesh=0;


	for(int i=0;i<meshObjects.size();i++)
	{
		SceneItem *item=meshObjects.at(i);
		if(item->database->indexCount>0 && item->options.exportItem)
			podScene->nNumMesh++;
	}

	int totalNodes=0;

	// add 1 for the main camera node

	int numMeshNodes=0;
	int numOtherNodes=0;

	calculateNumberNodes(numMeshNodes,numOtherNodes);

	totalNodes=numMeshNodes+numOtherNodes+1;	

	updateBoundingBox();
	podScene->nNumNode=totalNodes;
	podScene->nNumMeshNode=numMeshNodes;
	updateAllKeyFrames();

	bool needsRestFrame=checkIfRestPositionNeeded();

	createPODNodes(podScene,needsRestFrame);

	
	

	podScene->nNumMaterial=0;
	podScene->pMaterial=0;

	int currentNode=0;
	if(podScene->nNumMesh!=0)
	{
		QSet<int> savedPoints;
		
		podScene->pMesh=new SPODMesh[podScene->nNumMesh];
		
		for(int i=0;i<meshObjects.size();i++)			// for each submesh
		{
			SceneItem *item=meshObjects.at(i);
			if(item->database->indexCount>0 && item->options.exportItem)
			{
				serializeSubmeshVerticesPOD(&podScene->pMesh[currentNode],savedPoints,item,needsRestFrame);				
				if(globalOptions.triangleSort!=SceneItemOptions::PVRT_NONE && globalOptions.geometryOptions!=SceneItemOptions::STRIP)
				{
					sortGeometry(&podScene->pMesh[currentNode]);
				}
				currentNode++;
			}
		}
	}



	podScene->nNumTexture=0;
	podScene->pTexture=(SPODTexture * )0;
	
	if(actualGeneratedFrames==1)
		actualGeneratedFrames=0;
	podScene->nNumFrame=actualGeneratedFrames;
	podScene->nFlags=0;

	const char *options="bFixedPoint=0 ";
	const char *history="LayoutPODExport\n";
	podScene->SavePOD(c_fileName,options,history);


	podScene->Destroy();
	delete podScene;


	return true;
}

void SceneInventory::createPODNodes(CPVRTModelPOD *podScene,bool needsRestFrame)
{
	podScene->pNode=(SPODNode *)malloc(sizeof(SPODNode)* podScene->nNumNode);


	int currentNode=0;

	for(int i=0;i<meshObjects.size();i++)
	{
		SceneItem *sceneItem=meshObjects[i];
		if(sceneItem->options.exportItem)
		{			
			sceneItem->podNode=&podScene->pNode[currentNode];
			sceneItem->podNodeIndex=currentNode;
			sceneItem->podNode->nIdx=currentNode;
			sceneItem->podNode->pszName=createPODString(sceneItem->name);
			sceneItem->podNode->nIdxMaterial=-1;			
			sceneItem->podNode->nIdxParent=-1;
			currentNode++;
		}
	}

	// handle camera
	
	podScene->pNode[currentNode].nIdx=0;
	podScene->pNode[currentNode].nIdxParent=-1;

	createCameraNode(&podScene->pNode[currentNode]);


	QList<SceneItem*> &allItems=getAllItems();

	currentNode++;
	for(int i=0;i<allItems.size();i++)
	{
		SceneItem *sceneItem=allItems[i];

		if(!sceneItem->hasMesh && !sceneItem->collapsed[0])
		{
			sceneItem->podNode=&podScene->pNode[currentNode];
			sceneItem->podNode->nIdxParent=-1;
			sceneItem->podNodeIndex=currentNode;
			sceneItem->podNode->pszName=createPODString(sceneItem->name);
			sceneItem->podNode->nIdxMaterial=0;		
			currentNode++;
		}
	}

	podScene->nNumNode=currentNode;

	for(int i=0;i<allItems.size();i++)
	{		
		SceneItem *sceneItem=allItems[i];
		if(sceneItem->podNode!=0)
		{
			if(globalOptions.exportMatrix)
				createAnimationMatrix(sceneItem->podNode ,sceneItem,needsRestFrame);
			else
				createAnimation(sceneItem->podNode ,sceneItem);
			sceneItem->podNode->nIdxParent=-1;

			if(!sceneItem->collapsed[0] || sceneItem->hasMesh)
			{
				SceneItem *currentItem=sceneItem->parent;
			
				while(currentItem)
				{
					if(!currentItem->collapsed[0] || currentItem->parent==0)
						break;
					currentItem=currentItem->parent;
				}
				if(currentItem && !currentItem->collapsed[0])
					sceneItem->podNode->nIdxParent=currentItem->podNodeIndex;
			}
		}
	}
	
}

char * SceneInventory::createPODString( QString& string)
{
	QByteArray stringData = string.toLatin1();    
	const char *c_stringData = stringData.data(); 
	char *location=(char*)malloc(stringData.size()+1);
	location[stringData.size()]=0;
	strcpy(location,c_stringData);

	return location;
}

void SceneInventory::createCameraNode(SPODNode *node)
{
	using namespace MiniEngine;
	
	//quickly hack something for the main camera

	Vector3 firstCameraPos(Vector3::Zero);
	MiniEngine::Quat firstCameraRot(MiniEngine::MathUtils::DegreeToRadian(-90),MiniEngine::Vector3::UnitX);

	firstCameraPos.z=mSceneMinBox.distance(mSceneMaxBox);
	firstCameraPos.y=firstCameraPos.z/3;
	firstCameraPos.z*=4.0f;

	node->nIdx=0;
	node->pszName=createPODString(QString("FakeCamera"));
	node->nIdxMaterial=-1;
	node->nIdxParent=-1;
	node->nAnimFlags=0;
	node->pnAnimPositionIdx=0;
	node->pfAnimPosition=(float*)malloc(3*sizeof(float));	
	node->pfAnimPosition[0]=firstCameraPos.x;
	node->pfAnimPosition[1]=firstCameraPos.y;
	node->pfAnimPosition[2]=firstCameraPos.z;	
	node->pnAnimRotationIdx=0;	
	node->pfAnimRotation=(float*)malloc(4*sizeof(float));	
	node->pfAnimRotation[0]=firstCameraRot.x;
	node->pfAnimRotation[1]=firstCameraRot.y;
	node->pfAnimRotation[2]=firstCameraRot.z;
	node->pfAnimRotation[3]=firstCameraRot.w;
	node->pnAnimScaleIdx=0;
	node->pfAnimScale=(float*)malloc(3*sizeof(float));	
	node->pfAnimScale[0]=1.0f;
	node->pfAnimScale[1]=1.0f;
	node->pfAnimScale[2]=1.0f;
	node->pnAnimMatrixIdx=0;
	node->pfAnimMatrix=0;


}

void SceneInventory::serializeSubmeshVerticesPOD(SPODMesh *podMesh,QSet<int>  &sPoints,  SceneItem *item, bool needsRestFrame )
{

	float rgba[4];
	float uvs[2];

	QString trueS="true";
	QString falseS="false";


	SPODNode *node=item->podNode;

	// create Node for this mesh

	if(globalOptions.exportMatrix)
		createAnimationMatrix(node ,item,needsRestFrame);
	else
		createAnimation(node ,item);	



	const bool exportNormals=item->options.exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_NORMAL];
	bool exportDiffuse=item->options.exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_COLOR_DIFFUSE];
	bool exportWeights=item->options.exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_WEIGHTS];
	bool createTangent=item->options.exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_GENERATE_TANGENT] && item->options.exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_NORMAL];
	const bool vertexFormatFloat=(item->options.vertexFormat==SceneItemOptions::FORMAT_FLOAT);

	const float	scaleGeometryValue=item->options.scaleGeometryValue;
	const float scaleUvValue=item->options.scaleUvValue;

	sPoints.clear();
	sPoints.reserve(item->database->indexCount);

	unsigned int totalUvMaps=0;
	int uvMapCount=SceneItemOptions::EXPORT_VERTEX_UV0+item->database->uvMapsSet.size();
	
	for(int i=SceneItemOptions::EXPORT_VERTEX_UV0;i<uvMapCount;i++)
	{
		if(item->options.exportVertexOptions[i])
			totalUvMaps++;
	}

	if(totalUvMaps>0)
	{
		if(totalUvMaps>item->database->uvMapsSet.size())
			totalUvMaps=item->database->uvMapsSet.size();
	}
	else
		createTangent=false;

	unsigned int totalDiffuseMaps=item->database->colorMapsSet.size();
	if(totalDiffuseMaps==0)
		exportDiffuse=false;


	unsigned int maxWeightMaps=item->database->maxWeigthsPerPoint;
	if(maxWeightMaps==0)
		exportWeights=false;
	else if(maxWeightMaps>4)
		maxWeightMaps=4;
	


	unsigned int vertexStride=sizeof(float)*3;

	if(!vertexFormatFloat)
		vertexStride=sizeof(short)*3+2;

	if(exportNormals)
	{
		if(vertexFormatFloat)
			vertexStride+=sizeof(float)*3;
		else
			vertexStride+=sizeof(short)*3+2;
	}
	if(exportDiffuse )
	{
		vertexStride+=sizeof(int);
	}
	if(totalUvMaps>0)
	{
		if(vertexFormatFloat)
			vertexStride+=sizeof(float)*2*totalUvMaps;
		else
			vertexStride+=(sizeof(short)*2*totalUvMaps);
	}
	if(exportWeights)
	{
		vertexStride+=sizeof(int);	// bone index is always one int ( 4 bytes) for alignment purposes

		vertexStride+=sizeof(float)*maxWeightMaps;
	}

	if(createTangent)
	{
		if(vertexFormatFloat)
			vertexStride+=sizeof(float)*6;
		else
			vertexStride+=(sizeof(short)*6);

	}

	podMesh->nNumVertex=item->database->indexCount;
	podMesh->nNumFaces=item->database->numPolysScaned;
	podMesh->nNumUVW=totalUvMaps;

	// export the faces

	podMesh->sFaces.eType=EPVRTDataType::EPODDataUnsignedShort;
	podMesh->sFaces.n=1;
	podMesh->sFaces.nStride=sizeof(unsigned short);
	
	
	
	unsigned short *tmpFaces=(unsigned short *)malloc(sizeof(unsigned short)*podMesh->nNumFaces*3);
	unsigned short *tmpPointerFaces=tmpFaces;

	for(int i=0;i<item->database->numPolysScaned;i++)
	{
		Polygon  *poly=&item->database->polys[i];

		if(poly->pPoints[0].index>=0 && poly->pPoints[1].index>=0 && poly->pPoints[2].index>=0)
		{
			*tmpPointerFaces++=poly->pPoints[2].index;
			*tmpPointerFaces++=poly->pPoints[1].index;
			*tmpPointerFaces++=poly->pPoints[0].index;			
		}
	}

	podMesh->sFaces.pData=(unsigned char*)tmpFaces;
	podMesh->pnStripLength=(unsigned int*)0;
	podMesh->nNumStrips=0;



	// export vertices

	int dataOffset=0;

	if(vertexFormatFloat)
		podMesh->sVertex.eType=EPVRTDataType::EPODDataFloat;
	else
		podMesh->sVertex.eType=EPVRTDataType::EPODDataShort;	
	podMesh->sVertex.n=3;			// 3 values x,y,z
	podMesh->sVertex.nStride=vertexStride;	
	podMesh->sVertex.pData=(unsigned char*)dataOffset;				// no separate channel for position - the actual data will be placed in SPODMesh.pInterleaved 

	if(vertexFormatFloat)
		dataOffset+=(sizeof(float)*podMesh->sVertex.n);
	else
		dataOffset+=(sizeof(short)*podMesh->sVertex.n)+2;		//pad

	// export normals

	podMesh->sNormals.eType=EPVRTDataType::EPODDataNone ;
	podMesh->sNormals.n=0;			// 3 values x,y,z
	podMesh->sNormals.nStride=0;	
	podMesh->sNormals.pData=(unsigned char*)0;				// no separate channel for position - the actual data will be placed in SPODMesh.pInterleaved 
	if(exportNormals)
	{
		if(vertexFormatFloat)
			podMesh->sNormals.eType=EPVRTDataType::EPODDataFloat;
		else
			podMesh->sNormals.eType=EPVRTDataType::EPODDataShort;
		podMesh->sNormals.n=3;			// 3 values x,y,z
		podMesh->sNormals.nStride=vertexStride;	
		podMesh->sNormals.pData=(unsigned char*)dataOffset;				// no separate channel for position - the actual data will be placed in SPODMesh.pInterleaved 
	}
	if(vertexFormatFloat)
		dataOffset+=(sizeof(float)*podMesh->sNormals.n);
	else
		dataOffset+=(sizeof(short)*podMesh->sNormals.n)+2;		// pad

	// no tangents and no binormals

	podMesh->sTangents.eType=EPVRTDataType::EPODDataNone ;
	podMesh->sTangents.n=0;			// 3 values x,y,z
	podMesh->sTangents.nStride=0;	
	podMesh->sTangents.pData=(unsigned char*)0;				// no separate channel for position - the actual data will be placed in SPODMesh.pInterleaved 

	podMesh->sBinormals.eType=EPVRTDataType::EPODDataNone ;
	podMesh->sBinormals.n=0;			// 3 values x,y,z
	podMesh->sBinormals.nStride=0;	
	podMesh->sBinormals.pData=(unsigned char*)0;				// no separate channel for position - the actual data will be placed in SPODMesh.pInterleaved 


	if(createTangent)
	{
		podMesh->sTangents.n=3;			// 3 values x,y,z
		podMesh->sTangents.nStride=vertexStride;	
		podMesh->sTangents.pData=(unsigned char*)dataOffset;	
		if(vertexFormatFloat)
		{
			podMesh->sTangents.eType=EPVRTDataType::EPODDataFloat;
			dataOffset+=(sizeof(float)*podMesh->sTangents.n);
		}
		else
		{
			podMesh->sTangents.eType=EPVRTDataType::EPODDataShort;
			dataOffset+=(sizeof(short)*podMesh->sTangents.n)+2;
		}

		podMesh->sBinormals.n=3;			// 3 values x,y,z		
		podMesh->sBinormals.nStride=vertexStride;	
		podMesh->sBinormals.pData=(unsigned char*)dataOffset;	
		if(vertexFormatFloat)
		{
			podMesh->sBinormals.eType=EPVRTDataType::EPODDataFloat;
			dataOffset+=(sizeof(float)*podMesh->sBinormals.n);
		}
		else
		{
			podMesh->sBinormals.eType=EPVRTDataType::EPODDataShort;
			dataOffset+=(sizeof(short)*podMesh->sBinormals.n)+2;
		}

	}


	podMesh->psUVW=0;
	if(totalUvMaps)
	{
		podMesh->psUVW=(CPODData*)malloc(sizeof(CPODData)*totalUvMaps);
		for(int i=0;i<totalUvMaps;i++)
		{
			if(vertexFormatFloat)
				podMesh->psUVW[i].eType=EPODDataFloat;	
			else
				podMesh->psUVW[i].eType=EPODDataShort;	
			podMesh->psUVW[i].n=2;
			podMesh->psUVW[i].nStride=vertexStride;
			podMesh->psUVW[i].pData=(unsigned char*)dataOffset;
			if(vertexFormatFloat)
				dataOffset+=(sizeof(float)*podMesh->psUVW[i].n);
			else
				dataOffset+=(sizeof(short)*podMesh->psUVW[i].n)+2;		// pad
		}
	}	
	podMesh->sVtxColours.eType=EPODDataNone;
	podMesh->sVtxColours.n=0;
	podMesh->sVtxColours.nStride=0;
	podMesh->sVtxColours.pData=(unsigned char*)0;
	if(exportDiffuse)
	{
		podMesh->sVtxColours.eType=EPODDataARGB;
		podMesh->sVtxColours.n=1;
		podMesh->sVtxColours.nStride=vertexStride;
		podMesh->sVtxColours.pData=(unsigned char*)dataOffset;
		dataOffset+=(sizeof(int)*podMesh->sVtxColours.n);
	}

	podMesh->sBoneIdx.eType=EPVRTDataType::EPODDataNone;
	podMesh->sBoneIdx.n=0;
	podMesh->sBoneIdx.nStride=0;
	podMesh->sBoneIdx.pData=(unsigned char*)0;

	podMesh->sBoneWeight.eType=EPVRTDataType::EPODDataNone;
	podMesh->sBoneWeight.n=0;
	podMesh->sBoneWeight.nStride=0;
	podMesh->sBoneWeight.pData=(unsigned char*)0;

	if(exportWeights)
	{
		podMesh->sBoneWeight.eType=EPVRTDataType::EPODDataFloat;
		podMesh->sBoneWeight.n=maxWeightMaps;
		podMesh->sBoneWeight.nStride=vertexStride;	
		podMesh->sBoneWeight.pData=(unsigned char*)dataOffset;
		dataOffset+=(sizeof(float)*podMesh->sBoneWeight.n);

		podMesh->sBoneIdx.eType=EPVRTDataType::EPODDataUnsignedByte;
		podMesh->sBoneIdx.n=maxWeightMaps;
		podMesh->sBoneIdx.nStride=vertexStride;	
		podMesh->sBoneIdx.pData=(unsigned char*)dataOffset;
		dataOffset+=(sizeof(unsigned int));
	}





	// create actual vertex data

	int totalInsertedVertices=0;
	//podMesh->pInterleaved=new unsigned char[podMesh->nNumVertex*vertexStride];
	podMesh->pInterleaved=(unsigned char*)malloc(sizeof(unsigned char)*podMesh->nNumVertex*vertexStride);

	float *vertPointer=(float*)podMesh->pInterleaved;
	short *vertPointerShort=(short*)podMesh->pInterleaved;
	
	using namespace MiniEngine;
		
	float weightValues[256];

	LWFVector	position;

	int blaCount=0;

	for(int i=0;i<item->database->numPolysScaned;i++)
	{
		Polygon  *poly=&item->database->polys[i];

		for(int x=0;x<poly->numPnts;x++)
		{		
			if(!sPoints.contains(poly->pPoints[x].index) && poly->pPoints[x].index!=-1 )
			{
				
				item->database->meshInfo->pntBasePos(item->database->meshInfo,poly->pPoints[x].id,position);
				Vector3 lightwaveVec3(position[0] ,position[1],(position[2]));
				lightwaveVec3*=scaleGeometryValue;
				checkVector3(lightwaveVec3);

				if(vertexFormatFloat)
				{
					*vertPointer++=lightwaveVec3.x;
					*vertPointer++=lightwaveVec3.y;
					*vertPointer++=-lightwaveVec3.z;
				}
				else
				{
					*vertPointerShort++=(short)lightwaveVec3.x;
					*vertPointerShort++=(short)lightwaveVec3.y;
					*vertPointerShort++=(short)-lightwaveVec3.z;
					vertPointerShort++;
				}

				

				if(exportNormals)
				{
					Vector3 lightwaveVec3(poly->pPoints[x].normal[0] ,poly->pPoints[x].normal[1],(poly->pPoints[x].normal[2]));
					
					checkVector3(lightwaveVec3);
					lightwaveVec3.normalize();



					if(vertexFormatFloat)
					{
						*vertPointer++=lightwaveVec3.x;
						*vertPointer++=lightwaveVec3.y;
						*vertPointer++=-lightwaveVec3.z;
					}
					else
					{
						*vertPointerShort++=(short)(lightwaveVec3.x* scaleUvValue);
						*vertPointerShort++=(short)(lightwaveVec3.y* scaleUvValue);
						*vertPointerShort++=(short)(-lightwaveVec3.z* scaleUvValue);
						vertPointerShort++;
					}

				
				}

				if(createTangent)
				{
					if(vertexFormatFloat)
					{
						vertPointer+=6;
					}
					else
					{
						vertPointerShort+=8;
					}
				}

				if(totalUvMaps)
				{							
						QListIterator<UvMap *> iterUv(item->database->uvMaps);
						int countUv=0;
						while (iterUv.hasNext())
						{							
							if(item->options.exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_UV0+countUv])
							{
								UvMap *map=iterUv.next();		
								void * currentMap = item->database->meshInfo->pntVLookup( item->database->meshInfo, map->typeId, map->cName );
								int mapped=item->database->meshInfo->pntVPIDGet(item->database->meshInfo, poly->pPoints[x].id,poly->id, uvs, currentMap );
								if(!mapped)
									mapped=item->database->meshInfo->pntVIDGet(item->database->meshInfo, poly->pPoints[x].id, uvs, currentMap );	
								if(!mapped)
								{
									uvs[0]=0;
									uvs[1]=0;
								}
								if(vertexFormatFloat)
								{
									*vertPointer++=(uvs[0]*scaleUvValue);
									*vertPointer++=(uvs[1]*scaleUvValue);
								}
								else
								{
									*vertPointerShort++=(short)(uvs[0]* scaleUvValue);
									*vertPointerShort++=(short)(uvs[1]* scaleUvValue);
								}
							}
							countUv++;
						}
				}

				if(exportDiffuse )
				{
					QListIterator<ColorMap *> iterColor(item->database->colorMaps);
					if (iterColor.hasNext())
					{
						ColorMap *map=iterColor.next();

						void * currentMap = item->database->meshInfo->pntVLookup( item->database->meshInfo, map->typeId, map->cName );
		
						int mapped=item->database->meshInfo->pntVIDGet( item->database->meshInfo, poly->pPoints[x].id, rgba, currentMap );
						if(!mapped)
						{
							rgba[0]=1;
							rgba[1]=1;
							rgba[2]=1;
							rgba[3]=1;
						}

						unsigned int *diffusePointer;
					
						if(vertexFormatFloat)
							diffusePointer=(unsigned int *)vertPointer;
						else
							diffusePointer=(unsigned int *)vertPointerShort;	

						unsigned int val;
						unsigned int valFinal;

						val = static_cast<unsigned int>(rgba[3] * 255);
						valFinal = val << 24;

						val = static_cast<unsigned int>(rgba[2] * 255);
						valFinal += val << 16;

						val = static_cast<unsigned int>(rgba[1] * 255);
						valFinal += val << 8;

						val = static_cast<unsigned int>(rgba[0] * 255);
						valFinal += val;
					
						*diffusePointer++=valFinal;

						if(vertexFormatFloat)
							vertPointer=(float *)diffusePointer;
						else
							vertPointerShort=(short *)diffusePointer;
					}

				}
				if(exportWeights )
				{

					memset(weightValues,0,sizeof(float)*256);
					float mappedWeightValues[4]={0,0,0,0};

					unsigned char *vertPointerByte=0;

					float *boneWeigthPointer=0;

					if(vertexFormatFloat)
					{
						boneWeigthPointer=vertPointer;
						vertPointer+=maxWeightMaps;	// skip to boneIdx
						vertPointerByte=(unsigned char *)vertPointer;
						
					}
					else
					{
						boneWeigthPointer=(float*)vertPointerShort;
						vertPointerShort+=maxWeightMaps*2; // skip to boneIdx						
						vertPointerByte=(unsigned char *)vertPointerShort;
					}

					
					QListIterator<WeightMap *> iterWeight(item->database->weightMaps);
					int weightOffset=0;
					int usedWeights=0;
					while (iterWeight.hasNext())					
					{						
						WeightMap *map=iterWeight.next();
						void * currentMap = item->database->meshInfo->pntVLookup( item->database->meshInfo, map->typeId, map->cName );
						int mapped=item->database->meshInfo->pntVIDGet(item->database->meshInfo, poly->pPoints[x].id, &weightValues[weightOffset], currentMap );
						if(mapped && weightValues[weightOffset]!=0 && usedWeights<4 && weightValues[weightOffset]>0.0f)
						{							
							*vertPointerByte++=(unsigned char)weightOffset;	
							mappedWeightValues[usedWeights]=weightValues[weightOffset];
							++usedWeights;
						}						
						++weightOffset;
					}
					while(usedWeights<4)
					{
						*vertPointerByte++=0;
						usedWeights++;
					}
					if(vertexFormatFloat)
						vertPointer=(float *)vertPointerByte;
					else
						vertPointerShort=(short *)vertPointerByte;

					float tmpTotal=0;

					for(int i=0;i<4;i++)
					{
						tmpTotal+=mappedWeightValues[i];
						if(tmpTotal>1.0f)
						{
							mappedWeightValues[i]=0;
						}
					}
					float rest=0;
					if(tmpTotal<1.0f)
					{
						rest=1.0f-tmpTotal;
						for(int i=0;i<maxWeightMaps;i++)
						{
							if(mappedWeightValues[i]==0)
							{
								mappedWeightValues[i]=rest;
								break;
							}
						}
					}



					for(int i=0;i<maxWeightMaps;i++)
					{

						*boneWeigthPointer++=mappedWeightValues[i];
					}
					blaCount++;
				}

				sPoints.insert(poly->pPoints[x].index);
				totalInsertedVertices++;
			}
		}
	}

	podMesh->sBoneBatches.pnBatches=0;
	podMesh->sBoneBatches.pnBatchBoneCnt=0;
	podMesh->sBoneBatches.pnBatchOffset=0;
	podMesh->sBoneBatches.nBatchBoneMax=0;
	podMesh->sBoneBatches.nBatchCnt=0;

	PVRTMatrixIdentityF(podMesh->mUnpackMatrix);

	if(exportWeights && item->options.hasBones)
	{		
		char *newVertices;
		int vertexNumOut=0;

		int maxBones=item->database->weightMapsSet.size();

		if(item->options.maxBones>0 && item->options.maxBones<maxBones)
			maxBones=item->options.maxBones;

		EPVRTError error=podMesh->sBoneBatches.Create(&vertexNumOut, 
					&newVertices,
					(unsigned short*)podMesh->sFaces.pData,
					podMesh->nNumVertex, 
					(char*)podMesh->pInterleaved,
					podMesh->sVertex.nStride,
					(int)podMesh->sBoneWeight.pData,
					podMesh->sBoneWeight.eType,
					(int)podMesh->sBoneIdx.pData,
					podMesh->sBoneIdx.eType,
					podMesh->nNumFaces,
					maxBones,
					item->database->maxWeigthsPerPoint);
		if(error!=PVR_FAIL)
		{
			free(podMesh->pInterleaved);
			podMesh->pInterleaved=(unsigned char*)newVertices;	
			podMesh->nNumVertex=vertexNumOut;


			reasignBoneBatches(podMesh,item);
		}


	}


	if(createTangent)
	{
		int vertexNumOut=0;
		char *newVertices=0;

		EPVRTError error=PVRTVertexGenerateTangentSpace(&vertexNumOut,&newVertices,(unsigned short*)podMesh->sFaces.pData,podMesh->nNumVertex,(char*)podMesh->pInterleaved,podMesh->sVertex.nStride,
						(int)podMesh->sVertex.pData,podMesh->sVertex.eType,
						(int)podMesh->sNormals.pData,podMesh->sNormals.eType,
						(int)podMesh->psUVW[0].pData,podMesh->psUVW[0].eType,
						(int)podMesh->sTangents.pData,podMesh->sTangents.eType,
						(int)podMesh->sBinormals.pData,podMesh->sBinormals.eType,
						podMesh->nNumFaces,0);
		if(error!=PVR_FAIL)
		{
			free(podMesh->pInterleaved);
			podMesh->pInterleaved=(unsigned char*)newVertices;	
			podMesh->nNumVertex=vertexNumOut;
		}
	}
	if(globalOptions.geometryOptions==SceneItemOptions::STRIP)
	{
		unsigned short          *ppwStrips;
		PVRTTriStrip(&ppwStrips,&podMesh->pnStripLength,&podMesh->nNumStrips,(unsigned short*)podMesh->sFaces.pData,podMesh->nNumFaces);
		free(podMesh->sFaces.pData);
		podMesh->sFaces.pData=(unsigned char*)ppwStrips;

	}
	else if(globalOptions.triangleSort==SceneItemOptions::PVRT_STRIP)
	{
		PVRTTriStripList((unsigned short*)podMesh->sFaces.pData,podMesh->nNumFaces);
	}
	else if(globalOptions.triangleSort==SceneItemOptions::PVRT_LIST)
	{

	}


	// verify we didn't have any buffer overruns
	

/*
	
	if(currOptions->useStrips)
	{

				STRIPERCREATE sc;
				sc.DFaces			= 0;
				sc.WFaces			= (unsigned short*)podMesh->sFaces.pData;
				sc.NbFaces			= podMesh->nNumFaces;
				sc.AskForWords		= true;
				sc.ConnectAllStrips	= true;
				sc.OneSided			= true;
				sc.SGIAlgorithm		= true;

				Striper Strip;
				Strip.Init(sc);

				STRIPERRESULT sr;
				Strip.Compute(sr);

				podMesh->nNumFaces=*sr.StripLengths;
				podMesh->nNumStrips=1;
				podMesh->pnStripLength=new unsigned int[1];
				podMesh->pnStripLength[0]=podMesh->nNumFaces;
				delete []podMesh->sFaces.pData;
				podMesh->sFaces.pData=new unsigned char[podMesh->nNumFaces*sizeof(unsigned short)];
				memcpy(podMesh->sFaces.pData,sr.StripRuns,podMesh->nNumFaces*sizeof(unsigned short));	
	}
	else
	{
		
	

	}
*/

	//



	podMesh->ePrimitiveType=ePODTriangles;

}


void SceneInventory::updateAllKeyFrames()
{
	using namespace MiniEngine;

	QList<SceneItem*> &allItems=getAllItems();

	for(int i=0;i<allItems.size();i++)
	{
		SceneItem *item=allItems[i];
		if(item->lwType!=LWI_BONE)
		{
			item->keyFramesOther[0].position=item->keyFramesOther[1].position;
			item->keyFramesOther[0].scale=item->keyFramesOther[1].scale;
		}
	}


	for(int j=0;j<numFrames+1;j++)
	{
		QStack<SceneItem*> stack;

		for(int i=0;i<topChildren.size();i++)
		{
			stack.push(topChildren[i]);
		}

		while(!stack.empty())
		{

			SceneItem *item=stack.pop();

			LWFMatrix4 mat;
			LWMAT_identity4( mat );



			KeyFrame &keyFrame=item->keyFrames[j];

			if(globalOptions.exportMatrix)
			{
				LWMAT_getTransformKeyFrameMatrix(item,this,j,mat,false,false);

				memcpy(keyFrame.matrix,mat,sizeof(float)*16);
			}
			else
			{
				keyFrame.scale=Vector3::One;

				LWMAT_getTransformKeyFrame(item,this,j,mat,keyFrame.scale,false,false);



				LWDVector finalRot;
				LWDVector finalPos;
				LWDVector finalScale;

				LWMAT_getEuler(mat,finalRot);
				LWMAT_getTranslation(mat,finalPos);
				LWMAT_getScale(mat,finalScale);



				keyFrame.position.x=finalPos[0];keyFrame.position.y=finalPos[1]; keyFrame.position.z=finalPos[2];
				keyFrame.position.z*=-1;

				Quat ux=Quat(finalRot[0]*-1,Vector3::UnitY);
				Quat uy=Quat(finalRot[1]*-1,Vector3::UnitX);
				Quat uz=Quat(finalRot[2],Vector3::UnitZ);
				ux.normalize();
				uy.normalize();
				uz.normalize();

				keyFrame.quatOrient=ux*uy*uz;	
				keyFrame.quatOrient.normalize();

				checkVector3(keyFrame.scale,1.0f);
				checkVector3(keyFrame.position,0);
				checkQuat(keyFrame.quatOrient);

			}

			for(QList<SceneItem*>::iterator current=item->children.begin(); current!=item->children.end(); ++current)
			{			
				stack.push_back(*current);
			}		
		}
	}
}

void SceneInventory::transformWithParent(KeyFrame &parentKeyFrame, KeyFrame &itemKeyFrame)
{
	using namespace MiniEngine;

	Quat itemDerivedOrient=parentKeyFrame.quatOrient*itemKeyFrame.quatOrient;
	itemDerivedOrient.normalize();
	Vector3 itemDerivedScale=parentKeyFrame.scale*itemKeyFrame.scale;
	Vector3 itemDerivedPos=parentKeyFrame.quatOrient * ( parentKeyFrame.scale * itemKeyFrame.position);
	itemDerivedPos+=parentKeyFrame.position;					

	itemKeyFrame.quatOrient=itemDerivedOrient;
	itemKeyFrame.scale=itemDerivedScale;
	itemKeyFrame.position=itemDerivedPos;
	checkQuat(itemKeyFrame.quatOrient);
	checkVector3(itemKeyFrame.scale);
	checkVector3(itemKeyFrame.position);	
	

}

bool SceneInventory::checkIfRestPositionNeeded()
{

	if(numFrames>1)
	{
		for(int i=0;i<boneObjects.size();i++)
		{
			SceneItem *bone=boneObjects[i];
		
			if(!checkIfEqualVector(bone->keyFrames[0].position,bone->keyFrames[1].position,0.001f))
				return false;
			if(!checkIfEqualQuat(bone->keyFrames[0].quatOrient,bone->keyFrames[1].quatOrient,0.001f))
				return false;
		}
	}
	return false;
}

int SceneInventory::writeKeyFrameVector(MiniEngine::Vector3 &data,float *animPtr, float snapTo)
{
	using namespace MiniEngine;

	if(MathUtils::Equal(data.x,snapTo))
		data.x=snapTo;
	if(MathUtils::Equal(data.y,snapTo))
		data.y=snapTo;
	if(MathUtils::Equal(data.z,snapTo))
		data.z=snapTo;
	*animPtr++=data.x;
	*animPtr++=data.y;
	*animPtr++=data.z;

	return 3;
}

int SceneInventory::writeKeyFrameQuat(MiniEngine::Quat &data,float *animPtr)
{
	using namespace MiniEngine;

	if(MathUtils::Equal(data.x,0))
		data.x=0;
	if(MathUtils::Equal(data.y,0))
		data.y=0;
	if(MathUtils::Equal(data.z,0))
		data.z=0;
	if(MathUtils::Equal(data.w,0))
		data.w=0;

	*animPtr++=-data.x;
	*animPtr++=-data.y;
	*animPtr++=-data.z;
	*animPtr++=data.w;

	return 4;
}


void SceneInventory::createAnimation(SPODNode *node ,SceneItem *item)
{

	using namespace MiniEngine;
	node->nAnimFlags=0;

	// figure out which channels are worth saving

	bool needsRestFrame=(globalOptions.hasBones && globalOptions.includeRestFrame);

	KeyFrame prevFrame;

	int frameCount=0;
	int fromKeyFrame=1;
	int toKeyFrame=item->keyFrames.size();

	int finalNumFrame=numFrames;
	int finalFirstFrame=1;
	if(needsRestFrame)
	{
		fromKeyFrame=0;
	}

	if(!globalOptions.exportAnimation)
		toKeyFrame=fromKeyFrame+1;

	for(int i=fromKeyFrame;i<toKeyFrame;i++)
	{
		KeyFrame &keyFrame=item->keyFrames[i];		

		if(i==fromKeyFrame)
			prevFrame=keyFrame;

		if(keyFrame.position!=prevFrame.position)
			node->nAnimFlags|=ePODHasPositionAni;
		if(keyFrame.quatOrient!=prevFrame.quatOrient)
			node->nAnimFlags|=ePODHasRotationAni;
		if(keyFrame.scale!=prevFrame.scale)
			node->nAnimFlags|=ePODHasScaleAni;		
	}

	float *animPtr=0;

	int lastKeyFrame=toKeyFrame;	
	node->pnAnimPositionIdx=0;
	if((node->nAnimFlags&ePODHasPositionAni)==0)
		lastKeyFrame=fromKeyFrame+1;

	node->pfAnimPosition=(float*)malloc((lastKeyFrame-fromKeyFrame)*3*sizeof(float));	
	animPtr=node->pfAnimPosition;
	frameCount=0;
	for(int i=fromKeyFrame;i<lastKeyFrame;i++)
	{		
		animPtr+=writeKeyFrameVector(item->keyFrames[i].position,animPtr,0);
		++frameCount;
		if(frameCount>actualGeneratedFrames)
			actualGeneratedFrames=frameCount;
	}

	lastKeyFrame=toKeyFrame;
	node->pnAnimRotationIdx=0;
	if((node->nAnimFlags&ePODHasRotationAni)==0)
		lastKeyFrame=fromKeyFrame+1;
	
	node->pfAnimRotation=(float*)malloc((lastKeyFrame-fromKeyFrame)*4*sizeof(float));	
	animPtr=node->pfAnimRotation;
	frameCount=0;
	for(int i=fromKeyFrame;i<lastKeyFrame;i++)
	{
		animPtr+=writeKeyFrameQuat(item->keyFrames[i].quatOrient,animPtr);
		++frameCount;
		if(frameCount>actualGeneratedFrames)
			actualGeneratedFrames=frameCount;
	}

	lastKeyFrame=toKeyFrame;
	node->pnAnimScaleIdx=0;
	if((node->nAnimFlags&ePODHasScaleAni)==0)
		lastKeyFrame=fromKeyFrame+1;

	node->pfAnimScale=(float*)malloc((lastKeyFrame-fromKeyFrame)*3*sizeof(float));	
	animPtr=node->pfAnimScale;
	frameCount=0;
	for(int i=fromKeyFrame;i<lastKeyFrame;i++)
	{
		animPtr+=writeKeyFrameVector(item->keyFrames[i].scale,animPtr,1.0f);
		++frameCount;
		if(frameCount>actualGeneratedFrames)
			actualGeneratedFrames=frameCount;
	}

	node->pnAnimMatrixIdx=0;
	node->pfAnimMatrix=0;

}




void SceneInventory::createAnimationMatrix(SPODNode *node ,SceneItem *item, bool needsRestFrame)
{

	using namespace MiniEngine;

	
	node->nAnimFlags=0;

	// figure out which channels are worth saving

	KeyFrame prevFrame;
	int runKeyframes=1;

	int finalNumFrame=item->keyFrames.size();
	int finalFirstFrame=1;
	if(needsRestFrame)
	{
		finalFirstFrame=0;
		finalNumFrame+=1;
	}


	if(!globalOptions.exportAnimation)
		finalNumFrame=finalFirstFrame+1;


	for(int i=finalFirstFrame;i<finalNumFrame;i++)
	{
		KeyFrame &keyFrame=item->keyFrames[i];		

		if(i==0)
			prevFrame=keyFrame;

		if(memcmp(keyFrame.matrix,prevFrame.matrix,sizeof(float)*16)!=0)
		{
			node->nAnimFlags|=ePODHasMatrixAni;	
			runKeyframes=numFrames+1;
			break;
		}
	}

	float *animPtr=0;

	node->pnAnimPositionIdx=0;
	node->pfAnimPosition=0;
	node->pnAnimRotationIdx=0;
	node->pfAnimRotation=0;
	node->pnAnimScaleIdx=0;
	node->pfAnimScale=0;

	node->pfAnimMatrix=(float*)malloc(runKeyframes*16*sizeof(float));	
	animPtr=node->pfAnimMatrix;
	for(int i=0;i<runKeyframes;i++)
	{
		KeyFrame &keyFrame=item->keyFrames[i];
		memcpy(animPtr,keyFrame.matrix,sizeof(float)*16);
		animPtr+=16;
	}
	updateGeneratedFrames(runKeyframes);
	node->pnAnimMatrixIdx=0;

}

void SceneInventory::makeUserCollapsePermament()
{
	 QList<SceneItem*> &items=getAllItems();

	 for(int i=0;i<items.size();i++)
	 {
		SceneItem *item=items[i];
		item->collapsed[0]=item->collapsed[1];
	 }

}


void SceneInventory::collapseAll(int index,bool excludeLeafNodes,bool excludeNoWeightBones, bool exportSkeletal)
{
	 QList<SceneItem*> &items=getAllItems();

	 for(int i=0;i<items.size();i++)
	 {
		SceneItem *item=items[i];
		
		if(item->lwType==LWI_BONE)
		{
			if(!exportSkeletal)
				item->collapsed[index]=true;
			else
			{
				if(excludeNoWeightBones)
					item->collapsed[index]=(item->weightMap==0);
				else
					item->collapsed[index]=false;
			}
		}
		else
		{
			item->collapsed[index]=true;
		}
			
	 }

}

void SceneInventory::collapseAllButTop(int index,bool excludeLeafNodes,bool excludeNoWeightBones, bool exportSkeletal)
{
	 QList<SceneItem*> &items=getAllItems();

	 for(int i=0;i<items.size();i++)
	 {
		SceneItem *item=items[i];
		if(item->lwType==LWI_BONE)
		{
			if(!exportSkeletal)
				item->collapsed[index]=true;
			else
			{
				if(excludeNoWeightBones)
					item->collapsed[index]=(item->weightMap==0);
				else
					item->collapsed[index]=false;
			}

		}
		else
		{
			if(excludeLeafNodes)
				item->collapsed[index]=(item->parent!=0 || !item->hasMeshConnection);
			else
				item->collapsed[index]=(item->parent!=0);
		}

	 }

}
void SceneInventory::collapseNone(int index, bool excludeLeafNodes,bool excludeNoWeightBones, bool exportSkeletal)
{
	 QList<SceneItem*> &items=getAllItems();

	 for(int i=0;i<items.size();i++)
	 {
		SceneItem *item=items[i];
		if(item->lwType==LWI_BONE)
		{
			if(!exportSkeletal)
				item->collapsed[index]=true;
			else
			{
				if(excludeNoWeightBones)
					item->collapsed[index]=(item->weightMap==0);
				else
					item->collapsed[index]=false;		
			}
		}
		else
		{
			if(excludeLeafNodes)
				item->collapsed[index]=!item->hasMeshConnection;
			else
				item->collapsed[index]=false;
		}
	 }

}



//
// Database
//
//
//


Database::Database(LayoutPODExportMain *main,SceneInventory *iv,SceneItem *item)
{
	lwMain=main;
	inventory=iv;
	sceneItem=item;
	meshInfo=lwMain->objectInfo->meshInfo(sceneItem->itemId,1);	
	for(int i=0;i<LAST_INVALID;i++)
		countInvalid[i]=0;
	allocateStorage();
	hasErrors=false;

}

Database::~Database()
{
	freeStorage();
}



void Database::update()
{
	numPolysScaned;
	maxWeigthsPerPoint=0;
	numPointsWithoutUv=0;
	numPointsWithoutColor=0;
	numPointsWithoutWeight=0;
	scanGeometry();
	updatePointsPerPoly();
	indexCount=0;
	calculateNormalsAndIndexing();
}


void Database::scanGeometry()
{

	
	tmpCurrIndex=0;
	meshInfo->scanPoints(meshInfo,(LWPntScanFunc (__cdecl *))Database::store_point,this);
	tmpCurrIndex=0;
	meshInfo->scanPolys(meshInfo,(LWPolScanFunc  (__cdecl *))Database::store_polygon,this);
	numPolysScaned=tmpCurrIndex;

	QList<WeightMap*> &sceneWeightMaps=inventory->getWeightMaps();
	for(int i=0;i<sceneWeightMaps.size();i++)
	{
		if(weightMapsSet.contains(sceneWeightMaps[i]))
		{
			weightMaps.push_back(sceneWeightMaps[i]);
		}
	}	

	QList<UvMap*> &sceneUvMaps=inventory->getUvMaps();
	for(int i=0;i<sceneUvMaps.size();i++)
	{
		if(uvMapsSet.contains(sceneUvMaps[i]))
		{
			uvMaps.push_back(sceneUvMaps[i]);
		}
	}

	QList<ColorMap*> &sceneColorMaps=inventory->getColorMaps();
	for(int i=0;i<sceneColorMaps.size();i++)
	{
		if(colorMapsSet.contains(sceneColorMaps[i]))
		{
			colorMaps.push_back(sceneColorMaps[i]);
		}
	}

}
void Database::updatePointsPerPoly()
{
	int i,j,k;
	Polygon *cpol;

	// scan all polygons and for each point update that points number of polys

	for(i=0;i<numPolysScaned;i++)
	{
		cpol=&polys[i];
		if(!cpol)
			break;
		for(j=0;j<cpol->numPnts;j++)
		{
			if(mapPoints.contains((int)cpol->pPoints[j].id))
			{
				PointSimple *pSimple=mapPoints.value((int)cpol->pPoints[j].id);
				cpol->pPoints[j].normal[0]=cpol->normal[0];
				cpol->pPoints[j].normal[1]=cpol->normal[1];
				cpol->pPoints[j].normal[2]=cpol->normal[2];
				pSimple->numPolys++;
			}
		}
	}

	// now scan all points alocate buffer for each point to hold polys id

	for(i=0;i<numPoints;i++)
	{
		PointSimple *pSimple=&points[i];
		if(pSimple && pSimple->numPolys)
		{
			pSimple->polyId=(LWPolID*)malloc(pSimple->numPolys*sizeof(LWPolID));
			if(pSimple->polyId)
			{
				memset(pSimple->polyId,0,pSimple->numPolys*sizeof(LWPolID));
			}
		}
	}


	// now scan the polys again and for each point add that
	// polygon id to the coresponding point's list of polygons

	for(i=0;i<numPolys;i++)
	{
		cpol=&polys[i];
		if(!cpol)
			break;
		for(j=0;j<cpol->numPnts;j++)
		{
			if(mapPoints.contains((int)cpol->pPoints[j].id))
			{
				PointSimple *pSimple=mapPoints.value((int)cpol->pPoints[j].id);
				for(k=0;k<pSimple->numPolys;k++)
				{
					if(pSimple->polyId[k]==cpol->id)
						break;
					if(pSimple->polyId[k]==0 )
					{
						pSimple->polyId[k]=cpol->id;
						break;
					}
				}
			}
		}
	}


}

int Database::store_point( Database *db, LWPntID id )
{

	PointSimple *cpoint;

	cpoint=&db->points[db->tmpCurrIndex];

	if(!cpoint)
		return EDERR_BADARGS;
	cpoint->id=id;
	cpoint->numPolys=0;
	cpoint->polyId=0;
	db->mapPoints.insert((int)id,cpoint);
	db->tmpCurrIndex++;

	return 0;
}
int Database::store_polygon( Database *db,LWPolID id  )
{
	if(db->meshInfo->polType(db->meshInfo,id)!=LWPOLTYPE_FACE)
	{
		return 0;	
	}
	int numPoints=db->meshInfo->polSize(db->meshInfo,id);

	if(numPoints<3)
	{
		db->countInvalid[DEGENERATED]++;
		return 0;	
	}

	if(numPoints>3)
	{
		db->countInvalid[NON_TRIANGLE]++;
		return 0;	
	}


	Polygon *cpol;

	cpol=&db->polys[db->tmpCurrIndex];	
	if(!cpol)
		return 1;

	db->numPolysScaned++;
	cpol->id=id;
	cpol->numPnts=numPoints;

	db->meshInfo->polBaseNormal( db->meshInfo, cpol->id, cpol->normal);

	for(int i=0;i<cpol->numPnts;i++)
	{
		cpol->pPoints[i].id=db->meshInfo->polVertex(db->meshInfo, id, i);
		cpol->pPoints[i].index=-1;
		cpol->pPoints[i].uvMapCount=0;
		cpol->pPoints[i].colorMapCount=0;
		cpol->pPoints[i].weightMapCount=0;
	}		


	cpol->surface=db->inventory->getSurfaceMap().value(QString::fromAscii(db->meshInfo->polTag( db->meshInfo, cpol->id, LWPTAG_SURF )));

	// 

	QList<UvMap*>& uvMaps=db->inventory->getUvMaps();

	for(int i=0;i<uvMaps.size();i++)
	{
		float uvValue[2];

		UvMap *map=uvMaps[i];

		void * currentMap = db->meshInfo->pntVLookup( db->meshInfo, map->typeId, map->cName );
		for(int j=0;j<cpol->numPnts;j++)
		{			
			int mapped=db->meshInfo->pntVIDGet( db->meshInfo, cpol->pPoints[j].id, uvValue, currentMap );
			if(mapped)
			{
				db->uvMapsSet.insert(map);
				cpol->pPoints[j].uvMapCount++;
			}
		}		

	}

	QList<ColorMap*>& colorMaps=db->inventory->getColorMaps();

	for(int i=0;i<colorMaps.size();i++)
	{
		float rgbaValue[4];

		ColorMap *map=colorMaps[i];

		void * currentMap = db->meshInfo->pntVLookup( db->meshInfo, map->typeId, map->cName );
		for(int j=0;j<cpol->numPnts;j++)
		{			
			int mapped=db->meshInfo->pntVIDGet( db->meshInfo, cpol->pPoints[j].id, rgbaValue, currentMap );
			if(mapped)
			{
				db->colorMapsSet.insert(map);
				cpol->pPoints[j].colorMapCount++;
			}
		}		
	}


	QList<WeightMap*>& weightMaps=db->inventory->getWeightMaps();

	for(int i=0;i<weightMaps.size();i++)
	{
		float weightValue[32];

		WeightMap *map=weightMaps[i];

		void * currentMap = db->meshInfo->pntVLookup( db->meshInfo, map->typeId, map->cName );
		for(int j=0;j<cpol->numPnts;j++)
		{			
			int mapped=db->meshInfo->pntVIDGet( db->meshInfo, cpol->pPoints[j].id, weightValue, currentMap );
			if(mapped && weightValue[0]>0.0f)
			{
				db->weightMapsSet.insert(map);
				cpol->pPoints[j].weightMapCount++;
				if(db->maxWeigthsPerPoint<cpol->pPoints[j].weightMapCount)
					db->maxWeigthsPerPoint=cpol->pPoints[j].weightMapCount;
			}
		}		
	}


	db->mapPolys.insert((int)id,cpol);

	db->tmpCurrIndex++;

	return 0;
}



bool Database::allocateStorage()
{
	numPoints= meshInfo->numPoints(  meshInfo );
	numPolys = meshInfo->numPolygons(meshInfo );		

	polys=(Polygon*) malloc(numPolys*sizeof(Polygon));
	if(!polys)
		return false;
	memset(polys,0,numPolys*sizeof(Polygon));
	points= (PointSimple*)malloc(numPoints*sizeof(PointSimple));
	if(!points)
		return false;

	memset(points,0,numPoints*sizeof(PointSimple));
	mapPoints.reserve(numPoints);
	mapPolys.reserve(numPolys);

	return true;
}

void Database::freeStorage()
{
	if(polys)
		free(polys);

	if(points)
	{
		for(int i=0;i<numPoints;i++)
		{
			PointSimple *pSimple=&points[i];
			if(pSimple && pSimple->polyId)
			{
				free(pSimple->polyId);
			}
		}
		free(points);
	}

}

void Database::calculateNormalsAndIndexing()
{
	
	for (int  j = 0; j < numPolys; j++ )			// for number of polygons
	{
		Polygon *cpol=&polys[j];

		if(!cpol || !cpol->surface)
			continue;

		float smoothAngle=cpol->surface->smoothAngle;
		if(smoothAngle==0)
			smoothAngle=1.56250f;

		for ( int n = 0; n < cpol->numPnts; n++ )	// for number of vertices in a polygon
		{			
			QHash<int,PointSimple*>::const_iterator pointIter = mapPoints.find((int)cpol->pPoints[n].id);	
			
			if(pointIter!=mapPoints.end())
			{								
				PointSimple *cpoint=pointIter.value();

				if(cpol->pPoints[n].index==-1 )	
				{
					cpol->pPoints[n].index=indexCount++;
					if(cpol->pPoints[n].colorMapCount==0)
						numPointsWithoutColor++;
					int blaSize=uvMapsSet.size();
					int blaUvCount=cpol->pPoints[n].uvMapCount;
					if(cpol->pPoints[n].uvMapCount==0)
						numPointsWithoutUv++;
					if(cpol->pPoints[n].weightMapCount==0)
						numPointsWithoutWeight++;
				}

				for ( int g = 0; g < cpoint->numPolys; g++ )			// for all other polygons that share this point
				{

					Polygon *other=0;
					if(cpoint->polyId[g])
					{
						QHash<int,Polygon*>::const_iterator polytIter = mapPolys.find((int)cpoint->polyId[g]);	
						if(polytIter!=mapPolys.end())
							other=polytIter.value();
					}
					if(other)
					{
						float angle=acos(cpol->normal[ 0 ] * other->normal[ 0 ] + cpol->normal[ 1 ] * other->normal[ 1 ] + cpol->normal[ 2 ] * other->normal[ 2 ]);			

						if ( angle >  smoothAngle )				// if angle is greater than surface skip ( don't accumulate normals
						{
							continue;
						}
						else
						{

							if((!hasDiscUv(cpoint->id,other->id)) && (!hasDiscUv(cpoint->id,cpol->id)))
							{
								for(int x=0;x<other->numPnts;x++)
								{
									if( other->pPoints[x].id==cpol->pPoints[n].id && other->pPoints[x].index==-1 )
									{
										other->pPoints[x].index=cpol->pPoints[n].index;
									}
								}
							}


						}
						if(cpoint->polyId[g]!=cpol->id )
						{
							cpol->pPoints[n].normal[0]+=other->normal[0];
							cpol->pPoints[n].normal[1]+=other->normal[1];
							cpol->pPoints[n].normal[2]+=other->normal[2];
						}
					}
				}

				float r;

				r = ( float ) sqrt( cpol->pPoints[n].normal[0] * cpol->pPoints[n].normal[0] + cpol->pPoints[n].normal[1] * cpol->pPoints[n].normal[1] +
					cpol->pPoints[n].normal[2] * cpol->pPoints[n].normal[2]);
				if ( r > 0 ) 
				{
					cpol->pPoints[n].normal[0] /= r;
					cpol->pPoints[n].normal[ 1 ] /= r;
					cpol->pPoints[n].normal[ 2 ] /= r;
				}

			}
		}
	}
}

bool Database::hasDiscUv(LWPntID point, LWPolID polygon)
{
	float uvValue[2];

	

	QSetIterator<UvMap *> iter(uvMapsSet);
	while (iter.hasNext())
	{
		UvMap *map=iter.next();
		void * currentMap =meshInfo->pntVLookup( meshInfo, map->typeId, map->cName );
		int mapped=meshInfo->pntVPIDGet( meshInfo, point, polygon, uvValue, currentMap );						
		if(mapped!=0)
		{
			return true;
		}
	}
	return false;

}

bool SceneInventory::checkIfEqualVector(const MiniEngine::Vector3 &first,const MiniEngine::Vector3 &second,float tolerance)
{
	using namespace MiniEngine;

	if(!MathUtils::Equal(first.x,second.x,tolerance))
		return false;
	if(!MathUtils::Equal(first.y,second.y,tolerance))
		return false;
	if(!MathUtils::Equal(first.z,second.z,tolerance))
		return false;

	return true;
}
bool SceneInventory::checkIfEqualQuat(const MiniEngine::Quat &first,const MiniEngine::Quat &second,float tolerance)
{
	using namespace MiniEngine;

	if(!MathUtils::Equal(first.x,second.x,tolerance))
		return false;
	if(!MathUtils::Equal(first.y,second.y,tolerance))
		return false;
	if(!MathUtils::Equal(first.z,second.z,tolerance))
		return false;
	if(!MathUtils::Equal(first.w,second.w,tolerance))
		return false;

	return true;

}


void SceneInventory::checkVector3(MiniEngine::Vector3 &out,float value,float tolerance)
{
	using namespace MiniEngine;

	if(MathUtils::Equal(out.x,value))
		out.x=value;	
	if(MathUtils::Equal(out.y,value))
		out.y=value;
	if(MathUtils::Equal(out.z,value))
		out.z=value;
}
void SceneInventory::checkQuat(MiniEngine::Quat &out)
{
	using namespace MiniEngine;
	if(MathUtils::Equal(out.x,0))
		out.x=0;	
	if(MathUtils::Equal(out.y,0))
		out.y=0;
	if(MathUtils::Equal(out.z,0))
		out.z=0;
	if(MathUtils::Equal(out.w,1.0f))
		out.w=1.0f;

}

void SceneInventory::updateBoundingBox()
{
	using namespace MiniEngine;

	LWFVector	position;
	Vector3 tmp;

	mSceneMinBox.x=FLT_MAX;
	mSceneMinBox.y=FLT_MAX;
	mSceneMinBox.z=FLT_MAX;

	mSceneMaxBox.x=-FLT_MAX;
	mSceneMaxBox.y=-FLT_MAX;
	mSceneMaxBox.z=-FLT_MAX;


	for(int i=0;i<meshObjects.size();i++)
	{
		if(meshObjects[i]->options.exportItem)
		{
			Database *db=meshObjects[i]->database;

			for(int j=0;j<db->numPolysScaned;j++)
			{
				Polygon  *poly=&db->polys[j];
				for(int z=0;z<poly->numPnts;z++)
				{
					if(poly->pPoints[z].index>=0)
					{
						db->meshInfo->pntBasePos(db->meshInfo,poly->pPoints[z].id,position);
						Vector3 tmp(position[0],position[1],-position[2]);
						Vector3::createMin(mSceneMinBox, tmp,mSceneMinBox);
						Vector3::createMax(mSceneMaxBox, tmp,mSceneMaxBox);
					}
				}
			}

		}
	}


}

void SceneInventory::sortGeometry(SPODMesh *mesh)
{
	if(globalOptions.triangleSort==SceneItemOptions::PVRT_LIST)
	{
		PVRTGeometrySort(mesh->pInterleaved, (unsigned short*)mesh->sFaces.pData, mesh->sVertex.nStride, mesh->nNumVertex,
							mesh->nNumFaces,mesh->nNumVertex,mesh->nNumFaces,PVRTGEOMETRY_SORT_VERTEXCACHE);
	}
	else
	{
		PVRTTriStripList((unsigned short*)mesh->sFaces.pData,mesh->nNumFaces);

	}
}
