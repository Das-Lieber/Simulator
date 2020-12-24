#ifndef RLAPI_WRITER_H
#define RLAPI_WRITER_H

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include <Poly_Triangulation.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <BRepMesh_IncrementalMesh.hxx>

class RLAPI_Writer
{
public:
    //! default constructor
    RLAPI_Writer();

    //! triangulae process of theShape ,return a list of triangulations
    //! with this function to change the step/iges/brep format file into
    //! vrml format that can be used in the RL library, from the source
    //! code of OCC
    Handle(Poly_Triangulation) BuildMesh(const TopoDS_Shape &theShape);

    //! write the mesh after triangulating
    void WriteMeshToVrml(const Poly_Triangulation &aMesh,const QString &aVrmlFileName);

    //! rewrite the scene vrml file
    void GenerateSceneVrmlFile(const QString &aVrmlFileName, const QString &aSgVrmlFileName);

    //! rewrite the scene xml file
    void GenerateSceneXMLFile(const QString &aSgVrmlFileName, const QString &aMdlFileName);
};

#endif // RLAPI_WRITER_H
