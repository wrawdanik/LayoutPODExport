#ifndef WEIGHTMAPS_DIALOG_H
#define WEIGHTMAPS_DIALOG_H

#include <QDialog>
#include <QString>
#include <QButtonGroup>
#include <qlist>

class PropertyDropDown;
class QPushButton;
class QCheckBox;

namespace LmPODExport
{
	class Options;
	class Database;
	class PropertyDropDown;

	class WeightsMapDialog : public QDialog
	{
		Q_OBJECT

	public:

		WeightsMapDialog(Database *db,QWidget *parent=0);
		virtual ~WeightsMapDialog();

		public slots:

		virtual void accept ();		

	protected:

		

		protected slots:
		
		void checkClicked (int id);
		void boneTypeChange (int id);
	private:
		PropertyDropDown	*weightTypeDrop;
		QButtonGroup	uvChecks;
		QList<PropertyDropDown*>	assignments;
		Database		*db;
	};


};


#endif
