#include <JuceHeader.h>
class FilmStripKnob : public juce::Slider
{
public:
	FilmStripKnob(juce::Image image, const int numFrames, const bool stripIsHorizontal)
		: Slider(),
		filmStrip(image),
		numFrames_(numFrames),
		isHorizontal_(stripIsHorizontal)
	{
		if (!filmStrip.isNull())
		{
			setTextBoxStyle(NoTextBox, 0, 0, 0);
			if (isHorizontal_) {
				frameHeight = filmStrip.getHeight();
				frameWidth = filmStrip.getWidth() / numFrames_;
			}
			else {
				frameHeight = filmStrip.getHeight() / numFrames_;
				frameWidth = filmStrip.getWidth();
			}
		}
	}

	~FilmStripKnob() {}

	void paint(juce::Graphics& g)
	{
		if (!filmStrip.isNull()) {
			int value = (getValue() - getMinimum()) / (getMaximum() - getMinimum()) * (numFrames_ - 1);
			if (isHorizontal_) {
				g.drawImage(filmStrip, 0, 0, getWidth(), getHeight(),
					value * frameWidth, 0, frameWidth, frameHeight);
			}
			else {
				g.drawImage(filmStrip, 0, 0, getWidth(), getHeight(),
					0, value * frameHeight, frameWidth, frameHeight);
			}
		}
	}

	int getFrameWidth() const { return !filmStrip.isNull() ? frameWidth : 100; }
	int getFrameHeight() const { return !filmStrip.isNull() ? frameHeight : 24; }
private:
	juce::Image filmStrip;
	const int numFrames_;
	const bool isHorizontal_;
	int frameWidth, frameHeight;
};