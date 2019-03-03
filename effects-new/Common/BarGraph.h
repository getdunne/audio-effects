#pragma once

#include "JuceHeader.h"

class BarGraph	: public Component
{
public:
    BarGraph(Colour c) : colour(c), value(0.0f) {}
    virtual ~BarGraph() = default;

	// Component
	void paint(Graphics&) override;

	// BarGraph
	void setValue(float v);
    float getValue() { return value; }

private:
    Colour colour;
    float value;
};
