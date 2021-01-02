#include "OperationalModel.h"

OperationalModel::OperationalModel(QObject* parent) :
    QAbstractTableModel(parent)
{
}

OperationalModel::~OperationalModel()
{
}

void OperationalModel::initData(QList<double> Pos)
{
    OperationPos = Pos;
}

int OperationalModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
	return 6;
}

void OperationalModel::configurationChanged()
{
	this->beginResetModel();
	this->endResetModel();
}

QVariant OperationalModel::data(const QModelIndex& index, int role) const
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

Qt::ItemFlags OperationalModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
	{
		return Qt::NoItemFlags;
	}
	
	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QVariant OperationalModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (Qt::DisplayRole == role && Qt::Horizontal == orientation)
	{
		switch (section)
		{
		case 0:
			return "x";
			break;
		case 1:
			return "y";
			break;
		case 2:
			return "z";
			break;
		case 3:
			return "a";
			break;
		case 4:
			return "b";
			break;
		case 5:
			return "c";
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

int OperationalModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return 1;
}

bool OperationalModel::setData(const QModelIndex& index, const QVariant& value, int role)
{	
	if (index.isValid() && Qt::EditRole == role)
	{
        OperationPos.replace(index.column(),value.value<rl::math::Real>());
	}
    emit dataChanged(index,index);
	return false;
}
