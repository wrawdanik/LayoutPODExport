 #include <QtGui>

#include "Database.h"
#include "WeightMapsDialog.h"
#include "LmPODExportMain.h"
#include "ProgramOptions.h"
#include "MainDialog.h"

using namespace LmPODExport;

WeightsMapDialog::WeightsMapDialog( Database *db,QWidget *parent):QDialog(parent)
{
	this->db=db;
	setModal(true);
	setWindowTitle("LmOgreExport 1.0 - Assign Weight Maps");

	setMinimumWidth(280);

	QGroupBox *genOptionsBox = new QGroupBox("UV/Weight Maps");
		

	int maxCount=ProgramOptions::EXPORT_MAX_WEIGHTMAPS;
	if(db->weightMaps.size()<maxCount)
		maxCount=db->weightMaps.size();

	QGridLayout *gLay=new QGridLayout;

	QStringList oList;

	oList << "Bone Assignment";
	oList << "Uv Assignment";

	weightTypeDrop=new PropertyDropDown;
	weightTypeDrop->setOptions(oList);	
	weightTypeDrop->setEnabled(false);
	weightTypeDrop->setCurrentIndex(db->getProgramOptions()->weightType);

	oList.clear();

	for(int i=0;i<db->weightMaps.size();i++)
	{
		oList.append(db->weightMaps[i]->name);
	}
	uvChecks.setExclusive (false ) ;

	

	for(int i=0;i<maxCount;i++)
	{
		QLabel *label=new QLabel("UV");
		QCheckBox *op=new QCheckBox;
		PropertyDropDown *asg=new PropertyDropDown;
		asg->setOptions(oList);		

		op->setCheckState(db->getProgramOptions()->includeWeightMaps[i]>=0 ? Qt::Checked:Qt::Unchecked);		
		asg->setEnabled((db->getProgramOptions()->includeWeightMaps[i]>=0  && db->getProgramOptions()->weightType==ProgramOptions::EXPORT_WEIGHT_UV) ? true:false);		
		asg->setCurrentIndex(db->getProgramOptions()->includeWeightMaps[i]);
		gLay->addWidget(label,i,0,Qt::AlignRight);
		gLay->addWidget(op,i,1,Qt::AlignRight);
		gLay->addWidget(asg,i,2,Qt::AlignRight);
		uvChecks.addButton(op,i);
		op->setEnabled( db->getProgramOptions()->weightType==ProgramOptions::EXPORT_WEIGHT_UV ? true:false);		
		assignments.append(asg);
	}	
	gLay->setColumnStretch(3,10);
	gLay->setRowStretch(maxCount,20);	

	genOptionsBox->setLayout(gLay);
		
	QPushButton *ok=new QPushButton("Ok");
	QPushButton *cancel=new QPushButton("Cancel");


	QGridLayout *gLay2=new QGridLayout;

	gLay2->addWidget(ok,0,3,Qt::AlignRight);
	gLay2->addWidget(cancel,0,2,Qt::AlignRight);
	gLay2->setColumnStretch(0,10);
	gLay2->setColumnStretch(1,0);
	gLay2->setColumnStretch(2,0);
	gLay2->setMargin(4);

	QHBoxLayout *labelLayout=new QHBoxLayout;	
	labelLayout->addWidget(new QLabel("Bone Export Type:"));
	labelLayout->addWidget(weightTypeDrop);
	
	QVBoxLayout *mainLayout=new QVBoxLayout(this);	
	mainLayout->addSpacing(8);
	mainLayout->addLayout(labelLayout);
	mainLayout->addWidget(genOptionsBox);
	mainLayout->addLayout(gLay2);
	mainLayout->setMargin(4);

	connect(ok,SIGNAL(clicked()), this , SLOT (accept()));
	connect(cancel,SIGNAL(clicked()), this , SLOT (reject()));
	connect(&uvChecks, SIGNAL(buttonClicked(int)), this, SLOT (checkClicked(int)));
	connect(weightTypeDrop,SIGNAL(currentIndexChanged(int )),this, SLOT(boneTypeChange(int)));
}
WeightsMapDialog::~WeightsMapDialog()
{

}

void WeightsMapDialog::checkClicked (int id)
{
	assignments[id]->setEnabled(((QCheckBox*)uvChecks.button(id))->checkState() ==Qt::Checked ? true:false); 
}

void WeightsMapDialog::boneTypeChange (int id)
{
	QList<QAbstractButton *> buttons=uvChecks.buttons();
	for(int i=0;i<buttons.size();i++)
	{
		assignments[i]->setEnabled(id!=ProgramOptions::EXPORT_WEIGHT_BONE ? true:false);
		buttons[i]->setEnabled(id!=ProgramOptions::EXPORT_WEIGHT_BONE ? true:false);
	}
}

void WeightsMapDialog::accept ()
{	
	QList<QAbstractButton *> buttons=uvChecks.buttons();

	bool wasAny=false;

	// first disable all weights 

	for(int i=0;i<ProgramOptions::EXPORT_MAX_WEIGHTMAPS;i++)
		db->getProgramOptions()->includeWeightMaps[i]=-1;


	for(int i=0;i<buttons.size();i++)
	{
		QCheckBox *cBox=(QCheckBox*)buttons[i];

		if(cBox->checkState()==Qt::Checked)
		{
			db->getProgramOptions()->includeWeightMaps[i]=assignments[i]->currentIndex();
			wasAny=true;
		}
		else
			db->getProgramOptions()->includeWeightMaps[i]=-1;
	}


	db->getProgramOptions()->weightType=weightTypeDrop->currentIndex();

	QDialog::accept();
}