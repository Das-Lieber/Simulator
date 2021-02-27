#include "ProcessDataWidget.h"
#include "ui_ProcessDataWidget.h"
#include <QDebug>

ProcessDataWidget::ProcessDataWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProcessDataWidget)
{
    ui->setupUi(this);
    creatDataBase();
}

ProcessDataWidget::~ProcessDataWidget()
{
    if(mProcessData.isOpen())
        mProcessData.close();
    delete ui;
}

QList<TcpData> ProcessDataWidget::getProcessPnts()
{
    QList<TcpData> tcpPnts;
    TcpData aData;
    TcpData bData;
    double vx=0,vy=0,vz=0;
    if(mSqlTable->rowCount() == 0)
        return tcpPnts;

    for(int i=0;i<mSqlTable->rowCount();++i)
    {
        gp_Pnt aPnt = gp_Pnt(mSqlTable->data(mSqlTable->index(i,1)).toDouble(),
                             mSqlTable->data(mSqlTable->index(i,2)).toDouble(),
                             mSqlTable->data(mSqlTable->index(i,3)).toDouble());
        aData.tcpPos=aPnt;
        gp_Pnt bPnt = gp_Pnt(mSqlTable->data(mSqlTable->index(i,4)).toDouble(),
                             mSqlTable->data(mSqlTable->index(i,5)).toDouble(),
                             mSqlTable->data(mSqlTable->index(i,6)).toDouble());
        bData.tcpPos = bPnt;

        gp_Vec aVec(aPnt,bPnt);
        vx = aVec.Angle(gp_Vec(1,0,0))-M_PI/2;
        vy = aVec.Angle(gp_Vec(0,1,0))-M_PI/2-M_PI/4;
        vz = aVec.Angle(gp_Vec(0,0,1));
        aData.VX=vx;
        aData.VY=vy;
        aData.VZ=vz;
        bData.VX=vx;
        bData.VY=vy;
        bData.VZ=vz;

        tcpPnts.push_back(aData);
        tcpPnts.push_back(bData);
    }
    return tcpPnts;
}

TopoDS_Shape ProcessDataWidget::getShape()
{
    QList<TcpData> aList = getProcessPnts();
    if(aList.isEmpty())
        return TopoDS_Shape();

    TColgp_Array2OfPnt array(1,aList.size()/2,1,2);
    for (int k=0;k<aList.size()/2;++k) {
        array.SetValue(k+1,1,aList.at(2*k).tcpPos);
        array.SetValue(k+1,2,aList.at(2*k+1).tcpPos);
    }
    Handle(Geom_BSplineSurface) aSurf =GeomAPI_PointsToBSplineSurface(array).Surface();
    BRep_Builder aBuilder;
    TopoDS_Face aShape;
    aBuilder.MakeFace(aShape,aSurf,1e-6);
    return aShape;
}

void ProcessDataWidget::on_pushButton_addProcessItem_clicked()
{
    QSqlRecord aRecord = mProcessData.record(mProcessData.tables().at(0));
    mSqlTable->insertRecord(mSqlTable->rowCount(),aRecord);
}

void ProcessDataWidget::on_pushButton_importPTDFile_clicked()
{
    QString ptdName = QFileDialog::getOpenFileName(this,tr("select PTD File"),"","*.PTD *.ptd");
    if(ptdName.isEmpty())
        return;

    QFile ptdFile(ptdName);
    ptdFile.open(QIODevice::ReadOnly);
    QTextStream aStream(&ptdFile);
    aStream.readLine();//skip the first line
    int lineIndex = mSqlTable->rowCount();
    while(!aStream.atEnd())
    {
        mSqlTable->insertRow(lineIndex);
        QString aLine = aStream.readLine();
        QStringList record = aLine.split(',');
        for(int i=0;i<record.size();++i)
        {
            mSqlTable->setData(mSqlTable->index(lineIndex,i),record.at(i).toDouble());
        }
        lineIndex++;
    }
    ptdFile.close();
    mSqlTable->submitAll();
}

void ProcessDataWidget::on_pushButton_deleteOneRecord_clicked()
{
    QModelIndexList selectList = ui->tableView_processData->selectionModel()->selectedIndexes();
    for(int i=0;i<selectList.size();++i)
    {
        mSqlTable->removeRow(selectList.at(i).row());
    }
}

void ProcessDataWidget::on_pushButton_ensureOperate_clicked()
{
    if(!mSqlTable->submitAll())
    {
        QMessageBox::critical(this,tr("error"),mProcessData.lastError().text());
    }
}

void ProcessDataWidget::on_pushButton_cancelOperate_clicked()
{
    mSqlTable->revert();
    mSqlTable->submitAll();
}

void ProcessDataWidget::creatDataBase()
{
    mProcessData = QSqlDatabase::addDatabase("QSQLITE");
    mProcessData.setHostName("127.0.0.1");
    mProcessData.setDatabaseName("machingData.db");
    mProcessData.setUserName("local");
    mProcessData.setPassword("admin");
    if(!mProcessData.open())
    {
        QMessageBox::critical(this,tr("error"),mProcessData.lastError().text());
    }

    mSqlTable = new QSqlTableModel(this);
    SqlTableViewDelegate *aDelegate = new SqlTableViewDelegate(this);
    mSqlTable->setTable(mProcessData.tables().at(0));
    ui->tableView_processData->setModel(mSqlTable);
    ui->tableView_processData->setItemDelegate(aDelegate);
    ui->tableView_processData->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mSqlTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
    mSqlTable->select();
}
