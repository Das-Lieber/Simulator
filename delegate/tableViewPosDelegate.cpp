#include "tableViewPosDelegate.h"

tableViewPosDelegate::tableViewPosDelegate(QObject* parent) :
	QItemDelegate(parent)
{
}

tableViewPosDelegate::~tableViewPosDelegate()
{
}

QWidget* tableViewPosDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option)
	QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
	
	editor->setMaximum(std::numeric_limits<double>::max());
	editor->setMinimum(-std::numeric_limits<double>::max());
	
	switch (index.column())
	{
	case 0:
	case 1:
	case 2:
        editor->setDecimals(3);
        editor->setSingleStep(1.0);
		break;
	case 3:
	case 4:
	case 5:
        editor->setDecimals(4);
		editor->setSingleStep(1.0);
		break;
	default:
		break;
	}
	
	QObject::connect(editor, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
	
	return editor;
}

void tableViewPosDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	QDoubleSpinBox* doubleSpinBox = static_cast<QDoubleSpinBox*>(editor);
	doubleSpinBox->setValue(index.model()->data(index, Qt::EditRole).toDouble());
}

void tableViewPosDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	QDoubleSpinBox* doubleSpinBox = static_cast<QDoubleSpinBox*>(editor);
	doubleSpinBox->interpretText();
	
	if (!model->setData(index, doubleSpinBox->value(), Qt::EditRole))
	{
		doubleSpinBox->setValue(index.model()->data(index, Qt::EditRole).toDouble());
	}
}

void tableViewPosDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index)
	editor->setGeometry(option.rect);
}

void tableViewPosDelegate::valueChanged(double d)
{
    Q_UNUSED(d)
	emit commitData(static_cast<QWidget*>(QObject::sender()));
}
