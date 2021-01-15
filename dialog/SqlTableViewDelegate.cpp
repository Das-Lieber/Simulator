#include "SqlTableViewDelegate.h"

SqlTableViewDelegate::SqlTableViewDelegate(QObject *parent) : QItemDelegate(parent)
{

}

QWidget *SqlTableViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    QDoubleSpinBox* editor = new QDoubleSpinBox(parent);

    editor->setDecimals(5);
    editor->setMinimum(-std::numeric_limits<double>::max());
    editor->setMaximum(std::numeric_limits<double>::max());
    editor->setSingleStep(1.0);

    QObject::connect(editor, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));

    return editor;
}

void SqlTableViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QDoubleSpinBox* doubleSpinBox = static_cast<QDoubleSpinBox*>(editor);
    doubleSpinBox->setValue(index.model()->data(index, Qt::EditRole).toDouble());
}

void SqlTableViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDoubleSpinBox* doubleSpinBox = static_cast<QDoubleSpinBox*>(editor);
    doubleSpinBox->interpretText();

    if (!model->setData(index, doubleSpinBox->value(), Qt::EditRole))
    {
        doubleSpinBox->setValue(index.model()->data(index, Qt::EditRole).toDouble());
    }
}

void SqlTableViewDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}

void SqlTableViewDelegate::valueChanged(double d)
{
    Q_UNUSED(d)
    emit commitData(static_cast<QWidget*>(QObject::sender()));
}
