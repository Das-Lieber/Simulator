#include "RLConvertAPI.h"

//=======================================================================
//function : RLConvertAPI
//purpose  : *
//=======================================================================
RLConvertAPI::RLConvertAPI(QString &JointMdlFile, QString &JointSgFile, QString &JointModelFile,
                           QObject *parent) : QObject(parent)
  ,mJointMdlFileName(JointMdlFile)
  ,mJointSgFileName(JointSgFile)
  ,mJointModelFilePath(JointModelFile)
{
    aWriterAPI = new RLAPI_Writer;
    aReaderAPI = new RLAPI_Reader;
}

//=======================================================================
//function : RLConvertAPI
//purpose  : *
//=======================================================================
RLConvertAPI::~RLConvertAPI()
{
    delete aReaderAPI;
    aReaderAPI = nullptr;
    delete aWriterAPI;
    aWriterAPI = nullptr;
}

//=======================================================================
//function : LoadFiles
//purpose  : *
//=======================================================================
void RLConvertAPI::InitLoadData()
{
    if(aReaderAPI==nullptr)
        aReaderAPI = new RLAPI_Reader;

    aReaderAPI->ReadModelXMLFIle(mJointMdlFileName);
    aReaderAPI->ReadSceneXMLFile(mJointSgFileName);
    aReaderAPI->ReadAssembleArgs(mJointSgFileName);
    aReaderAPI->ReadAixsDirection(mJointMdlFileName);
    aReaderAPI->ReadJointModels(mJointModelFilePath);

    SetJointValue(aReaderAPI->JointModel->getHomePosition());
    aReaderAPI->MeasureAISShape->SetLocalTransformation(aReaderAPI->MeasureModelTrsf);
    MotionMaxValues = aReaderAPI->JointModel->getMaximum();
    MotionMinValues = aReaderAPI->JointModel->getMinimum();
    MotionSpeedValues = aReaderAPI->JointModel->getSpeed();
}

//=======================================================================
//function : ImportSceneModel
//purpose  : *
//=======================================================================
void RLConvertAPI::ImportSceneModel(const QString &theName)
{
    mImportModelFileName = theName;

    QFileInfo aInfo(mImportModelFileName);
    QString Path = aInfo.filePath().remove(aInfo.fileName());
    QString theVrmlFile(Path+aInfo.baseName()+".wrl");
    aInfo.setFile(theVrmlFile);

    if(!aInfo.exists())
    {
        //1、 build the mesh from the shape
        Handle(Poly_Triangulation) aMesh = aWriterAPI->BuildMesh(aReaderAPI->ReadAModelFile(theName)->Shape());

        //2、 generate a vrml file that has the same base name
        aWriterAPI->WriteMeshToVrml(aMesh,theVrmlFile);
    }

    //3、creat the scene vrml file
    aWriterAPI->GenerateSceneVrmlFile(theVrmlFile ,aReaderAPI->SceneVrmlFileName);

    //4、creat the scene xml file
    aWriterAPI->GenerateSceneXMLFile(aReaderAPI->SceneVrmlFileName,mJointSgFileName,aReaderAPI->JointModel->getDof());
}

//=======================================================================
//function : Reset
//purpose  : *
//=======================================================================
void RLConvertAPI::ResetSceneModel()
{    
    delete aReaderAPI->SceneModel;
    aReaderAPI->SceneModel = nullptr;
    aReaderAPI->CollisionScene.reset();
    aReaderAPI->AssembleTrsfs.clear();
    aReaderAPI->MotionCenters.clear();
    aReaderAPI->MeasureAISShape.reset(nullptr);

    aReaderAPI->ReadSceneXMLFile(mJointSgFileName);
    aReaderAPI->ReadAssembleArgs(mJointSgFileName);

    SetJointValue(aReaderAPI->JointModel->getHomePosition());
    aReaderAPI->MeasureAISShape->SetLocalTransformation(aReaderAPI->MeasureModelTrsf);
}

//=======================================================================
//function : SetIndexedJointValue
//purpose  : *
//=======================================================================
void RLConvertAPI::SetIndexedJointValue(const int &index, const double &angle)
{
    rl::mdl::Kinematic* aKinematic = dynamic_cast<rl::mdl::Kinematic*>(aReaderAPI->JointModel.get());
    rl::math::Vector pos = aKinematic->getPosition();
    if(aReaderAPI->JointType.at(index)==RLAPI_JointType::Revolute)
        pos(index) = angle * rl::math::constants::deg2rad;//rad used in calculate but deg used in ui
    else if(aReaderAPI->JointType.at(index)==RLAPI_JointType::Prismatic)
        pos(index) = angle;
    else
        pos(index) = angle * rl::math::constants::deg2rad;
    SetJointValue(pos);
}

//=======================================================================
//function : SetJointValue
//purpose  : *
//=======================================================================
void RLConvertAPI::SetJointValue(const rl::math::Vector &aVector)
{
    rl::mdl::Kinematic* aKinematic = dynamic_cast<rl::mdl::Kinematic*>(aReaderAPI->JointModel.get());

    aKinematic->setPosition(aVector);
    aKinematic->forwardPosition();

    for (std::size_t i = 0; i<aReaderAPI->SceneModel->getNumBodies(); ++i)
    {
        rl::math::Transform theTrans = aKinematic->getBodyFrame(i);
        aReaderAPI->CollisionScene->getModel(0)->getBody(i)->setFrame(theTrans);
    }

    gp_Trsf CumulativeTrsf;//set an Cumulative Trsf to change the axis after the current one
    for(int i=0;i<aReaderAPI->MotionAxis.size();++i)
    {
        gp_Trsf aTrsf = aReaderAPI->JointAISShapes.at(i+1)->Shape().Location().Transformation();
        if(aReaderAPI->JointType.at(i)==RLAPI_JointType::Revolute)
            aTrsf.SetRotation(aReaderAPI->MotionAxis.at(i),aVector(i));

        else if(aReaderAPI->JointType.at(i)==RLAPI_JointType::Prismatic)
            aTrsf.SetTranslation(gp_Vec(aReaderAPI->MotionAxis.at(i).Direction())*aVector(i));

        CumulativeTrsf.Multiply(aTrsf);

        gp_Trsf workTrsf;//the real work Trsf ,E * Cumulative * Assemble
        workTrsf.Multiply(CumulativeTrsf);
        workTrsf.Multiply(aReaderAPI->AssembleTrsfs.value(i));

        //set the value JointAISShapes a public member in the class,when u change here ,u needn't to display them again in the main function, update the occWidget is enough
        aReaderAPI->JointAISShapes.at(i+1)->SetLocalTransformation(workTrsf);
    }

    //here is the signal to update the occWidget
    emit JointPositionChanged();
    IsCollision();
}

//=======================================================================
//function : SetIndexedInverseValue
//purpose  : *
//=======================================================================
bool RLConvertAPI::SetIndexedInverseValue(const int &index, const double &value)
{
    if (rl::mdl::Kinematic* kinematic = dynamic_cast<rl::mdl::Kinematic*>(aReaderAPI->JointModel.get()))
    {
        rl::math::Transform transform = kinematic->getOperationalPosition(0);
        rl::math::Vector3 orientation = transform.linear().eulerAngles(2, 1, 0).reverse();

        switch (index)
        {
        case 0:
        case 1:
        case 2:
            transform.translation()(index) = value;
            break;
        case 3:
            transform.linear() = (
                        rl::math::AngleAxis(orientation.z(), rl::math::Vector3::UnitZ()) *
                        rl::math::AngleAxis(orientation.y(), rl::math::Vector3::UnitY()) *
                        rl::math::AngleAxis(value * rl::math::constants::deg2rad, rl::math::Vector3::UnitX())
                        ).toRotationMatrix();
            break;
        case 4:
            transform.linear() = (
                        rl::math::AngleAxis(orientation.z(), rl::math::Vector3::UnitZ()) *
                        rl::math::AngleAxis(value * rl::math::constants::deg2rad, rl::math::Vector3::UnitY()) *
                        rl::math::AngleAxis(orientation.x(), rl::math::Vector3::UnitX())
                        ).toRotationMatrix();
            break;
        case 5:
            transform.linear() = (
                        rl::math::AngleAxis(value * rl::math::constants::deg2rad, rl::math::Vector3::UnitZ()) *
                        rl::math::AngleAxis(orientation.y(), rl::math::Vector3::UnitY()) *
                        rl::math::AngleAxis(orientation.x(), rl::math::Vector3::UnitX())
                        ).toRotationMatrix();
            break;
        default:
            break;
        }

        rl::math::Vector q = kinematic->getPosition();
        rl::mdl::NloptInverseKinematics* aInverse = new rl::mdl::NloptInverseKinematics(kinematic);
        aInverse->setIterations(10000);
        aInverse->setDuration(std::chrono::milliseconds(500));

        for (std::size_t i = 0; i < kinematic->getOperationalDof(); ++i)
        {
            aInverse->addGoal(i == 0 ? transform : kinematic->getOperationalPosition(i), i);
        }

        bool solved = aInverse->solve();

        if (solved)
        {
            rl::math::Vector currentPos = aReaderAPI->JointModel->getPosition();
            SetJointValue(currentPos);
            delete aInverse;
            return true;
        }
        else
        {
            kinematic->setPosition(q);
            kinematic->forwardPosition();
            delete aInverse;
            return false;
        }
    }
    return false;
}

//=======================================================================
//function : SetInverseValue
//purpose  : *
//=======================================================================
bool RLConvertAPI::SetInverseValue(const QList<double> &TCPInfo)
{
    Q_ASSERT(TCPInfo.size() == 6);

    if (rl::mdl::Kinematic* kinematic = dynamic_cast<rl::mdl::Kinematic*>(aReaderAPI->JointModel.get()))
    {
        rl::math::Transform transform = kinematic->getOperationalPosition(0);

        transform.translation()(0) = TCPInfo[0];
        transform.translation()(1) = TCPInfo[1];
        transform.translation()(2) = TCPInfo[2];
        transform.linear() = (
                    rl::math::AngleAxis(TCPInfo[5], rl::math::Vector3::UnitZ()) *
                    rl::math::AngleAxis(TCPInfo[4], rl::math::Vector3::UnitY()) *
                    rl::math::AngleAxis(TCPInfo[3], rl::math::Vector3::UnitX())
                ).toRotationMatrix();

        rl::math::Vector q = kinematic->getPosition();
        rl::mdl::NloptInverseKinematics* aInverse = new rl::mdl::NloptInverseKinematics(kinematic);
        aInverse->setIterations(10000);
        aInverse->setDuration(std::chrono::milliseconds(500));

        for (std::size_t i = 0; i < kinematic->getOperationalDof(); ++i)
        {
            aInverse->addGoal(i == 0 ? transform : kinematic->getOperationalPosition(i), i);
        }

        bool solved = aInverse->solve();

        if (solved)
        {
            rl::math::Vector currentPos = aReaderAPI->JointModel->getPosition();
            SetJointValue(currentPos);
            delete aInverse;
            return true;
        }
        else
        {
            kinematic->setPosition(q);
            kinematic->forwardPosition();
            delete aInverse;
            return false;
        }
    }
    return false;
}

//=======================================================================
//function : UnLockBasePosition
//purpose  : *
//=======================================================================
void RLConvertAPI::LockBasePosition()
{
    aReaderAPI->JointModel->getJoint(0)->max(0) = aReaderAPI->JointModel->getPosition()(0);
    aReaderAPI->JointModel->getJoint(0)->min(0) = aReaderAPI->JointModel->getPosition()(0);
    aReaderAPI->JointModel->getJoint(1)->max(0) = aReaderAPI->JointModel->getPosition()(1);
    aReaderAPI->JointModel->getJoint(1)->min(0) = aReaderAPI->JointModel->getPosition()(1);
}

//=======================================================================
//function : UnLockBasePosition
//purpose  : *
//=======================================================================
void RLConvertAPI::UnLockBasePosition()
{
    aReaderAPI->JointModel->getJoint(0)->max(0) = MotionMaxValues[0];
    aReaderAPI->JointModel->getJoint(0)->min(0) = MotionMinValues[0];
    aReaderAPI->JointModel->getJoint(1)->max(0) = MotionMaxValues[1];
    aReaderAPI->JointModel->getJoint(1)->min(0) = MotionMinValues[1];
}

//=======================================================================
//function : IsCollision
//purpose  : *
//=======================================================================
void RLConvertAPI::IsCollision()
{
    if (rl::sg::SimpleScene* simpleScene = dynamic_cast<rl::sg::SimpleScene*>(aReaderAPI->CollisionScene.get()))
    {
        bool collision = false;        
        size_t index=0;
        for(std::size_t i=1;i<aReaderAPI->CollisionScene->getModel(0)->getNumBodies();++i)//body0和参考面接触，collision恒为true
        {
            for(std::size_t j=0;j<aReaderAPI->CollisionScene->getModel(1)->getNumBodies();++j)
            {
                collision = simpleScene->areColliding(aReaderAPI->CollisionScene->getModel(0)->getBody(i),
                                                     aReaderAPI->CollisionScene->getModel(1)->getBody(j));
                if(collision)
                {
                    index = i;
                    emit JointCollision(index);
                    return;
                }
            }
        }

        //self check
        bool selfCollision = false;
        size_t aIndex=0,bIndex=0;
        for(std::size_t m=0;m<aReaderAPI->CollisionScene->getModel(0)->getNumBodies();++m)
        {
            for(std::size_t n=aReaderAPI->CollisionScene->getModel(0)->getNumBodies()-1;n>m;--n)
            {
                selfCollision = simpleScene->areColliding(aReaderAPI->CollisionScene->getModel(0)->getBody(m),
                                                     aReaderAPI->CollisionScene->getModel(0)->getBody(n));
                if(selfCollision)
                {
                    if(n-m==1)
                        continue;

                    aIndex=m;
                    bIndex=n;
                    emit SelfCollision(aIndex,bIndex);
                    return;
                }
            }
        }

        emit NoCollision();
    }
}

//=======================================================================
//function : GetJointPosition
//purpose  : *
//=======================================================================
rl::math::Vector RLConvertAPI::GetJointPosition()
{
    rl::math::Vector currentPos = aReaderAPI->JointModel->getPosition();
    return currentPos;
}

//=======================================================================
//function : GetOperationalPosition
//purpose  : *
//=======================================================================
QList<double> RLConvertAPI::GetOperationalPosition()
{
    QList<double> pos;

    rl::math::Transform Tr = aReaderAPI->JointModel->getOperationalPosition(0);
    rl::math::Vector3 OR = Tr.rotation().eulerAngles(2, 1, 0).reverse();
    pos.append(Tr.translation()(0));
    pos.append(Tr.translation()(1));
    pos.append(Tr.translation()(2));
    pos.append(OR(0)* rl::math::constants::rad2deg);
    pos.append(OR(1)* rl::math::constants::rad2deg);
    pos.append(OR(2)* rl::math::constants::rad2deg);

    return pos;
}

//=======================================================================
//function : GetJointVelocity
//purpose  : *
//=======================================================================
QList<double> RLConvertAPI::GetJointVelocity()
{
    QList<double> speed;
    rl::math::Vector s = aReaderAPI->JointModel->getVelocity();
    for (int i=0;i<s.size();++i)
    {
        speed.append(s[i]);
    }
    return speed;
}

//=======================================================================
//function : GetJointAcceleration
//purpose  : *
//=======================================================================
QList<double> RLConvertAPI::GetJointAcceleration()
{
    QList<double> Acceleration;
    rl::math::Vector a = aReaderAPI->JointModel->getAcceleration();
    for (int i=0;i<a.size();++i)
    {
        Acceleration.append(a[i]);
    }
    return Acceleration;
}
