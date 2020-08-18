/*
  ==============================================================================

    CPGLookAndFeel.h
    Created: 16 Jun 2020 12:03:55pm
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Identifiers.h"

class CPGLookAndFeel : public LookAndFeel_V4 {
public:
    CPGLookAndFeel() {
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour((uint32) 0xff264653));
        setColour(juce::Slider::thumbColourId, Colours::red);
        setColour(juce::TextButton::buttonColourId, juce::Colour((uint32) 0xff0c2b38));
    }
    /*void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
    {
        //...
    }*/
    Label* createSliderTextBox(Slider& slider) override
    {
        Label* l = LookAndFeel_V4::createSliderTextBox(slider);
        //l->setBorderSize(BorderSize<int>(2, 2, 40, 2));
        l->setColour(Label::outlineColourId, Colours::transparentWhite);
        l->setColour(Label::outlineWhenEditingColourId, Colours::transparentWhite);
        return l;
    }

    Slider::SliderLayout getSliderLayout(Slider& slider) override
    {
        int minXSpace = 0;
        int minYSpace = 0;

        auto textBoxPos = slider.getTextBoxPosition();

        if (textBoxPos == Slider::TextBoxLeft || textBoxPos == Slider::TextBoxRight)
            minXSpace = 30;
        else
            minYSpace = 15;

        auto localBounds = slider.getLocalBounds();

        auto textBoxWidth = jmax<int>(0, jmin(slider.getTextBoxWidth(), localBounds.getWidth() - minXSpace));
        auto textBoxHeight = jmax<int>(0, jmin(slider.getTextBoxHeight(), localBounds.getHeight() - minYSpace));

        Slider::SliderLayout layout;

        // 2. set the textBox bounds

        if (textBoxPos != Slider::NoTextBox)
        {
            if (slider.isBar())
            {
                layout.textBoxBounds = localBounds;
            }
            else
            {
                layout.textBoxBounds.setWidth(textBoxWidth);
                layout.textBoxBounds.setHeight(textBoxHeight);

                if (textBoxPos == Slider::TextBoxLeft)           layout.textBoxBounds.setX(0);
                else if (textBoxPos == Slider::TextBoxRight)     layout.textBoxBounds.setX(localBounds.getWidth() - textBoxWidth);
                else /* above or below -> centre horizontally */ layout.textBoxBounds.setX((localBounds.getWidth() - textBoxWidth) / 2);

                if (textBoxPos == Slider::TextBoxAbove)          layout.textBoxBounds.setY(localBounds.getHeight() / 3.5);
                else if (textBoxPos == Slider::TextBoxBelow)     layout.textBoxBounds.setY(localBounds.getHeight() - textBoxHeight);
                else /* left or right -> centre vertically */    layout.textBoxBounds.setY((localBounds.getHeight() - textBoxHeight) / 2);
            }
        }

        // 3. set the slider bounds

        layout.sliderBounds = localBounds;

        if (slider.isBar())
        {
            layout.sliderBounds.reduce(1, 1);   // bar border
        }
        else
        {
            const int thumbIndent = getSliderThumbRadius(slider);

            if (slider.isHorizontal())    layout.sliderBounds.reduce(thumbIndent, 0);
            else if (slider.isVertical()) layout.sliderBounds.reduce(0, thumbIndent);
        }

        return layout;
    }
};
