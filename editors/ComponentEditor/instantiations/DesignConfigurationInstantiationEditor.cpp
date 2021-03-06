//-----------------------------------------------------------------------------
// File: DesignConfigurationInstantiationEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 26.01.2016
//
// Description:
// Editor for design configuration instantiations.
//-----------------------------------------------------------------------------

#include "DesignConfigurationInstantiationEditor.h"

#include <common/widgets/nameGroupEditor/namegroupeditor.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <mainwindow/mainwindow.h>

#include <QApplication>
#include <QScrollArea>
#include <QHBoxLayout>

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationEditor::DesignConfigurationInstantiationEditor()
//-----------------------------------------------------------------------------
DesignConfigurationInstantiationEditor::DesignConfigurationInstantiationEditor(QSharedPointer<Component> component, 
    QSharedPointer<DesignConfigurationInstantiation> instantiation,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    LibraryInterface* libHandler, QWidget* parent):
ItemEditor(component, libHandler, parent), 
    instantiation_(instantiation),
    nameGroupEditor_(new NameGroupEditor(instantiation, this, 
    tr("Design configuration instance name and description"))),
    parameters_(instantiation->getParameters(), component->getChoices(), parameterFinder, expressionFormatter, this),
    designConfigurationEditor_(0)
{
    // find the main window for VLNV editor.
    QWidget* parentW = NULL;
    foreach (QWidget* widget, QApplication::topLevelWidgets())
    {
        MainWindow* mainWnd = dynamic_cast<MainWindow*>(widget);
        if (mainWnd)
        {
            parentW = mainWnd;
            break;
        }
    }

    designConfigurationEditor_ = new VLNVEditor(VLNV::DESIGNCONFIGURATION, libHandler, parentW, this);
    designConfigurationEditor_->setTitle(tr("Design configuration reference"));
    designConfigurationEditor_->setMandatory(true);
    designConfigurationEditor_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(&parameters_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&parameters_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(&parameters_, SIGNAL(openReferenceTree(QString)),
        this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);
    connect(&parameters_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(nameGroupEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(designConfigurationEditor_, SIGNAL(vlnvEdited()), this, SLOT(onHierRefChange()), Qt::UniqueConnection);
   
    refresh();

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationEditor::~DesignConfigurationInstantiationEditor()
//-----------------------------------------------------------------------------
DesignConfigurationInstantiationEditor::~DesignConfigurationInstantiationEditor()
{
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationEditor::refresh()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiationEditor::refresh()
{
	nameGroupEditor_->refresh();
    designConfigurationEditor_->setVLNV(*instantiation_->getDesignConfigurationReference());
    parameters_.refresh();
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationEditor::onHierRefChange()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiationEditor::onHierRefChange()
{
	instantiation_->getDesignConfigurationReference()->setVLNV(designConfigurationEditor_->getVLNV());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationEditor::showEvent()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiationEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/designConfigurationInstantiation.html");
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationEditor::setupLayout()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiationEditor::setupLayout()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* editorLayout = new QHBoxLayout();
    editorLayout->addWidget(nameGroupEditor_);
    editorLayout->addWidget(designConfigurationEditor_, 0, Qt::AlignTop);

    QWidget* topWidget = new QWidget(scrollArea);
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setWidget(topWidget);

    QVBoxLayout* topLayout = new QVBoxLayout(topWidget);
    topLayout->addLayout(editorLayout);
    topLayout->addWidget(&parameters_);
    topLayout->setContentsMargins(0, 0, 0, 0);

}
