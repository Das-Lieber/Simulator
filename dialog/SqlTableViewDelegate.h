#ifndef SQLTABLEVIEWDELEGATE_H
#define SQLTABLEVIEWDELEGATE_H

#include <QItemDelegate>
#include <QDoubleSpinBox>

class SqlTableViewDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit SqlTableViewDelegate(QObject *parent = nullptr);

protected:
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;

public slots:
    void valueChanged(double d);
};

#endif // SQLTABLEVIEWDELEGATE_H
