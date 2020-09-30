
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../GlobalIncludes.h"
#define INLAY_DEF 1

class GlasDisplay : public SettableTooltipClient, public Component {
public:
	GlasDisplay();
	~GlasDisplay();

	void paint(Graphics &) override;
	void setImage(juce::Image p_panel) {
		m_glas_panel = p_panel;
	}
	void setText(std::string p_text) {
		m_text_no_suffix = p_text;
		m_text           = m_text_no_suffix + m_text_value_suffix;
		repaint();
	}
	void setInlay(int p_inlay) {
		m_inlay = p_inlay;
	}
	void setInlayTop(int p_inlay) {
		m_inlay_top = p_inlay;
	}

	void setColor(juce::Colour p_color) {
		m_color = p_color;
	}

	void mouseDown(const MouseEvent &event) override;
	void mouseDrag(const MouseEvent &event) override;
	void mouseUp(const MouseEvent &event) override;
	void setTextValueSuffix(std::string p_suffix) {
		m_text_value_suffix = p_suffix;
		m_text              = m_text_no_suffix + m_text_value_suffix;
	}

	std::function<void()> onMouseDown                         = []() {}; // overwriteable with lambda
	std::function<void(const MouseEvent &)> toParentMouseDown = [](const MouseEvent &) {};
	std::function<void(const MouseEvent &)> toParentMouseDrag = [](const MouseEvent &) {};
	std::function<void(const MouseEvent &)> toParentMouseUp   = [](const MouseEvent &) {};

  void setTextOffsetTop(int p_offset){
    m_text_offset_top = p_offset;
  }

  void setTextOffsetLeft(int p_offset){
    m_text_offset_left = p_offset;
  }

	void setGUIBig() {
		m_GUI_big = true;
		m_menu_feels.setGUIBig();
	}
	void setGUISmall() {
		m_GUI_big = false;
		m_menu_feels.setGUISmall();
	}

private:
	bool m_GUI_big                  = false;

  	int m_text_offset_top = 0;
  	int m_text_offset_left = 0;

	std::string m_text_value_suffix = "";

	juce::Colour m_color = juce::Colours::black;
	int m_inlay          = INLAY_DEF;
	int m_inlay_top      = 0;
	juce::Image m_glas_panel;
	std::string m_text           = "text";
	std::string m_text_no_suffix = "text";
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GlasDisplay)

	OdinMenuFeels m_menu_feels;
};