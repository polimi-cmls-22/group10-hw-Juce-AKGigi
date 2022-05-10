#include <JuceHeader.h>

class Point
{
public:
	Point(float x, float y);
	~Point();
	float getX();
	float getY();
private:
	float x;
	float y;
};

class PointsVectorToDraw
{
public:
	PointsVectorToDraw(juce::Colour lineColour);
	~PointsVectorToDraw();
	void push(Point point);
	std::vector<Point> getPoints();
	juce::Colour getLineColour();
private:
	juce::Colour lineColour;
	std::vector<Point> points;
};


class FunctionGraph
{
public:
	FunctionGraph(juce::Rectangle<int> graphRegion, juce::Colour bgColour);
	~FunctionGraph();
	void paint(juce::Graphics& g);
	void resize(juce::Graphics& g);
	void push(PointsVectorToDraw points);
private:
	juce::Rectangle<int> graphRegion;
	std::vector<PointsVectorToDraw> pointsSet;
	juce::Colour bgColour;
};