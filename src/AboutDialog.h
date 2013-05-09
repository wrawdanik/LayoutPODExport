#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H

#include <QDialog>


class QTextBrowser;

namespace LayoutPODExport
{
	
	

	class AboutDialog : public QDialog
	{
		Q_OBJECT

	public:

		AboutDialog(QWidget *parent);

signals:

   
		public slots:

	protected:



		protected slots:



		
	private:

		QTextBrowser *browser;
	};


};


#endif
