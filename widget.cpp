#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

#if _DEBUG
    setWindowTitle("Simulator_d");
#else
    setWindowTitle("Simulator");
#endif

    QString JointMdlFile = "./mdl/GP8.xml";
    QString JoingtSgFile = "./scene/GP_DMISModel.xml";
    QString JointModelFile = "./brep/GP8";
    aConvertAPI = new RLConvertAPI(JointMdlFile,JoingtSgFile,JointModelFile,this);
    aConvertAPI->InitLoadData();

    aMdlWidget = new OCCWidget();
    aMdlWidget->resize(881,510);
    ui->gridLayout->addWidget(aMdlWidget,0,2,13,6);

    connect(aConvertAPI,&RLConvertAPI::JointPositionChanged,this,[=](){
        aMdlWidget->getView()->Update();
        displayJointPosition();
        displayOperationalPosition();
    });
    connect(aConvertAPI,&RLConvertAPI::JointCollision,this,[=](int index){
        ui->label_collision->setText(tr("Joint %1 COLLISION!!!").arg(index));\
        aMdlWidget->getView()->SetBgGradientColors(Quantity_NOC_RED4,Quantity_NOC_WHITESMOKE,Aspect_GFM_VER);
    });
    connect(aConvertAPI,&RLConvertAPI::NoCollision,this,[=](){
        ui->label_collision->setText(tr("NOT COLLISION"));
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

    ui->doubleSpinBox_joint0->setRange(aConvertAPI->MotionMinValues(0),aConvertAPI->MotionMaxValues(0));
    ui->doubleSpinBox_joint1->setRange(aConvertAPI->MotionMinValues(1),aConvertAPI->MotionMaxValues(1));
    ui->doubleSpinBox_joint2->setRange(aConvertAPI->MotionMinValues(2)*rl::math::constants::rad2deg,aConvertAPI->MotionMaxValues(2)*rl::math::constants::rad2deg);
    ui->doubleSpinBox_joint3->setRange(aConvertAPI->MotionMinValues(3)*rl::math::constants::rad2deg,aConvertAPI->MotionMaxValues(3)*rl::math::constants::rad2deg);
    ui->doubleSpinBox_joint4->setRange(aConvertAPI->MotionMinValues(4)*rl::math::constants::rad2deg,aConvertAPI->MotionMaxValues(4)*rl::math::constants::rad2deg);
    ui->doubleSpinBox_joint5->setRange(aConvertAPI->MotionMinValues(5)*rl::math::constants::rad2deg,aConvertAPI->MotionMaxValues(5)*rl::math::constants::rad2deg);
    ui->doubleSpinBox_joint6->setRange(aConvertAPI->MotionMinValues(6)*rl::math::constants::rad2deg,aConvertAPI->MotionMaxValues(6)*rl::math::constants::rad2deg);
    ui->doubleSpinBox_joint7->setRange(aConvertAPI->MotionMinValues(7)*rl::math::constants::rad2deg,aConvertAPI->MotionMaxValues(7)*rl::math::constants::rad2deg);

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

Widget::~Widget()
{
    delete mPlannerThread;
    delete ui;
}

void Widget::displayJointPosition()
{
    rl::math::Vector currentPos = aConvertAPI->GetJointPosition();
    disconnectConfigDoubleSpinBoxes();
    ui->doubleSpinBox_joint0->setValue(currentPos(0));
    ui->doubleSpinBox_joint1->setValue(currentPos(1));
    ui->doubleSpinBox_joint2->setValue(currentPos(2)*rl::math::constants::rad2deg);
    ui->doubleSpinBox_joint3->setValue(currentPos(3)*rl::math::constants::rad2deg);
    ui->doubleSpinBox_joint4->setValue(currentPos(4)*rl::math::constants::rad2deg);
    ui->doubleSpinBox_joint5->setValue(currentPos(5)*rl::math::constants::rad2deg);
    ui->doubleSpinBox_joint6->setValue(currentPos(6)*rl::math::constants::rad2deg);
    ui->doubleSpinBox_joint7->setValue(currentPos(7)*rl::math::constants::rad2deg);
    connectConfigDoubleSpinBoxes();
}

void Widget::displayOperationalPosition()
{
    QList<double> currentPos = aConvertAPI->GetOperationalPosition();
    disconnectOperationalDoubleSpinBoxes();
    ui->doubleSpinBox_posX->setValue(currentPos.value(0));
    ui->doubleSpinBox_posY->setValue(currentPos.value(1));
    ui->doubleSpinBox_posZ->setValue(currentPos.value(2));
    ui->doubleSpinBox_posA->setValue(currentPos.value(3));
    ui->doubleSpinBox_posB->setValue(currentPos.value(4));
    ui->doubleSpinBox_posC->setValue(currentPos.value(5));
    connectOperationalDoubleSpinBoxes();
}

void Widget::drawPathLine()
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

void Widget::connectThread()
{
    connect(mPlannerThread,&RLAPI_PlanThread::ComputeSuccess,this,[=](int time){
        ui->label_plannerState->setText(QString("Solved %1ms,Optimizing...").arg(time));
    });
    connect(mPlannerThread,&RLAPI_PlanThread::ReadyToSetJointValue,this,[=](const rl::math::Vector &thePos){
        aConvertAPI->SetJointValue(thePos);
        drawPathLine();
    });
    connect(mPlannerThread,&RLAPI_PlanThread::ComputeOver,this,[=](double pathLength){
        totalPathLen += pathLength;
        ui->label_plannerState->setText(QString("Solved, Value %1").arg(totalPathLen));
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
        ui->label_plannerState->setText("Compute Time Out!");
    });
    connect(mPlannerThread,&RLAPI_PlanThread::ComputeFailed,this,[=](const ComputeError &errorCode){
        if(errorCode==ComputeError::InvalidConfig)
            QMessageBox::critical(this,"error","Invalid start point or end point!");
        else if(errorCode==ComputeError::ArgumentError)
            QMessageBox::critical(this,"error","Unsuitable planner arguments");

        mPlannerThread->quit();
        ui->label_plannerState->setText("Compute Failed!");
    });
}

void Widget::on_doubleSpinBox_joint0_valueChanged(double arg1)
{
    aConvertAPI->SetIndexedJointValue(0,arg1);
}

void Widget::on_doubleSpinBox_joint1_valueChanged(double arg1)
{
    aConvertAPI->SetIndexedJointValue(1,arg1);
}

void Widget::on_doubleSpinBox_joint2_valueChanged(double arg1)
{
    aConvertAPI->SetIndexedJointValue(2,arg1);
}

void Widget::on_doubleSpinBox_joint3_valueChanged(double arg1)
{
    aConvertAPI->SetIndexedJointValue(3,arg1);
}

void Widget::on_doubleSpinBox_joint4_valueChanged(double arg1)
{
    aConvertAPI->SetIndexedJointValue(4,arg1);
}

void Widget::on_doubleSpinBox_joint5_valueChanged(double arg1)
{
    aConvertAPI->SetIndexedJointValue(5,arg1);
}

void Widget::on_doubleSpinBox_joint6_valueChanged(double arg1)
{
    aConvertAPI->SetIndexedJointValue(6,arg1);
}

void Widget::on_doubleSpinBox_joint7_valueChanged(double arg1)
{
    aConvertAPI->SetIndexedJointValue(7,arg1);
}

void Widget::on_doubleSpinBox_posX_valueChanged(double arg1)
{
    aConvertAPI->SetIndexedInverseValue(0,arg1);
}

void Widget::on_doubleSpinBox_posY_valueChanged(double arg1)
{
    aConvertAPI->SetIndexedInverseValue(1,arg1);
}

void Widget::on_doubleSpinBox_posZ_valueChanged(double arg1)
{
    aConvertAPI->SetIndexedInverseValue(2,arg1);
}

void Widget::on_doubleSpinBox_posA_valueChanged(double arg1)
{
    aConvertAPI->SetIndexedInverseValue(3,arg1);
}

void Widget::on_doubleSpinBox_posB_valueChanged(double arg1)
{
    aConvertAPI->SetIndexedInverseValue(4,arg1);
}

void Widget::on_doubleSpinBox_posC_valueChanged(double arg1)
{
    aConvertAPI->SetIndexedInverseValue(5,arg1);
}

void Widget::on_pushButton_setStartPos_clicked()
{
    mStartVec = aConvertAPI->GetJointPosition();
    ui->label_plannerState->setText("start position set");

    QList<double> currentPos = aConvertAPI->GetOperationalPosition();
    currentPahtPnt = gp_Pnt(currentPos.value(0),currentPos.value(1),currentPos.value(2)-1);//don't set them as same ,or can't draw the first part of the path line
}

void Widget::on_pushButton_setEndPos_clicked()
{
    rl::math::Vector EndPos = aConvertAPI->GetJointPosition();
    mEndList.push_back(EndPos);
    ui->label_plannerState->setText("end position set");
}

void Widget::on_pushButton_computePath_clicked()
{
    ui->label_plannerState->setText("Solving......");

    AIS_ListOfInteractive aList;
    aMdlWidget->getContext()->DisplayedObjects(aList);
    AIS_ListIteratorOfListOfInteractive anIterator;
    if(pathLines.size()>0)
    {
        for(anIterator.Init(aList);anIterator.More();anIterator.Next())
        {
            Handle(AIS_Shape) anAIS = Handle(AIS_Shape)::DownCast(anIterator.Value());
            for(int i=0;i<pathLines.size();++i)
            {
                if(anAIS->Shape().IsSame(pathLines.at(i)->Shape()))
                {
                    aMdlWidget->getContext()->Erase(anAIS,false);
                    pathLines.removeAt(i);
                }
            }
        }
    }

    RLAPI_ConfigurationOptimizer anOptimizer;
    anOptimizer.SetStartConfigurations(mStartVec);
    anOptimizer.SetEndConfigurations(mEndList);
    anOptimizer.theDynamic = aConvertAPI->GetMdlDynamic();
    optimizedEndList = anOptimizer.Process();

    totalPathLen=0;
    endsIterator=optimizedEndList.begin();
    mPlannerThread->GetComputeArguments(mStartVec,*endsIterator);
    lastEndVect = *endsIterator;
    endsIterator++;
    mPlannerThread->start();
}

void Widget::on_pushButton_importModel_clicked()
{
    QString modelFileName = QFileDialog::getOpenFileName(this,"choose file","",tr("Support Type(*.stp *.step *.STP *.STEP *.iges *.igs *.IGES *.IGS *.brep *.brp\n)"
                                                                                  "*.stp *.step *.STP *.STEP\n"
                                                                                  "*.iges *.igs *.IGES *.IGS\n"
                                                                                  "*.brep *.brp"));
    if(modelFileName.isEmpty())
        return;

    aConvertAPI->ImportSceneModel(modelFileName);
    aMdlWidget->getContext()->Erase(aConvertAPI->GetMeasureModelShape(),Standard_False);
    aConvertAPI->ResetSceneModel();

    mPlannerThread->deleteLater();
    mPlannerThread = new RLAPI_PlanThread(*aConvertAPI->GetMdlDynamic(),*aConvertAPI->GetSolidScene(),aConvertAPI->GetModelMinSize());
    connectThread();

    displayJointPosition();
    displayOperationalPosition();

    aMdlWidget->getContext()->Display(aConvertAPI->GetMeasureModelShape(),Standard_False);
    aMdlWidget->getView()->FitAll();
}

void Widget::on_pushButton_exitThread_clicked()
{
    if(mPlannerThread->isRunning())
    {
        mPlannerThread->requestInterruption();
    }
    mPlannerThread->quit();
    mPlannerThread->wait();
}

void Widget::disconnectOperationalDoubleSpinBoxes()
{
    ui->doubleSpinBox_posX->disconnect();
    ui->doubleSpinBox_posY->disconnect();
    ui->doubleSpinBox_posZ->disconnect();
    ui->doubleSpinBox_posA->disconnect();
    ui->doubleSpinBox_posB->disconnect();
    ui->doubleSpinBox_posC->disconnect();
}

void Widget::connectOperationalDoubleSpinBoxes()
{
    connect(ui->doubleSpinBox_posA,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&Widget::on_doubleSpinBox_posA_valueChanged);
    connect(ui->doubleSpinBox_posB,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&Widget::on_doubleSpinBox_posB_valueChanged);
    connect(ui->doubleSpinBox_posC,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&Widget::on_doubleSpinBox_posC_valueChanged);
    connect(ui->doubleSpinBox_posX,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&Widget::on_doubleSpinBox_posX_valueChanged);
    connect(ui->doubleSpinBox_posY,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&Widget::on_doubleSpinBox_posY_valueChanged);
    connect(ui->doubleSpinBox_posZ,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&Widget::on_doubleSpinBox_posZ_valueChanged);
}

void Widget::disconnectConfigDoubleSpinBoxes()
{
    ui->doubleSpinBox_joint0->disconnect();
    ui->doubleSpinBox_joint1->disconnect();
    ui->doubleSpinBox_joint2->disconnect();
    ui->doubleSpinBox_joint3->disconnect();
    ui->doubleSpinBox_joint4->disconnect();
    ui->doubleSpinBox_joint5->disconnect();
    ui->doubleSpinBox_joint6->disconnect();
    ui->doubleSpinBox_joint7->disconnect();
}

void Widget::connectConfigDoubleSpinBoxes()
{
    connect(ui->doubleSpinBox_joint0,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&Widget::on_doubleSpinBox_joint0_valueChanged);
    connect(ui->doubleSpinBox_joint1,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&Widget::on_doubleSpinBox_joint1_valueChanged);
    connect(ui->doubleSpinBox_joint2,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&Widget::on_doubleSpinBox_joint2_valueChanged);
    connect(ui->doubleSpinBox_joint3,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&Widget::on_doubleSpinBox_joint3_valueChanged);
    connect(ui->doubleSpinBox_joint4,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&Widget::on_doubleSpinBox_joint4_valueChanged);
    connect(ui->doubleSpinBox_joint5,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&Widget::on_doubleSpinBox_joint5_valueChanged);
    connect(ui->doubleSpinBox_joint6,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&Widget::on_doubleSpinBox_joint6_valueChanged);
    connect(ui->doubleSpinBox_joint7,QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,&Widget::on_doubleSpinBox_joint7_valueChanged);
}
