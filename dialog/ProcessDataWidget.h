#ifndef PROCESSDATAWIDGET_H
#define PROCESSDATAWIDGET_H

#include <QWidget>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlRecord>
#include <QModelIndexList>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>

#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <BRep_Builder.hxx>

#include "dialog/SqlTableViewDelegate.h"

struct TcpData
{
    gp_Pnt tcpPos;
    double VX;
    double VY;
    double VZ;
};

namespace Ui {
class ProcessDataWidget;
}

class ProcessDataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProcessDataWidget(QWidget *parent = nullptr);
    ~ProcessDataWidget();

    QList<TcpData> getProcessPnts();
    TopoDS_Shape getShape();

private slots:
    void on_pushButton_addProcessItem_clicked();
    void on_pushButton_importPTDFile_clicked();
    void on_pushButton_deleteOneRecord_clicked();
    void on_pushButton_ensureOperate_clicked();
    void on_pushButton_cancelOperate_clicked();

private:
    Ui::ProcessDataWidget *ui;

    QSqlDatabase mProcessData;
    QSqlTableModel *mSqlTable;

    void creatDataBase();
};

#endif // PROCESSDATAWIDGET_H
