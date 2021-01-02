#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QMetaType>
#include <QMessageBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QTableView>
#include <QHeaderView>

#include <Geom_TrimmedCurve.hxx>
#include <GC_MakeSegment.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <AIS_ListOfInteractive.hxx>

#include "robotics/RLAPI_PlanThread.h"
#include "robotics/RLAPI_ConfigurationOptimizer.h"
#include "robotics/RLConvertAPI.h"

#include "dock/CustomDockWidget.h"
#include "dock/CustomDockTabBar.h"

#include "delegate/ConfigurationDelegate.h"
#include "delegate/ConfigurationModel.h"
#include "delegate/OperationalDelegate.h"
#include "delegate/OperationalModel.h"

#include "occWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionView_Start_Position_triggered();
    void on_actionView_End_Position_triggered();
    void on_actionSet_Start_Position_triggered();
    void on_actionSet_End_Position_triggered();
    void on_actionStart_Planner_triggered();
    void on_actionPause_Planner_triggered();
    void on_actionExit_Planner_triggered();
    void on_actionImport_Model_triggered();

private:
    Ui::MainWindow *ui;
    CustomDockWidget *mDockWidget;//point to the active dock widget in current window
    RLAPI_PlanThread *mPlannerThread;
    OCCWidget *aMdlWidget;
    RLConvertAPI *aConvertAPI;
    CustomDockWidget *mConfigDock;
    CustomDockWidget *mOperationDock;
    ConfigurationModel *mConfigModel;
    OperationalModel *mOperationModel;

    std::map<Qt::DockWidgetArea, CustomDockTabBar*> mDockWidgetToolBar;
    rl::math::Vector mStartVec;
    rl::plan::VectorList mEndList;
    rl::plan::VectorList optimizedEndList;
    rl::plan::VectorList::iterator endsIterator;
    rl::math::Vector lastEndVect;
    double totalPathLen;

    gp_Pnt currentPahtPnt;
    gp_Pnt endPnt;
    QList<Handle(AIS_Shape)> pathLines;

    //init the robot and dock model
    void initRL();
    void creatConfigDock();
    void creatOperationDock();

    //robot control, data display and thread
    void displayJointPosition();
    void displayOperationalPosition();
    void drawPathLine();
    void connectThread();

    //dock widget needs these functions
    void creatDockWidgetToolBar();
    void hideDockWidget(CustomDockWidget* dockWidget);
    void dockWidgetPinned(CustomDockWidget* dockWidget);
    void dockWidgetUnpinned(CustomDockWidget* dockWidget);
    void dockWidgetDocked(CustomDockWidget* dockWidget);
    void dockWidgetUndocked(CustomDockWidget* dockWidget);
    Qt::ToolBarArea dockAreaToToolBarArea(Qt::DockWidgetArea area);
    CustomDockTabBar* getDockWidgetBar(Qt::DockWidgetArea area);
    void showDockWidget(CustomDockWidget* dockWidget);
};
#endif // MAINWINDOW_H
