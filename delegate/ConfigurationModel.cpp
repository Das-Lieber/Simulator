#include "ConfigurationModel.h"

ConfigurationModel::ConfigurationModel(QObject* parent) :
    QAbstractTableModel(parent)
{
}

ConfigurationModel::~ConfigurationModel()
{
}

int ConfigurationModel::columnCount(const QModelIndex& parent) const
{
	return 1;
}

QVariant ConfigurationModel::data(const QModelIndex& index, int role) const
{
//	if (!index.isValid())
//	{
//		return QVariant();
//	}
	
//	rl::math::Vector q = MainWindow::instance()->kinematicModels[this->id]->getPosition();
	
//	switch (role)
//	{
//	case Qt::DisplayRole:
//        if (Types.at(index.row())==RLAPI_JointType::Prismatic)
//		{
//            return QString::number(q(index.row()), 'f', 3) + QString(" mm");
//        }
//        else if (Types.at(index.row())==RLAPI_JointType::Revolute)
//            return QString::number(q(index.row()) * rl::math::constants::rad2deg, 'f', 4) + QChar(176);
//        else
//			return q(index.row());
//		break;
//	case Qt::EditRole:
//			return q(index.row());
//		break;
//	case Qt::ForegroundRole:
//        if (q(index.row()) < Mins(index.row()) || q(index.row()) > Maxs(index.row()))
//		{
//			return QBrush(Qt::red);
//		}
//		break;
//	case Qt::TextAlignmentRole:
//		return QVariant(Qt::AlignRight | Qt::AlignVCenter);
//		break;
//	default:
//		break;
//	}
	
    return QVariant();
}

Qt::ItemFlags ConfigurationModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
	{
		return Qt::NoItemFlags;
	}
	
	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QVariant ConfigurationModel::headerData(int section, Qt::Orientation orientation, int role) const
{	
	if (Qt::DisplayRole == role && Qt::Vertical == orientation)
	{
            for (std::size_t i = 0, j = 0; i < Dofs; ++i)
			{				
                for (std::size_t k = 0; k < Dofs; ++j, ++k)
				{
					if (section == j)
					{
                        return QString(tr("Joint%1").arg(j));
					}
				}
			}
	}
	
	return QVariant();
}

void ConfigurationModel::operationalChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
	this->beginResetModel();
	this->endResetModel();
}

int ConfigurationModel::rowCount(const QModelIndex& parent) const
{	
    return Dofs;
}

bool ConfigurationModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (index.isValid() && Qt::EditRole == role)
	{

			
			emit dataChanged(index, index);
	}
	
	return false;
}
