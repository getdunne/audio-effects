#include "BarGraph.h"

void BarGraph::paint (Graphics& g)
{
    auto area = getLocalBounds();

    g.setColour(Colours::black);
    g.fillRect(area);

    if (value > 0.0f)
    {
        g.setColour(colour);
        if (area.getWidth() > area.getHeight())
        {
            g.fillRect(Rectangle<int>(area.getX(),
                area.getY(),
                int(value * area.getWidth()),
                area.getHeight()));
        }
        else
        {
            g.fillRect(Rectangle<int>(area.getX(),
                area.getY() + int((1.0f - value) * area.getHeight()),
                area.getWidth(),
                int(value * area.getHeight())));
        }
    }

    g.setColour(Colours::white);
    g.drawRect(area);
}

void BarGraph::setValue(float v)
{
    value = v;
    if (value < 0.0f) value = 0.0f;
    if (value > 1.0f) value = 1.0f;
	repaint();
}
