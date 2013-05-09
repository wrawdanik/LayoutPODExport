#ifndef GLOBAL_OPTIONS_DIALOG_H
#define GLOBAL_OPTIONS_DIALOG_H

#include <QDialog>
#include <QString>
#include <QAction>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QIntValidator>
#include <QDoubleValidator>



class QListWidget;
class QTreeWidget;
class QGroupBox;

namespace LayoutPODExport
{
	
	class LayoutPODExportMain;
	class SceneInventory;
	class SceneItemOptions;
	

	class GlobalOptionsDialog : public QDialog
	{
		Q_OBJECT

	public:

		GlobalOptionsDialog(QWidget *parent,SceneInventory *scene,LayoutPODExportMain *mainData);
		~GlobalOptionsDialog();

signals:

   
		public slots:
		virtual void accept ();
		virtual void reject ();

	protected:



		protected slots:

		void openSceneHierarchy();
		void currentCollapseIndexChanged(int index);
		void collapsedStatusChanged();
		void geometryOptionsChanged();
		
	private:

		void verifyControls();
		void applyToAllObjects();
		void parseLineEditValues(SceneItemOptions *options);


		LayoutPODExportMain *lwMain;
		SceneItemOptions	*globalOptions;

		QPushButton	*okButton;
		QPushButton *cancelButton;
	
		QGroupBox  *gBoxAnimation;
		QCheckBox	*animationSkeletalEnable;
		QCheckBox	*animationSkeletalRestFrame;

		QComboBox	*animationFormat;

		QIntValidator	*intValidator;
		QDoubleValidator	*floatValidator;

		QComboBox *collapseNodes;
		QCheckBox	*excludeLeafNodes;
		QCheckBox	*excludeNoWeightBones;

		QComboBox	*vertexFormat;
		QComboBox	*geometryOptions;
		QComboBox	*triangleSort;
		QLineEdit	*vertexScale;
		QLineEdit	*uvScale;
		QLineEdit	*maxBones;




		QListWidget	*vertexOptionsList;

		SceneInventory *scene;

		QTreeWidget	*hierarchyTree;
	};


};


#endif
