#include "GUI.hpp"

void Canvas::submit_callback(Fl_Widget * w, void* pixels){
		Fl_Widget *p = w->parent();//get canvas
		
		Fl_Widget *q;
		do{
			q = p->parent();
			if(q) p = q;
		}while(q);

		w->hide();
	}


Canvas::Canvas() : Fl_Box(0,0, INPUTSIZE * SCALE, INPUTSIZE * SCALE){
    Fl_Button *submit = new Fl_Button(INPUTSIZE * SCALE + 100, 0, 40, 20, "SUBMIT");
    
    for (int i = 0; i < INPUTSIZE * INPUTSIZE; i++)
        pixelState[i] = 0;
    submit->callback(submit_callback, (void*)"Hi");
}

int Canvas::handle(int event){
      if (Fl::event_inside(0, 0, w(), h())) {
		switch (event){
		case FL_PUSH:
			std::cout << "pushed\n";
			return 1;
		case FL_DRAG:
			fl_color(FL_DARK_BLUE);
			fl_begin_polygon();
			fl_circle(Fl::event_x(), Fl::event_y(), (SCALE-1)/2);
			pixelState[location(Fl::event_x(), Fl::event_y())] = true;
			fl_end_polygon();
			break;
        case FL_RELEASE: 
			std::cout << "released\n";
			for (int i = 0; i < INPUTSIZE; ++i)
				for (int j = 0; j < INPUTSIZE; ++j)
					inputdraw.push_back(pixelState[INPUTSIZE*i + j] ? 255 : 0);
			break;
		default:
			break;
		}
      }
      return Fl_Box::handle(event);
    }
void Canvas::hide(){
    Canvas::hide();
    exit(0);
}