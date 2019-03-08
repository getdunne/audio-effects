#include "ReverbProcessor.h"
#include "ReverbEditor.h"

ReverbEditor::ReverbEditor (ReverbProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , dampingKnob(ReverbParameters::dampingMin, ReverbParameters::dampingMax, ReverbParameters::dampingLabel)
    , labeledDampingKnob(ReverbParameters::dampingName, dampingKnob)
    , densityKnob(ReverbParameters::densityMin, ReverbParameters::densityMax, ReverbParameters::densityLabel)
    , labeledDensityKnob(ReverbParameters::densityName, densityKnob)
    , bandwidthKnob(ReverbParameters::bandwidthMin, ReverbParameters::bandwidthMax, ReverbParameters::bandwidthLabel)
    , labeledBandwidthKnob(ReverbParameters::bandwidthName, bandwidthKnob)
    , decayKnob(ReverbParameters::decayMin, ReverbParameters::decayMax, ReverbParameters::decayLabel)
    , labeledDecayKnob(ReverbParameters::decayName, decayKnob)
    , predelayKnob(ReverbParameters::predelayMin, ReverbParameters::predelayMax, ReverbParameters::predelayLabel)
    , labeledPredelayKnob(ReverbParameters::predelayName, predelayKnob)
    , roomSizeKnob(ReverbParameters::roomSizeMin, ReverbParameters::roomSizeMax, ReverbParameters::roomSizeLabel)
    , labeledRoomSizeKnob(ReverbParameters::roomSizeName, roomSizeKnob)
    , dryWetMixKnob(ReverbParameters::dryWetMixMin, ReverbParameters::dryWetMixMax, ReverbParameters::dryWetMixLabel)
    , labeledDryWetMixKnob(ReverbParameters::dryWetMixName, dryWetMixKnob)
    , earlyLateMixKnob(ReverbParameters::earlyLateMixMin, ReverbParameters::earlyLateMixMax, ReverbParameters::earlyLateMixLabel)
    , labeledEarlyRefMixKnob(ReverbParameters::earlyLateMixName, earlyLateMixKnob)
{
    setLookAndFeel(lookAndFeel);

    mainGroup.setText("MVerb");
    addAndMakeVisible(&mainGroup);

    dampingKnob.setDoubleClickReturnValue(true, double(ReverbParameters::dampingDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDampingKnob);
    densityKnob.setDoubleClickReturnValue(true, double(ReverbParameters::densityDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDensityKnob);
    bandwidthKnob.setDoubleClickReturnValue(true, double(ReverbParameters::bandwidthDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledBandwidthKnob);
    decayKnob.setDoubleClickReturnValue(true, double(ReverbParameters::decayDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDecayKnob);
    predelayKnob.setDoubleClickReturnValue(true, double(ReverbParameters::predelayDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledPredelayKnob);
    roomSizeKnob.setDoubleClickReturnValue(true, double(ReverbParameters::roomSizeDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledRoomSizeKnob);
    dryWetMixKnob.setDoubleClickReturnValue(true, double(ReverbParameters::dryWetMixDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledDryWetMixKnob);
    earlyLateMixKnob.setDoubleClickReturnValue(true, double(ReverbParameters::earlyLateMixDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledEarlyRefMixKnob);

    processor.parameters.attachControls(
        dampingKnob,
        densityKnob,
        bandwidthKnob,
        decayKnob,
        predelayKnob,
        roomSizeKnob,
        dryWetMixKnob,
        earlyLateMixKnob );

    setSize (60 + 100 * 8 + 10 * (8 - 1), 180);
}

ReverbEditor::~ReverbEditor()
{
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void ReverbEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    mainGroup.setBounds(bounds);
    auto widgetsArea = bounds.reduced(10);
    widgetsArea.removeFromTop(20);
    int width = (widgetsArea.getWidth() - (8 - 1) * 10) / 8;
    labeledDampingKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledDensityKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledBandwidthKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledDecayKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledPredelayKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledRoomSizeKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledDryWetMixKnob.setBounds(widgetsArea.removeFromLeft(width));
    widgetsArea.removeFromLeft(10);
    labeledEarlyRefMixKnob.setBounds(widgetsArea);
}

void ReverbEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(lookAndFeel->findColour(ResizableWindow::backgroundColourId));
}