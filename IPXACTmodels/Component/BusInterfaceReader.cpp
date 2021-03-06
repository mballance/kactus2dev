//-----------------------------------------------------------------------------
// File: businterfaceReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 22.09.2015
//
// Description:
// Reader class for IP-XACT businterface element.
//-----------------------------------------------------------------------------

#include "BusInterfaceReader.h"

#include "MasterInterface.h"
#include "MirroredSlaveInterface.h"
#include "SlaveInterface.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/common/ParameterReader.h>

#include <IPXACTmodels/XmlUtils.h>

#include <QXmlStreamWriter>
#include <QDomNamedNodeMap>

//-----------------------------------------------------------------------------
// Function: businterfaceReader::businterfaceReader()
//-----------------------------------------------------------------------------
BusinterfaceReader::BusinterfaceReader(QObject* parent) :
CommonItemsReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::~businterfaceReader()
//-----------------------------------------------------------------------------
BusinterfaceReader::~BusinterfaceReader()
{

}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::createbusinterfaceFrom()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> BusinterfaceReader::createbusinterfaceFrom(QDomNode const& businterfaceNode) const
{
	// Create the new bus interface.
	QSharedPointer<BusInterface> newbusinterface (new BusInterface());

	// Get the attributes for the bus interface.
	QMap<QString, QString> attributes;

	QDomNamedNodeMap attributeMap = businterfaceNode.attributes();
	for (int j = 0; j < attributeMap.size(); ++j)
	{
		QString name = attributeMap.item(j).nodeName();
		QString value = attributeMap.item(j).nodeValue();
		attributes.insert(name, value);
	}
	newbusinterface->setAttributes(attributes);
	
	parseNameGroup(businterfaceNode, newbusinterface);
    parseIsPresent(businterfaceNode, newbusinterface);	

	// All abstraction types will be added here.
    QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionTypes
        (new QList<QSharedPointer<AbstractionType> > ());

	// Go through all the child nodes and call appropriate constructors.
	QDomNodeList children = businterfaceNode.childNodes();

    QDomElement businterfaceElement = businterfaceNode.toElement();
    
    QDomElement busTypeElement = businterfaceElement.firstChildElement(QStringLiteral("ipxact:busType"));
    newbusinterface->setBusType(parseVLNVAttributes(busTypeElement, VLNV::BUSDEFINITION));

    parseAbstractionTypes(businterfaceElement.firstChildElement(QStringLiteral("ipxact:abstractionTypes")), newbusinterface);

    parseInterfaceMode(businterfaceElement, newbusinterface);

    QDomElement connnectionRequirementNode = businterfaceElement.firstChildElement(QStringLiteral("ipxact:connectionRequired"));
    if (!connnectionRequirementNode.isNull())
    {
        QString connectionRequired = connnectionRequirementNode.childNodes().at(0).nodeValue();
        newbusinterface->setConnectionRequired(connectionRequired == QLatin1String("true"));
    }

    QDomElement lauElement = businterfaceElement.firstChildElement(QStringLiteral("ipxact:bitsInLau"));
    if (!lauElement.isNull())
    {
        QString bitsInLau = lauElement.firstChild().nodeValue();
        newbusinterface->setBitsInLau(bitsInLau);
    }

    QDomElement bitSteeringElement = businterfaceElement.firstChildElement(QStringLiteral("ipxact:bitSteering"));
    if (!bitSteeringElement.isNull())
    {
        QString bitSteering = bitSteeringElement.childNodes().at(0).nodeValue();
        if (bitSteering == QLatin1String("on"))
        {
            newbusinterface->setBitSteering(BusInterface::BITSTEERING_ON);
        }
        else if (bitSteering == QLatin1String("off"))
        {
            newbusinterface->setBitSteering(BusInterface::BITSTEERING_OFF);
        }

        newbusinterface->setBitSteeringAttributes(XmlUtils::parseAttributes(bitSteeringElement));
    }

    QDomElement endiannessElement = businterfaceElement.firstChildElement(QStringLiteral("ipxact:endianness"));
    if (!endiannessElement.isNull())
    {
        QString endianness = endiannessElement.childNodes().at(0).nodeValue();
        if (endianness == QLatin1String("little"))
        {
            newbusinterface->setEndianness(BusInterface::LITTLE);
        }
        else if (endianness == QLatin1String("big"))
        {
            newbusinterface->setEndianness(BusInterface::BIG);
        }
    }

    newbusinterface->getParameters()->append(*CommonItemsReader::parseAndCreateParameters(businterfaceNode));

    readBusInterfaceExtensions(businterfaceNode, newbusinterface);

    return newbusinterface;
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::parseIsPresent()
//-----------------------------------------------------------------------------
void BusinterfaceReader::parseIsPresent(QDomNode const& businterfaceNode, QSharedPointer<BusInterface> newbusinterface) const
{
	QString newIsPresent = businterfaceNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue();
	if (!newIsPresent.isEmpty())
	{
		newbusinterface->setIsPresent(newIsPresent);
	}
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::parseNameGroup()
//-----------------------------------------------------------------------------
void BusinterfaceReader::parseNameGroup(QDomNode const& businterfaceNode,
	QSharedPointer<BusInterface> newbusinterface) const
{
	NameGroupReader nameReader;
	nameReader.parseNameGroup(businterfaceNode, newbusinterface);
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::readAbstractionTypes()
//-----------------------------------------------------------------------------
void BusinterfaceReader::parseAbstractionTypes(QDomElement const& abstractionTypesElement,
    QSharedPointer<BusInterface> busInterface) const
{
    QDomNodeList abstractionNodes = abstractionTypesElement.elementsByTagName(QStringLiteral("ipxact:abstractionType"));

    int abstractionTypeCount = abstractionNodes.count();

	for (int i = 0; i < abstractionTypeCount; i++)
    {
        QSharedPointer<AbstractionType> newAbstractionType(new AbstractionType());

        QDomNode abstractionNode = abstractionNodes.at(i);

        QDomElement viewRefElement = abstractionNode.firstChildElement(QStringLiteral("ipxact:viewRef"));
        if (!viewRefElement.isNull())
        {
            QString viewRef = viewRefElement.firstChild().nodeValue();
            newAbstractionType->setViewRef(XmlUtils::removeWhiteSpace(viewRef));
        }

        QDomElement abstractionRefElement = abstractionNode.firstChildElement(QStringLiteral("ipxact:abstractionRef"));
        newAbstractionType->setAbstractionRef(parseConfigurableVLNVReference(abstractionRefElement,
            VLNV::ABSTRACTIONDEFINITION));

        QDomElement portMapsElement = abstractionNode.firstChildElement(QStringLiteral("ipxact:portMaps"));
        if (!portMapsElement.isNull())
        {
            readPortMaps(portMapsElement, newAbstractionType);
        }

        busInterface->getAbstractionTypes()->append(newAbstractionType);
	}
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::readPortMaps()
//-----------------------------------------------------------------------------
void BusinterfaceReader::readPortMaps(QDomElement const& portMapsElement,
    QSharedPointer<AbstractionType> abstractionType) const
{
    QDomNodeList portMapNodes = portMapsElement.elementsByTagName(QStringLiteral("ipxact:portMap"));

    int portMapCount = portMapNodes.count();
    for (int i = 0; i < portMapCount; i++)
    {
        QDomElement portMapElement = portMapNodes.at(i).toElement();

        QSharedPointer<PortMap> portMap (new PortMap());

        if (portMapElement.hasAttribute(QStringLiteral("invert")))
        {
            if (portMapElement.attribute(QStringLiteral("invert")) == QLatin1String("true"))
            {
                portMap->setInvert(true);
            }
            else
            {
                portMap->setInvert(false);
            }
        }

        QDomElement isPresentElement = portMapElement.firstChildElement(QStringLiteral("ipxact:isPresent"));
        if (!isPresentElement.isNull())
        {
            portMap->setIsPresent(isPresentElement.firstChild().nodeValue());
        }

        readLogicalPort(portMapElement.firstChildElement(QStringLiteral("ipxact:logicalPort")), portMap);
        readPhysicalPort(portMapElement.firstChildElement(QStringLiteral("ipxact:physicalPort")), portMap);

        QDomElement logicalTieOffElement = portMapElement.firstChildElement(QStringLiteral("ipxact:logicalTieOff"));
        if (!logicalTieOffElement.isNull())
        {
            portMap->setLogicalTieOff(logicalTieOffElement.firstChild().nodeValue());
        }

        QDomElement isInformativeElement = portMapElement.firstChildElement(QStringLiteral("ipxact:isInformative"));
        if (!isInformativeElement.isNull())
        {
            if (isInformativeElement.firstChild().nodeValue() == QLatin1String("true"))
            {
                portMap->setIsInformative(true);
            }
            else
            {
                portMap->setIsInformative(false);
            }
        }

        abstractionType->getPortMaps()->append(portMap);
    }
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::readLogicalPort()
//-----------------------------------------------------------------------------
void BusinterfaceReader::readLogicalPort(QDomElement const& logicalPortElement, 
    QSharedPointer<PortMap> portMap) const
{
    QDomElement nameElement = logicalPortElement.firstChildElement(QStringLiteral("ipxact:name"));
    QString portName = XmlUtils::removeWhiteSpace(nameElement.firstChild().nodeValue());

    QSharedPointer<PortMap::LogicalPort> logicalPort(new PortMap::LogicalPort(portName));

    QDomElement rangeElement = logicalPortElement.firstChildElement(QStringLiteral("ipxact:range"));
    if (!rangeElement.isNull())
    {
        QString leftRange = rangeElement.firstChildElement(QStringLiteral("ipxact:left")).firstChild().nodeValue();
        QString rightRange = rangeElement.firstChildElement(QStringLiteral("ipxact:right")).firstChild().nodeValue();

        logicalPort->range_ = QSharedPointer<Range>(new Range(leftRange, rightRange));
    }

	portMap->setLogicalPort(logicalPort);
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::readPhysicalPort()
//-----------------------------------------------------------------------------
void BusinterfaceReader::readPhysicalPort(QDomElement const& physicalPortElement,
    QSharedPointer<PortMap> portMap) const
{
    if (physicalPortElement.isNull())
    {
        return;
    }

    QDomElement nameElement = physicalPortElement.firstChildElement(QStringLiteral("ipxact:name"));
    QString portName = XmlUtils::removeWhiteSpace(nameElement.firstChild().nodeValue());
	QSharedPointer<PortMap::PhysicalPort> physicalPort(new PortMap::PhysicalPort(portName));

    QDomElement partSelectElement = physicalPortElement.firstChildElement(QStringLiteral("ipxact:partSelect"));
    if (!partSelectElement.isNull())
    {
        // Part select includes range.
        QDomNode rangeNode = partSelectElement.firstChildElement(QStringLiteral("ipxact:range"));
        QString leftRange = rangeNode.firstChildElement(QStringLiteral("ipxact:left")).firstChild().nodeValue();
        QString rightRange = rangeNode.firstChildElement(QStringLiteral("ipxact:right")).firstChild().nodeValue();

        QSharedPointer<PartSelect> newPartSelect (new PartSelect(leftRange, rightRange));

        // Part select includes indices.
        QDomElement indicesNode = partSelectElement.firstChildElement(QStringLiteral("ipxact:indices"));
        if (!indicesNode.isNull())
        {
            QDomNodeList indexNodes = indicesNode.elementsByTagName(QStringLiteral("ipxact:index"));

            int indexCount = indexNodes.count();
            for (int index = 0; index < indexCount; index++)
            {
                QString indexValue = indexNodes.at(index).firstChild().nodeValue();
                newPartSelect->getIndices()->append(indexValue);
            }
        }

        physicalPort->partSelect_ = newPartSelect;
    }

	portMap->setPhysicalPort(physicalPort);
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::readInterfaceMode()
//-----------------------------------------------------------------------------
void BusinterfaceReader::parseInterfaceMode(QDomElement const& busInterfaceElement,
    QSharedPointer<BusInterface> newbusinterface) const
{
    QDomElement masterNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:master"));
	if (!masterNode.isNull())
	{
		QSharedPointer<MasterInterface> newmode(new MasterInterface());
        readMasterInterface(masterNode, newmode);
        
        newbusinterface->setInterfaceMode(General::MASTER);
		newbusinterface->setMaster(newmode);
        return;
	}

	QDomElement slaveNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:slave"));
    if (!slaveNode.isNull())
	{
		QSharedPointer<SlaveInterface> newmode(new SlaveInterface());
		readSlaveInterface(slaveNode, newmode);

        newbusinterface->setInterfaceMode(General::SLAVE);
		newbusinterface->setSlave(newmode);
        return;
	}

    QDomElement systemNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:system"));
    if (!systemNode.isNull())
    {
        QString systemGroup = systemNode.firstChildElement(QStringLiteral("ipxact:group")).firstChild().nodeValue();
        
        newbusinterface->setInterfaceMode(General::SYSTEM);
        newbusinterface->setSystem(systemGroup);
        return;
    }

    QDomElement mirroredMasterNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:mirroredMaster"));
    if (!mirroredMasterNode.isNull())
    {		
        newbusinterface->setInterfaceMode(General::MIRROREDMASTER);
        return;
    }

    QDomElement mirroredSlaveNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:mirroredSlave"));
	if (!mirroredSlaveNode.isNull())
	{
		QSharedPointer<MirroredSlaveInterface> newmode(new MirroredSlaveInterface());
		parseMirroredSlaveInterface(mirroredSlaveNode, newmode);

        newbusinterface->setInterfaceMode(General::MIRROREDSLAVE);
		newbusinterface->setMirroredSlave(newmode);
        return;
	}

    QDomElement mirroredSystemNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:mirroredSystem"));
	if (!mirroredSystemNode.isNull())
	{
        QString systemGroup = mirroredSystemNode.firstChildElement(QStringLiteral("ipxact:group")).firstChild().nodeValue();
        
        newbusinterface->setInterfaceMode(General::MIRROREDSYSTEM);
        newbusinterface->setSystem(systemGroup);
        return;
	}

    QDomElement monitorNode = busInterfaceElement.firstChildElement(QStringLiteral("ipxact:monitor"));
	if (!monitorNode.isNull())
    {
        QSharedPointer<BusInterface::MonitorInterface> newmode(new BusInterface::MonitorInterface());
        parseMonitorInterface(monitorNode, newmode);

        newbusinterface->setInterfaceMode(General::MONITOR);
        newbusinterface->setMonitor(newmode);        
    }
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::readMasterInterface()
//-----------------------------------------------------------------------------
void BusinterfaceReader::readMasterInterface(QDomElement const& masterInterfaceElement, 
    QSharedPointer<MasterInterface> masterInterface) const
{
    QDomElement addressSpaceRefElement = masterInterfaceElement.firstChildElement(QStringLiteral("ipxact:addressSpaceRef"));
 
    QString addressSpaceRef = addressSpaceRefElement.attribute(QStringLiteral("ipxact:addressSpaceRef"));
    masterInterface->setAddressSpaceRef(XmlUtils::removeWhiteSpace(addressSpaceRef));

    QDomElement isPresentElement = addressSpaceRefElement.firstChildElement(QStringLiteral("ipxact:isPresent"));
    if (!isPresentElement.isNull())
    {
        masterInterface->setIsPresent(isPresentElement.firstChild().nodeValue());
    }

    QDomElement baseAddressElement = addressSpaceRefElement.firstChildElement(QStringLiteral("ipxact:baseAddress"));
    if (!baseAddressElement.isNull())
    {
        QString baseAddress = baseAddressElement.firstChild().nodeValue();
        masterInterface->setBaseAddress(XmlUtils::removeWhiteSpace(baseAddress));
    }

    QMap<QString, QString> baseAttributes = XmlUtils::parseAttributes(addressSpaceRefElement);
    masterInterface->setBaseAttributes(baseAttributes);
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::readSlaveInterface()
//-----------------------------------------------------------------------------
void BusinterfaceReader::readSlaveInterface(QDomElement const& slaveIntefaceElement, 
    QSharedPointer<SlaveInterface> slaveInterface) const
{
    QDomElement memoryMapRefElement = slaveIntefaceElement.firstChildElement(QStringLiteral("ipxact:memoryMapRef"));
    if (!memoryMapRefElement.isNull())
    {
        QString memoryMapRef = memoryMapRefElement.attribute(QStringLiteral("memoryMapRef"));
        slaveInterface->setMemoryMapRef(XmlUtils::removeWhiteSpace(memoryMapRef));
    }

    QDomNodeList bridgeNodes = slaveIntefaceElement.elementsByTagName(QStringLiteral("ipxact:transparentBridge"));
    int bridgeCount = bridgeNodes.count();
    for (int i = 0; i < bridgeCount; i++)
    {
        QDomElement bridgeElement = bridgeNodes.at(i).toElement();
  
        QSharedPointer<SlaveInterface::Bridge> newBridge(new SlaveInterface::Bridge());
        newBridge->masterRef_ = XmlUtils::removeWhiteSpace(bridgeElement.attribute(QStringLiteral("masterRef")));

        QString bridgePresence = bridgeElement.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue();
        if (!bridgePresence.isEmpty())
        {
            newBridge->isPresent_ = bridgePresence;
        }

        // Another bridge is extracted.
        slaveInterface->getBridges()->append(newBridge);
    }

    QDomNodeList fileSetRefGroupNodes = slaveIntefaceElement.elementsByTagName(QStringLiteral("ipxact:fileSetRefGroup"));
    int filesetRefCount = fileSetRefGroupNodes.count();
	// Go through all child elements.
    for (int i = 0; i < filesetRefCount; i++)
    {
        QDomElement fileSetRefElement = fileSetRefGroupNodes.at(i).toElement();

        QSharedPointer<SlaveInterface::FileSetRefGroup> fileSetGroup(new SlaveInterface::FileSetRefGroup());

        QDomNode groupNode = fileSetRefElement.firstChildElement(QStringLiteral("ipxact:group"));
        fileSetGroup->group_ = XmlUtils::removeWhiteSpace(groupNode.firstChild().nodeValue());

        QDomNodeList filesetNodes = fileSetRefElement.elementsByTagName(QStringLiteral("ipxact:fileSetRef"));
        int filesetNodeCount = filesetNodes.count();
        for (int j = 0; j < filesetNodeCount; j++)
        {              
              fileSetGroup->fileSetRefs_.append(filesetNodes.at(j).firstChild().nodeValue());
        }

        // Another file set ref group is extracted.
        slaveInterface->getFileSetRefGroup()->append( fileSetGroup );
	}
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::readMirroredSlaveInterface()
//-----------------------------------------------------------------------------
void BusinterfaceReader::parseMirroredSlaveInterface(QDomElement const& mirroredInterfaceElement, 
    QSharedPointer<MirroredSlaveInterface> mirroredSlaveInterface) const
{
    QDomElement baseAddressElement = mirroredInterfaceElement.firstChildElement(QStringLiteral("ipxact:baseAddresses"));

    if (baseAddressElement.isNull())
    {
        return;
    }

    QDomNodeList remapAddressNodes = baseAddressElement.elementsByTagName(QStringLiteral("ipxact:remapAddress"));
    int remapAddressCount = remapAddressNodes.count();
    for (int i = 0; i < remapAddressCount; i++)
    { 
        QDomNode remapAddressNode = remapAddressNodes.at(i);
        QString remapAddress = remapAddressNode.childNodes().at(0).nodeValue();

        QSharedPointer<MirroredSlaveInterface::RemapAddress> remap(
            new MirroredSlaveInterface::RemapAddress(remapAddress));

        remap->remapAttributes_ = XmlUtils::parseAttributes(remapAddressNode);
        remap->state_ = remap->remapAttributes_.take(QStringLiteral("state"));

        mirroredSlaveInterface->getRemapAddresses()->append(remap);	
    }
   
    QDomNode rangeNode = baseAddressElement.firstChildElement(QStringLiteral("ipxact:range"));
    mirroredSlaveInterface->setRange(rangeNode.firstChild().nodeValue() );
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::createMonitorInterface()
//-----------------------------------------------------------------------------
void BusinterfaceReader::parseMonitorInterface(QDomElement const& monitorElement,
	QSharedPointer<BusInterface::MonitorInterface> monitorInterface) const
{
	QString interfaceMode = monitorElement.attribute(QStringLiteral("interfaceMode"));
	interfaceMode = XmlUtils::removeWhiteSpace(interfaceMode);

	monitorInterface->interfaceMode_ = General::str2Interfacemode(interfaceMode, General::MONITOR);
    monitorInterface->group_ = monitorElement.firstChildElement(QStringLiteral("ipxact:group")).firstChild().nodeValue();
}

//-----------------------------------------------------------------------------
// Function: businterfaceReader::readBusInterfaceExtensions()
//-----------------------------------------------------------------------------
void BusinterfaceReader::readBusInterfaceExtensions(QDomNode const& interfaceNode,
    QSharedPointer<BusInterface> newInterface) const
{
    QDomNode extensionsNode = interfaceNode.firstChildElement(QStringLiteral("ipxact:vendorExtensions"));
    QDomElement positionElement = extensionsNode.firstChildElement(QStringLiteral("kactus2:position"));
    if (!positionElement.isNull())
    {
        int positionX = positionElement.attribute(QStringLiteral("x")).toInt();
        int positionY = positionElement.attribute(QStringLiteral("y")).toInt();
        newInterface->setDefaultPos(QPointF(positionX, positionY));
    }

    parseVendorExtensions(interfaceNode, newInterface);
}
