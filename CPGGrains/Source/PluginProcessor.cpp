/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
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
    for (int i = 0; i < 4; i++) {
        grainParams.addParameterListener(String(i) + "frequency", this);
    }

    network = std::make_unique<MatsuokaEngine>();
    network->setFreqCompensation(0.973200f);
    network->addNode(1);
    network->addNode(2);
    //network->addNode(3);
    //network->addNode(4);
    //network->calibrate();

    //allGrains.ensureStorageAllocated(1000);
    formatManager.registerBasicFormats();
    numSamples = chooseFile();

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
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto* leftPointer = buffer.getWritePointer(0);
    auto* rightPointer = buffer.getWritePointer(1);
    network->doQueuedActions();
    
    for (int i = 0; i < buffer.getNumSamples(); i++) {
        if (grainList.size() <= maxGrains) {
            for (auto node : network->getNodeList()) {
                GrainGenerator* grainGen = &generators[node];
                grainGen->checkTrigger(node, network->getNodeOutput(node));
                int grainChannel = grainGen->getGrain();
                while (grainChannel != -1) {
                    /*int i = 0;
                    Grain* g = &grains[i];
                    while (i < 256 && !g->grainIsPlaying()) {
                        g = &grains[i];
                        i++;
                    }*/
                    float* pan = grainParams.getRawParameterValue(String(grainChannel) + "pan");
                    float* volume = grainParams.getRawParameterValue(String(grainChannel) + "volume");
                    /*g->setStartAndEnd(getStartAndEndSample(grainChannel, numSamples));
                    g->setPan(*pan);
                    g->setVolume(*volume);
                    g->setPlaying(true);*/
                    grainList.push_back(new Grain(getStartAndEndSample(grainChannel, numSamples), *pan, *volume));
                    grainChannel = grainGen->getGrain();
                }
            }
        }
        std::list<Grain*>::iterator it = grainList.begin();
        while (it != grainList.end())
        {
            int grainSample = (*it)->nextSampleIndex();
            if (grainSample == -1)
            {
                grainList.erase(it++);  // alternatively, i = items.erase(i);
            }
            else
            {
                processSample((*it), i, leftPointer, rightPointer, readPointer[grainSample]);
                ++it;
            }
        }

        /*
        for (int j = 0; j < 256; j++) {
            Grain* g = &grains[j];
            int grainSample = g->nextSampleIndex();
            if (grainSample == -1) {
                g->setPlaying(false);
            }
            else if (g->grainIsPlaying()) {
                processSample(g, i, leftPointer, rightPointer, readPointer[grainSample]);
            }
        }

        for (int j = 0; j < allGrains.size(); j++) {
            auto* g = allGrains[j];
            int grainSample = g->nextSampleIndex();
            if (grainSample == -1) {
                allGrains.remove(j);
            }
            else {
                //matsuOutput[i] = 0;
                //writePointer[i] = 0;
                
                //float leftOut = readPointer[grainSample] * std::fmin(1.0 - g->getPan(), 1.0);
                //float rightOut = readPointer[grainSample] * std::fmin(1.0 + *pan, 1.0);
                //outSample += leftOut;
                //RWritePointer[currentIndex] += rightOut;
                processSample(g, i, leftPointer, rightPointer, readPointer[grainSample]);
                //double grainSample = g->nextSampleIndex();
                //grainSample = grainSample * sampleSR / getSampleRate()
                //outSample += readPointer[grainSample];
            }
        }*/
        //writePointer[i] = outSample;
        //matsuOutput[i] = network->getNodeOutput(0);
        //writePointer[i] = network->getNodeOutput(1);
        network->stepBareBones();
    }
}

void CpggrainsAudioProcessor::processSample(Grain* g, int currentIndex, float* LWritePointer, float* RWritePointer, float sampleValue) {
    float leftOut = sampleValue * std::fmin(1.0 - g->getPan(), 1.0);
    float rightOut = sampleValue * std::fmin(1.0 + g->getPan(), 1.0);
    LWritePointer[currentIndex] += leftOut * g->getVolume();
    RWritePointer[currentIndex] += rightOut * g->getVolume();
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
            readPointer = fileBuffer.getReadPointer(0);
            sampleSR = reader->sampleRate;
            return reader->lengthInSamples;
        }
   // }
}

std::tuple<int, int> CpggrainsAudioProcessor::getStartAndEndSample(int nodeNumber, int endOfSample)
{
    float grainLength = grainParams.getParameterAsValue(String(nodeNumber) + "grainLength").getValue();
    float startTime = grainParams.getParameterAsValue(String(nodeNumber) + "startTime").getValue();
    int startSample = startTime / 1000 * getSampleRate();
    int endSample = (int)startSample + (grainLength / 1000 * getSampleRate());
    endSample = std::min(endSample, endOfSample);
    return std::make_tuple<>(startSample, endSample);
}

AudioProcessorValueTreeState::ParameterLayout CpggrainsAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<AudioParameterFloat>> params;
    for (int i = 0; i <= 4; i++) {
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
            NormalisableRange<float>(0.03125f, 16.0f, 0.01f, 1.0f),
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
    }
    return { params.begin(), params.end() };
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CpggrainsAudioProcessor();
}
