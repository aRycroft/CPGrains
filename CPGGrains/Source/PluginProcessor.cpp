/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

/*
!!!PROBLEMS!!!
What happens when the grainlength + startTime is longer than the total length of the sample buffer?
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CpggrainsAudioProcessor::CpggrainsAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
    )
#endif
    ,
    grainParams(*this, nullptr, Identifier("Params"), createParameterLayout())
{

    network = std::make_unique<MatsuokaEngine>((int)getSampleRate(), false, false, false);
    //network->setFreqCompensation(0.973200f);
    //network->calibrate();

    //allGrains.ensureStorageAllocated(1000);
    formatManager.registerBasicFormats();
    numSamples = chooseFile();

    for (int nodeID = 0; nodeID <= NUM_NODES; ++nodeID) {
        network->setNodeQuantiser_Grid(nodeID, MatsuokaEngine::gridType::unQuantised);
        if (nodeID != 0) {
            network->addChild(0, nodeID);
        }
    }

    std::vector<float> curvX = { DEFAULT_CURVE_X };
    std::vector<float> curvY = { DEFAULT_CURVE_Y };


    network->loadConnectionWeightCurve(curvX, curvY);
    network->setUnityConnectionWeight(UNITY_CONN_WEIGHT);
    network->setConnectionWeightScaling(true);
    network->setFreqCompensation(P_COMPENSATION);

    for (int nodeID = 0; nodeID <= NUM_NODES; ++nodeID) {
        network->setNodeFrequency(nodeID, 1.0, false);
        for (int connectToID = 0; connectToID < NUM_NODES; ++connectToID) {
            if (nodeID != connectToID) {
                network->setConnection(nodeID, connectToID, 0.0);
            }
        }
    }
    network->setNodeFrequency(0, 10000.0, true);
    network->doQueuedActions();

    for (int i = 0; i <= NUM_NODES; i++) {
        grainParams.addParameterListener(String(i) + "frequency", this);
        grainParams.addParameterListener(String(i) + "x", &posListener);
        grainParams.addParameterListener(String(i) + "y", &posListener);
        float* length, * startTime, * pan, * vol;
        length = grainParams.getRawParameterValue(String(i) + "grainLength");
        startTime = grainParams.getRawParameterValue(String(i) + "startTime");
        pan = grainParams.getRawParameterValue(String(i) + "pan");
        vol = grainParams.getRawParameterValue(String(i) + "volume");
        generators[i].setStatePointers(length, startTime, pan, vol);
    }

    network->calibrate();
    //network.addChild(0, 1);
    //generators.add(std::unique_ptr<GrainGenerator>());
    
    //allGrains.add(new Grain(1000, 441000));
}

CpggrainsAudioProcessor::~CpggrainsAudioProcessor()
{
}

//==============================================================================
const String CpggrainsAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CpggrainsAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CpggrainsAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CpggrainsAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CpggrainsAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CpggrainsAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CpggrainsAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CpggrainsAudioProcessor::setCurrentProgram (int index)
{
}

const String CpggrainsAudioProcessor::getProgramName (int index)
{
    return {};
}

void CpggrainsAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void CpggrainsAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (int i = 0; i <= NUM_NODES; i++) {
        generators[i].setSampleRateAndBufferSize(getSampleRate(), numSamples);
    }
    network->setSampleRate(getSampleRate());
    //network->calibrate();
}

void CpggrainsAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CpggrainsAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif
void CpggrainsAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto* leftPointer = buffer.getWritePointer(0);
    auto* rightPointer = buffer.getWritePointer(1);
    buffer.clear();
    network->doQueuedActions();
    for (int i = 0; i < buffer.getNumSamples(); i++) {
        for (int j = 1; j <= NUM_NODES; j++) {
            generators[j].checkTrigger(j, network->getNodeOutput(j));
            generators[j].synthesizeGrains(samplePointer, leftPointer, rightPointer, i);
        }
        network->stepBareBones();
        //leftPointer[i] = network->getNodeOutput(1);
        //rightPointer[i] = network->getNodeOutput(2);
    }
}

//==============================================================================
bool CpggrainsAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* CpggrainsAudioProcessor::createEditor()
{
    return new CpggrainsAudioProcessorEditor (*this, grainParams);
}

//==============================================================================
void CpggrainsAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CpggrainsAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void CpggrainsAudioProcessor::parameterChanged(const String& parameterID, float newValue)
{
    auto nodeNumber = parameterID.getCharPointer();
    int node = nodeNumber.getIntValue64();
    network->setNodeFrequency(node, newValue, false);
}

void CpggrainsAudioProcessor::setNodeFrequency(int nodeNumber, double freq)
{
    //network->calibrate();
}

void CpggrainsAudioProcessor::setWeight(int parentNode, int childNode, float value)
{
    network->setConnection(childNode, parentNode, value);
}

int CpggrainsAudioProcessor::chooseFile()
{                                                                    // [1]
    /*FileChooser chooser("Select a Wave file shorter than 2 seconds to play...",
        {},
        "*.wav");

    if (chooser.browseForFileToOpen())
    {*/
        File file{ "C:/Users/Alex/Documents/MProj/CPGrains/Samples/Guitar.wav" };
        //auto file = chooser.getResult();
        std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(file)); // [2]

        if (reader.get() != nullptr)
        {
            auto duration = reader->lengthInSamples / reader->sampleRate;                 // [3]
            fileBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);  // [4]
            reader->read(&fileBuffer,                                                      // [5]
                0,                                                                //  [5.1]
                (int)reader->lengthInSamples,                                    //  [5.2]
                0,                                                                //  [5.3]
                true,                                                             //  [5.4]
                true);                                                            //  [5.5]
            samplePointer = fileBuffer.getReadPointer(0);
            sampleSR = reader->sampleRate;
            return reader->lengthInSamples;
        }
   // }
}

AudioProcessorValueTreeState::ParameterLayout CpggrainsAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<AudioParameterFloat>> params;
    for (int i = 0; i <= NUM_NODES; i++) {
        params.push_back(std::make_unique<AudioParameterFloat>(
            String(i) + "grainLength",
            "Grain Length" + String(i),
            NormalisableRange<float>(0.001f, 5000.0f, 0.001f),
            50.0f,
            String(),
            AudioProcessorParameter::genericParameter,
            [](float value, int) { return String(value, 1); },
            [](const String& text) { return text.getFloatValue();}
            ));
        params.push_back(std::make_unique<AudioParameterFloat>(
            String(i) + "startTime",
            "Start Time" + String(i),
            NormalisableRange<float>(0.0f, 10000.0f, 0.001f),
            5000.0f,
            String(),
            AudioProcessorParameter::genericParameter,
            [](float value, int) { return String(value, 1); },
            [](const String& text) { return text.getFloatValue();}
        ));
        params.push_back(std::make_unique<AudioParameterFloat>(
            String(i) + "frequency",
            "Frequency" + String(i),
            NormalisableRange<float>(0.03125f, 200.0f, 0.01f, 1.0f),
            2.0f,
            String(),
            AudioProcessorParameter::genericParameter,
            [](float value, int) { return String(value, 1); },
            [](const String& text) { return text.getFloatValue();}));
        params.push_back(std::make_unique<AudioParameterFloat>(
            String(i) + "pan",
            "Pan" + String(i),
            NormalisableRange<float>(-1.0f, 1.0f, 0.001f, 1.0f),
            0.0f,
            String(),
            AudioProcessorParameter::genericParameter,
            [](float value, int) { return String(value, 1); },
            [](const String& text) { return text.getFloatValue();}));
        params.push_back(std::make_unique<AudioParameterFloat>(
            String(i) + "volume",
            "Volume" + String(i),
            NormalisableRange<float>(0.0f, 1.0f, 0.001f, 1.0f),
            0.7f,
            String(),
            AudioProcessorParameter::genericParameter,
            [](float value, int) { return String(value, 1); },
            [](const String& text) { return text.getFloatValue();}));
        params.push_back(std::make_unique<AudioParameterFloat>(
            String(i) + "x",
            "XPos" + String(i),
            NormalisableRange<float>(0.0f, 1.0f, 0.001f, 1.0f),
            0.5f,
            String(),
            AudioProcessorParameter::genericParameter,
            [](float value, int) { return String(value, 1); },
            [](const String& text) { return text.getFloatValue();}));
        params.push_back(std::make_unique<AudioParameterFloat>(
            String(i) + "y",
            "YPos" + String(i),
            NormalisableRange<float>(0.0f, 1.0f, 0.001f, 1.0f),
            0.5f,
            String(),
            AudioProcessorParameter::genericParameter,
            [](float value, int) { return String(value, 1); },
            [](const String& text) { return text.getFloatValue();}));
    }
    return { params.begin(), params.end() };
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CpggrainsAudioProcessor();
}
