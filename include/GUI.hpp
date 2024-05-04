#ifndef GUI_HPP
#define GUI_HPP

#include <iostream>
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Box.H>
#include "Constants.hpp"
#include <vector>

class Canvas : public Fl_Box{
	
	// (x,y) = INPUTSIZE*y + x
	bool pixelState[INPUTSIZE * INPUTSIZE];
	
	int location(int x, int y){
		return INPUTSIZE*(y/SCALE) + x/SCALE;
	}
public:
	std::vector<double> inputdraw;
	Canvas();
	static void submit_callback(Fl_Widget * w, void* pixels);
	int handle(int event) override;
	void hide();
};

class MainWindow : public Fl_Window{
	Canvas * canvas = new Canvas();
public:
	MainWindow() : Fl_Window(WINDOWWIDTH,WINDOWWIDTH){
		canvas->label("Draw here");
		canvas->color(FL_YELLOW);
		canvas->box(FL_DOWN_BOX);
	}
	Canvas* getCanvas(){
        return canvas;
    }
};

#endif