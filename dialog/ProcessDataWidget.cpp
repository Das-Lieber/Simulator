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

QList<gp_Pnt> ProcessDataWidget::getProcessPnts()
{
    QList<gp_Pnt> tcpPnts;
    for(int i=0;i<mSqlTable->rowCount();++i)
    {
        gp_Pnt aPnt = gp_Pnt(mSqlTable->data(mSqlTable->index(i,1)).toDouble(),
                             mSqlTable->data(mSqlTable->index(i,2)).toDouble(),
                             mSqlTable->data(mSqlTable->index(i,3)).toDouble());
        tcpPnts.append(aPnt);
        gp_Pnt bPnt = gp_Pnt(mSqlTable->data(mSqlTable->index(i,4)).toDouble(),
                             mSqlTable->data(mSqlTable->index(i,5)).toDouble(),
                             mSqlTable->data(mSqlTable->index(i,6)).toDouble());
        tcpPnts.append(bPnt);
    }
    return tcpPnts;
}

QList<double> ProcessDataWidget::getProcessVecs()
{
    QList<double> tcpVecs;
    for(int i=0;i<mSqlTable->rowCount();++i)
    {
        double XZ = mSqlTable->data(mSqlTable->index(i,21)).toDouble();
        tcpVecs.append(XZ-M_PI);
        double YZ = mSqlTable->data(mSqlTable->index(i,20)).toDouble();
        tcpVecs.append(YZ-M_PI*1.25);
        double ZZ = std::atan(sqrt(tan(YZ)*tan(YZ)+tan(XZ)*tan(XZ)));
        tcpVecs.append(ZZ);
    }
    return tcpVecs;
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
