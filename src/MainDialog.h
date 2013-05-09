#ifndef MAIN_DIALOG_H
#define MAIN_DIALOG_H

#include <QDialog>
#include <QString>
#include <QTreeView>
#include <QToolButton>
#include <QAction>
#include <QLabel>
#include <QActionGroup>
#include <QComboBox>
#include <QList>
#include <QMenu>
#include <QFont>
#include <QStatusBar>


#include "Database.h"

class QTableWidget;
class QPushButton;
class QCheckBox;
class	QStandardItemModel;
class   QStandardItem;
class QVBoxLayout;
class QToolButton;
class QSpinBox;
class QSettings;


namespace LayoutPODExport
{
	class LayoutPODExportMain;
	class Database;

	class GenericListWidget : public QTreeView
	{
		Q_OBJECT

	public:
		GenericListWidget(QWidget *parent=0);
		void calculateItemSizeHint(QStandardItem *item);
		void updateLabels();
		void setLabels(const QStringList &labels);
	protected:


	QStandardItemModel *sModel;
	private:
		
	

		QStringList iLabels;
		
	};

	class GeometryStatsList : public GenericListWidget
	{
		Q_OBJECT


	public:
		GeometryStatsList(QWidget *parent=0);

		void updateStatsDisplay(SceneInventory *scene);
		virtual QSize sizeHint () const;
		void commitData();

	signals:

		void totalMeshCountChanged();

	protected slots:
		void dataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight );
		void comboDataChanged(int index);
	protected:

		enum ITEM_TYPES
		{
			SCENE_ITEM_EXPORT,
			VERTEX_DATA_TYPE,
			FLOAT_MESH_SCALE,
			FLOAT_UV_SCALE
		};

		
		void commitDataIForItem(QStandardItem *item);
		void createObjectEntry(SceneItem *sceneItem,QStandardItem *parent);
		//void createOptionsEntry(SceneItem *sceneItem,QStandardItem *parent);
		void createVertexEntry(SceneItem *sceneItem,QStandardItem *parent);
		void createFacesEntry(SceneItem *sceneItem,QStandardItem *parent);
		void createBonesEntry(SceneItem *sceneItem,QStandardItem *parent);
		void createErrorsEntry(SceneItem *sceneItem,QStandardItem *parent);
		QStandardItem* createParentErrorEntry(SceneItem *sceneItem,QStandardItem *parent);

		void updateTopLevelEntry(SceneItem *sceneItem, QStandardItem *item);

		QSize	iSize;
		bool checkForChanges;

		QList<QComboBox*>	geometryOptionsWidgets;
		QList<QComboBox*>	vertexFormatWidgets;
	};


	class MainDialog : public QDialog
	{
		Q_OBJECT

	public:

		MainDialog(LayoutPODExportMain *mainData);
		virtual ~MainDialog();


signals:


		public slots:
		virtual void accept ();
		virtual void reject ();

	protected:



		protected slots:

		void writeFile();
		void handleTotalMeshCountChanged();
		void displayGlobalOptions();
		void displayAbout();


		
	private:


		void initSettings();
		void saveSettings();

		void applyMotionToItems();
		void removeMotionFromItems();
		int lwCommand(LWLayoutGeneric *gen, const char *cmdname,const char *fmt, ...);
		void playAnimation();

		


		LayoutPODExportMain *lwMain;
		
		GeometryStatsList	*geometryList;

		QPushButton	*closeButton;
		QPushButton *exportButton;

		QLabel		*statusLabel;

		QLabel		*infoLabel;
		QPushButton *globalOptionsButton;
		QPushButton	*helpButton;

		SceneInventory *scene;
		QSettings	*settings;
	
	};


};


#endif
