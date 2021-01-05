#ifndef CONFIGURATIONDELEGATE_H
#define CONFIGURATIONDELEGATE_H

#include <QItemDelegate>
#include <QDoubleSpinBox>
#include <QModelIndex>

#include <rl/math/Constants.h>

#include "robotics/RLAPI_Reader.h"

class tableViewJointDelegate : public QItemDelegate
{
	Q_OBJECT
	
public:
    tableViewJointDelegate(QObject* parent = nullptr);
    virtual ~tableViewJointDelegate();

    void setMaxValue(const rl::math::Vector &max) {
        Maxs = max;
    }
    void setMinValue(const rl::math::Vector &min) {
        Mins = min;
    }
    void setJointType(const QList<RLAPI_JointType> &type) {
        Types = type;
    }
	
protected:
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;	
	void setEditorData(QWidget* editor, const QModelIndex& index) const;	
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;	
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;	
	
public slots:
    void valueChanged(double d);

private:
    rl::math::Vector Maxs;
    rl::math::Vector Mins;
    QList<RLAPI_JointType> Types;

};

#endif // CONFIGURATIONDELEGATE_H
