#ifndef OPERATIONALDELEGATE_H
#define OPERATIONALDELEGATE_H

#include <QItemDelegate>
#include <QDoubleSpinBox>
#include <QModelIndex>

class OperationalDelegate : public QItemDelegate
{
	Q_OBJECT
	
public:
	OperationalDelegate(QObject* parent = nullptr);	
	virtual ~OperationalDelegate();	
	
public slots:
	void valueChanged(double d);

protected:
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;

};

#endif // OPERATIONALDELEGATE_H
