/*
  ==============================================================================

    StyleSheet.h
    Created: 16 Jun 2023 4:55:24pm
    Author:  dog1

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
namespace juce
{
    class CustomLNF : public LookAndFeel_V4
    {
    public:
        void drawLinearSlider(Graphics& g, int x, int y, int width, int height,
            float sliderPos,
            float minSliderPos,
            float maxSliderPos,
            const Slider::SliderStyle style, Slider& slider);

    };

};