#ifndef SCENE_HIERARCHY_DIALOG_H
#define SCENE_HIERARCHY_DIALOG_H

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
class QTreeWidgetItem;

namespace LayoutPODExport
{
	
	class LayoutPODExportMain;
	class SceneInventory;
	class SceneItemOptions;
	class SceneItem;
	

	class SceneHierarchyDialog : public QDialog
	{
		Q_OBJECT

	public:

		SceneHierarchyDialog(QWidget *parent,SceneInventory *scene,LayoutPODExportMain *mainData, bool isSkeletal);
		~SceneHierarchyDialog();

signals:

   
		public slots:
		virtual void accept ();
		virtual void reject ();

	protected:



		protected slots:


		
	private:

		void populateTree();
		void createTreeNode(SceneItem *item, QTreeWidgetItem *parent);
		void expandAllNodes();

		LayoutPODExportMain *lwMain;
		SceneItemOptions	*globalOptions;
		SceneInventory *scene;
		QTreeWidget	*hierarchyTree;
		QList<QTreeWidgetItem*> allItems;
		bool	isSkeletal;
	};


};


#endif
