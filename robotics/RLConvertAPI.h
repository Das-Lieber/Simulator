#ifndef RLCONVERTAPI_H
#define RLCONVERTAPI_H

#include <QObject>

#include <rl/sg/Model.h>
#include <rl/sg/Body.h>
#include <rl/math/Constants.h>
#include <rl/mdl/Kinematic.h>
#include <rl/mdl/Dynamic.h>
#include <rl/mdl/JacobianInverseKinematics.h>

#include "RLAPI_Reader.h"
#include "RLAPI_Writer.h"

class RLConvertAPI : public QObject
{
    Q_OBJECT
public:
    //! constructor
    RLConvertAPI(QString &JointMdlFile,
                 QString &JoingtSgFile,
                 QString &JointModelFile,
                 QObject *parent = nullptr);

    //! deconstructor
    ~RLConvertAPI();

    //! a convenient api to perform all the read process
    void InitLoadData();

    //! the api for import a new model to the scene
    void ImportSceneModel(const QString &theName);

    //! used to reset the RL args when import a new model to the scene
    void ResetSceneModel();

    //! convenient api to set the index link moves to the angle postion
    //! angle is "rad" unit
    void SetIndexedJointValue(const int &index, const double &angle);

    //! convenient api to set all the links move to the angle that
    //! included in the aVector
    void SetJointValue(const rl::math::Vector &aVector);

    //! convenient api to solve inverse motion, if value is an angle, use
    //! "deg" unit
    bool SetIndexedInverseValue(const int &index, const double &value);

    //! get all the links' angle/position, return by a dof-count vector
    rl::math::Vector GetJointPosition();

    //! get the tool coordinate posiotion, the list contain x,y,z,a,b,c 6 values
    //! in which a,b,c has been changed into "deg" unit
    QList<double> GetOperationalPosition();

    //! get the speed value of each joint
    QList<double> GetJointVelocity();

    //! get the acceleration value of each joint
    QList<double> GetJointAcceleration();

    //! get the dynamic value, used for motion planning
    rl::mdl::Dynamic *GetMdlDynamic(){
        rl::mdl::Dynamic *aDynamic = dynamic_cast<rl::mdl::Dynamic*>(aReaderAPI->JointModel.get());
        return aDynamic;
    };

    //! get the min size of all model, used for motion planning optimize
    double GetModelMinSize(){        
        return aReaderAPI->MeasureModelSize;
    }

    //! get the scene information,also used for motion planning
    rl::sg::pqp::Scene *GetSolidScene(){
        return aReaderAPI->CollisionScene.get();
    }

    //! get the links' shape,used for displaying
    QList<Handle(AIS_Shape)> GetJointModelShapes(){
        return aReaderAPI->JointAISShapes;
    }

    //! get the models' shape in the scene,used for displaying
    Handle(AIS_Shape) GetMeasureModelShape(){
        return aReaderAPI->MeasureAISShape;
    }

    //! get the dof of the CMM
    std::size_t GetJointModelDof(){
        return aReaderAPI->JointModel->getDof();
    }

    //! get the type of each joint
    QList<RLAPI_JointType> GetJointType(){
        return aReaderAPI->JointType;
    }

    rl::math::Vector MotionMaxValues;
    rl::math::Vector MotionMinValues;
    rl::math::Vector MotionSpeedValues;

private:
    QString mJointMdlFileName;
    QString mJointSgFileName;
    const QString mJointModelFilePath;

    QString mImportModelFileName;

    RLAPI_Reader *aReaderAPI;
    RLAPI_Writer *aWriterAPI;

    //! collision detection function, use the pqp engin
    void IsCollision();

signals:
    //! collision signal with the collision link's index
    void JointCollision(const size_t &index);

    //! safe signal, used to clear the effection of collision
    void NoCollision();

    //! link move signal, used to refresh the position in the main thread
    void JointPositionChanged();
};

#endif // RLCONVERTAPI_H
