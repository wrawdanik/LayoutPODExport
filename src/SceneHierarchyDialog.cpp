 #include <QtGui>
#include <QComboBox>
#include "LayoutPODExportMain.h"
#include "Database.h"
#include "QGroupBox.h"
#include "Database.h"
#include "SceneItemOptions.h"
#include "SceneHierarchyDialog.h"
#include <QTreeWidget>

using namespace LayoutPODExport;

SceneHierarchyDialog::SceneHierarchyDialog(QWidget *parent,SceneInventory *scene,LayoutPODExportMain *mainData, bool isSkeletal):QDialog(parent)
{

	this->isSkeletal=isSkeletal;
	this->scene=scene;
	lwMain=mainData;
	this->globalOptions=&scene->getGlobalOptions();
	setModal(true);
	setWindowTitle("LayoutPODExport Scene Hierarchy");




	QVBoxLayout *mainLayout=new QVBoxLayout(this); 

	hierarchyTree=new QTreeWidget();
	mainLayout->addWidget(hierarchyTree);
	


	QPushButton *okButton=new QPushButton("OK");

	QGridLayout *gLayBottom=new QGridLayout;		
	gLayBottom->addWidget(okButton,0,1,Qt::AlignRight);
	gLayBottom->setColumnStretch(0,10);
	gLayBottom->setColumnStretch(1,0);
	gLayBottom->setColumnStretch(2,0);
	gLayBottom->setMargin(4);

	mainLayout->addLayout(gLayBottom);
	this->raise();


	connect(okButton,SIGNAL(clicked( )),this, SLOT(accept()));


	populateTree();
	expandAllNodes();


	connect(hierarchyTree,SIGNAL(itemChanged(QTreeWidgetItem *,int )),this, SLOT(itemChanged(QTreeWidgetItem *,int)));

}
SceneHierarchyDialog::~SceneHierarchyDialog()
{
}


void SceneHierarchyDialog::accept()
{
	QDialog::accept();
}

void SceneHierarchyDialog::reject()
{
	QDialog::reject();

}

void SceneHierarchyDialog::populateTree()
{

	QStringList labels;	
	labels << "Scene Nodes";
	hierarchyTree->setHeaderLabels ( labels );
	hierarchyTree->setColumnCount(1);
	hierarchyTree->setRootIsDecorated (true);

	QList<SceneItem*> &topLevel=scene->getTopLevelItems();

	for(int i=0;i<topLevel.size();i++)
	{	
		SceneItem *item=topLevel[i];	
		createTreeNode(item,0);				
	}
}


void SceneHierarchyDialog::createTreeNode(SceneItem *item, QTreeWidgetItem *parent)
{
	QTreeWidgetItem *treeItem=parent;
	if(!item->collapsed[1] || item->hasMesh)
	{
		treeItem=new QTreeWidgetItem(parent,item->lwType);
		allItems.push_back(treeItem);
		treeItem->setText(0,item->name);

		treeItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
		if(item->hasMesh)
			treeItem->setIcon(0,QIcon(":/icons/mesh.png"));
		else
		{

			if(item->lwType==LWI_BONE)
			{
				if(item->hasMeshConnection)
					treeItem->setIcon(0,QIcon(":/icons/bone.png"));
				else
					treeItem->setIcon(0,QIcon(":/icons/bone_red.png"));
			}
			else
			{
				if(item->hasMeshConnection)
					treeItem->setIcon(0,QIcon(":/icons/node.png"));
				else
					treeItem->setIcon(0,QIcon(":/icons/node_red.png"));
			}
		}
		if(parent==0)
			hierarchyTree->addTopLevelItem (treeItem);
		if(item->collapsed[1])
			treeItem=parent;
	}

	if(item->children.size()>0)
	{
		for(int i=0;i<item->children.size();i++)
		{
			createTreeNode(item->children[i],treeItem);
		}
	}

}

void SceneHierarchyDialog::expandAllNodes()
{
	for(int i=0;i<allItems.size();i++)
		allItems[i]->setExpanded(true);
}


