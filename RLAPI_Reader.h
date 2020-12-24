#ifndef RLAPI_READER_H
#define RLAPI_READER_H

#include <QObject>
#include <QDir>
#include <QFileInfoList>
#include <QTextStream>

#include <rl/sg/pqp/Scene.h>
#include <rl/sg/pqp/Model.h>
#include <rl/sg/XmlFactory.h>
#include <rl/mdl/Model.h>
#include <rl/mdl/XmlFactory.h>
#include <rl/xml/DomParser.h>
#include <rl/xml/Document.h>
#include <rl/xml/Path.h>
#include <rl/xml/NodeSet.h>

#include <STEPControl_Reader.hxx>
#include <IGESControl_Reader.hxx>
#include <AIS_Shape.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>

class RLAPI_Reader
{
public:
    //! constructor
    RLAPI_Reader();

    //! deconstructor
    ~RLAPI_Reader();

    //! import the scene xml file to RL part
    void ReadSceneXMLFile(const QString &aSgFileName);

    //! import the kinematic model to RL part
    void ReadModelXMLFIle(const QString &aMdlFileName);

    //! read the CMM links under the path
    void ReadJointModels(const QString &aFilePath);

    //! use the OCC library to read a step/iges/brep format file
    Handle(AIS_Shape) ReadAModelFile(const QString &aModelFileName);

    //! parse the vrml file name in the scene xml file,then get the
    //! assemble arguments of the links and the models in the scene
    void ReadAssembleArgs(const QString &aSgFileName);

    //! *parse the mdl xml file to get the detail args of each joint's axis
    void ReadAixsDirection(const QString &aMdlFileName);

    std::shared_ptr<rl::sg::pqp::Scene> CollisionScene;
    rl::sg::Model *SceneModel;
    std::shared_ptr<rl::mdl::Model> JointModel;

    QList<Handle(AIS_Shape)> JointAISShapes;
    QList<Handle(AIS_Shape)> MeasureAISShapes;
    QList<gp_Trsf> AssembleTrsfs;
    QList<gp_Pnt> MotionCenters;
    QList<gp_Ax1> MotionAxis;
    QList<bool> IsRevoluteType;
    QList<gp_Trsf> MeasureModelTrsfs;
    QList<double> MeasureModelSize;
    QString SceneVrmlFileName;

private:
    //! parse the scene vrml file to get the sub-models' file path
    void ReadSceneModels(const QString &SceneVrmlFile);

    //! use the regular expression to get the translation part of
    //! each joint
    void ParseJointAssemble(const QString &JointVrmlFile);

    //! get the translation part of each model in the scene
    void ParseSceneModelTrsf(const QString &aSceneModelFile);
};

#endif // RLAPI_READER_H
