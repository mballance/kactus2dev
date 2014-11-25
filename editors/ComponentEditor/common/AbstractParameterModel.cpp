//-----------------------------------------------------------------------------
// File: AbstractParameterModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.11.2014
//
// Description:
// Base class for models editing parameters and model parameters.
//-----------------------------------------------------------------------------

#include "AbstractParameterModel.h"

#include <IPXACTmodels/choice.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/Enumeration.h>

#include <IPXACTmodels/validators/ParameterValidator.h>

#include <QColor>

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::AbstractParameterModel()
//-----------------------------------------------------------------------------
AbstractParameterModel::AbstractParameterModel(QSharedPointer<QList<QSharedPointer<Choice> > > choices,
    QObject *parent): 
QAbstractTableModel(parent), choices_(choices)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::~AbstractParameterModel()
//-----------------------------------------------------------------------------
AbstractParameterModel::~AbstractParameterModel()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::data()
//-----------------------------------------------------------------------------
QVariant AbstractParameterModel::data( QModelIndex const& index, int role /*= Qt::DisplayRole */ ) const 
{
	if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
		return QVariant();
    }

    QSharedPointer<Parameter> parameter = getParameterOnRow(index.row());

    if (role == Qt::DisplayRole)
    {
        if (index.column() == nameColumn())
        {
            return parameter->getName();
        }
        else if (index.column() == displayNameColumn())
        {
            return parameter->getDisplayName();
        }
        else if (index.column() == formatColumn())
        {
            return parameter->getValueFormat();
        }
        else if (index.column() == bitwidthColumn())
        {
            return parameter->getBitStringLength();
        }
        else if (index.column() == minimumColumn())
        {
            return parameter->getMinimumValue();
        }
        else if (index.column() == maximumColumn())
        {
            return parameter->getMaximumValue();
        }
        else if (index.column() == choiceColumn())
        {
            return parameter->getChoiceRef();
        }
        else if (index.column() == valueColumn())
        {
            return evaluateValueFor(parameter);
        }
        else if (index.column() == resolveColumn())
        {
            return parameter->getValueResolve();
        }
        else if (index.column() == arraySizeColumn())
        {
            return parameter->getAttribute("arraySize");
        }
        else if (index.column() == arrayOffsetColumn())
        {
            return parameter->getAttribute("arrayOffset");
        }
        else if (index.column() == descriptionColumn())
        {
            return parameter->getDescription();
        }
        else 
        {
            return QVariant();
        }
    }
    else if (Qt::BackgroundRole == role) 
    {
        if (index.column() == nameColumn() ||
            index.column() == valueColumn()) 
        {
            return QColor("lemonChiffon");
        }
        else if (index.column() == bitwidthColumn())
        {
            if (parameter->getValueFormat() == "bitString")
            {
                return QColor("lemonChiffon");
            }
            else
            {
                return QColor("whiteSmoke");
            }
        }
        else if ((index.column() == minimumColumn() || index.column() == maximumColumn()) &&
            (parameter->getValueFormat() != "float" && parameter->getValueFormat() != "long" && 
             parameter->getValueFormat() != "bitString"))
        {
            return QColor("whiteSmoke");
        }
        else
        {
            return QColor("white");
        }
    }
    else if (Qt::ForegroundRole == role)
    {
        if (validateColumnForParameter(index.column(), parameter))
        {
            return QColor("black");
        }
        else 
        {
            return QColor("red");
        }
    }

	// if unsupported role
	else 
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::headerData()
//-----------------------------------------------------------------------------
QVariant AbstractParameterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
        return QVariant();

    if (role == Qt::DisplayRole) 
    {
        if (section == nameColumn())
        {
            return tr("Name");
        }
        else if (section == displayNameColumn())
        {
            return tr("Display\nname");
        }
        else if (section == formatColumn())
        {
            return tr("Format");   
        }
        else if (section == bitwidthColumn())
        {
            return tr("Bit string\nlength");   
        }
        else if (section == minimumColumn())
        {
            return tr("Minimum\nvalue");
        }
        else if (section == maximumColumn())
        {        
            return tr("Maximum\nvalue");
        }
        else if (section == choiceColumn())
        {        
            return tr("Choice");
        }     
        else if (section == valueColumn())
        {        
            return tr("Value");
        }  
        else if (section == resolveColumn())
        {        
            return tr("Resolve");
        }  
        else if (section == arraySizeColumn())
        {        
            return tr("Array\nsize");
        }  
        else if (section == arrayOffsetColumn())
        {        
            return tr("Array\noffset");
        } 
        else if (section == descriptionColumn())
        { 
            return tr("Description");
        }
        else
        {
            return QVariant();
        }
    }

    // if unsupported role
    else 
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::setData()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::setData(QModelIndex const& index, const QVariant& value, int role /*= Qt::EditRole */) 
{
	if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
		return false;
    }

    if (role == Qt::EditRole)
    {
        QSharedPointer<Parameter> parameter = getParameterOnRow(index.row());

        if (index.column() == nameColumn())
        {
            parameter->setName(value.toString());
        }
        else if (index.column() == displayNameColumn())
        {
            parameter->setDisplayName(value.toString());
        }
        else if (index.column() == formatColumn())
        {
            parameter->setValueFormat(value.toString());
        }
        else if (index.column() == bitwidthColumn())
        {
            parameter->setBitStringLength(value.toString());
        }
        else if (index.column() == minimumColumn())
        {
            parameter->setMinimumValue(value.toString());
        }
        else if (index.column() == maximumColumn())
        {
            parameter->setMaximumValue(value.toString());
        }
        else if (index.column() == choiceColumn())
        {
            parameter->setChoiceRef(value.toString());
        }
        else if (index.column() == valueColumn())
        {
            parameter->setValue(value.toString());
        }
        else if (index.column() == resolveColumn())
        {
            parameter->setValueResolve(value.toString());
        }
        else if (index.column() == arraySizeColumn())
        {
            parameter->setAttribute("arraySize", value.toString());
        }
        else if (index.column() == arrayOffsetColumn())
        {
            parameter->setAttribute("arrayOffset", value.toString());
        }
        else if (index.column() == descriptionColumn())
        {
            parameter->setDescription(value.toString());
        }
        else
        {
            return false;
        }

        emit dataChanged(index, index);
        emit contentChanged();
        return true;
    }

    // is unsupported role
    else 
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags AbstractParameterModel::flags(QModelIndex const& index ) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
    }

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::isValid()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::isValid() const
{
    ParameterValidator validator;

	// check all parameters
	for (int i = 0; i < rowCount(); i++)
	{
        QSharedPointer<Parameter> parameter = getParameterOnRow(i);

        if (!validator.validate(parameter.data(), choices_)) 
        {
            return false;
        }
	}
	
	// all parameters are valid
	return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::isValid()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::isValid(QStringList& errorList, QString const& parentIdentifier) const
{
    bool valid = true;

    ParameterValidator validator;
    for (int i = 0; i < rowCount(); i++)
    {
        QSharedPointer<Parameter> parameter = getParameterOnRow(i);

        errorList.append(validator.findErrorsIn(parameter.data(), parentIdentifier, choices_));

        // if one parameter is invalid, model is invalid.
        if (!validator.validate(parameter.data(), choices_))
        {
            valid = false;
        }
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::evaluateValueFor()
//-----------------------------------------------------------------------------
QString AbstractParameterModel::evaluateValueFor(QSharedPointer<Parameter> parameter) const
{
    if (parameter->getChoiceRef().isEmpty())
    {
        return parameter->getValue();
    }
    else
    {
        QSharedPointer<Choice> choice = findChoice(parameter->getChoiceRef());
        return findDisplayValueForEnumeration(choice, parameter->getValue());
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::findChoice()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> AbstractParameterModel::findChoice(QString const& choiceName) const
{
    foreach (QSharedPointer<Choice> choice, *choices_)
    {
        if (choice->getName() == choiceName)
        {
            return choice;
        }
    }	

    return QSharedPointer<Choice>(new Choice(QDomNode()));
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::findDisplayValueForEnumeration()
//-----------------------------------------------------------------------------
QString AbstractParameterModel::findDisplayValueForEnumeration(QSharedPointer<Choice> choice,
    QString const& enumerationValue) const
{
    foreach (QSharedPointer<Enumeration> enumeration, *choice->enumerations())
    {
        if (enumeration->getValue() == enumerationValue && !enumeration->getText().isEmpty())
        {
            return enumeration->getText();
        }
    }

    return enumerationValue;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::validateColumnForParameter()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::validateColumnForParameter(int column, QSharedPointer<Parameter> parameter) const
{
    ParameterValidator validator;

    if (column == nameColumn())
    {
        return validator.hasValidName(parameter.data());
    }
    else if (column == formatColumn())
    {
        return validator.hasValidFormat(parameter.data()) &&
            validator.hasValidBitStringLength(parameter.data()) &&
            validator.hasValidValueForFormat(parameter->getValue(), parameter->getValueFormat());
    }
    else if (column == bitwidthColumn())
    {
        return validator.hasValidBitStringLength(parameter.data());
    }
    else if (column == minimumColumn())
    {
        return validator.hasValidMinimumValue(parameter.data()) && 
            !validator.valueIsLessThanMinimum(parameter.data());
    }
    else if (column == maximumColumn())
    {
        return validator.hasValidMaximumValue(parameter.data()) && 
            !validator.valueIsGreaterThanMaximum(parameter.data());
    }
    else if (column == choiceColumn())
    {
        return validator.hasValidChoice(parameter.data(), choices_) &&
            validator.hasValidValueForChoice(parameter.data(), choices_);
    }
    else if (column == valueColumn())
    {
        return validator.hasValidValue(parameter.data(), choices_);
    }
    else if (column == resolveColumn())
    {
        return validator.hasValidResolve(parameter.data());
    }

    return true;
}
