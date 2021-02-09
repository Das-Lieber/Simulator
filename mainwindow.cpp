#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

QString JointMdlFile = "./mdl/GP8.xml";
QString JoingtSgFile = "./scene/GP_DMISModel.xml";
QString JointModelFile = "./brep/GP8";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isRayTraceEnable(false)
    , isAntialiasingEnable(false)
    , threadWait(false)
    , m_dockWidget(nullptr)
{
    ui->setupUi(this);

#if _DEBUG
    setWindowTitle(tr("Simulator_d"));
#else
    setWindowTitle(tr("Simulator"));
#endif    

    aMdlWidget = new OCCWidget(this);
    statusLabel = new QLabel(this);
    //add the progress on the task bar
    mTaskBarButton = new QWinTaskbarButton(this);
    mProcessData = new ProcessDataWidget;    
}

MainWindow::~MainWindow()
{
    delete mPlannerThread;
    delete mProcessData;
    delete aDHSetting;
    delete ui;
}

void MainWindow::initOCC()
{
    emit initStepChanged(ApplicationInitSteps::OpenCasCadeModel);

    QGridLayout *centerLayOut = new QGridLayout;
    centerLayOut->addWidget(aMdlWidget);
    ui->centralwidget->setLayout(centerLayOut);

    Handle(AIS_Shape) processModel = new AIS_Shape(mProcessData->getShape());
    aMdlWidget->getContext()->Display(processModel,Standard_False);
}

void MainWindow::initInterface()
{
    emit initStepChanged(ApplicationInitSteps::LoadUI);

    statusLabel->setMinimumWidth(200);
    ui->statusbar->addPermanentWidget(statusLabel);

    mTaskBarButton->setWindow(this->windowHandle());
    mTaskBarProgress = mTaskBarButton->progress();
    mTaskBarProgress->setVisible(false);

    createDockWidgetBar(Qt::LeftDockWidgetArea);
    createDockWidgetBar(Qt::RightDockWidgetArea);
    createDockWidgetBar(Qt::TopDockWidgetArea);
    createDockWidgetBar(Qt::BottomDockWidgetArea);
    creatEditLocationDock();
    creatDHSettingDock();

    QRibbon::install(this);
}

void MainWindow::initRL()
{
    aConvertAPI = new RLConvertAPI(JointMdlFile,JoingtSgFile,JointModelFile,this);    

    connect(aConvertAPI,&RLConvertAPI::JointPositionChanged,this,[=](){
        aMdlWidget->getView()->Update();
        if(RLAPI_PlanThread::PlannerSolved)
        {
            displayJointPosition();
            displayOperationalPosition();
        }
        if(DHSettingWidget::existOne)
        {
            updateDHCoordinates();
        }
    });
    connect(aConvertAPI,&RLConvertAPI::JointCollision,this,[=](const size_t &index){
        statusLabel->setText(tr("Joint %1 COLLISION!!!").arg(index));
        aMdlWidget->getView()->SetBgGradientColors(Quantity_NOC_RED4,Quantity_NOC_WHITESMOKE,Aspect_GFM_VER);
    });
    connect(aConvertAPI,&RLConvertAPI::SelfCollision,this,[=](const size_t &aIndex, const size_t &bIndex){
        statusLabel->setText(tr("Joint %1 and Joint %2 COLLISION!!!").arg(aIndex).arg(bIndex));
        aMdlWidget->getView()->SetBgGradientColors(Quantity_NOC_RED4,Quantity_NOC_WHITESMOKE,Aspect_GFM_VER);
    });
    connect(aConvertAPI,&RLConvertAPI::NoCollision,this,[=](){
        statusLabel->setText(tr("NOT COLLISION"));
        aMdlWidget->getView()->SetBgGradientColors(Quantity_NOC_GRAY80,Quantity_NOC_WHITESMOKE,Aspect_GFM_VER);
    });
    connect(aConvertAPI,&RLConvertAPI::RLSatutsChanged,this,[=](const RLConvertAPI::RLStatus &status){
        switch(status)
        {
        case RLConvertAPI::RLStatus::ReadingMdlXML:
            emit initStepChanged(ApplicationInitSteps::CreateDynamicModel);break;
        case RLConvertAPI::RLStatus::ReadingSceneXML:
            emit initStepChanged(ApplicationInitSteps::CreateScene);break;
        case RLConvertAPI::RLStatus::ParsingAssembleArgs:
            emit initStepChanged(ApplicationInitSteps::ParseScene);break;
        case RLConvertAPI::RLStatus::ParsingMotionArgs:
            emit initStepChanged(ApplicationInitSteps::ParseDHArguments);break;
        case RLConvertAPI::RLStatus::ReadingJointFiles:
            emit initStepChanged(ApplicationInitSteps::Load3DFile);break;
        }
    });

    aConvertAPI->InitLoadData();
    aDHSetting = new RLAPI_DHSetting;

    for (int i=0;i<aConvertAPI->GetJointModelShapes().size();++i)
    {
        Handle(AIS_Shape) aShape = aConvertAPI->GetJointModelShapes().at(i);
        if(i==0||i==1)
            aShape->SetColor(Quantity_NOC_GRAY60);
        aMdlWidget->getContext()->Display(aShape,Standard_False);
        aMdlWidget->getContext()->Deactivate(aShape);
    }

    aMdlWidget->getView()->FitAll();

    creatConfigDock();
    creatOperationDock();

    mPlannerThread = new RLAPI_PlanThread(*aConvertAPI->GetMdlDynamic(),*aConvertAPI->GetSolidScene(),aConvertAPI->GetModelMinSize());
    qRegisterMetaType<rl::math::Vector>("rl::math::Vector");
    qRegisterMetaType<ComputeError>("ComputeError");
    connectThread();

    emit initStepChanged(ApplicationInitSteps::ParsePTDContents);
    parseProcessData();    

    ui->statusbar->showMessage(tr("Init Success!"));
}

void MainWindow::creatConfigDock()
{
    QTableView *configTable = new QTableView;
    configTable->setWindowTitle(tr("Configuration"));
    configTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    configTable->horizontalHeader()->hide();

    QWidget *bottomWidget = new QWidget;
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    QSlider *aSlider = new QSlider(Qt::Horizontal);
    QLabel *jointLabel = new QLabel;
    QLabel *maxLabel = new QLabel;
    QLabel *minLabel = new QLabel;
    aSlider->setTickPosition(QSlider::TicksAbove);
    aSlider->setTickInterval(10);
    bottomLayout->addWidget(jointLabel);
    bottomLayout->addWidget(minLabel);
    bottomLayout->addWidget(aSlider);
    bottomLayout->addWidget(maxLabel);
    bottomWidget->setLayout(bottomLayout);
    connect(configTable,&QTableView::doubleClicked,this,[=](const QModelIndex &index){
        sliderIndex = index.row();
        jointLabel->setText(tr("Joint%1:").arg(index.row()));

        if(aConvertAPI->GetJointType()[index.row()]==RLAPI_JointType::Revolute)
        {
            aSlider->setMaximum(aConvertAPI->MotionMaxValues(index.row())*rl::math::constants::rad2deg);
            aSlider->setMinimum(aConvertAPI->MotionMinValues(index.row())*rl::math::constants::rad2deg);
            maxLabel->setText(QString::number(aConvertAPI->MotionMaxValues(index.row())*rl::math::constants::rad2deg));
            minLabel->setText(QString::number(aConvertAPI->MotionMinValues(index.row())*rl::math::constants::rad2deg));
            aSlider->setSingleStep(1.0);
            aSlider->setValue(aConvertAPI->GetJointPosition()(index.row())*rl::math::constants::rad2deg);
        }
        else
        {
            aSlider->setMaximum(aConvertAPI->MotionMaxValues(index.row()));
            aSlider->setMinimum(aConvertAPI->MotionMinValues(index.row()));
            maxLabel->setText(QString::number(aConvertAPI->MotionMaxValues(index.row())));
            minLabel->setText(QString::number(aConvertAPI->MotionMinValues(index.row())));
            aSlider->setSingleStep(3.0);
            aSlider->setValue(aConvertAPI->GetJointPosition()(index.row()));
        }
        connect(aSlider,&QSlider::sliderMoved,this,[=](int value){
            mConfigModel->initData(aConvertAPI->GetJointPosition());
            mConfigModel->updateModel();
            aConvertAPI->SetIndexedJointValue(sliderIndex,value);
            displayOperationalPosition();
        });

    });

    tableViewJointDelegate *configDelegate = new tableViewJointDelegate(this);
    configDelegate->setMaxValue(aConvertAPI->MotionMaxValues);
    configDelegate->setMinValue(aConvertAPI->MotionMinValues);
    configDelegate->setJointType(aConvertAPI->GetJointType());

    mConfigModel = new tableViewJointModel(this);
    mConfigModel->SetMaxValue(aConvertAPI->MotionMaxValues);
    mConfigModel->SetMinValue(aConvertAPI->MotionMinValues);
    mConfigModel->SetJointType(aConvertAPI->GetJointType());
    mConfigModel->SetJointModelDofs(aConvertAPI->GetJointModelDof());
    mConfigModel->initData(aConvertAPI->GetJointPosition());

    configTable->setItemDelegate(configDelegate);
    configTable->setModel(mConfigModel);

    connect(mConfigModel,&tableViewJointModel::changePositionAndValue,this,[=](const int& index,const double& value){
        aConvertAPI->SetIndexedJointValue(index,value);
        displayOperationalPosition();
        if(index==sliderIndex)
            aSlider->setValue(value);
    });

    mConfigDock = new CustomDockWidget;
    connect(mConfigDock, &CustomDockWidget::signal_pinned, this, &MainWindow::dockWidgetPinned);
    connect(mConfigDock, &CustomDockWidget::signal_unpinned, this, &MainWindow::dockWidgetUnpinned);
    connect(mConfigDock, &CustomDockWidget::signal_docked, this, &MainWindow::dockWidgetDocked);
    connect(mConfigDock, &CustomDockWidget::signal_undocked, this, &MainWindow::dockWidgetUndocked);
    mConfigDock->setWidget(configTable);    
    mConfigDock->addWidget(bottomWidget);
    addDockWidget(Qt::LeftDockWidgetArea,mConfigDock);    
}

void MainWindow::creatOperationDock()
{
    QTableView *operationTable = new QTableView;
    operationTable->setWindowTitle(tr("Operation"));
    operationTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    operationTable->verticalHeader()->hide();    

    tableViewPosDelegate *operationDelegate = new tableViewPosDelegate(this);

    mOperationModel = new tableViewPosModel(this);
    mOperationModel->initData(aConvertAPI->GetOperationalPosition());

    operationTable->setItemDelegate(operationDelegate);
    operationTable->setModel(mOperationModel);

    connect(mOperationModel,&tableViewPosModel::changePositionAndValue,this,[=](const int& index,const double& value){
        bool solve = aConvertAPI->SetIndexedInverseValue(index,value);
        if(!solve)
            mOperationModel->initData(aConvertAPI->GetOperationalPosition());
        displayJointPosition();
    });

    mOperationDock = new CustomDockWidget;
    connect(mOperationDock, &CustomDockWidget::signal_pinned, this, &MainWindow::dockWidgetPinned);
    connect(mOperationDock, &CustomDockWidget::signal_unpinned, this, &MainWindow::dockWidgetUnpinned);
    connect(mOperationDock, &CustomDockWidget::signal_docked, this, &MainWindow::dockWidgetDocked);
    connect(mOperationDock, &CustomDockWidget::signal_undocked, this, &MainWindow::dockWidgetUndocked);
    mOperationDock->setWidget(operationTable);
    addDockWidget(Qt::BottomDockWidgetArea,mOperationDock);
    this->resizeDocks({mOperationDock},{100},Qt::Vertical);
}

void MainWindow::displayJointPosition()
{
    mConfigModel->initData(aConvertAPI->GetJointPosition());
    mConfigModel->updateModel();
}

void MainWindow::displayOperationalPosition()
{
    mOperationModel->initData(aConvertAPI->GetOperationalPosition());
    mOperationModel->updateModel();
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
        statusLabel->setText(tr("Solved %1ms,Optimizing...").arg(time));
    });
    connect(mPlannerThread,&RLAPI_PlanThread::ReadyToSetJointValue,this,[=](const rl::math::Vector &thePos){
        aConvertAPI->SetJointValue(thePos);
        drawPathLine();
    });
    connect(mPlannerThread,&RLAPI_PlanThread::ComputeOver,this,[=](double pathLength){
        totalPathLen += pathLength;
        statusLabel->setText(tr("Solved, Value %1").arg(totalPathLen));
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
            aMdlWidget->getView()->Update();
        }
        currentPahtPnt = gp_Pnt(0,0,0);
    });
    connect(mPlannerThread,&RLAPI_PlanThread::ComputeTimeOut,this,[=](){
//        mPlannerThread->terminate();//joints in mess ,need to init position after!!!
//        aConvertAPI->SetJointValue(mStartVec);
//        statusLabel->setText(tr("Compute Time Out!"));
    });
    connect(mPlannerThread,&RLAPI_PlanThread::ComputeFailed,this,[=](const ComputeError &errorCode){
        if(errorCode==ComputeError::InvalidConfig)
            QMessageBox::critical(this,tr("error"),tr("Invalid start point or end point!"));
        else if(errorCode==ComputeError::ArgumentError)
            QMessageBox::critical(this,tr("error"),tr("Unsuitable planner arguments"));

        mPlannerThread->quit();
        statusLabel->setText(tr("Compute Failed!"));
    });
}



void MainWindow::creatEditLocationDock()
{
    mEditDockDlg = new CustomDockWidget;
    connect(mEditDockDlg, &CustomDockWidget::signal_pinned, this, &MainWindow::dockWidgetPinned);
    connect(mEditDockDlg, &CustomDockWidget::signal_unpinned, this, &MainWindow::dockWidgetUnpinned);
    connect(mEditDockDlg, &CustomDockWidget::signal_docked, this, &MainWindow::dockWidgetDocked);
    connect(mEditDockDlg, &CustomDockWidget::signal_undocked, this, &MainWindow::dockWidgetUndocked);
    addDockWidget(Qt::RightDockWidgetArea,mEditDockDlg);
    hideDockWidget(mEditDockDlg);
}

void MainWindow::creatDHSettingDock()
{
    mDHDockDlg = new CustomDockWidget;
    connect(mDHDockDlg, &CustomDockWidget::signal_pinned, this, &MainWindow::dockWidgetPinned);
    connect(mDHDockDlg, &CustomDockWidget::signal_unpinned, this, &MainWindow::dockWidgetUnpinned);
    connect(mDHDockDlg, &CustomDockWidget::signal_docked, this, &MainWindow::dockWidgetDocked);
    connect(mDHDockDlg, &CustomDockWidget::signal_undocked, this, &MainWindow::dockWidgetUndocked);
    addDockWidget(Qt::RightDockWidgetArea,mDHDockDlg);
    hideDockWidget(mDHDockDlg);
}

void MainWindow::on_actionView_Start_Position_triggered()
{
    if(mStartVec.size()==0)
    {
        statusLabel->setText(tr("start position not set!"));
        return;
    }

    aConvertAPI->SetJointValue(mStartVec);
    displayJointPosition();
    displayOperationalPosition();
}

void MainWindow::on_actionView_End_Position_triggered()
{
    if(mEndList.size()==0)
    {
        statusLabel->setText(tr("end position not set!"));
        return;
    }

    aConvertAPI->SetJointValue(*mEndList.rbegin());
    displayJointPosition();
    displayOperationalPosition();
}

void MainWindow::on_actionSet_Start_Position_triggered()
{
//    mStartVec = aConvertAPI->GetJointPosition();
//    statusLabel->setText(tr("start position set"));

//    QList<double> currentPos = aConvertAPI->GetOperationalPosition();
//    currentPahtPnt = gp_Pnt(currentPos.value(0),
//                            currentPos.value(1),
//                            currentPos.value(2)-1);//don't set them as same ,or can't draw the first part of the path line
}

void MainWindow::on_actionSet_End_Position_triggered()
{
//    rl::math::Vector EndPos = aConvertAPI->GetJointPosition();
//    mEndList.push_back(EndPos);
//    statusLabel->setText(tr("end position set"));
}

void MainWindow::on_actionStart_Planner_triggered()
{
    statusLabel->setText(tr("Solving......"));

    // 1.remove the old path line
    if(pathLines.size()>0){
        for(int i=0;i<pathLines.size();++i)
        {
            aMdlWidget->getContext()->Erase(pathLines[i],false);
        }
    }
    pathLines.clear();

    // 2.optimize the path node
    if(mStartVec.size()==0)
    {
        statusLabel->setText(tr("start position not set!"));
        return;
    }
    if(mEndList.size()==0)
    {
        statusLabel->setText(tr("end position not set!"));
        return;
    }

//    if(mEndList.size()>1)
//    {
//        RLAPI_ConfigurationOptimizer anOptimizer;
//        anOptimizer.SetStartConfigurations(mStartVec);
//        anOptimizer.SetEndConfigurations(mEndList);
//        anOptimizer.theDynamic = aConvertAPI->GetMdlDynamic();
//        optimizedEndList = anOptimizer.Process();
//    }
//    else optimizedEndList = mEndList;
    optimizedEndList = mEndList;

    // 3.start
    totalPathLen=0;
    endsIterator=optimizedEndList.begin();
    mPlannerThread->GetComputeArguments(mStartVec,*endsIterator);
    lastEndVect = *endsIterator;
    endsIterator++;
    mPlannerThread->start();
}

void MainWindow::on_actionPause_Planner_triggered()
{
    if(mPlannerThread->isRunning()&&!threadWait)
    {
        threadWait = true;
        ui->actionPause_Planner->setIcon(QIcon(":/Simulator/icons/thread_resume.png"));
        ui->actionPause_Planner->setToolTip(tr("Resume Thread"));
        mPlannerThread->pause();
    }
    else if(mPlannerThread->isRunning()&&threadWait)
    {
        threadWait = false;
        ui->actionPause_Planner->setIcon(QIcon(":/Simulator/icons/thread_pause.png"));
        ui->actionPause_Planner->setToolTip(tr("Pause Thread"));
        mPlannerThread->resume();
    }
}

void MainWindow::on_actionExit_Planner_triggered()
{
    if(!mPlannerThread->isRunning())
        return;

    int result = QMessageBox::warning(this,tr("warning"),tr("Sure to exit the plan thread?\n"
                                               "This may cause uncertain consequences."),QMessageBox::Yes,QMessageBox::No);
    if(result==QMessageBox::Yes)
    {
        mPlannerThread->terminate();
        RLAPI_PlanThread::PlannerSolved = false;
    }
    else
        return;
}

void MainWindow::on_actionImport_Model_triggered()
{
    QString modelFileName = QFileDialog::getOpenFileName(this,tr("choose file"),"",tr("Support Type(*.stp *.step *.STP *.STEP *.iges *.igs *.IGES *.IGS *.brep *.brp\n)"
                                                                                      "*.stp *.step *.STP *.STEP\n"
                                                                                      "*.iges *.igs *.IGES *.IGS\n"
                                                                                      "*.brep *.brp"));
    if(modelFileName.isEmpty())
        return;

    mTaskBarProgress->setVisible(true);
    mTaskBarProgress->setValue(10);

    aConvertAPI->ImportSceneModel(modelFileName);    
    mTaskBarProgress->setValue(30);

    aMdlWidget->getContext()->Erase(aConvertAPI->GetMeasureModelShape(),Standard_False);
    mTaskBarProgress->setValue(40);

    aConvertAPI->ResetSceneModel();
    mTaskBarProgress->setValue(80);

    mPlannerThread->deleteLater();
    mPlannerThread = new RLAPI_PlanThread(*aConvertAPI->GetMdlDynamic(),*aConvertAPI->GetSolidScene(),aConvertAPI->GetModelMinSize());
    connectThread();

    mConfigModel->initData(aConvertAPI->GetJointPosition());
    mOperationModel->initData(aConvertAPI->GetOperationalPosition());

    aMdlWidget->getContext()->Display(aConvertAPI->GetMeasureModelShape(),Standard_False);
    aMdlWidget->getView()->FitAll();

    mTaskBarProgress->setValue(100);
    mTaskBarProgress->setVisible(false);
}

void MainWindow::on_actionOperate_Model_triggered()
{
    if(!aMdlWidget->getManipulator()->IsAttached())
    {
        QInputDialog aDialog;
        bool ok;
        int index = aDialog.getInt(this,tr("oprate"),tr("Joint"),0,0,aConvertAPI->GetJointModelDof(),1,&ok);
        if(ok)
            aMdlWidget->getManipulator()->Attach(aConvertAPI->GetJointModelShapes().at(index));
        else return;
    }
    else
        aMdlWidget->getManipulator()->Detach();

    aMdlWidget->getView()->Update();
}

void MainWindow::on_actionSave_As_Picture_triggered()
{
    QString picName = QFileDialog::getSaveFileName(this,tr("save file"),"","*.bmp *.png *.jpg");
    if(picName.isEmpty())
        return;

    Image_PixMap map;
    aMdlWidget->getView()->ToPixMap(map,aMdlWidget->width(),aMdlWidget->height(),Graphic3d_BT_RGBA);
    QImage image = QImage(map.Data(),aMdlWidget->width(),aMdlWidget->height(),QImage::Format_RGBA8888);
    image = image.mirrored(false, true);//need to mirror
    image.save(picName);
}

void MainWindow::on_actionRay_Trace_triggered()
{
    if(isRayTraceEnable)
    {
        aMdlWidget->getView()->ChangeRenderingParams().Method = Graphic3d_RM_RASTERIZATION;
        isRayTraceEnable = false;
        if(isAntialiasingEnable)
        {
            ui->actionAnti_Aliasing->setChecked(false);
            aMdlWidget->getView()->ChangeRenderingParams().IsAntialiasingEnabled = Standard_False;
            isAntialiasingEnable = false;
        }
        ui->actionAnti_Aliasing->setEnabled(false);
    }
    else
    {
        aMdlWidget->getView()->ChangeRenderingParams().Method = Graphic3d_RM_RAYTRACING;
        isRayTraceEnable = true;
        ui->actionAnti_Aliasing->setEnabled(true);
    }

    aMdlWidget->getContext()->UpdateCurrentViewer();
}

void MainWindow::on_actionAnti_Aliasing_triggered()
{
    if(isAntialiasingEnable)
    {
        aMdlWidget->getView()->ChangeRenderingParams().IsAntialiasingEnabled = Standard_False;
        isAntialiasingEnable = false;
    }
    else
    {
        aMdlWidget->getView()->ChangeRenderingParams().IsAntialiasingEnabled = Standard_True;
        isAntialiasingEnable = true;
    }

    aMdlWidget->getContext()->UpdateCurrentViewer();
}

void MainWindow::on_actionView_Back_triggered()
{
    aMdlWidget->getView()->SetProj(V3d_Ypos);
    aMdlWidget->getView()->FitAll();
}

void MainWindow::on_actionView_Top_triggered()
{
    aMdlWidget->getView()->SetProj(V3d_Zpos);
    aMdlWidget->getView()->FitAll();
}

void MainWindow::on_actionView_Front_triggered()
{
    aMdlWidget->getView()->SetProj(V3d_Yneg);
    aMdlWidget->getView()->FitAll();
}

void MainWindow::on_actionView_Bottom_triggered()
{
    aMdlWidget->getView()->SetProj(V3d_Zneg);
    aMdlWidget->getView()->FitAll();
}

void MainWindow::on_actionView_Left_triggered()
{
    aMdlWidget->getView()->SetProj(V3d_Xneg);
    aMdlWidget->getView()->FitAll();
}

void MainWindow::on_actionView_Right_triggered()
{
    aMdlWidget->getView()->SetProj(V3d_Xpos);
    aMdlWidget->getView()->FitAll();
}

void MainWindow::on_actionView_Shade_triggered()
{
    aMdlWidget->getContext()->SetDisplayMode(AIS_Shaded,Standard_True);
    ui->menuDisplay_Model->setIcon(QIcon(":/Simulator/icons/view_shade.png"));
}

void MainWindow::on_actionView_Wire_triggered()
{
    aMdlWidget->getContext()->SetDisplayMode(AIS_WireFrame,Standard_True);
    ui->menuDisplay_Model->setIcon(QIcon(":/Simulator/icons/view_wire.png"));
}

void MainWindow::parseProcessData()
{
    QList<TcpData> tcpPnts = mProcessData->getProcessPnts();
    aConvertAPI->LockBasePosition();
    QList<double> tcpInfo;

    for(int k=0;k<tcpPnts.size()/2;++k)
    {
        tcpInfo.clear();

        // only the up point is useful
        tcpInfo.append(tcpPnts[2*k].tcpPos.X());
        tcpInfo.append(tcpPnts[2*k].tcpPos.Y());
        tcpInfo.append(tcpPnts[2*k].tcpPos.Z());
        tcpInfo.append(tcpPnts[2*k].VX);
        tcpInfo.append(tcpPnts[2*k].VY);
        tcpInfo.append(tcpPnts[2*k].VZ);
        qDebug()<<tcpPnts[2*k].tcpPos.X()<<tcpPnts[2*k].tcpPos.Y()<<tcpPnts[2*k].tcpPos.Z()
                <<tcpPnts[2*k].VX<<tcpPnts[2*k].VY<<tcpPnts[2*k].VZ;

        if(aConvertAPI->SetInverseValue(tcpInfo))
        {
            displayJointPosition();
            displayOperationalPosition();
            if(k==0)
                mStartVec = aConvertAPI->GetJointPosition();
            else
                mEndList.push_back(aConvertAPI->GetJointPosition());
        }
    }
}

void MainWindow::updateDHCoordinates()
{
    aDHSetting->SetBasePosition(gp_Pnt(aConvertAPI->GetJointPosition()[0],
                                aConvertAPI->GetJointPosition()[1],0));
    QList<double> delta;
    for(int i=0;i<6;++i)
    {
        delta.append(aConvertAPI->GetJointPosition()[i+2]);
    }
    delta.append(0);
    aDHSetting->SetJointDelta(delta);

    aDHSetting->ComputeFK();
}

void MainWindow::on_actionEdit_Location_triggered()
{
    if(EditLocationWidget::existOne)
    {
        QMessageBox::critical(this,tr("error"),tr("there is an edit widget!"));
        return;
    }

    EditLocationWidget *aWidget = new EditLocationWidget;
    QStringList aList;
    for(std::size_t i=0;i<aConvertAPI->GetJointModelDof()+1;++i)
        aList<<QString("Link%1").arg(i);

    aWidget->setEditJointNameList(aList);
    aWidget->show();

    connect(aWidget,&EditLocationWidget::applyTrsf,this,[=](const int &index, const gp_Trsf &aTrsf){
        gp_Trsf before = aConvertAPI->GetJointModelShapes().at(index)->LocalTransformation();
        aConvertAPI->GetJointModelShapes().at(index)->SetLocalTransformation(before.Multiplied(aTrsf));
        aMdlWidget->getView()->Update();
    });
    connect(aWidget,&EditLocationWidget::requestClose,this,[=](){
        mEditDockDlg->removeWidget();
        hideDockWidget(mEditDockDlg);
    });

    mEditDockDlg->setWidget(aWidget);    
    if(mEditDockDlg->isHidden())
        showDockWidget(mEditDockDlg);   
}

void MainWindow::on_actionDH_Setting_triggered()
{
    if(DHSettingWidget::existOne)
    {
        QMessageBox::critical(this,tr("error"),tr("there is an setting widget!"));
        return;
    }

    updateDHCoordinates();

    mDHCoords = aDHSetting->GetCoords();
    for(int i=0;i<mDHCoords.size();++i)
    {
        aMdlWidget->getContext()->Display(mDHCoords[i],Standard_True);
    }

    DHSettingWidget *aWidget = new DHSettingWidget();
    aWidget->SetTheta(aDHSetting->GetTheta());
    aWidget->SetD(aDHSetting->GetD());
    aWidget->SetAlpha(aDHSetting->GetAlpha());
    aWidget->SetA(aDHSetting->GetA());

    QList<double> type;
    for(int i=0;i<aConvertAPI->GetJointType().size();++i)
    {
        if(aConvertAPI->GetJointType()[i] == RLAPI_JointType::Revolute)
            type.append(0);
        else if(aConvertAPI->GetJointType()[i] == RLAPI_JointType::Prismatic)
            type.append(1);
    }
    aWidget->SetType(type);
    aWidget->SetMin(aConvertAPI->MotionMinValues);
    aWidget->SetMax(aConvertAPI->MotionMaxValues);

    mDHDockDlg->setWidget(aWidget);    
    if(mDHDockDlg->isHidden())
        showDockWidget(mDHDockDlg);

    connect(aWidget,&DHSettingWidget::requestClose,this,[=](){
        mDHDockDlg->removeWidget();
        hideDockWidget(mDHDockDlg);
        for(int i=0;i<aDHSetting->GetCoords().size();++i)
        {
            aMdlWidget->getContext()->Erase(aDHSetting->GetCoords()[i],Standard_True);
        }
    });

    connect(aWidget,&DHSettingWidget::requestCompute,this,[=](){
        QList<double> theta = aWidget->GetTheta();
        aDHSetting->SetTheta(theta);
        QList<double> d = aWidget->GetD();
        aDHSetting->SetD(d);
        QList<double> alpha = aWidget->GetAlpha();
        aDHSetting->SetAlpha(alpha);
        QList<double> a = aWidget->GetA();
        aDHSetting->SetA(a);

        aDHSetting->Compute();
        aMdlWidget->getView()->Update();
    });

    connect(aWidget,&DHSettingWidget::requestWriteMdl,this,[=](){
        aDHSetting->ApplyDHArgs(JointMdlFile);
    });
}

void MainWindow::on_actionProcess_Data_triggered()
{    
    mProcessData->show();    
}

static
Qt::ToolBarArea dockAreaToToolBarArea(Qt::DockWidgetArea area)
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

void MainWindow::createDockWidgetBar(Qt::DockWidgetArea area)
{
    if(m_dockWidgetBar.find(area) != std::end(m_dockWidgetBar)) {
        return;
    }

    CustomDockTabBar* dockWidgetBar = new CustomDockTabBar(area);
    m_dockWidgetBar[area] = dockWidgetBar;
    connect(dockWidgetBar, &CustomDockTabBar::signal_dockWidgetButton_clicked, this, &MainWindow::showDockWidget);

    addToolBar(dockAreaToToolBarArea(area), dockWidgetBar);
}

void MainWindow::dockWidgetUnpinned(CustomDockWidget* dockWidget)
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

        if(dockWidget->isHidden())
            return;

        dockWidget->setState(DockWidgetState::Hidden);

        if(!dockWidget->isHidden())
        {
            dockWidgetBar->addDockWidget(dockWidget);

            dockWidget->setTabifiedDocks(dockWidgetList);

            QMainWindow::removeDockWidget(dockWidget);
        }
    } );

    if(dockWidget->getArea() == Qt::LeftDockWidgetArea)
    {
        getDockWidgetBar(Qt::TopDockWidgetArea)->insertSpacing();
        getDockWidgetBar(Qt::BottomDockWidgetArea)->insertSpacing();
    }
}

void MainWindow::dockWidgetPinned(CustomDockWidget* dockWidget)
{
    if(dockWidget == nullptr) {
        return;
    }

    CustomDockTabBar* dockWidgetBar = getDockWidgetBar(dockWidget->getArea());
    if(dockWidgetBar == nullptr) {
        return;
    }

    m_dockWidget = nullptr;

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

            dockWidget->setState(DockWidgetState::Docked);

            dockWidget->show();
        }
    } );

    dockWidget->raise();

    if((dockWidget->getArea() == Qt::LeftDockWidgetArea) &&
       dockWidgetBar->isHidden())
    {
        getDockWidgetBar(Qt::TopDockWidgetArea)->removeSpacing();
        getDockWidgetBar(Qt::BottomDockWidgetArea)->removeSpacing();
    }
}

void MainWindow::showDockWidget(CustomDockWidget* dockWidget)
{
    if(dockWidget == nullptr) {
        return;
    }

    if(dockWidget->isHidden())
    {
        if(dockWidget->isFloating())
        {
            QMainWindow::addDockWidget(dockWidget->getArea(), dockWidget);
            dockWidget->setFloating(false);

            QMainWindow::removeDockWidget(dockWidget);
        }

        adjustDockWidget(dockWidget);

        dockWidget->show();
        dockWidget->raise();

        dockWidget->setFocus();

        m_dockWidget = dockWidget;
    }
    else
    {
        hideDockWidget(dockWidget);
    }
}

void MainWindow::adjustDockWidget(CustomDockWidget* dockWidget)
{
    if(dockWidget == nullptr) {
        return;
    }

    QRect rect = getDockWidgetsAreaRect();
    switch(dockWidget->getArea())
    {
        case Qt::LeftDockWidgetArea: {
            dockWidget->setGeometry(rect.left(), rect.top(), dockWidget->width(), rect.height());
        }
        break;

        case Qt::TopDockWidgetArea: {
            dockWidget->setGeometry(rect.left(), rect.top(), rect.width(), dockWidget->height());
        }
        break;

        case Qt::RightDockWidgetArea: {
            dockWidget->setGeometry(rect.left() + rect.width() - dockWidget->width(), rect.top(), dockWidget->width(), rect.height());
        }
        break;

        case Qt::BottomDockWidgetArea: {
            dockWidget->setGeometry(rect.left(), rect.top() + rect.height() - dockWidget->height(), rect.width(), dockWidget->height());
        }
        break;
    }
}

CustomDockTabBar* MainWindow::getDockWidgetBar(Qt::DockWidgetArea area)
{
    Q_ASSERT(m_dockWidgetBar.find(area) != std::end(m_dockWidgetBar));

    auto it = m_dockWidgetBar.find(area);
    if(it != std::end(m_dockWidgetBar)) {
        return it->second;
    }

    return nullptr;
}

void MainWindow::addDockWidget(Qt::DockWidgetArea area, CustomDockWidget* dockWidget)
{
    addDockWidget(area, dockWidget, Qt::Vertical);
}

void MainWindow::addDockWidget(Qt::DockWidgetArea area, CustomDockWidget* dockWidget, Qt::Orientation orientation)
{
    if(dockWidget == nullptr) {
        return;
    }

    connect(dockWidget, &CustomDockWidget::signal_pinned, this, &MainWindow::dockWidgetPinned);
    connect(dockWidget, &CustomDockWidget::signal_unpinned, this, &MainWindow::dockWidgetUnpinned);
    connect(dockWidget, &CustomDockWidget::signal_docked, this, &MainWindow::dockWidgetDocked);
    connect(dockWidget, &CustomDockWidget::signal_undocked, this, &MainWindow::dockWidgetUndocked);

    m_dockWidgets.push_back(dockWidget);

    std::list<CustomDockWidget*> aList = getDockWidgetListAtArea(area);
    if(aList.size()==0)
    {
        QMainWindow::addDockWidget(area, dockWidget, orientation);
    }
    else
    {
        QMainWindow::tabifyDockWidget(*aList.begin(),dockWidget);
    }

    QString title = dockWidget->windowTitle();
    if(title.isEmpty()) {
        title = QObject::tr("Noname");
    }
}

void MainWindow::removeDockWidget(CustomDockWidget* dockWidget)
{
    if(dockWidget == nullptr) {
        return;
    }

    auto it = std::find(m_dockWidgets.begin(), m_dockWidgets.end(), dockWidget);
    if(it == m_dockWidgets.end()) {
        return;
    }

    m_dockWidgets.erase(it);

    if(dockWidget->isMinimized()) {
        dockWidgetPinned(dockWidget);
    }

    QMainWindow::removeDockWidget(dockWidget);

    dockWidget->setParent(nullptr);
}

void MainWindow::dockWidgetDocked(CustomDockWidget* dockWidget)
{
    if(dockWidget == nullptr) {
        return;
    }
}

void MainWindow::dockWidgetUndocked(CustomDockWidget* dockWidget)
{
    hideDockWidget(m_dockWidget);

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

        if((dockWidget->getArea() == Qt::LeftDockWidgetArea) &&
           dockWidgetBar->isHidden())
        {
            getDockWidgetBar(Qt::TopDockWidgetArea)->removeSpacing();
            getDockWidgetBar(Qt::BottomDockWidgetArea)->removeSpacing();
        }

        dockWidget->show();
    }
}

std::list<CustomDockWidget*> MainWindow::getDockWidgetListAtArea(Qt::DockWidgetArea area)
{
    std::list<CustomDockWidget*> dockWidgetList;
    std::copy_if(std::begin(m_dockWidgets), std::end(m_dockWidgets), std::back_inserter(dockWidgetList), [area](const CustomDockWidget* dockWidget) {
        return (dockWidget->getArea() == area) && (dockWidget->isDocked());
    });

    return dockWidgetList;
}

QRect MainWindow::getDockWidgetsAreaRect()
{
    int left = centralWidget()->x();
    std::list<CustomDockWidget*> leftAreaDockWidgets = getDockWidgetListAtArea(Qt::LeftDockWidgetArea);
    std::for_each(std::begin(leftAreaDockWidgets), std::end(leftAreaDockWidgets), [&left](const CustomDockWidget* dockWidget)
    {
        if((dockWidget->x() >= 0) && (dockWidget->width() > 0)) {
            left = std::min(left, dockWidget->x());
        }
    });

    int top = centralWidget()->y();
    std::list<CustomDockWidget*> topAreaDockWidgets = getDockWidgetListAtArea(Qt::TopDockWidgetArea);
    std::for_each(std::begin(topAreaDockWidgets), std::end(topAreaDockWidgets), [&top](const CustomDockWidget* dockWidget)
    {
        if((dockWidget->y() >= 0) && (dockWidget->height() > 0)) {
            top = std::min(top, dockWidget->y());
        }
    });

    int right = centralWidget()->x()+centralWidget()->width();
    std::list<CustomDockWidget*> rightAreaDockWidgets = getDockWidgetListAtArea(Qt::RightDockWidgetArea);
    std::for_each(std::begin(rightAreaDockWidgets), std::end(rightAreaDockWidgets), [&right](const CustomDockWidget* dockWidget)
    {
        if((dockWidget->x() >= 0) && (dockWidget->width() > 0)) {
            right = std::max(right, dockWidget->x() + dockWidget->width());
        }
    });

    int bottom = centralWidget()->y()+centralWidget()->height();
    std::list<CustomDockWidget*> bottomAreaDockWidgets = getDockWidgetListAtArea(Qt::BottomDockWidgetArea);
    std::for_each(std::begin(bottomAreaDockWidgets), std::end(bottomAreaDockWidgets), [&bottom](const CustomDockWidget* dockWidget)
    {
        if((dockWidget->y() >= 0) && (dockWidget->height() > 0)) {
            bottom = std::max(bottom, dockWidget->y() + dockWidget->height());
        }
    });

    return QRect(left, top, right-left, bottom-top);
}

void MainWindow::hideDockWidget(CustomDockWidget* dockWidget)
{
    if((dockWidget == nullptr) || (dockWidget->isHidden())) {
        return;
    }

    m_dockWidget = nullptr;

    dockWidget->hide();
}
