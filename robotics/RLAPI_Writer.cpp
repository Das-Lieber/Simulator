#include "RLAPI_Writer.h"
#include <QDebug>

//=======================================================================
//function : RLAPI_Writer
//purpose  : *
//=======================================================================
RLAPI_Writer::RLAPI_Writer()
{
}

//=======================================================================
//function : BuildMesh
//purpose  : *
//=======================================================================
Handle(Poly_Triangulation) RLAPI_Writer::BuildMesh(const TopoDS_Shape &theShape)
{
    Standard_Integer aNbNodes = 0;
    Standard_Integer aNbTriangles = 0;
    TopExp_Explorer anExpSF(theShape,TopAbs_FACE);

    // calculate total number of the nodes and triangles
    for(;anExpSF.More();anExpSF.Next())
    {
        TopLoc_Location loc;

        TopoDS_Face aFace = TopoDS::Face(anExpSF.Current());
        BRepMesh_IncrementalMesh(aFace,0.1);
        Handle_Poly_Triangulation triFace = BRep_Tool::Triangulation(aFace, loc);

        if (! triFace.IsNull())
        {
            aNbNodes += triFace->NbNodes ();
            aNbTriangles += triFace->NbTriangles ();
        }
    }

    // create temporary triangulation
    Handle(Poly_Triangulation) aMesh = new Poly_Triangulation (aNbNodes, aNbTriangles, Standard_False);

    // fill temporary triangulation
    Standard_Integer aNodeOffset = 0;
    Standard_Integer aTriangleOffet = 0;
    for (TopExp_Explorer anExpSF(theShape, TopAbs_FACE);anExpSF.More();anExpSF.Next())
    {
        TopLoc_Location aLoc;
        Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation (TopoDS::Face (anExpSF.Current()), aLoc);

        const TColgp_Array1OfPnt& aNodes = aTriangulation->Nodes();
        const Poly_Array1OfTriangle& aTriangles = aTriangulation->Triangles();

        // copy nodes
        gp_Trsf aTrsf = aLoc.Transformation();
        for (Standard_Integer aNodeIter = aNodes.Lower(); aNodeIter <= aNodes.Upper(); ++aNodeIter)
        {
            gp_Pnt aPnt = aNodes (aNodeIter);
            aPnt.Transform (aTrsf);
            aMesh->ChangeNode (aNodeIter + aNodeOffset) = aPnt;
        }

        // copy triangles
        const TopAbs_Orientation anOrientation = anExpSF.Current().Orientation();
        for (Standard_Integer aTriIter = aTriangles.Lower(); aTriIter <= aTriangles.Upper(); ++aTriIter)
        {
            Poly_Triangle aTri = aTriangles (aTriIter);

            Standard_Integer anId[3];
            aTri.Get (anId[0], anId[1], anId[2]);
            if (anOrientation == TopAbs_REVERSED)
            {
                // Swap 1, 2.
                Standard_Integer aTmpIdx = anId[1];
                anId[1] = anId[2];
                anId[2] = aTmpIdx;
            }

            // Update nodes according to the offset.
            anId[0] += aNodeOffset;
            anId[1] += aNodeOffset;
            anId[2] += aNodeOffset;

            aTri.Set (anId[0], anId[1], anId[2]);
            aMesh->ChangeTriangle (aTriIter + aTriangleOffet) =  aTri;
        }

        aNodeOffset += aNodes.Size();
        aTriangleOffet += aTriangles.Size();
    }
    return aMesh;
}

//=======================================================================
//function : WriteMeshToVrml
//purpose  : *
//=======================================================================
void RLAPI_Writer::WriteMeshToVrml(const Poly_Triangulation &aMesh, const QString &aVrmlFileName)
{
    QFile theVrmlFile(aVrmlFileName);
    QTextStream theStream(&theVrmlFile);
    theVrmlFile.open(QIODevice::WriteOnly);
    theStream<<"#VRML V2.0 utf8 \n"
               "DEF DMISModel Transform {\n"
               "             translation 0 0 0 \n"
               "             rotation 0 0 1  0 \n"
               "             scale 1 1 1 \n"
               "             scaleOrientation 0 0 1  0 \n"
               "             center 0 0 0 \n"
               "             children  \n"
               "                Switch { \n"
               "                  whichChoice 0 \n"
               "                  choice [ \n"
               "                    Shape { \n"
               "                      appearance \n"
               "                      Appearance { \n"
               "                        material \n"
               "                        Material { \n"
               "                            diffuseColor 0.5 0.5 0.5 \n"
               "                        } \n"
               "                      } \n"
               "                      geometry  \n"
               "                      DEF SoFCIndexedFaceSet IndexedFaceSet { \n"
               "                        coord \n"
               "                        Coordinate { \n"
               "                          point [ \n";

    const TColgp_Array1OfPnt& aNodes = aMesh.Nodes();
    const Poly_Array1OfTriangle& aTriangles = aMesh.Triangles();
    const Standard_Integer NBTriangles = aMesh.NbTriangles();
    Standard_Integer anElem[3] = {0, 0, 0};
    for (Standard_Integer aTriIter = 1; aTriIter <= NBTriangles; ++aTriIter)
    {
        const Poly_Triangle& aTriangle = aTriangles (aTriIter);
        aTriangle.Get (anElem[0], anElem[1], anElem[2]);

        const gp_Pnt aP1 = aNodes (anElem[0]);
        const gp_Pnt aP2 = aNodes (anElem[1]);
        const gp_Pnt aP3 = aNodes (anElem[2]);
        theStream<<"                              "<<aP1.X()<<" "<<aP1.Y()<<" "<<aP1.Z()<<",\n";
        theStream<<"                              "<<aP2.X()<<" "<<aP2.Y()<<" "<<aP2.Z()<<",\n";
        theStream<<"                              "<<aP3.X()<<" "<<aP3.Y()<<" "<<aP3.Z()<<",\n";
    }

    theStream<<"                              ]\n"
               "                        }\n"
               "                        coordIndex [\n";

    for (Standard_Integer aTriIter = 1; aTriIter <= NBTriangles; ++aTriIter)
    {
        theStream<<"                              "<<3*(aTriIter-1)<<", "<<3*(aTriIter-1)+1<<", "<<3*(aTriIter-1)+2<<", "<<-1<<", \n";
    }

    theStream<<"                              ]\n"
               "                        ccw TRUE \n"
               "                        solid FALSE \n"
               "                        convex TRUE \n"
               "                        creaseAngle 0 \n"
               "                       } \n"
               "                 }] \n"
               "             } \n"
               "         } \n";

    theVrmlFile.close();
}

//=======================================================================
//function : GenerateSceneVrmlFile
//purpose  : *
//=======================================================================
void RLAPI_Writer::GenerateSceneVrmlFile(const QString &aVrmlFileName, const QString &aSgVrmlFileName)
{
    QFile aFile(aSgVrmlFileName);
    QFileInfo aInfo(aVrmlFileName);

    aFile.open(QIODevice::ReadOnly);
    QTextStream aStream(&aFile);
    QString vrmlContent;
    while(!aStream.atEnd())
    {
        QString aLine = aStream.readLine();

        //replace the model name to the aVrmlFileName
        if(aLine.contains("url")&&aLine.contains("models"))
        {
            aLine = QString("          url \"models/%1\" \n").arg(aInfo.fileName());
            vrmlContent.append(aLine);
        }
        else
        {
            vrmlContent.append(aLine+"\n");
            continue;
        }
    }
    aFile.close();
    aFile.open(QIODevice::WriteOnly);
    aFile.write(vrmlContent.toLocal8Bit());
    aFile.close();
}

//=======================================================================
//function : GenerateSceneXMLFile
//purpose  : *
//=======================================================================
void RLAPI_Writer::GenerateSceneXMLFile(const QString &aSgVrmlFileName, const QString &aMdlFileName, const std::size_t Dof)
{
    QFileInfo aInfo(aSgVrmlFileName);
    QString xmlContent = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                     "<rlsg xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"rlsg.xsd\">\n"
                                     "    <scene href=\"%1\">\n"
                                     "        <model name=\"Robot\">\n").arg(aInfo.fileName());

    for(std::size_t i=0;i<Dof+1;++i)
    {
        QString tmp = QString("            <body name=\"link%1\"/>\n").arg(i);
        xmlContent.append(tmp);
    }

    xmlContent.append(QString("        </model>\n"
                              "        <model name=\"DMISModel\">\n"
                              "            <body name=\"\"/>\n"
                              "        </model>\n"
                              "    </scene>\n"
                              "</rlsg>\n"));

    QFile out(aMdlFileName);
    out.open(QIODevice::WriteOnly);
    out.write(xmlContent.toLocal8Bit());
    out.close();
}

//=======================================================================
//function : GenerateSceneXMLFile
//purpose  : *
//=======================================================================
void RLAPI_Writer::WriteArgsToMdlXMLFile(const QString &aMdlXMLFileName, const QList<double> &mdlArgs)
{
}
