#include "RLAPI_Reader.h"

//=======================================================================
//function : RLAPI_Reader
//purpose  : *
//=======================================================================
RLAPI_Reader::RLAPI_Reader()
{
    CollisionScene = std::make_shared<rl::sg::pqp::Scene>();
}

//=======================================================================
//function : RLAPI_Reader
//purpose  : *
//=======================================================================
RLAPI_Reader::~RLAPI_Reader()
{    
    delete SceneModel;
    SceneModel = nullptr;
    JointModel.reset();
    CollisionScene.reset();//if reset before deleting the SceneModel ptr, the SceneModel will be the hanging ptr,danger!

    JointAISShapes.clear();
    AssembleTrsfs.clear();
    MotionCenters.clear();
    MotionAxis.clear();
    JointType.clear();
}

//=======================================================================
//function : ReadSceneXMLFile
//purpose  : *
//=======================================================================
void RLAPI_Reader::ReadSceneXMLFile(const QString &aSgFileName)
{
    rl::sg::XmlFactory aFactory;
    if(CollisionScene.get()==nullptr)
        CollisionScene = std::make_shared<rl::sg::pqp::Scene>();

    aFactory.load(aSgFileName.toLocal8Bit().toStdString(),CollisionScene.get());
    SceneModel = CollisionScene->getModel(0);
}

//=======================================================================
//function : ReadModelXMLFIle
//purpose  : *
//=======================================================================
void RLAPI_Reader::ReadModelXMLFIle(const QString &aMdlFileName)
{
    rl::mdl::XmlFactory factory;
    JointModel = factory.create(aMdlFileName.toLocal8Bit().toStdString());
}

//=======================================================================
//function : ReadJointModels
//purpose  : *
//=======================================================================
void RLAPI_Reader::ReadJointModels(const QString &aFilePath)
{
    QDir jointFileDir(aFilePath);
    QStringList theDirFilter;
    theDirFilter<<"*.brep"<<"*.brp";
    QFileInfoList aInfoList = jointFileDir.entryInfoList(theDirFilter,QDir::Files);
    for(int i=0;i<aInfoList.size();++i)
    {
        QString aFileName = jointFileDir.absolutePath()+"/"+aInfoList.at(i).fileName();
        Handle(AIS_Shape) anAIS = ReadAModelFile(aFileName);
        anAIS->SetColor(Quantity_Color(0.4, 0.4, 0.4,Quantity_TOC_RGB));
        JointAISShapes.append(anAIS);
    }
}

//=======================================================================
//function : ReadAModelFile
//purpose  : *
//=======================================================================
Handle(AIS_Shape) RLAPI_Reader::ReadAModelFile(const QString &aModelFileName)
{
    QFileInfo info(aModelFileName);
    std::shared_ptr<XSControl_Reader> aReader;
    if(info.suffix()=="step"||info.suffix()=="stp"||info.suffix()=="STEP"||info.suffix()=="STP")
    {
        aReader = std::make_shared<STEPControl_Reader>();
    }
    else if(info.suffix()=="iges"||info.suffix()=="igs"||info.suffix()=="IGES"||info.suffix()=="IGS")
    {
        aReader = std::make_shared<IGESControl_Reader>();
    }
    else if(info.suffix()=="brep"||info.suffix()=="brp")
    {
        TopoDS_Shape aShape;
        BRep_Builder aBuilder;
        BRepTools::Read(aShape,Standard_CString(aModelFileName.toLocal8Bit()),aBuilder);
        return new AIS_Shape(aShape);
    }
    if(!aReader->ReadFile(Standard_CString(aModelFileName.toLocal8Bit())))
        return nullptr;

    aReader->TransferRoots();
    return new AIS_Shape(aReader->OneShape());
}

//=======================================================================
//function : ReadAssembleArgs
//purpose  : *
//=======================================================================
void RLAPI_Reader::ReadAssembleArgs(const QString &aSgFileName)
{
    rl::xml::DomParser aParser;
    rl::xml::Document aDocument = aParser.readFile(aSgFileName.toLocal8Bit().toStdString(),"",
                                                   XML_PARSE_NOENT | XML_PARSE_XINCLUDE);
    aDocument.substitute(XML_PARSE_NOENT | XML_PARSE_XINCLUDE);
    rl::xml::Path aPath(aDocument);
    rl::xml::NodeSet sceneNodes = aPath.eval("(/rl/sg|/rlsg)/scene").getValue<rl::xml::NodeSet>();

    SoInput aInput;
    aInput.openFile(sceneNodes.at(0).getLocalPath(sceneNodes.at(0).getProperty("href")).c_str());
    SceneVrmlFileName = "./"+QString(sceneNodes.at(0).getLocalPath(sceneNodes.at(0).getProperty("href")).c_str());

    ReadSceneModels(SceneVrmlFileName);
}

//=======================================================================
//function : ReadSceneModels
//purpose  : *
//=======================================================================
void RLAPI_Reader::ReadSceneModels(const QString &SceneVrmlFile)
{
    QFile theFile(SceneVrmlFile);

    QFileInfo aInfo(theFile);
    QString tmpPath = aInfo.filePath().remove(aInfo.fileName());//sub model's url always based on the total vrml path

    QTextStream aStream(&theFile);
    QStringList theUrlFileNameList;
    theFile.open(QIODevice::ReadOnly);
    while(!aStream.atEnd())
    {
        QString aLine = aStream.readLine();
        if(aLine.contains("url"))
        {
            aLine.remove('\t');
            aLine.remove("url");
            aLine.remove("\"");
            aLine.remove(" ");
            theUrlFileNameList.append(tmpPath+aLine);
        }
        else
            continue;
    }
    theFile.close();

    ParseJointAssemble(theUrlFileNameList.at(0));

    QStringList theDirFilter;
    theDirFilter<<"*.step"<<"*.STEP"<<"*.stp"<<"*.STP"<<"*.iges"<<"*.IGES"<<"*.igs"<<"*.IGS"<<"*.brep"<<"*.brp";

    //the first one is joint in default ,so begain with 1
    for(int i=1;i<theUrlFileNameList.size();++i)
    {
        QString aFileName = theUrlFileNameList.at(i);
        ParseSceneModelTrsf(aFileName);

        aInfo.setFile(aFileName);
        QDir modelFileDir(aInfo.filePath().remove(aInfo.fileName()));
        QFileInfoList aInfoList = modelFileDir.entryInfoList(theDirFilter,QDir::Files);
        double theXmax=0,theXmin=0,theYmax=0,theYmin=0,theZmax=0,theZmin=0;
        for(int j=0;j<aInfoList.size();++j)
        {
            if(aInfoList.at(j).baseName()!=aInfo.baseName())//get the step/iges/brep file that has the same name
                continue;

            QString tmpFileName = modelFileDir.absolutePath()+"/"+aInfoList.at(j).fileName();
            MeasureAISShape = ReadAModelFile(tmpFileName);
            MeasureAISShape->SetColor(Quantity_Color(0.596,0.759,0.778,Quantity_TOC_RGB));
            MeasureAISShape->BoundingBox().Get(theXmin,theYmin,theZmin,theXmax,theYmax,theZmax);
            double theMdlSize = ((theXmax-theXmin)+(theYmax-theYmin)+(theZmax-theZmin))/3;
            MeasureModelSize = theMdlSize;
            break;
        }
    }
}

//=======================================================================
//function : ParseJointAssemble
//purpose  : *
//=======================================================================
void RLAPI_Reader::ParseJointAssemble(const QString &JointVrmlFile)
{
    QFile aFile(JointVrmlFile);
    aFile.open(QIODevice::ReadOnly);
    QTextStream aStream(&aFile);
    std::size_t translationIndex = 0;
    QList<double> transArgs;
    QRegExp aExp("(-?\\d\\d*\\.\\d+)|(-?\\d+)");
    while(!aStream.atEnd())
    {
        QString aLine = aStream.readLine();
        if(!aLine.contains("translation"))
            continue;

        int pos = 0;

        while ((pos = aExp.indexIn(aLine, pos)) != -1) {
            transArgs << aExp.cap(0).toDouble();
            pos += aExp.matchedLength();
        }
        translationIndex++;
    }
    aFile.close();
    if(translationIndex!=JointModel->getDof())
        Standard_Failure::Raise("Read joints's assemble arguments wrong!");

    gp_Trsf aJointTrsf;
    gp_Pnt aCenterPnt;
    for(std::size_t i=0;i<translationIndex;++i)
    {
        aCenterPnt.SetCoord(transArgs.at(3*i),transArgs.at(3*i+1),transArgs.at(3*i+2));
        aJointTrsf.SetValues(1,0,0,transArgs.at(3*i),
                             0,1,0,transArgs.at(3*i+1),
                             0,0,1,transArgs.at(3*i+2));
        AssembleTrsfs.append(aJointTrsf);
        MotionCenters.append(aCenterPnt);
    }
}

//=======================================================================
//function : ParseSceneModelTrsf
//purpose  : *
//=======================================================================
void RLAPI_Reader::ParseSceneModelTrsf(const QString &aSceneModelFile)
{
    QFile aFile(aSceneModelFile);
    aFile.open(QIODevice::ReadOnly);
    QTextStream aStream(&aFile);
    QList<double> transArgs;
    QRegExp aExp("(-?\\d\\d*\\.\\d+)|(-?\\d+)");
    while(!aStream.atEnd())
    {
        QString aLine = aStream.readLine();
        if(!aLine.contains("translation"))
            continue;

        int pos = 0;

        while ((pos = aExp.indexIn(aLine, pos)) != -1) {
            transArgs << aExp.cap(0).toDouble();
            pos += aExp.matchedLength();
        }
        break;
    }
    aFile.close();
    gp_Trsf aModelTrsf;
    aModelTrsf.SetValues(1,0,0,transArgs.at(0),
                         0,1,0,transArgs.at(1),
                         0,0,1,transArgs.at(2));
    MeasureModelTrsf = aModelTrsf;
}

//=======================================================================
//function : ReadAixsDirection
//purpose  : *
//=======================================================================
void RLAPI_Reader::ReadAixsDirection(const QString &aMdlFileName)
{
    rl::xml::DomParser aParser;
    rl::xml::Document aDocument = aParser.readFile(aMdlFileName.toLocal8Bit().toStdString(),"",
                                                   XML_PARSE_NOENT | XML_PARSE_XINCLUDE);
    aDocument.substitute(XML_PARSE_NOENT | XML_PARSE_XINCLUDE);

    rl::xml::Path aPath(aDocument);
    rl::xml::NodeSet aNodeSet = aPath.eval("(/rl/mdl|/rlmdl)/model").getValue<rl::xml::NodeSet>();
    for(int i=0;i<aNodeSet.size();++i)
    {
        rl::xml::Path subPath(aDocument,aNodeSet.at(i));
        rl::xml::NodeSet transforms = subPath.eval("prismatic|revolute")
                .getValue<rl::xml::NodeSet>();

        for(int k=0;k<transforms.size();++k)
        {
            rl::xml::Path tmpPath(aDocument,transforms.at(k));

            if(transforms.at(k).getName()=="prismatic")
                JointType.append(RLAPI_JointType::Prismatic);
            else if(transforms.at(k).getName()=="revolute")
                JointType.append(RLAPI_JointType::Revolute);

            double axisx = tmpPath.eval("number(axis/x)").getValue<rl::math::Real>(0);
            double axisy = tmpPath.eval("number(axis/y)").getValue<rl::math::Real>(0);
            double axisz = tmpPath.eval("number(axis/z)").getValue<rl::math::Real>(1);
            MotionAxis.append(gp_Ax1(MotionCenters.at(k),
                                     gp_Dir(axisx,axisy,axisz)));
        }
    }
}
