#ifndef UV_TEMPLATE_DIALOG_H
#define UV_TEMPLATE_DIALOG_H

#include <QDialog>
#include <QString>
#include <lwmeshedt.h>

class PropertyDropDown;
class QPushButton;
class QLineEdit;
class QPushButton;
class QPainter;
class QLabel;

namespace LmPODExport
{
	class LmPODExportMain;
	class Database;
	class UvMap;

	class UvData
	{
	public:
		QPainter	*painter;
		LmPODExportMain *mainData;
	};


	class UvTemplateDialog : public QDialog
	{
		Q_OBJECT

	public:

		UvTemplateDialog(Database *db,QWidget *parent=0);
		virtual ~UvTemplateDialog();

	protected:

		static EDError scan_poly_for_uv(UvData *uvData, const EDPolygonInfo *polygon );

		protected slots:
	
		void generateImage();
		void selectColor();
	private:

		LmPODExportMain *lwMain;
		Database		*db;
		PropertyDropDown	*uvMapDrop;
		QLineEdit		*uvSizeEdit;
		QPushButton	*uvFColor;
		QLabel		*savedMsg;

	};


};


#endif
