#include "ConfigurationDelegate.h"

ConfigurationDelegate::ConfigurationDelegate(QObject* parent) :
    QItemDelegate(parent)
{
}

ConfigurationDelegate::~ConfigurationDelegate()
{
}

QWidget* ConfigurationDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option)
	QDoubleSpinBox* editor = new QDoubleSpinBox(parent);

    if (RLAPI_JointType::Revolute == Types.at(index.row()))
	{
		editor->setDecimals(2);
        editor->setMinimum(Mins(index.row()) * rl::math::constants::rad2deg);
        editor->setMaximum(Maxs(index.row()) * rl::math::constants::rad2deg);
		editor->setSingleStep(1.0);
	}
	else
	{
		editor->setDecimals(4);
        editor->setMinimum(Mins(index.row()));
        editor->setMaximum(Maxs(index.row()));
        editor->setSingleStep(3.0);
    }

	QObject::connect(editor, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
	
	return editor;
}

void ConfigurationDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	QDoubleSpinBox* doubleSpinBox = static_cast<QDoubleSpinBox*>(editor);
	doubleSpinBox->setValue(index.model()->data(index, Qt::EditRole).toDouble());
}

void ConfigurationDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	QDoubleSpinBox* doubleSpinBox = static_cast<QDoubleSpinBox*>(editor);
	doubleSpinBox->interpretText();
	
	if (!model->setData(index, doubleSpinBox->value(), Qt::EditRole))
	{
		doubleSpinBox->setValue(index.model()->data(index, Qt::EditRole).toDouble());
	}
}

void ConfigurationDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index)
	editor->setGeometry(option.rect);
}

void ConfigurationDelegate::valueChanged(double d)
{
    Q_UNUSED(d)
	emit commitData(static_cast<QWidget*>(QObject::sender()));
}
