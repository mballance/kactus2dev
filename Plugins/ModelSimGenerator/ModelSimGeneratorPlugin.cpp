//-----------------------------------------------------------------------------
// File: ModelSimGeneratorPlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 10.06.2016
//
// Description:
// ModelSim generator plugin.
//-----------------------------------------------------------------------------

#include "ModelSimGeneratorPlugin.h"

#include "ModelSimWriterFactory/ModelSimWriterFactory.h"

#include <Plugins/common/LanguageHighlighter.h>
#include <Plugins/PluginSystem/IPluginUtility.h>
#include <Plugins/PluginSystem/GeneratorPlugin/HDLGenerationDialog.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::ModelSimGeneratorPlugin()
//-----------------------------------------------------------------------------
ModelSimGeneratorPlugin::ModelSimGeneratorPlugin() : QObject(0)
{
    settings_.generateInterfaces_ = false;
    settings_.lastFileSetName_ = "";
    settings_.lastViewName_ = "";
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::ModelSimGeneratorPlugin()
//-----------------------------------------------------------------------------
ModelSimGeneratorPlugin::~ModelSimGeneratorPlugin()
{
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::getName()
//----------------------------------------------------------------------------
QString ModelSimGeneratorPlugin::getName() const
{
    return "ModelSim Generator";
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::getVersion()
//-----------------------------------------------------------------------------
QString ModelSimGeneratorPlugin::getVersion() const
{
    return "2.0";
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::getDescription()
//-----------------------------------------------------------------------------
QString ModelSimGeneratorPlugin::getDescription() const
{
	return "Generates a ModelSim do file, which adds the files in design to the active ModelSim project.";
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::getVendor()
//-----------------------------------------------------------------------------
QString ModelSimGeneratorPlugin::getVendor() const
{
    return tr("TUT");
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::getLicence()
//-----------------------------------------------------------------------------
QString ModelSimGeneratorPlugin::getLicence() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString ModelSimGeneratorPlugin::getLicenceHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
QWidget* ModelSimGeneratorPlugin::getSettingsWidget()
{
    return new QWidget();
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::getIcon()
//-----------------------------------------------------------------------------
QIcon ModelSimGeneratorPlugin::getIcon() const
{
    return QIcon(":icons/ModelSimGenerator.png");
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool ModelSimGeneratorPlugin::checkGeneratorSupport(QSharedPointer<Component const> component,
    QSharedPointer<Design const> design,
    QSharedPointer<DesignConfiguration const> designConfiguration) const
{
    // If design or design configuration exists, their implementation overrides the top component.
    if (design)
    {
        return design->getImplementation() == KactusAttribute::HW;
    }

    if (designConfiguration)
    {
        return designConfiguration->getImplementation() == KactusAttribute::HW;
    }

    // Else the availability is determined based on the top component.
    return component && component->getImplementation() == KactusAttribute::HW;
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void ModelSimGeneratorPlugin::runGenerator(IPluginUtility* utility, 
    QSharedPointer<Component> component,
    QSharedPointer<Design> design,
    QSharedPointer<DesignConfiguration> designConfiguration)
{
    // First state we are running. Tell the version.
    utility->printInfo(tr("Running %1 %2.").arg(getName(), getVersion()));

    // Must have a component under any condition.
    if (!component)
    {
        utility->printError(tr("Null component given as a parameter."));
        return;
    }

    MessagePasser messages;

    GenerationTuple input;
    input.component = component;
    input.design = design;
    input.designConfiguration = designConfiguration;
    input.messages = &messages;

    ModelSimWriterFactory factory(utility->getLibraryInterface(), &messages, &settings_,
        utility->getKactusVersion(), getVersion());

    // Create model for the configuration widget.
    QSharedPointer<GenerationControl> configuration(new GenerationControl
        (utility->getLibraryInterface(), &factory, input, &settings_));

    // Create the dialog and execute: The user will ultimately accept the configuration.
    HDLGenerationDialog dialog(configuration, "ModelSim", utility->getParentWidget());

    connect(&messages, SIGNAL(errorMessage(const QString&)),
        &dialog, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);
    connect(&messages, SIGNAL(noticeMessage(const QString&)),
        & dialog, SLOT(onNoticeMessage(const QString&)), Qt::UniqueConnection);

    dialog.onViewChanged();

    if (dialog.exec() != QDialog::Accepted)
    {
        utility->printInfo(tr("Generation aborted."));
        return;
    }

    // Finally, save the changes.
    utility->printInfo(tr("Generation complete."));
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> ModelSimGeneratorPlugin::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}