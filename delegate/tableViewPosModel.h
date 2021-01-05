#ifndef OPERATIONALMODEL_H
#define OPERATIONALMODEL_H

#include <QAbstractTableModel>
#include <QStatusBar>

#include <rl/math/Constants.h>

#include "tableViewJointModel.h"

class tableViewPosModel : public QAbstractTableModel
{
	Q_OBJECT
	
public:
    tableViewPosModel(QObject* parent = nullptr);
    virtual ~tableViewPosModel();

    void initData(QList<double> Pos);
		
public slots:
    void updateModel();
	
protected:
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	
private:
    QList<double> OperationPos;

signals:
    void changePositionAndValue(const int& index,const double& value);
};

#endif // OPERATIONALMODEL_H
