//-----------------------------------------------------------------------------
// File: DesignDiagram.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 24.4.2012
//
// Description:
// Base class for all design diagrams.
//-----------------------------------------------------------------------------

#include "DesignDiagram.h"

#include <common/diagramgrid.h>
#include <common/GenericEditProvider.h>
#include <common/graphicsItems/ComponentItem.h>

#include <AdHocEditor/AdHocEditor.h>

#include <LibraryManager/libraryinterface.h>

#include <models/designconfiguration.h>
#include <models/component.h>

#include <QWidget>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: DesignDiagram::DesignDiagram()
//-----------------------------------------------------------------------------
DesignDiagram::DesignDiagram(LibraryInterface* lh, MainWindow* mainWnd,
                             GenericEditProvider& editProvider, QWidget* parent)
    : QGraphicsScene(parent),
      lh_(lh),
      mainWnd_(mainWnd),
      editProvider_(editProvider),
      component_(),
      designConf_(),
      mode_(MODE_SELECT),
      instanceNames_(),
      locked_(false)
{
    setSceneRect(0, 0, 100000, 100000);

    connect(this, SIGNAL(componentInstantiated(ComponentItem*)),
        this, SLOT(onComponentInstanceAdded(ComponentItem*)), Qt::UniqueConnection);
    connect(this, SIGNAL(componentInstanceRemoved(ComponentItem*)),
        this, SLOT(onComponentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::~DesignDiagram()
//-----------------------------------------------------------------------------
DesignDiagram::~DesignDiagram()
{
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::clearScene()
//-----------------------------------------------------------------------------
void DesignDiagram::clearScene()
{
    clear();
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::setDesign()
//-----------------------------------------------------------------------------
bool DesignDiagram::setDesign(QSharedPointer<Component> component, QString const& viewName)
{
    // Clear the edit provider.
    editProvider_.clear();

    // clear the previous design configuration
    designConf_.clear();

    // Clear the scene.
    clearScene();

    // Set the new component.
    component_ = component;

    VLNV designVLNV = component_->getHierRef(viewName);

    // Check for a valid VLNV type.
    designVLNV.setType(lh_->getDocumentType(designVLNV));

    if (!designVLNV.isValid())
    {
        emit errorMessage(tr("Component %1 did not contain a hierarchical view").arg(component_->getVlnv()->getName()));
        return false;
    }

    QSharedPointer<Design> design;

    // if the component contains a direct reference to a design
    if (designVLNV.getType() == VLNV::DESIGN) {
        QSharedPointer<LibraryComponent> libComp = lh_->getModel(designVLNV);	
        design = libComp.staticCast<Design>();
    }

    // if component had reference to a design configuration
    else if (designVLNV.getType() == VLNV::DESIGNCONFIGURATION) {
        QSharedPointer<LibraryComponent> libComp = lh_->getModel(designVLNV);
        designConf_ = libComp.staticCast<DesignConfiguration>();

        designVLNV = designConf_->getDesignRef();

        if (designVLNV.isValid()) {
            QSharedPointer<LibraryComponent> libComp = lh_->getModel(designVLNV);	
            design = libComp.staticCast<Design>();
        }

        // if design configuration did not contain a reference to a design.
        if (!design) {
            emit errorMessage(tr("Component %1 did not contain a hierarchical view").arg(
                component_->getVlnv()->getName()));
            return false;
        }
    }

    openDesign(design);
    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::attach()
//-----------------------------------------------------------------------------
void DesignDiagram::attach(AdHocEditor* editor)
{
    connect(this, SIGNAL(contentChanged()), editor, SLOT(onContentChanged()), Qt::UniqueConnection);
    connect(this, SIGNAL(destroyed(QObject*)), editor, SLOT(clear()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::detach()
//-----------------------------------------------------------------------------
void DesignDiagram::detach(AdHocEditor* editor)
{
    disconnect(editor);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::addInstanceName()
//-----------------------------------------------------------------------------
void DesignDiagram::addInstanceName(QString const& name)
{
    instanceNames_.append(name);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::removeInstanceName()
//-----------------------------------------------------------------------------
void DesignDiagram::removeInstanceName(const QString& name)
{
    instanceNames_.removeAll(name);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::updateInstanceName()
//-----------------------------------------------------------------------------
void DesignDiagram::updateInstanceName(const QString& oldName, const QString& newName)
{
    instanceNames_.removeAll(oldName);
    instanceNames_.append(newName);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::setMode()
//-----------------------------------------------------------------------------
void DesignDiagram::setMode(DrawMode mode)
{
    if (mode_ != mode)
    {
        mode_ = mode;

        if (mode_ != MODE_SELECT)
        {
            emit clearItemSelection();
        }

        emit modeChanged(mode);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::setProtection()
//-----------------------------------------------------------------------------
void DesignDiagram::setProtection(bool locked)
{
    locked_ = locked;
    clearSelection();
    emit clearItemSelection();
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getMode()
//-----------------------------------------------------------------------------
DrawMode DesignDiagram::getMode() const
{
    return mode_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::isProtected()
//-----------------------------------------------------------------------------
bool DesignDiagram::isProtected() const
{
    return locked_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getLibraryInterface()
//-----------------------------------------------------------------------------
LibraryInterface* DesignDiagram::getLibraryInterface() const
{
    return lh_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getMainWindow()
//-----------------------------------------------------------------------------
MainWindow* DesignDiagram::getMainWindow() const
{
    return mainWnd_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getEditProvider()
//-----------------------------------------------------------------------------
GenericEditProvider& DesignDiagram::getEditProvider()
{
    return editProvider_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getEditedComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> DesignDiagram::getEditedComponent() const
{
    return component_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getDesignConfiguration()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfiguration> DesignDiagram::getDesignConfiguration() const
{
    return designConf_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::onComponentInstanceAdded()
//-----------------------------------------------------------------------------
void DesignDiagram::onComponentInstanceAdded(ComponentItem* item)
{
    instanceNames_.append(item->name());
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::onComponentInstanceRemoved()
//-----------------------------------------------------------------------------
void DesignDiagram::onComponentInstanceRemoved(ComponentItem* item)
{
    instanceNames_.removeAll(item->name());
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::createInstanceName()
//-----------------------------------------------------------------------------
QString DesignDiagram::createInstanceName(QSharedPointer<Component> component)
{
    QString instanceName = component->getVlnv()->getName();
    instanceName.remove(QString(".comp"));
    return createInstanceName(instanceName);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::createInstanceName()
//-----------------------------------------------------------------------------
QString DesignDiagram::createInstanceName(QString const& baseName)
{
    // Determine a unique name by using a running number.
    int runningNumber = 0;
    QString name = baseName + "_" + QString::number(runningNumber);

    while (instanceNames_.contains(name))
    {
        ++runningNumber;
        name = baseName + "_" + QString::number(runningNumber);
    }

    instanceNames_.append(name);
    return name;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::drawBackground()
//-----------------------------------------------------------------------------
void DesignDiagram::drawBackground(QPainter* painter, QRectF const& rect)
{
    painter->setWorldMatrixEnabled(true);
    painter->setBrush(Qt::lightGray);

    qreal left = int(rect.left()) - (int(rect.left()) % GridSize );
    qreal top = int(rect.top()) - (int(rect.top()) % GridSize );

    for (qreal x = left; x < rect.right(); x += GridSize ) {
        for (qreal y = top; y < rect.bottom(); y += GridSize )
            painter->drawPoint(x, y);
    }
}
