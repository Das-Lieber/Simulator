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
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#include <QInputDialog>

#include <Geom_TrimmedCurve.hxx>
#include <GC_MakeSegment.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>

#include "delegate/tableViewJointDelegate.h"
#include "delegate/tableViewJointModel.h"
#include "delegate/tableViewPosDelegate.h"
#include "delegate/tableViewPosModel.h"

#include "dialog/EditLocationWidget.h"
#include "dialog/DHSettingWidget.h"
#include "dialog/ProcessDataWidget.h"

#include "dock/CustomDockWidget.h"
#include "dock/CustomDockTabBar.h"

#include "OCC/occWidget.h"

#include "QRibbon/QRibbon.h"

#include "robotics/RLAPI_PlanThread.h"
#include "robotics/RLAPI_ConfigurationOptimizer.h"
#include "robotics/RLConvertAPI.h"
#include "robotics/RLAPI_DHSetting.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initRL();

private slots:
    //motion planning
    void on_actionView_Start_Position_triggered();
    void on_actionView_End_Position_triggered();
    void on_actionSet_Start_Position_triggered();
    void on_actionSet_End_Position_triggered();

    //thread
    void on_actionStart_Planner_triggered();
    void on_actionPause_Planner_triggered();
    void on_actionExit_Planner_triggered();

    //file
    void on_actionImport_Model_triggered();

    //setting
    void on_actionOperate_Model_triggered();
    void on_actionDH_Setting_triggered();
    void on_actionEdit_Location_triggered();
    void on_actionProcess_Data_triggered();

    //view
    void on_actionSave_As_Picture_triggered();    
    void on_actionView_Wire_triggered();
    void on_actionRay_Trace_triggered();
    void on_actionAnti_Aliasing_triggered();
    void on_actionView_Back_triggered();
    void on_actionView_Top_triggered();
    void on_actionView_Front_triggered();
    void on_actionView_Bottom_triggered();
    void on_actionView_Left_triggered();
    void on_actionView_Right_triggered();
    void on_actionView_Shade_triggered();    

private:
    Ui::MainWindow *ui;
    QLabel *statusLabel;
    QWinTaskbarButton *mTaskBarButton;
    QWinTaskbarProgress *mTaskBarProgress;
    bool isRayTraceEnable;
    bool isAntialiasingEnable;

    RLAPI_PlanThread *mPlannerThread;
    OCCWidget *aMdlWidget;
    RLConvertAPI *aConvertAPI;
    CustomDockWidget *mConfigDock;
    CustomDockWidget *mOperationDock;
    tableViewJointModel *mConfigModel;
    tableViewPosModel *mOperationModel;
    ProcessDataWidget *mProcessData;
    CustomDockWidget *mEditDockDlg;
    CustomDockWidget *mDHDockDlg;

    rl::math::Vector mStartVec;
    rl::plan::VectorList mEndList;
    rl::plan::VectorList optimizedEndList;
    rl::plan::VectorList::iterator endsIterator;
    rl::math::Vector lastEndVect;
    double totalPathLen;
    bool threadWait;

    gp_Pnt currentPahtPnt;
    gp_Pnt endPnt;
    QList<Handle(AIS_Shape)> pathLines;

    void parseProcessData();

    //init the robot and dock model
    void creatConfigDock();
    void creatOperationDock();

    //robot control, data display and thread
    void displayJointPosition();
    void displayOperationalPosition();
    void drawPathLine();
    void connectThread();

    //dock widget needs these functions
    void creatDockWidgetToolBar();

    void addDockWidget(Qt::DockWidgetArea area, CustomDockWidget* dockWidget);
    void addDockWidget(Qt::DockWidgetArea area, CustomDockWidget* dockWidget, Qt::Orientation orientation);
    void showDockWidget(CustomDockWidget* dockWidget);
    void adjustDockWidget(CustomDockWidget* dockWidget);
    void hideDockWidget(CustomDockWidget* dockWidget);

    void dockWidgetPinned(CustomDockWidget* dockWidget);
    void dockWidgetUnpinned(CustomDockWidget* dockWidget);
    void dockWidgetDocked(CustomDockWidget* dockWidget);
    void dockWidgetUndocked(CustomDockWidget* dockWidget);

    Qt::ToolBarArea dockAreaToToolBarArea(Qt::DockWidgetArea area);
    CustomDockTabBar* getDockWidgetBar(Qt::DockWidgetArea area);
    QRect getDockWidgetsAreaRect();
    std::list<CustomDockWidget*> getDockWidgetListAtArea(Qt::DockWidgetArea area);


    CustomDockWidget* m_dockWidget;
    std::list<CustomDockWidget*> m_dockWidgets;
    std::map<Qt::DockWidgetArea, CustomDockTabBar*> m_dockWidgetToolBar;

    //init the function dock widget
    void creatEditLocationDock();
    void creatDHSettingDock();
};
#endif // MAINWINDOW_H
