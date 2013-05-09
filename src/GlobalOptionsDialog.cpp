 #include <QtGui>
#include <QFileInfo>
#include <QComboBox>
#include "LayoutPODExportMain.h"
#include <QMessageBox>
#include "Database.h"
#include "GlobalOptionsDialog.h"
#include "QGroupBox.h"
#include <QListWidget>
#include "Database.h"
#include "SceneItemOptions.h"
#include <QTreeWidget>
#include "SceneHierarchyDialog.h"

using namespace LayoutPODExport;

GlobalOptionsDialog::GlobalOptionsDialog(QWidget *parent,SceneInventory *scene,LayoutPODExportMain *mainData):QDialog(parent)
{

	this->scene=scene;

	lwMain=mainData;
	this->globalOptions=&scene->getGlobalOptions();
	setModal(true);
	setWindowTitle("LayoutPODExport Global Options");

	intValidator=new QIntValidator(this);
	intValidator->setRange(0,1024);

	floatValidator=new QDoubleValidator(this);


	QVBoxLayout *mainLayout=new QVBoxLayout(this); 

	gBoxAnimation=new QGroupBox("Animation");
	gBoxAnimation->setCheckable(true);
	gBoxAnimation->setWhatsThis ("Enable/Disable all animation channels.");
	
	animationSkeletalEnable=new QCheckBox("Export Skeletal");
	animationSkeletalEnable->setEnabled(false);
	animationSkeletalEnable->setWhatsThis ("Include/exclude skeletal animations (bone batching data.)");

	animationSkeletalRestFrame=new QCheckBox("Add Rest Frame (0)");
	animationSkeletalRestFrame->setEnabled(false);
	animationSkeletalRestFrame->setWhatsThis ("Include rest frame as the first frame (required by POD format). If your animation already includes the rest pose as the first frame there is no need to have this option on.");

	animationFormat=new QComboBox();
	animationFormat->addItem ("Transformation");
	animationFormat->addItem ("Matrix");
	animationSkeletalRestFrame->setWhatsThis ("Transformation (vector3,vector3,quaternion) - 10 floats per frame. Matrix - 16 floats per frame.");

	intValidator=new QIntValidator();
	intValidator->setRange(0,1024);

	QGridLayout *gLayAnimation=new QGridLayout;		
	gLayAnimation->addWidget(animationSkeletalEnable,0,0,Qt::AlignLeft);
	gLayAnimation->addWidget(animationSkeletalRestFrame,0,1,Qt::AlignLeft);
	gLayAnimation->addWidget(new QLabel("Format:"),0,2,Qt::AlignRight);
	gLayAnimation->addWidget(animationFormat,0,3,Qt::AlignLeft);


	gLayAnimation->setMargin(16);
	gLayAnimation->setVerticalSpacing(8);
	gLayAnimation->setColumnStretch(0,0);
	gLayAnimation->setColumnStretch(1,0);
	gLayAnimation->setColumnStretch(2,0);
	gLayAnimation->setColumnStretch(3,0);

	gBoxAnimation->setLayout(gLayAnimation);
	mainLayout->addWidget(gBoxAnimation);


	QGroupBox *gBoxHierarchy=new QGroupBox("Nodes/Bones Hierarchy");

	QGridLayout *gLayHierarchy=new QGridLayout;	

	collapseNodes=new QComboBox();
	
	collapseNodes->addItem ("None");
	collapseNodes->addItem ("All");
	collapseNodes->addItem ("All but Topmost");
	collapseNodes->setWhatsThis ("Various ways to collapse helper/null nodes. Unless you are sure you will need helper nodes during runtime, it is more efficient to collapse them.");

	QPushButton *treeButton=new QPushButton("View Scene Hierarchy ...");

	excludeLeafNodes=new QCheckBox("Exclude Leaf Nodes");
	excludeLeafNodes->setWhatsThis ("Exclude leaf nodes/nulls.These types of nodes cannot influence the mesh itself and unless you need them ( for instance as an attachment point for some other object) it is more efficient to collapse them.");
	excludeNoWeightBones=new QCheckBox("Exclude Bones without Weight Map");
	excludeNoWeightBones->setWhatsThis ("Exclude/collapse bones with no valid weight map attached.Unless you know you need them ( for instance as an attachment point for some other object) , it is best to collapse them.");

	maxBones=new QLineEdit();
	maxBones->setMaximumWidth(128);
	maxBones->setValidator(intValidator);
	maxBones->setWhatsThis ("This value determines the maximum number of bone matrices that can affect a mesh and it should be based on the maximum number of matrices supported by the GPU. If the mesh contains more bone matrices than the max value , it will be split. Set to 0 to export all matrices (useful for CPU based skinning)");

	gLayHierarchy->addWidget(excludeLeafNodes,0,0,Qt::AlignLeft);
	gLayHierarchy->addWidget(new QLabel("Collapse Nodes:"),0,1,Qt::AlignRight);
	gLayHierarchy->addWidget(collapseNodes,0,2,Qt::AlignRight);
	gLayHierarchy->addWidget(excludeNoWeightBones,1,0,Qt::AlignLeft);
	gLayHierarchy->addWidget(new QLabel("Matrix Palette Size:"),1,1,Qt::AlignLeft);
	gLayHierarchy->addWidget(maxBones,1,2,Qt::AlignRight);
	gLayHierarchy->addWidget(treeButton,2,1,1,2,Qt::AlignRight);
	
	
	gBoxHierarchy->setLayout(gLayHierarchy);
	mainLayout->addWidget(gBoxHierarchy);

	QGroupBox *gBoxGlobalOptions=new QGroupBox("Default Global Settings");


	vertexFormat=new QComboBox();
	vertexFormat->addItem ("Float");
	vertexFormat->addItem ("Short");
	vertexFormat->setWhatsThis ("Currently disabled.");

	vertexFormat->setEnabled(false);

	geometryOptions=new QComboBox();
	geometryOptions->addItem ("Indexed List");
	geometryOptions->addItem ("Strip");
	geometryOptions->setWhatsThis ("Primitive type as either indexed lists or triangle strips. Certain models can render faster using triangle strips but for the majority of models indexed lists are the preferred option.");

	triangleSort=new QComboBox();
	triangleSort->addItem ("None");
	triangleSort->addItem ("PVRTGeometrySort");
	triangleSort->addItem ("PVRTTriStrip");
	triangleSort->setWhatsThis ("Triangle sorting method.PVRTGeometrySort sorts vertices based on their cacheability while PVRTTriStrip creates a strip ordered indexed list. Try both to see which one perfoms better on your hardware.");

	vertexScale=new QLineEdit();
	vertexScale->setMaximumWidth(128);		
	vertexScale->setValidator(floatValidator);
	vertexScale->setText(QString::number(globalOptions->scaleGeometryValue));
	vertexScale->setWhatsThis ("Scale model vertices by this value before exporting.");


	uvScale=new QLineEdit();
	uvScale->setMaximumWidth(128);
	uvScale->setValidator(floatValidator);
	uvScale->setText(QString::number(globalOptions->scaleUvValue));
	uvScale->setWhatsThis ("Scale model uv coordinates by this value before exporting.");

	

	QGridLayout *gLayGlobal=new QGridLayout;		
	gLayGlobal->addWidget(new QLabel("Geometry Scale:"),0,0,Qt::AlignLeft);
	gLayGlobal->addWidget(vertexScale,0,1,Qt::AlignLeft);
	gLayGlobal->addWidget(new QLabel("UV Scale:"),0,2,Qt::AlignRight);
	gLayGlobal->addWidget(uvScale,0,3,Qt::AlignLeft);

	gLayGlobal->addWidget(new QLabel("Format:"),1,0,Qt::AlignRight);
	gLayGlobal->addWidget(vertexFormat,1,1,Qt::AlignLeft);
	gLayGlobal->addWidget(new QLabel("Geometry:"),1,2,Qt::AlignRight);
	gLayGlobal->addWidget(geometryOptions,1,3,Qt::AlignLeft);
	gLayGlobal->addWidget(new QLabel("Triangle Sort:"),2,2,Qt::AlignRight);
	gLayGlobal->addWidget(triangleSort,2,3,Qt::AlignLeft);	
	gLayGlobal->setMargin(16);
	gLayGlobal->setVerticalSpacing(8);
	gLayGlobal->setColumnStretch(0,0);
	gLayGlobal->setColumnStretch(1,0);
	gLayGlobal->setColumnStretch(2,0);
	gLayGlobal->setColumnStretch(3,0);

	gBoxGlobalOptions->setLayout(gLayGlobal);
	mainLayout->addWidget(gBoxGlobalOptions);


	QGroupBox *gBoxGlobalVertexOptions=new QGroupBox("Default Global Vertex Channels");
	gBoxGlobalVertexOptions->setWhatsThis ("Globally enable/disable vertex channels for all meshes and enable generation of tangents/binormals ( requires a uv channel)");
	QVBoxLayout *gBoxGlobalVertexOptionsLayout=new QVBoxLayout(this); 

	QString channelNames[SceneItemOptions::VERTEX_OPTIONS_LAST] = {"Normal", "Color","UV1","UV2","UV3","UV4","UV5","UV6","UV7","UV8","Weights","Tangent"};

	vertexOptionsList=new QListWidget();
	gBoxGlobalVertexOptionsLayout->addWidget(vertexOptionsList);
	
	for(int i=0;i<SceneItemOptions::VERTEX_OPTIONS_LAST;i++)
	{
		QListWidgetItem *item=new QListWidgetItem(channelNames[i]);
		
		QList<SceneItem*> &meshItems=scene->getMeshItems();

		bool isPresent=false;
		bool isEnabled=false;


		if(i!=SceneItemOptions::EXPORT_VERTEX_GENERATE_TANGENT)
		{
			for(int j=0;j<meshItems.size();j++)
			{
				if(meshItems[j]->options.isVertexChannel[i])
					isPresent=true;
				if(meshItems[j]->options.exportVertexOptions[i])
					isEnabled=true;
			}
		}
		else
		{
			isPresent=true;
			isEnabled=true;
		}
		if(!globalOptions->exportVertexOptions[i])
		{
			isEnabled=false;
		}
		if(isPresent)
			item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
		else
			item->setFlags(Qt::ItemIsUserCheckable);
		if(isEnabled)
			item->setCheckState(Qt::Checked);
		else
			item->setCheckState(Qt::Unchecked);
		vertexOptionsList->addItem (item);
	}
	
	gBoxGlobalVertexOptions->setLayout(gBoxGlobalVertexOptionsLayout);
	mainLayout->addWidget(gBoxGlobalVertexOptions);


	okButton=new QPushButton("OK");
	cancelButton=new QPushButton("Cancel");

	QGridLayout *gLayBottom=new QGridLayout;		
	gLayBottom->addWidget(okButton,0,1,Qt::AlignRight);
	gLayBottom->addWidget(cancelButton,0,2,Qt::AlignRight);
	gLayBottom->setColumnStretch(0,10);
	gLayBottom->setColumnStretch(1,0);
	gLayBottom->setColumnStretch(2,0);
	gLayBottom->setMargin(4);

	mainLayout->addLayout(gLayBottom);
	this->raise();


	connect(okButton,SIGNAL(clicked( )),this, SLOT(accept()));
	connect(cancelButton,SIGNAL(clicked()), this , SLOT (reject()));
	connect(treeButton , SIGNAL(clicked()),this,SLOT(openSceneHierarchy()));
	connect(collapseNodes,SIGNAL(activated ( int )),this, SLOT(currentCollapseIndexChanged(int )));
	connect(gBoxAnimation,SIGNAL(toggled ( bool )),this, SLOT(animationSettingChanged()));
	connect(animationSkeletalEnable,SIGNAL(stateChanged  ( int )),this, SLOT(collapsedStatusChanged()));
	connect(excludeLeafNodes,SIGNAL(stateChanged  ( int )),this, SLOT(collapsedStatusChanged()));
	connect(geometryOptions,SIGNAL(activated ( int )),this, SLOT(geometryOptionsChanged()));



	verifyControls();
	collapsedStatusChanged();

}
GlobalOptionsDialog::~GlobalOptionsDialog()
{
}


void GlobalOptionsDialog::accept()
{
	QDialog::accept();
	applyToAllObjects();

	globalOptions->exportMatrix=(animationFormat->currentIndex()!=0);
	globalOptions->exportAnimation=gBoxAnimation->isChecked();	
	globalOptions->excludeLeafNodes=(excludeLeafNodes->checkState()==Qt::Checked);
	globalOptions->excludeNoWeightBones=(excludeNoWeightBones->checkState()==Qt::Checked);
	globalOptions->collapseScene=(SceneItemOptions::COLLAPSE_SCENE)collapseNodes->currentIndex();
	globalOptions->triangleSort=(SceneItemOptions::TRIANGLE_SORT)triangleSort->currentIndex();
	globalOptions->includeRestFrame=(animationSkeletalRestFrame->checkState()==Qt::Checked);
	globalOptions->exportSkeletalAnimation=(animationSkeletalEnable->checkState()==Qt::Checked);
	parseLineEditValues(globalOptions);

	scene->makeUserCollapsePermament();
	scene->verifyObjectsIntegrity();
	scene->verifyErrors();
}

void GlobalOptionsDialog::reject()
{
	QDialog::reject();

}


void GlobalOptionsDialog::verifyControls()
{
	QList<SceneItem*> &meshes	=scene->getMeshItems();
	
	for(int i=0;i<meshes.size();i++)
	{
		SceneItem *item=meshes[i];
		if(item->options.exportItem && item->options.hasBones)
		{
			animationSkeletalEnable->setEnabled(true);
			animationSkeletalEnable->setChecked(globalOptions->exportSkeletalAnimation);
			animationSkeletalRestFrame->setEnabled(true);
			break;
		}
	}
	if(globalOptions->includeRestFrame)
		animationSkeletalRestFrame->setChecked(true);
	
	vertexFormat->setCurrentIndex((int)globalOptions->vertexFormat);
	geometryOptions->setCurrentIndex((int)globalOptions->geometryOptions);

	gBoxAnimation->setChecked(globalOptions->exportAnimation);

	if(globalOptions->exportMatrix)
		animationFormat->setCurrentIndex(1);
	else
		animationFormat->setCurrentIndex(0);

	if(globalOptions->excludeLeafNodes)
		excludeLeafNodes->setCheckState(Qt::Checked);
	else
		excludeLeafNodes->setCheckState(Qt::Unchecked);

	if(globalOptions->excludeNoWeightBones)
		excludeNoWeightBones->setCheckState(Qt::Checked);
	else
		excludeNoWeightBones->setCheckState(Qt::Unchecked);

	collapseNodes->setCurrentIndex((int)globalOptions->collapseScene);
	maxBones->setText(QString::number(globalOptions->maxBones));
	triangleSort->setCurrentIndex((int)globalOptions->triangleSort);
	triangleSort->setEnabled(globalOptions->geometryOptions!=SceneItemOptions::STRIP);
		
}

void GlobalOptionsDialog::parseLineEditValues(SceneItemOptions *options)
{
	float val;
	bool result;
	val=vertexScale->text().toFloat(&result); 
	if(result)
		options->scaleGeometryValue=val;
	val=uvScale->text().toFloat(&result); 
	if(result)
		options->scaleUvValue=val;

	int valInt;
	valInt=maxBones->text().toInt(&result); 
	if(result)
		options->maxBones=valInt;
	else
		options->maxBones=0;



}

void GlobalOptionsDialog::applyToAllObjects()
{
	QList<SceneItem*> &meshes	=scene->getMeshItems();

	for(int i=0;i<SceneItemOptions::VERTEX_OPTIONS_LAST;i++)
	{
		QListWidgetItem *widgetItem=vertexOptionsList->item(i);
		globalOptions->exportVertexOptions[i]=(widgetItem->checkState()==Qt::Checked);
	}

	globalOptions->vertexFormat=(SceneItemOptions::VERTEX_FORMAT)vertexFormat->currentIndex();
	globalOptions->geometryOptions=(SceneItemOptions::GEOMETRY_OPTIONS)geometryOptions->currentIndex();

	for(int i=0;i<meshes.size();i++)
	{
		SceneItem *item=meshes[i];
		item->options.vertexFormat=(SceneItemOptions::VERTEX_FORMAT)vertexFormat->currentIndex();
		item->options.geometryOptions=(SceneItemOptions::GEOMETRY_OPTIONS)geometryOptions->currentIndex();

		parseLineEditValues(&item->options);

		for(int j=0;j<SceneItemOptions::VERTEX_OPTIONS_LAST;j++)
		{
			if(item->options.isVertexChannel[j])
			{
				QListWidgetItem *widgetItem=vertexOptionsList->item(j);
				item->options.exportVertexOptions[j]=(widgetItem->checkState()==Qt::Checked);
			}
		}
	}	

	

}


void GlobalOptionsDialog::openSceneHierarchy()
{
	SceneHierarchyDialog *dialog=new SceneHierarchyDialog(this,scene,lwMain,((animationSkeletalEnable->checkState()==Qt::Checked) && gBoxAnimation->isChecked()));
	QRect parentFrame=frameGeometry ();
	dialog->setGeometry(parentFrame.x()+120,parentFrame.y()+120,480,620);
	dialog->exec();
}

void GlobalOptionsDialog::currentCollapseIndexChanged(int index)
{

	bool collapseLeafNodes=(excludeLeafNodes->checkState()==Qt::Checked);
	bool collapseNoWeightBones=(excludeNoWeightBones->checkState()==Qt::Checked);
	bool exportSkeletal=(animationSkeletalEnable->checkState()==Qt::Checked);

	switch(index)
	{
	case	0:
		scene->collapseNone(1,collapseLeafNodes,collapseNoWeightBones,exportSkeletal);
		break;
	case	1:
		scene->collapseAll(1,collapseLeafNodes,collapseNoWeightBones,exportSkeletal);
		break;
	case	2:
		scene->collapseAllButTop(1,collapseLeafNodes,collapseNoWeightBones,exportSkeletal);
		break;
	}
}



void GlobalOptionsDialog::collapsedStatusChanged()
{
	currentCollapseIndexChanged(collapseNodes->currentIndex());
}

void GlobalOptionsDialog::geometryOptionsChanged()
{
	triangleSort->setEnabled(geometryOptions->currentIndex()==0);
}