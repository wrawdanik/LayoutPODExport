 #include <QtGui>
#include <QFileInfo>
#include <QComboBox>
#include "MainDialog.h"
#include "Database.h"
#include "LayoutPODExportMain.h"
#include <QMessageBox>
#include "GlobalOptionsDialog.h"
#include <QTextBrowser>
#include "AboutDialog.h"
#include "QSettings.h"
#include <lwmotion.h>


using namespace LayoutPODExport;

GenericListWidget::GenericListWidget(QWidget *parent):QTreeView(parent)
{
	setRootIsDecorated (false);
	setUniformRowHeights (true) ;
	setSortingEnabled(true);

	sModel = new QStandardItemModel(this);

	setFrameStyle(QFrame::Box|QFrame::Plain);
	setModel(sModel);

}

void GenericListWidget::setLabels(const QStringList &labels)
{
	this->iLabels=labels;
	sModel->setColumnCount(labels.size());
}

void GenericListWidget::calculateItemSizeHint(QStandardItem *item)
{
	QFontMetrics metrics(item->font());

	QRect sRect=metrics.boundingRect(item->text());
	sRect.setBottom(sRect.bottom()+4);
	sRect.setRight(sRect.right()+16);
	item->setSizeHint(sRect.size());
}


void GenericListWidget::updateLabels()
{
	for(int i=0;i<iLabels.size();i++)
	{
		QStandardItem *item=new QStandardItem(iLabels.at(i));
		sModel->setHorizontalHeaderItem (i, item );
	}
}




// ---------------- Geometry Stats List

GeometryStatsList::GeometryStatsList(QWidget *parent):GenericListWidget(parent)
{
	QStringList labels;	

	setRootIsDecorated (true);
	setSortingEnabled(false);
	labels << "Object";
	labels << "Properties";
	iSize.setWidth(720);
	iSize.setHeight(720);
	setLabels(labels);
	setAllColumnsShowFocus(false);
	setSelectionMode (QAbstractItemView::NoSelection); 
	//setSelectionBehavior(QAbstractItemView::SelectRows);
	//setDragDropOverwriteMode(false);
} 

void GeometryStatsList::updateStatsDisplay(SceneInventory *scene)
{
	checkForChanges=false;
	sModel->clear();
	updateLabels();
	
	QList<SceneItem*> &objects=scene->getMeshItems();

	if(objects.size()==0)
	{
		setRootIsDecorated (false);
		QStandardItem *itemInvalid=new QStandardItem("No valid objects found !!");		
		itemInvalid->setForeground(QBrush(QColor(255,0,0)));
		sModel->appendRow(itemInvalid);		
		resizeColumnToContents (0) ;
		return;		

	}

	setRootIsDecorated (true);

	QStandardItem *parentItem = sModel->invisibleRootItem();

	QList<QStandardItem*> items;

	for (int i = 0; i < objects.size(); ++i) 
	{		
		SceneItem *sceneItem=objects.at(i);
		createObjectEntry(sceneItem,parentItem);
	}

	setColumnWidth(0,200);

	checkForChanges=true;	
	
}

// A separate method to allow for easy color/icon swapping for top level items which can get selected/deselected at any time.
// It disables update handling for the duration of the method ( since it can be invoked from the update handler ) to prevent infinite recursion.

void GeometryStatsList::updateTopLevelEntry(SceneItem *sceneItem, QStandardItem *item)
{
	bool oldcheckForChanges=checkForChanges;
	checkForChanges=false;
	if(!sceneItem->options.exportItem)
	{
		item->setForeground(QBrush(QColor(255,0,0)));
		item->setCheckState(Qt::Unchecked);
		item->setBackground(QBrush(QColor(255,240,240)));
		item->setIcon(QIcon(":/icons/object_off.png"));
	}
	else
	{
		item->setForeground(QBrush(QColor(0,0,255)));
		item->setCheckState(Qt::Checked);
		item->setIcon(QIcon(":/icons/object.png"));
	}
	checkForChanges=oldcheckForChanges;
}

void GeometryStatsList::createObjectEntry(SceneItem *sceneItem,QStandardItem *parent)
{
	QList<QStandardItem*> items;
	QStandardItem *itemObjectName=new QStandardItem("Object: "+sceneItem->name);	
	itemObjectName->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);

	// nothing to export - disable completely
	if(sceneItem->database->indexCount==0)
	{
		itemObjectName->setFlags(0);
	}
	updateTopLevelEntry(sceneItem,itemObjectName);

	

	QVariant userItemData(QVariant::UserType);
	userItemData.setValue((void*)sceneItem);

	itemObjectName->setData(QVariant((int)SCENE_ITEM_EXPORT), Qt::UserRole + 1);
	itemObjectName->setData(userItemData, Qt::UserRole + 2);


	QStandardItem *itemObjectFilePath=new QStandardItem(sceneItem->filePath);

	items << itemObjectName << itemObjectFilePath ;
	parent->appendRow(items);
	setExpanded(itemObjectName->index(),true);
	createVertexEntry(sceneItem,itemObjectName);
	createFacesEntry(sceneItem,itemObjectName);
	createBonesEntry(sceneItem,itemObjectName);
	if(sceneItem->database->hasErrors)
		createErrorsEntry(sceneItem,itemObjectName);
	
}
/*
void GeometryStatsList::createOptionsEntry(SceneItem *sceneItem,QStandardItem *parent)
{
	QList<QStandardItem*> items;

	QStandardItem *itemEmpty=0;
	QStandardItem *itemType=0;
	QStandardItem *itemData=0;

	QVariant userItemData(QVariant::UserType);
	userItemData.setValue((void*)sceneItem);

	QStandardItem *itemInfo=new QStandardItem("Options");
	itemInfo->setForeground(QBrush(QColor(0,128,0)));
	itemInfo->setFlags(Qt::ItemIsEnabled);	
	itemEmpty=new QStandardItem;
	itemEmpty->setFlags(0);	

	items << itemInfo << itemEmpty;
	parent->appendRow(items);

	items.clear();
	itemType=new QStandardItem("Format");	
	itemType->setFlags(Qt::ItemIsEnabled);
	
	QComboBox *itemCombo=new QComboBox();
	itemCombo->addItem ("Float",userItemData);
	itemCombo->addItem ("Short",userItemData);
	itemCombo->setMaximumWidth(160);
	vertexFormatWidgets.push_back(itemCombo);
	itemCombo->setEnabled(false);
	
	itemEmpty=new QStandardItem;
	itemEmpty->setFlags(0);	
	if(sceneItem->options.vertexFormat==SceneItemOptions::FORMAT_FLOAT)
		itemCombo->setCurrentIndex (0);
	else
		itemCombo->setCurrentIndex (1);

	connect(itemCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboDataChanged(int)));

	items << itemType << itemEmpty ;
	itemInfo->appendRow(items);	
	setIndexWidget (itemEmpty->index(), itemCombo );

	items.clear();
	itemType=new QStandardItem("Geometry");	
	itemType->setFlags(Qt::ItemIsEnabled);	

	itemCombo=new QComboBox();
	itemCombo->addItem ("Indexed List",userItemData);
	itemCombo->addItem ("Strip",userItemData);
	itemCombo->setMaximumWidth(160);
	geometryOptionsWidgets.push_back(itemCombo);
	itemCombo->setEnabled(false);

	if(sceneItem->options.geometryOptions==SceneItemOptions::INDEXED_LIST)
		itemCombo->setCurrentIndex (0);
	else
		itemCombo->setCurrentIndex (1);	

	connect(itemCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboDataChanged(int)));

	itemEmpty=new QStandardItem;
	itemEmpty->setFlags(0);	
	items << itemType << itemEmpty ;
	itemInfo->appendRow(items);	
	setIndexWidget (itemEmpty->index(), itemCombo );

	items.clear();
	itemType=new QStandardItem("Geometry Scale:");	
	itemType->setFlags(Qt::ItemIsEnabled);
	itemData=new QStandardItem(QString::number((double)sceneItem->options.scaleGeometryValue));
	itemData->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
	itemData->setData(QVariant((float)sceneItem->options.scaleGeometryValue),Qt::EditRole);
	itemData->setData(QVariant((int)FLOAT_MESH_SCALE), Qt::UserRole + 1);
	itemData->setData(userItemData, Qt::UserRole + 2);



	items << itemType << itemData ;
	itemInfo->appendRow(items);	

	items.clear();
	itemType=new QStandardItem("UV Scale:");	
	itemType->setFlags(Qt::ItemIsEnabled);
	itemData=new QStandardItem(QString::number(sceneItem->options.scaleUvValue));
	itemData->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
	itemData->setData(QVariant((float)sceneItem->options.scaleUvValue),Qt::EditRole);
	itemData->setData(QVariant((int)FLOAT_UV_SCALE), Qt::UserRole + 1);
	itemData->setData(userItemData, Qt::UserRole + 2);

	items << itemType << itemData ;
	itemInfo->appendRow(items);	

	items.clear();
	itemType=new QStandardItem("Animations:");
	itemType->setFlags(Qt::ItemIsEnabled);
	itemData=new QStandardItem("0");
	itemData->setFlags(Qt::ItemIsEnabled);
	items << itemType << itemData ;
	itemInfo->appendRow(items);	

	
}

*/
void GeometryStatsList::createVertexEntry(SceneItem *sceneItem,QStandardItem *parent)
{
	QList<QStandardItem*> items;

	QStandardItem *itemVertices=new QStandardItem("Vertices");	
	QStandardItem *itemVerticesCount=new QStandardItem(QString::number(sceneItem->database->indexCount));	
	items << itemVertices << itemVerticesCount ;
	parent->appendRow(items);

	if(sceneItem->database->indexCount==0)
		return;

	QStandardItem *itemEmpty=0;
	QStandardItem *itemType=0;

	QVariant userItemData(QVariant::UserType);
	userItemData.setValue((void*)sceneItem);

	// position
	items.clear();
	itemEmpty=new QStandardItem;
	itemEmpty->setFlags(Qt::ItemIsEnabled);
	itemType=new QStandardItem("Position");			
	itemType->setFlags(Qt::ItemIsEnabled);
	items << itemEmpty  << itemType;		;
	itemVertices->appendRow(items);	
	setExpanded(itemVertices->index(),true);

	// normal
	items.clear();
	itemEmpty=new QStandardItem;
	itemEmpty->setFlags(Qt::ItemIsEnabled);
	itemType=new QStandardItem("Normal");	
	itemType->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);
	itemType->setCheckable(true);	
	if(sceneItem->options.exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_NORMAL])
		itemType->setCheckState(Qt::Checked);
	else
		itemType->setCheckState(Qt::Unchecked);
	items << itemEmpty << itemType;
	itemVertices->appendRow(items);	
	itemType->setData(QVariant((int)VERTEX_DATA_TYPE), Qt::UserRole + 1);
	itemType->setData(userItemData, Qt::UserRole + 2);
	itemType->setData(QVariant((int)SceneItemOptions::EXPORT_VERTEX_NORMAL), Qt::UserRole + 3);

	// uv maps
	items.clear();
	int count=0;
	QSetIterator<UvMap *> iterUv(sceneItem->database->uvMapsSet);
	while (iterUv.hasNext())
	{
		UvMap *map=iterUv.next();

		itemType=new QStandardItem("UV   ("+map->name+")");
		itemType->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);
		if(sceneItem->options.exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_UV0+count])
			itemType->setCheckState(Qt::Checked);
		else
			itemType->setCheckState(Qt::Unchecked);
		itemType->setData(QVariant((int)VERTEX_DATA_TYPE), Qt::UserRole + 1);		
		itemType->setData(userItemData, Qt::UserRole + 2);
		itemType->setData(QVariant((int)SceneItemOptions::EXPORT_VERTEX_UV0+count), Qt::UserRole + 3);


		itemEmpty=new QStandardItem;
		itemEmpty->setFlags(Qt::ItemIsEnabled);
		items << itemEmpty << itemType;	
		itemVertices->appendRow(items);	
		items.clear();		
		++count;
	}		

	items.clear();

	QSetIterator<ColorMap *> iterColor(sceneItem->database->colorMapsSet);
	while (iterColor.hasNext())
	{
		ColorMap *map=iterColor.next();
		itemEmpty=new QStandardItem;
		itemEmpty->setFlags(Qt::ItemIsEnabled);
		itemType=new QStandardItem("Color   ("+map->name+")");		
		itemType->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);
		if(sceneItem->options.exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_COLOR_DIFFUSE])
			itemType->setCheckState(Qt::Checked);
		else
			itemType->setCheckState(Qt::Unchecked);
		items << itemEmpty << itemType;				
		itemVertices->appendRow(items);	

		itemType->setData(QVariant((int)VERTEX_DATA_TYPE), Qt::UserRole + 1);		
		itemType->setData(userItemData, Qt::UserRole + 2);
		itemType->setData(QVariant((int)SceneItemOptions::EXPORT_VERTEX_COLOR_DIFFUSE), Qt::UserRole + 3);
	}

	if(sceneItem->database->weightMapsSet.size()>0)
	{
		items.clear();


		itemEmpty=new QStandardItem;
		itemEmpty->setFlags(Qt::ItemIsEnabled);
		itemType=new QStandardItem("Weights  ("+QString::number(sceneItem->database->weightMapsSet.size())+"/"+QString::number(sceneItem->database->maxWeigthsPerPoint)+")");		
		itemType->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);
		if(sceneItem->options.exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_WEIGHTS])
			itemType->setCheckState(Qt::Checked);
		else
			itemType->setCheckState(Qt::Unchecked);
		items << itemEmpty << itemType;				
		itemVertices->appendRow(items);	

		itemType->setData(QVariant((int)VERTEX_DATA_TYPE), Qt::UserRole + 1);		
		itemType->setData(userItemData, Qt::UserRole + 2);
		itemType->setData(QVariant((int)SceneItemOptions::EXPORT_VERTEX_WEIGHTS), Qt::UserRole + 3);
	}

		items.clear();

	
		itemEmpty=new QStandardItem;
		itemEmpty->setFlags(Qt::ItemIsEnabled);
		itemType=new QStandardItem("Tangent");		
		itemType->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);
		if(sceneItem->options.exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_GENERATE_TANGENT])
			itemType->setCheckState(Qt::Checked);
		else
			itemType->setCheckState(Qt::Unchecked);
		items << itemEmpty << itemType;				
		itemVertices->appendRow(items);	

		itemType->setData(QVariant((int)VERTEX_DATA_TYPE), Qt::UserRole + 1);		
		itemType->setData(userItemData, Qt::UserRole + 2);
		itemType->setData(QVariant((int)SceneItemOptions::EXPORT_VERTEX_GENERATE_TANGENT), Qt::UserRole + 3);
}

void GeometryStatsList::createFacesEntry(SceneItem *sceneItem,QStandardItem *parent)
{
	QList<QStandardItem*> items;

	QStandardItem *itemFaces=new QStandardItem("Faces");	
	QStandardItem *itemFacesCount=new QStandardItem(QString::number(sceneItem->database->numPolysScaned));	
	items << itemFaces << itemFacesCount ;
	parent->appendRow(items);
}
void GeometryStatsList::createBonesEntry(SceneItem *sceneItem,QStandardItem *parent)
{
	if(sceneItem->database->weightMapsSet.size()==0)
		return;

	QList<QStandardItem*> items;
	QStandardItem *itemBones=new QStandardItem("Bones");	
	QStandardItem *itemBonesCount=new QStandardItem(QString::number(sceneItem->database->weightMapsSet.size()));	
	items << itemBones << itemBonesCount ;
	parent->appendRow(items);


	QSetIterator<WeightMap *> i(sceneItem->database->weightMapsSet);
	while (i.hasNext())
	{
		WeightMap *wMap=i.next();
		items.clear();
		QStandardItem *itemEmpty=new QStandardItem;
		itemEmpty->setFlags(Qt::ItemIsEnabled);
		QStandardItem *boneWeightName=new QStandardItem(wMap->name);		
		items << itemEmpty << boneWeightName;
		itemBones->appendRow(items);
	}

}

void GeometryStatsList::createErrorsEntry(SceneItem *sceneItem,QStandardItem *parent)
{
	QStandardItem *itemErrors=new QStandardItem("Errors");	
	itemErrors->setForeground(QBrush(QColor(192,0,0)));
	parent->appendRow(itemErrors);

	QList<QStandardItem*> items;

	if(sceneItem->database->indexCount==0)
	{
		QStandardItem *errorUv=new QStandardItem("No valid geometry found");
		errorUv->setForeground(QBrush(QColor(128,0,0)));
		QStandardItem *itemEmpty=new QStandardItem;
		itemEmpty->setFlags(0);	
		items << itemEmpty << errorUv ;
		itemErrors->appendRow(items);

	}

	if(sceneItem->database->uvMaps.size()>0)
	{
		if(sceneItem->database->numPointsWithoutUv>0)
		{
			QStandardItem *errorUv=new QStandardItem("Missing UV mappings for "+QString::number(sceneItem->database->numPointsWithoutUv)+" points");
			errorUv->setForeground(QBrush(QColor(128,0,0)));
			QStandardItem *itemEmpty=new QStandardItem;
			itemEmpty->setFlags(0);	
			items << itemEmpty << errorUv ;
			itemErrors->appendRow(items);
		}
	}
	if(sceneItem->database->colorMaps.size()>0)
	{
		if(sceneItem->database->numPointsWithoutColor>0)
		{
			QStandardItem *errorUv=new QStandardItem("Missing Color mappings for "+QString::number(sceneItem->database->numPointsWithoutColor)+" points");
			errorUv->setForeground(QBrush(QColor(128,0,0)));
			QStandardItem *itemEmpty=new QStandardItem;
			itemEmpty->setFlags(0);	
			items << itemEmpty << errorUv ;
			itemErrors->appendRow(items);
		}
	}
	if(sceneItem->database->weightMaps.size()>0)
	{
		if(sceneItem->database->numPointsWithoutWeight>0)
		{
			QStandardItem *errorUv=new QStandardItem("Missing Weight mappings for "+QString::number(sceneItem->database->numPointsWithoutWeight)+" points");
			errorUv->setForeground(QBrush(QColor(128,0,0)));
			QStandardItem *itemEmpty=new QStandardItem;
			itemEmpty->setFlags(0);	
			items << itemEmpty << errorUv ;
			itemErrors->appendRow(items);
		}
	}
	if(sceneItem->database->countInvalid[Database::DEGENERATED]>0)
	{
		QStandardItem *errorBad=new QStandardItem("Degenerated polygons : "+QString::number(sceneItem->database->countInvalid[Database::DEGENERATED]));
		errorBad->setForeground(QBrush(QColor(128,0,0)));
		QStandardItem *itemEmpty=new QStandardItem;
		itemEmpty->setFlags(0);	
		items.clear();
		items << itemEmpty << errorBad ;
		itemErrors->appendRow(items);
	}
	if(sceneItem->database->countInvalid[Database::NON_TRIANGLE]>0)
	{
		QStandardItem *errorBad=new QStandardItem("Non-triangle polygons : "+QString::number(sceneItem->database->countInvalid[Database::NON_TRIANGLE]));
		errorBad->setForeground(QBrush(QColor(128,0,0)));
		QStandardItem *itemEmpty=new QStandardItem;
		itemEmpty->setFlags(0);	
		items.clear();
		items << itemEmpty << errorBad ;
		itemErrors->appendRow(items);
	}
	if(sceneItem->database->countInvalid[Database::OTHER]>0)
	{
		QStandardItem *errorBad=new QStandardItem("Other invalid polygons : "+QString::number(sceneItem->database->countInvalid[Database::OTHER]));
		errorBad->setForeground(QBrush(QColor(128,0,0)));
		QStandardItem *itemEmpty=new QStandardItem;
		itemEmpty->setFlags(0);	
		items.clear();
		items << itemEmpty << errorBad ;
		itemErrors->appendRow(items);
	}
	if(itemErrors)
	{
		parent->appendRow(itemErrors);
		setExpanded(itemErrors->index(),true);
	}

}

QStandardItem* GeometryStatsList::createParentErrorEntry(SceneItem *sceneItem,QStandardItem *parent)
{

	QStandardItem *itemErrors=new QStandardItem("Errors");	
	itemErrors->setForeground(QBrush(QColor(192,0,0)));
	parent->appendRow(itemErrors);
	return itemErrors;
}


void GeometryStatsList::comboDataChanged(int index)
{
	for(int i=0;i<geometryOptionsWidgets.size();i++)
	{
		QComboBox *combo=geometryOptionsWidgets[i];

		QVariant userItemData=combo->itemData(combo->currentIndex());
		if(!userItemData.isValid())
			return;
		SceneItem *sceneItem=static_cast <SceneItem*> (userItemData.value<void*>());
		sceneItem->options.geometryOptions=(SceneItemOptions::GEOMETRY_OPTIONS)combo->currentIndex();
	}
	for(int i=0;i<vertexFormatWidgets.size();i++)
	{
		QComboBox *combo=vertexFormatWidgets[i];

		QVariant userItemData=combo->itemData(combo->currentIndex());
		if(!userItemData.isValid())
			return;
		SceneItem *sceneItem=static_cast <SceneItem*> (userItemData.value<void*>());
		sceneItem->options.vertexFormat=(SceneItemOptions::VERTEX_FORMAT)combo->currentIndex();
	}
}

void GeometryStatsList::commitData()
{

}

void GeometryStatsList::dataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight )
{
	if(checkForChanges)
	{
		QAbstractItemView::dataChanged(topLeft,bottomRight);
		if(checkForChanges && topLeft.isValid())
		{
			QStandardItem *item=sModel->itemFromIndex(topLeft);//,topLeft.column());
			if(item)
			{
				commitDataIForItem(item);
			}
		}
	}
}



void GeometryStatsList::commitDataIForItem(QStandardItem *item)
{
	QVariant uItemTypeData=item->data(Qt::UserRole + 1);

	if(uItemTypeData.isValid())
	{				
		QVariant userItemData=item->data(Qt::UserRole + 2);	
		if(!userItemData.isValid())
			return;
		SceneItem *sceneItem=static_cast <SceneItem*> (userItemData.value<void*>());
		if(!sceneItem)
			return;
		int type=uItemTypeData.value<int>();
		switch(type)
		{
		case	SCENE_ITEM_EXPORT:		
			{						
				if(item->flags()&Qt::ItemIsUserCheckable)
				{
					sceneItem->options.exportItem=item->checkState()==Qt::Checked ? true:false;
				}
				else
					sceneItem->options.exportItem=true;
				sceneItem->options.exportItemUserOverride=true;
				updateTopLevelEntry(sceneItem,item);
				emit totalMeshCountChanged();
				return;
			}
		case	VERTEX_DATA_TYPE:
			{
				QVariant subMeshVertexType=item->data(Qt::UserRole + 3);					
				if(subMeshVertexType.isValid())
				{
					int vertexType=subMeshVertexType.value<int>();
					sceneItem->options.exportVertexOptions[vertexType]=item->checkState()==Qt::Checked ? true:false;
				}
				return;

			}
		case	FLOAT_MESH_SCALE:
			{
				QString label=item->text();
				bool wasOK;
				float val=label.toFloat(&wasOK);
				if(!wasOK)
					val=sceneItem->options.scaleGeometryValue;
				sceneItem->options.scaleGeometryValue=fabs(val);	
				item->setData(QVariant((float)sceneItem->options.scaleGeometryValue),Qt::EditRole);
				return;

			}
		case	FLOAT_UV_SCALE:
			{
				QString label=item->text();
				bool wasOK;
				float val=label.toFloat(&wasOK);
				if(!wasOK)
					val=sceneItem->options.scaleUvValue;
				sceneItem->options.scaleUvValue=fabs(val);	
				item->setData(QVariant((float)sceneItem->options.scaleUvValue),Qt::EditRole);
				return;

			}
		}
	}
}


QSize GeometryStatsList::sizeHint () const
{
	return iSize;
}



// Geometry List
// Object Name
//       Info
//				   format - drop down ( short, float)
//				   geometry - drop down ( indedex list, indexed strip)
//				   scale   - mesh (edit) , uv (edit)
//				   animation - 0 animations defined ( or lists number of animations)  - button "define animation range"				   
//						 name - start 0   end
//       Vertices  - vertex count
//				   - position
//				   - uv ( uv map name)
//				   - color ( color map drop down to select)
//		 Faces     - faces count
//		 Bones     - bones count
//		 Invalid Geometry - count


MainDialog::MainDialog(LayoutPODExportMain *mainData):QDialog(0)
{
	

	lwMain=mainData;
	setModal(true);
	
	QString podVersion;

	podVersion.append(PVRTMODELPOD_VERSION);

#ifdef _DEBUG
	setWindowTitle("LayoutPODExport 1.2 (debug)    ( PVRTModelPOD version "+podVersion+" )");
#else
	setWindowTitle("LayoutPODExport 1.2    ( PVRTModelPOD version "+podVersion+" )");
#endif

	
	

//	Main layout for the Scene Inventory section	


	QVBoxLayout *mainLayout=new QVBoxLayout(this); 

	infoLabel=new QLabel("List of valid objects");
	globalOptionsButton=new QPushButton("Global Options ...");
	helpButton=new QPushButton("About ...");

	QGridLayout *gLayTop=new QGridLayout;		
	gLayTop->addWidget(infoLabel,0,0,Qt::AlignLeft);
	gLayTop->addWidget(globalOptionsButton,0,1,Qt::AlignRight);
	gLayTop->addWidget(helpButton,0,2,Qt::AlignRight);
	gLayTop->setColumnStretch(0,10);
	gLayTop->setColumnStretch(1,2);
	gLayTop->setColumnStretch(2,2);
	gLayTop->setMargin(4);


	mainLayout->addSpacing(8);
	mainLayout->addLayout(gLayTop);
	//mainLayout->addWidget(new QLabel("Valid Objects:"),0,Qt::AlignLeft);
	mainLayout->addSpacing(4);

	geometryList=new GeometryStatsList;
	geometryList->updateLabels();
	mainLayout->addWidget(geometryList,10);
	geometryList->setWhatsThis ("View and/or enable/disable meshes,individual vertex channels and other mesh related info.");


	closeButton=new QPushButton("Close");
	exportButton=new QPushButton("Export POD");
	
	statusLabel=new QLabel("Ready ...");

	QGridLayout *gLay=new QGridLayout;		
	gLay->addWidget(statusLabel,0,0,Qt::AlignLeft);
	gLay->addWidget(exportButton,0,1,Qt::AlignRight);
	gLay->addWidget(closeButton,0,2,Qt::AlignRight);
	gLay->setColumnStretch(0,10);
	gLay->setColumnStretch(1,0);
	gLay->setColumnStretch(2,0);
	gLay->setMargin(4);

	mainLayout->addLayout(gLay);
	mainLayout->setMargin(4);

	scene=new SceneInventory(mainData);	
	initSettings();
	scene->reloadScene();
	

	geometryList->updateStatsDisplay(scene);

	connect(globalOptionsButton,SIGNAL(clicked()),this,SLOT(displayGlobalOptions()));
	connect(helpButton,SIGNAL(clicked()),this,SLOT(displayAbout()));
	connect(exportButton,SIGNAL(clicked( )),this, SLOT(writeFile()));
	connect(closeButton,SIGNAL(clicked()), this , SLOT (accept()));
	connect(geometryList, SIGNAL(totalMeshCountChanged()), this, SLOT(handleTotalMeshCountChanged()));


	handleTotalMeshCountChanged();

	// store LW global

	int *p;
	LWGlobalPool *memfunc;
	LWMemChunk mem;

	memfunc = (LWGlobalPool *)lwMain->globalFunc( LWGLOBALPOOL_RENDER_GLOBAL, GFUSE_TRANSIENT );
	if ( memfunc ) 
	{
		const char *chunkName="LayoutPODExportMotion";

		mem = memfunc->find( chunkName );
		if ( !mem )
			 mem = memfunc->create( chunkName, sizeof( int* ));
		if ( mem )
		{
			p = ( int * ) mem;
			*p=(int)scene;
		}
	}

	applyMotionToItems();

	this->raise();
	

}
MainDialog::~MainDialog()
{
	saveSettings();

	removeMotionFromItems();
	delete scene;
}



void MainDialog::accept()
{
	QDialog::accept();
}

void MainDialog::reject()
{
	QDialog::reject();
}

void MainDialog::displayGlobalOptions()
{
	GlobalOptionsDialog *globalOptionsDialog=new GlobalOptionsDialog(this,scene,lwMain);
	globalOptionsDialog->setMinimumHeight (600 );
	if(globalOptionsDialog->exec()==QDialog::Accepted)
	{
		geometryList->updateStatsDisplay(scene);
		handleTotalMeshCountChanged();

	}
}

void MainDialog::displayAbout()
{
	AboutDialog *textBrowser=new AboutDialog(this);
	QRect parentFrame=frameGeometry ();
	textBrowser->setGeometry(parentFrame.x()+360,parentFrame.y()+160,480,620);
	textBrowser->show();
}

void MainDialog::writeFile()
{

	QString finalFile=scene->getSceneFileName();
	if(finalFile.contains(".lws",Qt::CaseInsensitive))
	{
		finalFile.replace(".lws",".pod");
	}
	else
	{
		finalFile.append(".pod");
	}

	

	QString fileName = QFileDialog::getSaveFileName(this, "Save POD File",
	finalFile,"POD (*.pod)");

	if(fileName.isNull() || fileName.length()==0)
		return;

	playAnimation();
	scene->serializeToPODFile(fileName);

}

void MainDialog::handleTotalMeshCountChanged()
{	
	QList<SceneItem*> &items=scene->getMeshItems();

	int meshCount=0;

	for(int i=0;i<items.size();i++)
	{
		SceneItem *item=items.at(i);
		if(item->database->indexCount>0 && item->options.exportItem)
			meshCount++;
	}
	exportButton->setEnabled(meshCount>0);

}

void MainDialog::applyMotionToItems()
{
	QList<SceneItem*> &sceneItems=scene->getAllItems();

	// handle objects

	char cmd[256];

	for(int i=0;i<sceneItems.size();i++)
	{
		SceneItem *sceneItem=sceneItems[i];
		//if(sceneItem->lwType==LWI_OBJECT || sceneItem->lwType==LWI_BONE)
		{
			lwCommand(lwMain->layoutGeneric,"EditObjects",0);
			sprintf(cmd,"SelectItem %x",sceneItem->itemId);
			lwCommand(lwMain->layoutGeneric,cmd,0);
			sprintf(cmd,"ApplyServer %s LayoutPODExportMotion",(const char*)LWITEMMOTION_HCLASS);
			lwCommand(lwMain->layoutGeneric,cmd,0);					
		}
	}
}

void MainDialog::removeMotionFromItems()
{
	QList<SceneItem*> &sceneItems=scene->getAllItems();

	// handle objects

	char cmd[256];

	for(int i=0;i<sceneItems.size();i++)
	{
		SceneItem *sceneItem=sceneItems[i];
		//if(sceneItem->lwType==LWI_OBJECT || sceneItem->lwType==LWI_BONE)
		{
			lwCommand(lwMain->layoutGeneric,"EditObjects",0);
			sprintf(cmd,"SelectItem %x",sceneItem->itemId);
			lwCommand(lwMain->layoutGeneric,cmd,0);
			sprintf(cmd,"RemoveServer %s LayoutPODExportMotion",(const char*)LWITEMMOTION_HCLASS);
			lwCommand(lwMain->layoutGeneric,cmd,0);					
		}
	}
}


int MainDialog::lwCommand(LWLayoutGeneric *gen, const char *cmdname,const char *fmt, ...)
{
	char cmd[ 1024 ], arg[ 1024 ];

   if ( fmt ) 
   {
      va_list ap;
      va_start( ap, fmt );
      vsprintf( arg, fmt, ap );
      va_end( ap );
      sprintf( cmd, "%s %s", cmdname, arg );
      return gen->evaluate( gen->data, cmd );
   }
   else
   {
      return gen->evaluate( gen->data, cmdname );
   }
}

void MainDialog::playAnimation()
{
	int i;

	geometryList->setEnabled(false);
	closeButton->setEnabled(false);
	exportButton->setEnabled(false);
	globalOptionsButton->setEnabled(false);
	closeButton->repaint();
	exportButton->repaint();
	geometryList->repaint();
	globalOptionsButton->repaint();
	statusLabel->setText("");
	statusLabel->repaint();

	for(i=scene->getFirstFrame();i<scene->getLastFrame()+1;i++)
	{
		lwCommand(lwMain->layoutGeneric, "GoToFrame", "%d", i );
		lwCommand(lwMain->layoutGeneric, "RefreshNow", NULL );
	}

	geometryList->setEnabled(true);
	closeButton->setEnabled(true);
	exportButton->setEnabled(true);
	globalOptionsButton->setEnabled(true);
	statusLabel->setText("Ready ... (POD file successfully exported)");
	this->raise();

}

void MainDialog::initSettings()
{

	settings=new QSettings (QSettings::IniFormat,QSettings::UserScope,"warmi.net","LayoutPODExport",this);
	scene->getGlobalOptions().geometryOptions=(SceneItemOptions::GEOMETRY_OPTIONS)settings->value("geometryOptions",scene->getGlobalOptions().geometryOptions).toInt();
	scene->getGlobalOptions().vertexFormat=(SceneItemOptions::VERTEX_FORMAT)settings->value("vertexFormat",scene->getGlobalOptions().vertexFormat).toInt();
	scene->getGlobalOptions().exportAnimation=settings->value("exportAnimation",scene->getGlobalOptions().exportAnimation).toBool();
	scene->getGlobalOptions().exportSkeletalAnimation=settings->value("exportSkeletalAnimation",scene->getGlobalOptions().exportSkeletalAnimation).toBool();
	scene->getGlobalOptions().exportMatrix=settings->value("exportMatrix",scene->getGlobalOptions().exportMatrix).toBool();
	scene->getGlobalOptions().excludeLeafNodes=settings->value("excludeLeafNodes",scene->getGlobalOptions().excludeLeafNodes).toBool();
	scene->getGlobalOptions().excludeNoWeightBones=settings->value("excludeNoWeightBones",scene->getGlobalOptions().excludeNoWeightBones).toBool();
	scene->getGlobalOptions().collapseScene=(SceneItemOptions::COLLAPSE_SCENE)settings->value("collapseScene",scene->getGlobalOptions().collapseScene).toInt();
	scene->getGlobalOptions().maxBones=settings->value("maxBones",scene->getGlobalOptions().maxBones).toInt();
	scene->getGlobalOptions().exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_GENERATE_TANGENT]=settings->value("exportVertexOptions_Tangent",scene->getGlobalOptions().exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_GENERATE_TANGENT]).toBool();
	scene->getGlobalOptions().triangleSort=(SceneItemOptions::TRIANGLE_SORT)settings->value("triangleSort",scene->getGlobalOptions().triangleSort).toInt();
	scene->getGlobalOptions().includeRestFrame=settings->value("includeRestFrame",scene->getGlobalOptions().includeRestFrame).toBool();

}

void MainDialog::saveSettings()
{
	settings->setValue("geometryOptions",(int)scene->getGlobalOptions().geometryOptions);
	settings->setValue("vertexFormat",(int)scene->getGlobalOptions().vertexFormat);
	settings->setValue("exportAnimation",(int)scene->getGlobalOptions().exportAnimation);
	settings->setValue("exportSkeletalAnimation",(int)scene->getGlobalOptions().exportSkeletalAnimation);	
	settings->setValue("exportMatrix",(int)scene->getGlobalOptions().exportMatrix);
	settings->setValue("excludeLeafNodes",(int)scene->getGlobalOptions().excludeLeafNodes);
	settings->setValue("excludeNoWeightBones",(int)scene->getGlobalOptions().excludeNoWeightBones);
	settings->setValue("collapseScene",(int)scene->getGlobalOptions().collapseScene);
	settings->setValue("maxBones",scene->getGlobalOptions().maxBones);
	settings->setValue("exportVertexOptions_Tangent",(int)scene->getGlobalOptions().exportVertexOptions[SceneItemOptions::EXPORT_VERTEX_GENERATE_TANGENT]);
	settings->setValue("triangleSort",(int)scene->getGlobalOptions().triangleSort);
	settings->setValue("includeRestFrame",(int)scene->getGlobalOptions().includeRestFrame);
}