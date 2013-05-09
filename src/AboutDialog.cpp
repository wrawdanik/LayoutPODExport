 #include <QtGui>
#include <QTextBrowser>
#include "AboutDialog.h"
#include "LayoutPODExportMain.h"

using namespace LayoutPODExport;

AboutDialog::AboutDialog(QWidget *parent):QDialog(parent)
{
	QVBoxLayout *mainLayout=new QVBoxLayout(this); 
	browser=new QTextBrowser();
	mainLayout->addWidget(browser);
	
	browser->setOpenExternalLinks(true);

	QString text;

	text.append("<b>LayoutPODExport by Walter Rawdanik</b><br><br>");
	text.append("LayoutPODExport is a file exporter plug-in for Lightwave Layout application capable of exporting Imagination Technologies<a href='http://www.imgtec.com/powervr/insider/'> POD </a>scene files.<br><br>");
	text.append("Currently supported features are:");
	text.append("<dl>");
	text.append("<dt>Meshes</dt>");
	text.append("<dd>- position</dd>");
	text.append("<dd>- normal</dd>");
	text.append("<dd>- color</dd>");
	text.append("<dd>- up to 8 UV coordinates</dd>");
	text.append("<dd>- weights/matrix</dd>");
	text.append("<dd>- tangent/binormal</dd>");
	text.append("<dt>Skeletal and Nodal animations</dt>");
	text.append("<dd>- up to 3 weights per bone</dd>");
	text.append("<dd>- bone batching</dd>");
	text.append("<dt>Choice of two vertex formats (float/short)</dt>");
	text.append("<dt>Tangent space generation</dt>");
	text.append("<dt>Polygon/vertex sorting</dt>");
	text.append("<dt>Polygon striping</dt>");
	text.append("</dl>");
	text.append("<br>");
	text.append("Limitations:<p>");
	text.append("The exported supports only triangle based geometry and won't export any camera/light/material information - ");
	text.append("in other words the plugin only deals with geometry and animation data.<p>");
	text.append("If you set your model to use short (16 bytes) format for vertex channels, you will need to make sure that your models is scaled appropriately.<Br>You can do that either within the Layout app itself or quickly apply scaling");
	text.append(" using the Geometry Scale option within the exporter itself (the exporter will only apply scaling to meshes ( it won't touch animation channels) and thus this option is not appropriate for scaling animated meshes).<p>");	
	text.append("Lastly, if you are not sure about certain functionality and usage of a particular option/widget, you can a click on a <b>'?'</b> button at the top and then click the relevant widget to pop up the <b>'What's This?'</b> window.<P>");	
	text.append("<br> Comments, bug reports, etc., should be directed to <b>podlayout@warmi.net</b>");

	

	browser->setHtml(text);
	
}
