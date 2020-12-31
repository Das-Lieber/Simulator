#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mDockWidget(nullptr)
{
    ui->setupUi(this);

#if _DEBUG
    setWindowTitle("Simulator_d");
#else
    setWindowTitle("Simulator");
#endif

    aMdlWidget = new OCCWidget();
    QGridLayout *centerLayOut = new QGridLayout;
    centerLayOut->addWidget(aMdlWidget);
    ui->centralwidget->setLayout(centerLayOut);

    creatDockWidgetToolBar();

    initRL();
    creatConfigDock();
    creatOperationDock();

//    mStartVec.resize(5);
//    mStartVec(0) = 4;
//    mStartVec(1) = 154;
//    mStartVec(2) = -417;
//    mStartVec(3) = 90*rl::math::constants::deg2rad;
//    mStartVec(4) = 90*rl::math::constants::deg2rad;

//    rl::math::Vector defaultEndPos(5);
//    defaultEndPos(0) = -93;
//    defaultEndPos(1) = 57;
//    defaultEndPos(2) = -506;
//    defaultEndPos(3) = 0*rl::math::constants::deg2rad;
//    defaultEndPos(4) = 90*rl::math::constants::deg2rad;
//    mEndList.push_back(defaultEndPos);
//    rl::math::Vector defaultEndPos1(5);
//    defaultEndPos1(0) = -90;
//    defaultEndPos1(1) = 12;
//    defaultEndPos1(2) = -410;
//    defaultEndPos1(3) = 0*rl::math::constants::deg2rad;
//    defaultEndPos1(4) = 70*rl::math::constants::deg2rad;
//    mEndList.push_back(defaultEndPos1);

    mPlannerThread = new RLAPI_PlanThread(*aConvertAPI->GetMdlDynamic(),*aConvertAPI->GetSolidScene(),aConvertAPI->GetModelMinSize());
    qRegisterMetaType<rl::math::Vector>("rl::math::Vector");
    qRegisterMetaType<ComputeError>("ComputeError");
    connectThread();
}

MainWindow::~MainWindow()
{
    delete mPlannerThread;
    delete ui;
}

void MainWindow::initRL()
{
    QString JointMdlFile = "./mdl/GP8.xml";
    QString JoingtSgFile = "./scene/GP_DMISModel.xml";
    QString JointModelFile = "./brep/GP8";
    aConvertAPI = new RLConvertAPI(JointMdlFile,JoingtSgFile,JointModelFile,this);
    aConvertAPI->InitLoadData();

    connect(aConvertAPI,&RLConvertAPI::JointPositionChanged,this,[=](){
        aMdlWidget->getView()->Update();
        displayJointPosition();
        displayOperationalPosition();
    });
    connect(aConvertAPI,&RLConvertAPI::JointCollision,this,[=](int index){
        ui->statusbar->showMessage(tr("Joint %1 COLLISION!!!").arg(index));
        aMdlWidget->getView()->SetBgGradientColors(Quantity_NOC_RED4,Quantity_NOC_WHITESMOKE,Aspect_GFM_VER);
    });
    connect(aConvertAPI,&RLConvertAPI::NoCollision,this,[=](){
        ui->statusbar->showMessage(tr("NOT COLLISION"));
        aMdlWidget->getView()->SetBgGradientColors(Quantity_NOC_BLUE4,Quantity_NOC_WHITESMOKE,Aspect_GFM_VER);
    });

    for (int i=0;i<aConvertAPI->GetJointModelShapes().size();++i)
    {
        aMdlWidget->getContext()->Display(aConvertAPI->GetJointModelShapes().at(i),Standard_False);
    }
    displayJointPosition();
    displayOperationalPosition();

    aMdlWidget->getContext()->Display(aConvertAPI->GetMeasureModelShape(),Standard_False);
    aMdlWidget->getView()->FitAll();
}

void MainWindow::creatConfigDock()
{
    QTableView *configTable = new QTableView;
    ConfigurationDelegate *configDelegate = new ConfigurationDelegate;
    ConfigurationModel *configModel = new ConfigurationModel;
    configDelegate->setMaxValue(aConvertAPI->MotionMaxValues);
    configDelegate->setMinValue(aConvertAPI->MotionMinValues);
    configDelegate->setJointType(aConvertAPI->GetJointType());
    configModel->setMaxValue(aConvertAPI->MotionMaxValues);
    configModel->setMinValue(aConvertAPI->MotionMinValues);
    configModel->setJointType(aConvertAPI->GetJointType());
    configModel->setJointModelDofs(aConvertAPI->GetJointModelDof());
    configTable->setItemDelegate(configDelegate);
    configTable->setModel(configModel);

    mConfigDock = new CustomDockWidget;
    connect(mConfigDock, &CustomDockWidget::signal_pinned, this, &MainWindow::dockWidgetPinned);
    connect(mConfigDock, &CustomDockWidget::signal_unpinned, this, &MainWindow::dockWidgetUnpinned);
    connect(mConfigDock, &CustomDockWidget::signal_docked, this, &MainWindow::dockWidgetDocked);
    connect(mConfigDock, &CustomDockWidget::signal_undocked, this, &MainWindow::dockWidgetUndocked);
    mConfigDock->setWidget(configTable);
    addDockWidget(Qt::LeftDockWidgetArea,mConfigDock);
}

void MainWindow::creatOperationDock()
{

    mOperationDock = new CustomDockWidget;
    connect(mOperationDock, &CustomDockWidget::signal_pinned, this, &MainWindow::dockWidgetPinned);
    connect(mOperationDock, &CustomDockWidget::signal_unpinned, this, &MainWindow::dockWidgetUnpinned);
    connect(mOperationDock, &CustomDockWidget::signal_docked, this, &MainWindow::dockWidgetDocked);
    connect(mOperationDock, &CustomDockWidget::signal_undocked, this, &MainWindow::dockWidgetUndocked);
//    addDockWidget(Qt::LeftDockWidgetArea,mOperationDock);
}

void MainWindow::displayJointPosition()
{

}

void MainWindow::displayOperationalPosition()
{

}

void MainWindow::drawPathLine()
{
    QList<double> currentPos = aConvertAPI->GetOperationalPosition();
    gp_Pnt tmp(currentPos.value(0),currentPos.value(1),currentPos.value(2));

    if((currentPahtPnt.X()==0)&&(currentPahtPnt.Y()==0)&&(currentPahtPnt.Z()==0))
        currentPahtPnt = tmp;
    if((currentPahtPnt.X()==tmp.X())&&(currentPahtPnt.Y()==tmp.Y())&&(currentPahtPnt.Z()==tmp.Z()))
        return;

    Handle(Geom_TrimmedCurve) geom_line =  GC_MakeSegment(currentPahtPnt,tmp).Value();
    TopoDS_Shape t_topo_line = BRepBuilderAPI_MakeEdge(geom_line);
    Handle(AIS_Shape) anAIS = new AIS_Shape(t_topo_line);
    anAIS->SetColor(Quantity_NOC_SEAGREEN1);
    anAIS->SetWidth(3);
    pathLines.append(anAIS);
    aMdlWidget->getContext()->Display(anAIS,false);

    currentPahtPnt = tmp;
}

void MainWindow::connectThread()
{
    connect(mPlannerThread,&RLAPI_PlanThread::ComputeSuccess,this,[=](int time){
        ui->statusbar->showMessage(tr("Solved %1ms,Optimizing...").arg(time));
    });
    connect(mPlannerThread,&RLAPI_PlanThread::ReadyToSetJointValue,this,[=](const rl::math::Vector &thePos){
        aConvertAPI->SetJointValue(thePos);
        drawPathLine();
    });
    connect(mPlannerThread,&RLAPI_PlanThread::ComputeOver,this,[=](double pathLength){
        totalPathLen += pathLength;
        ui->statusbar->showMessage(tr("Solved, Value %1").arg(totalPathLen));
        if(endsIterator!=optimizedEndList.end())
        {
            mPlannerThread->GetComputeArguments(lastEndVect,*endsIterator);
            lastEndVect = *endsIterator;
            endsIterator++;
            mPlannerThread->start();
        }
        else
        {
            mPlannerThread->quit();
        }
        currentPahtPnt = gp_Pnt(0,0,0);
    });
    connect(mPlannerThread,&RLAPI_PlanThread::ComputeTimeOut,this,[=](){
        mPlannerThread->terminate();//joints in mess ,need to init position after!!!
        aConvertAPI->SetJointValue(mStartVec);
        ui->statusbar->showMessage(tr("Compute Time Out!"));
    });
    connect(mPlannerThread,&RLAPI_PlanThread::ComputeFailed,this,[=](const ComputeError &errorCode){
        if(errorCode==ComputeError::InvalidConfig)
            QMessageBox::critical(this,"error","Invalid start point or end point!");
        else if(errorCode==ComputeError::ArgumentError)
            QMessageBox::critical(this,"error","Unsuitable planner arguments");

        mPlannerThread->quit();
        ui->statusbar->showMessage(tr("Compute Failed!"));
    });
}

void MainWindow::creatDockWidgetToolBar()
{
    Qt::DockWidgetArea area = Qt::LeftDockWidgetArea;
    CustomDockTabBar* leftDockWidgetBar = new CustomDockTabBar(area);
    mDockWidgetToolBar[area] = leftDockWidgetBar;
    connect(leftDockWidgetBar, &CustomDockTabBar::signal_dockWidgetButton_clicked, this, &MainWindow::showDockWidget);
    addToolBar(dockAreaToToolBarArea(area), leftDockWidgetBar);
    area = Qt::RightDockWidgetArea;
    CustomDockTabBar* rightDockWidgetBar = new CustomDockTabBar(area);
    mDockWidgetToolBar[area] = rightDockWidgetBar;
    connect(rightDockWidgetBar, &CustomDockTabBar::signal_dockWidgetButton_clicked, this, &MainWindow::showDockWidget);
    addToolBar(dockAreaToToolBarArea(area), rightDockWidgetBar);
    area = Qt::TopDockWidgetArea;
    CustomDockTabBar* topDockWidgetBar = new CustomDockTabBar(area);
    mDockWidgetToolBar[area] = topDockWidgetBar;
    connect(topDockWidgetBar, &CustomDockTabBar::signal_dockWidgetButton_clicked, this, &MainWindow::showDockWidget);
    addToolBar(dockAreaToToolBarArea(area), topDockWidgetBar);
    area = Qt::BottomDockWidgetArea;
    CustomDockTabBar* bottomDockWidgetBar = new CustomDockTabBar(area);
    mDockWidgetToolBar[area] = bottomDockWidgetBar;
    connect(bottomDockWidgetBar, &CustomDockTabBar::signal_dockWidgetButton_clicked, this, &MainWindow::showDockWidget);
    addToolBar(dockAreaToToolBarArea(area), bottomDockWidgetBar);
}

void MainWindow::hideDockWidget(CustomDockWidget *dockWidget)
{
    if((dockWidget == nullptr) || (dockWidget->isHidden())) {
        return;
    }

    mDockWidget = nullptr;

    dockWidget->hide();
}

void MainWindow::dockWidgetPinned(CustomDockWidget *dockWidget)
{
    if(dockWidget == nullptr) {
        return;
    }

    CustomDockTabBar* dockWidgetBar = getDockWidgetBar(dockWidget->getArea());
    if(dockWidgetBar == nullptr) {
        return;
    }

    mDockWidget = nullptr;

    std::vector<CustomDockWidget*> dockWidgetList = dockWidget->getTabifiedDocks();
    dockWidgetList.push_back(dockWidget);

    CustomDockWidget* prevDockWidget = nullptr;

    std::for_each(std::begin(dockWidgetList), std::end(dockWidgetList), [&](CustomDockWidget* dockWidget)
    {
        if(dockWidgetBar->removeDockWidget(dockWidget))
        {
            if(prevDockWidget == nullptr) {
                QMainWindow::addDockWidget(dockWidget->getArea(), dockWidget);
            }
            else {
                tabifyDockWidget(prevDockWidget, dockWidget);
            }

            prevDockWidget = dockWidget;

            dockWidget->setDockWidgetState(DockWidgetState::Docked);

            dockWidget->show();
        }
    } );

    dockWidget->raise();
}

void MainWindow::dockWidgetUnpinned(CustomDockWidget *dockWidget)
{
    if(dockWidget == nullptr) {
        return;
    }

    CustomDockTabBar* dockWidgetBar = getDockWidgetBar(dockWidget->getArea());
    if(dockWidgetBar == nullptr) {
        return;
    }

    QList<QDockWidget*> dockWidgetList = tabifiedDockWidgets(dockWidget);
    dockWidgetList.push_back(dockWidget);

    std::for_each(std::begin(dockWidgetList), std::end(dockWidgetList), [&](QDockWidget* qDockWidget)
    {
        CustomDockWidget* dockWidget = static_cast<CustomDockWidget*>(qDockWidget);

        dockWidget->setDockWidgetState(DockWidgetState::Hidden);

        if(!dockWidget->isHidden())
        {
            dockWidgetBar->addDockWidget(dockWidget);

            dockWidget->setTabifiedDocks(dockWidgetList);

            QMainWindow::removeDockWidget(dockWidget);
        }
    } );
}

void MainWindow::dockWidgetDocked(CustomDockWidget *dockWidget)
{
    if(dockWidget == nullptr) {
        return;
    }
}

void MainWindow::dockWidgetUndocked(CustomDockWidget *dockWidget)
{
    hideDockWidget(mDockWidget);

    CustomDockTabBar* dockWidgetBar = getDockWidgetBar(dockWidget->getArea());
    if(dockWidgetBar == nullptr) {
        return;
    }

    dockWidget->clearTabifiedDocks();

    if(dockWidgetBar->removeDockWidget(dockWidget))
    {
        if(!dockWidget->isFloating()) {
            QMainWindow::addDockWidget(dockWidget->getArea(), dockWidget);
        }

        dockWidget->show();
    }
}

Qt::ToolBarArea MainWindow::dockAreaToToolBarArea(Qt::DockWidgetArea area)
{
    switch(area)
    {
        case Qt::LeftDockWidgetArea: return Qt::LeftToolBarArea;
        case Qt::RightDockWidgetArea: return Qt::RightToolBarArea;
        case Qt::TopDockWidgetArea: return Qt::TopToolBarArea;
        case Qt::BottomDockWidgetArea: return Qt::BottomToolBarArea;
        default:
            return Qt::ToolBarArea(0);
    }
}

CustomDockTabBar *MainWindow::getDockWidgetBar(Qt::DockWidgetArea area)
{
    Q_ASSERT(mDockWidgetToolBar.find(area) != std::end(mDockWidgetToolBar));

    auto it = mDockWidgetToolBar.find(area);
    if(it != std::end(mDockWidgetToolBar)) {
        return it->second;
    }

    return nullptr;
}

void MainWindow::showDockWidget(CustomDockWidget *dockWidget)
{
    if(dockWidget == nullptr) {
        return;
    }

    if(dockWidget->isHidden())
    {
        hideDockWidget(mDockWidget);

        if(dockWidget->isFloating())
        {
            QMainWindow::addDockWidget(dockWidget->getArea(), dockWidget);
            dockWidget->setFloating(false);

            QMainWindow::removeDockWidget(dockWidget);
        }

        dockWidget->show();
        dockWidget->raise();

        dockWidget->setFocus();

        mDockWidget = dockWidget;
    }
    else
    {
        hideDockWidget(dockWidget);
    }
}
