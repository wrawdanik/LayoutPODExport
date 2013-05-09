 #include <QtGui>
#include <QFileInfo>

#include "MainDialog.h"
#include "Database.h"
#include "UvTemplateDialog.h"
#include "LmPODExportMain.h"
#include "ProgramOptions.h"

using namespace LmPODExport;

UvTemplateDialog::UvTemplateDialog( Database *db,QWidget *parent):QDialog(parent)
{
	lwMain=db->getLightwaveMain();
	this->db=db;
	setModal(true);
	setWindowTitle("LmOgreExport 1.0 - UV Template");

	setMinimumWidth(320);

	QGroupBox *genOptionsBox = new QGroupBox("UV Template");
	
	uvMapDrop=new PropertyDropDown;
	QStringList pChoices;

	for(int i=0;i<db->uvMaps.size();i++)
	{
		pChoices << db->uvMaps[i]->name;
	}

	uvMapDrop->setOptions(pChoices);

	uvSizeEdit=new QLineEdit;
	uvSizeEdit->setValidator(new QIntValidator(uvSizeEdit));
	uvSizeEdit->setMaxLength(4);
	uvSizeEdit->setText(QString::number(db->getProgramOptions()->uvTemplateSize));
	uvSizeEdit->setFixedWidth(52);
	uvFColor=new QPushButton;

	QPalette palette = uvFColor->palette();
	palette.setColor(QPalette::Button, db->getProgramOptions()->uvTemplateColor);
	uvFColor->setPalette(palette); 

	QGridLayout *gLay=new QGridLayout;

	gLay->addWidget(new QLabel("UV Map:"),0,0,Qt::AlignRight);
	gLay->addWidget(uvMapDrop,0,1,Qt::AlignLeft);

	gLay->addWidget(new QLabel("UV Template Image Size::"),1,0,Qt::AlignRight);
	gLay->addWidget(uvSizeEdit,1,1,Qt::AlignLeft);

	gLay->addWidget(new QLabel("Foreground Color:"),2,0,Qt::AlignRight);
	gLay->addWidget(uvFColor,2,1,Qt::AlignLeft);


	gLay->setMargin(8);
	gLay->setColumnStretch(0,0);
	gLay->setColumnStretch(1,0);
	gLay->setColumnStretch(3,10);	
	gLay->setRowStretch(4,20);	

	genOptionsBox->setLayout(gLay);
		
	savedMsg=new QLabel;
	QPushButton *close=new QPushButton("Close");
	QPushButton *saveFile=new QPushButton("Save Template");

	QGridLayout *gLay2=new QGridLayout;

	gLay2->addWidget(savedMsg,0,0,Qt::AlignLeft);
	gLay2->addWidget(close,0,3,Qt::AlignRight);
	gLay2->addWidget(saveFile,0,2,Qt::AlignRight);
	gLay2->setColumnStretch(0,0);
	gLay2->setColumnStretch(1,10);
	gLay2->setColumnStretch(2,0);
	gLay2->setColumnStretch(3,0);
	gLay2->setMargin(4);

	QVBoxLayout *mainLayout=new QVBoxLayout(this);	
	mainLayout->addSpacing(8);
	mainLayout->addWidget(genOptionsBox);
	mainLayout->addLayout(gLay2);
	mainLayout->setMargin(4);

	connect(close,SIGNAL(clicked()), this , SLOT (accept()));
	connect(saveFile,SIGNAL(clicked()), this , SLOT (generateImage()));
	connect(uvFColor,SIGNAL(clicked()), this , SLOT (selectColor()));

}
UvTemplateDialog::~UvTemplateDialog()
{

}


void UvTemplateDialog::selectColor()
{
	QPalette palette = uvFColor->palette();
	palette.setColor(QPalette::Button, db->getProgramOptions()->uvTemplateColor);
	

	QColor clr=QColorDialog::getColor (palette.color(QPalette::Button),this);
	if(clr.isValid())
	{
		palette.setColor(QPalette::Button, clr);
		uvFColor->setPalette(palette); 
		db->getProgramOptions()->uvTemplateColor=clr;
	}
}	

void UvTemplateDialog::generateImage()
{

	QString mapName=uvMapDrop->text();

	QString fileName = QFileDialog::getSaveFileName(this, "Save UV Image Template", db->getProgramOptions()->lastSavedPathUVTemplate+"\\"+mapName,"Image (*.png)");

	if(fileName.isNull())
	{
		savedMsg->setText("Save cancelled.");
		return;
	}
	UvData uvData;

	int number=uvSizeEdit->text().toInt();
		
	QImage	img(QSize(number,number),QImage::Format_ARGB32);
	
	QColor empty(255,255,255,0);

	img.fill(empty.rgba());

	QPainter painter;

	QPalette palette = uvFColor->palette();

	painter.begin(&img);
	painter.setRenderHints (QPainter::Antialiasing,false);	
	painter.setPen(palette.color(QPalette::Button));
	

	uvData.painter=&painter;
	uvData.mainData=lwMain;

	// set the current uv map to one we will be editing
	lwMain->editOp->pointVSet(lwMain->editOp->state,0,LWVMAP_TXUV,mapName.toAscii().data());

	lwMain->editOp->polyScan( lwMain->editOp->state, (EDPolyScanFunc (__cdecl *))UvTemplateDialog::scan_poly_for_uv, &uvData, OPLYR_NONEMPTY );	

	painter.end();

	img.save (fileName, "png");
	savedMsg->setText("Template saved.");

	QFileInfo filePath(fileName);
	db->getProgramOptions()->lastSavedPathUVTemplate=filePath.path();
}


EDError UvTemplateDialog::scan_poly_for_uv(UvData *uvData, const EDPolygonInfo *polygon )
{

	float	uvValue[2];
	float	luvValue[2];
	int foundNoUVPoint=0;;

	if(!polygon && !polygon->pol)
		return EDERR_NONE;

	for(int h=0;h<polygon->numPnts;h++)
	{
		uvValue[0]=0;
		uvValue[1]=0;
		if(!uvData->mainData->editOp->pointVEval(uvData->mainData->editOp->state,polygon->points[h],polygon->pol,uvValue))
		{
			foundNoUVPoint=1;
			break;
		}
	}
	
	QRect viewport=uvData->painter->viewport();

	if(!foundNoUVPoint)
	{
		for(int h=0;h<polygon->numPnts;h++)
		{
			uvValue[0]=0;
			uvValue[1]=0;
			uvData->mainData->editOp->pointVEval(uvData->mainData->editOp->state,polygon->points[h],polygon->pol,uvValue);

			if(h)
			{
				uvData->painter->drawLine(QPointF((luvValue[0]*(float)viewport.width()), (float)(viewport.width())-((luvValue[1]*(float)viewport.width()))),
										  QPointF((uvValue[0]*(float)viewport.width()),(float)viewport.width()-((uvValue[1]*(float)viewport.width()))));

			}
			luvValue[0]=uvValue[0];
			luvValue[1]=uvValue[1];

		}
		uvData->mainData->editOp->pointVEval(uvData->mainData->editOp->state,polygon->points[0],polygon->pol,uvValue);

		uvData->painter->drawLine(QPointF((luvValue[0]*(float)viewport.width()), (float)viewport.width()-((luvValue[1]*(float)viewport.width()))),
				  QPointF((uvValue[0]*(float)viewport.width()),(float)viewport.width()-((uvValue[1]*(float)viewport.width()))));		
	}

	return EDERR_NONE;
}