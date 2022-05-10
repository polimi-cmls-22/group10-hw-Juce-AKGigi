#include "functionGraph.h"

//Point (a single point)
Point::Point(float x, float y)
{
	this->x = x;
	this->y = y;
}

Point::~Point()
{
}

float Point::getX()
{
	return this->x;
}

float Point::getY()
{
	return this->y;
}

//Vector of Points
PointsVectorToDraw::PointsVectorToDraw(juce::Colour lineColour)
{
	this->lineColour = lineColour;
}

PointsVectorToDraw::~PointsVectorToDraw()
{
}

void PointsVectorToDraw::push(Point point)
{
	this->points.push_back(point);
}

std::vector<Point> PointsVectorToDraw::getPoints()
{
	return this->points;
}

juce::Colour PointsVectorToDraw::getLineColour()
{
	return this->lineColour;
}

//Graph (create a graph from vector of points)
FunctionGraph::FunctionGraph(juce::Rectangle<int> graphRegion, juce::Colour bgColour)
{
	this->graphRegion = graphRegion;
	this->bgColour = bgColour;
}

FunctionGraph::~FunctionGraph()
{
}

void FunctionGraph::push(PointsVectorToDraw points)
{
	this->pointsSet.push_back(points);
}


void FunctionGraph::paint(juce::Graphics& g)
{
	g.setColour(bgColour);
	g.fillRect(graphRegion.getX()-10, graphRegion.getY()-10, graphRegion.getWidth()+20, graphRegion.getHeight()+20);
	g.setColour(juce::Colours::black);
	g.drawRoundedRectangle(graphRegion.getX()-12, graphRegion.getY()-12, graphRegion.getWidth()+24, graphRegion.getHeight()+24, 5, 3);
	g.drawRect(graphRegion);
	
	juce::Line<float> linex = juce::Line<float>(graphRegion.getCentreX(), graphRegion.getBottom(), graphRegion.getCentreX(), graphRegion.getY());
	juce::Line<float> liney = juce::Line<float>(graphRegion.getX(), graphRegion.getCentreY(), graphRegion.getRight(), graphRegion.getCentreY());
	float len[] = { 4, 2 };
	g.drawDashedLine(linex, len, 2);
	g.drawDashedLine(liney, len, 2);

	for (int i = 0; i < pointsSet.size(); i++) {
		g.setColour(pointsSet[i].getLineColour());
		juce::Path line;
		for (int j = 0; j < pointsSet[i].getPoints().size(); j++) {
			float x = pointsSet[i].getPoints()[j].getX();
			float y = pointsSet[i].getPoints()[j].getY();
			if (j == 0)
				line.startNewSubPath((((x + 1) / 2) * graphRegion.getWidth()) + graphRegion.getX(), (((1-y) / 2) * graphRegion.getHeight()) + graphRegion.getY());
			else
				line.lineTo((((x + 1) / 2) * graphRegion.getWidth()) + graphRegion.getX(), (((1-y) / 2) * graphRegion.getHeight()) + graphRegion.getY());
		}
		g.strokePath(line, juce::PathStrokeType(2.0f));
	}
}

void FunctionGraph::resize(juce::Graphics& g)
{
}


