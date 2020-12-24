#ifndef WIDGET_H
#define WIDGET_H

#include <QMainWindow>
#include <QThread>
#include <QMetaType>
#include <QMessageBox>
#include <QFileDialog>

#include <Geom_TrimmedCurve.hxx>
#include <GC_MakeSegment.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <AIS_ListOfInteractive.hxx>

#include "RLAPI_PlanThread.h"
#include "RLAPI_ConfigurationOptimizer.h"
#include "occWidget.h"
#include "RLConvertAPI.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();   

private slots:
    void on_doubleSpinBox_joint0_valueChanged(double arg1);
    void on_doubleSpinBox_joint1_valueChanged(double arg1);
    void on_doubleSpinBox_joint2_valueChanged(double arg1);
    void on_doubleSpinBox_joint3_valueChanged(double arg1);
    void on_doubleSpinBox_joint4_valueChanged(double arg1);
    void on_doubleSpinBox_joint5_valueChanged(double arg1);
    void on_doubleSpinBox_joint6_valueChanged(double arg1);
    void on_doubleSpinBox_joint7_valueChanged(double arg1);

    void on_doubleSpinBox_posX_valueChanged(double arg1);
    void on_doubleSpinBox_posY_valueChanged(double arg1);
    void on_doubleSpinBox_posZ_valueChanged(double arg1);
    void on_doubleSpinBox_posA_valueChanged(double arg1);
    void on_doubleSpinBox_posB_valueChanged(double arg1);
    void on_doubleSpinBox_posC_valueChanged(double arg1);

    void on_pushButton_setStartPos_clicked();
    void on_pushButton_setEndPos_clicked();
    void on_pushButton_computePath_clicked();
    void on_pushButton_importModel_clicked();
    void on_pushButton_exitThread_clicked();

private:
    Ui::Widget *ui;
    RLAPI_PlanThread *mPlannerThread;
    OCCWidget *aMdlWidget;
    RLConvertAPI *aConvertAPI;

    rl::math::Vector mStartVec;
    rl::plan::VectorList mEndList;
    rl::plan::VectorList optimizedEndList;
    rl::plan::VectorList::iterator endsIterator;
    rl::math::Vector lastEndVect;
    double totalPathLen;

    gp_Pnt currentPahtPnt;
    gp_Pnt endPnt;
    QList<Handle(AIS_Shape)> pathLines;

    void displayJointPosition();
    void displayOperationalPosition();
    void drawPathLine();
    void connectThread();

    void disconnectOperationalDoubleSpinBoxes();
    void connectOperationalDoubleSpinBoxes();
    void disconnectConfigDoubleSpinBoxes();
    void connectConfigDoubleSpinBoxes();
};
#endif // WIDGET_H
