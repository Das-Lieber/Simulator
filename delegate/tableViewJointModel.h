#ifndef CONFIGURATIONMODEL_H
#define CONFIGURATIONMODEL_H

#include <QAbstractTableModel>
#include <QBrush>

#include <rl/math/Vector.h>
#include <rl/math/Constants.h>

#include "robotics/RLAPI_Reader.h"

class tableViewJointModel : public QAbstractTableModel
{
	Q_OBJECT
	
public:
    tableViewJointModel(QObject* parent = nullptr);
    virtual ~tableViewJointModel();
	
    void SetMaxValue(const rl::math::Vector &max) {
        Maxs = max;
    }
    void SetMinValue(const rl::math::Vector &min) {
        Mins = min;
    }
    void SetJointType(const QList<RLAPI_JointType> &type) {
        Types = type;
    }
    void SetJointModelDofs(const std::size_t &dof) {
        Dofs = dof;
    }
    void initData(const rl::math::Vector &homePos);

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
    rl::math::Vector Maxs;
    rl::math::Vector Mins;
    QList<RLAPI_JointType> Types;
    std::size_t Dofs;
    rl::math::Vector Pos;

signals:
    void changePositionAndValue(const int& index,const double& value);

};

#endif // CONFIGURATIONMODEL_H
