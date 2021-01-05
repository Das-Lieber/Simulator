#include "tableViewPosModel.h"

tableViewPosModel::tableViewPosModel(QObject* parent) :
    QAbstractTableModel(parent)
{
}

tableViewPosModel::~tableViewPosModel()
{
}

void tableViewPosModel::initData(QList<double> Pos)
{
    OperationPos = Pos;
}

int tableViewPosModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
	return 6;
}

void tableViewPosModel::updateModel()
{
	this->beginResetModel();
	this->endResetModel();
}

QVariant tableViewPosModel::data(const QModelIndex& index, int role) const
{	
	if (!index.isValid())
	{
		return QVariant();
	}
	
	switch (role)
	{
	case Qt::DisplayRole:
		switch (index.column())
		{
		case 0:
		case 1:
		case 2:
            return QString::number(OperationPos.at(index.column()), 'f', 3) + QString("mm");
			break;
		case 3:
		case 4:
		case 5:
            return QString::number(OperationPos.at(index.column()), 'f', 4) + QChar(176);
			break;
		default:
			break;
		}
		break;
	case Qt::EditRole:
		switch (index.column())
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
            return OperationPos.at(index.column());
			break;
		default:
			break;
		}
		break;
	case Qt::TextAlignmentRole:
		return QVariant(Qt::AlignRight | Qt::AlignVCenter);
		break;
	default:
		break;
	}
	
	return QVariant();
}

Qt::ItemFlags tableViewPosModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
	{
		return Qt::NoItemFlags;
	}
	
	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QVariant tableViewPosModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (Qt::DisplayRole == role && Qt::Horizontal == orientation)
	{
		switch (section)
		{
		case 0:
            return "X(mm)";
			break;
		case 1:
            return "Y(mm)";
			break;
		case 2:
            return "Z(mm)";
			break;
		case 3:
            return "a(°)";
			break;
		case 4:
            return "b(°)";
			break;
		case 5:
            return "c(°)";
			break;
		default:
			break;
		}
	}
	
	if (Qt::DisplayRole == role && Qt::Vertical == orientation)
	{
		return section;
	}
	
	return QVariant();
}

int tableViewPosModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return 1;
}

bool tableViewPosModel::setData(const QModelIndex& index, const QVariant& value, int role)
{	
	if (index.isValid() && Qt::EditRole == role)
	{
        OperationPos.replace(index.column(),value.value<rl::math::Real>());
	}
    emit changePositionAndValue(index.column(),value.value<rl::math::Real>());
	return false;
}
