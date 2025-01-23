#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Draw.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_BMP_Image.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_File_Chooser.H> 
#include <FL/fl_ask.H> 

#include <vector>
#include <iostream>
#include <cmath>
#include "SomeCamera.hpp"
#include "SomeLine.hpp"
#include "SomeTriangle.hpp"

#define VERY_BIG_DOUBLE 9999999999999
using namespace dxvll_space_2d;
using namespace std;

void help_callback(Fl_Widget* widget, void* data) {
    fl_message("This is ???.\n");
}


#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

void createConsole() {
#ifdef _WIN32
    AllocConsole();
    FILE* console;
    freopen_s(&console, "CONOUT$", "w", stdout);
    freopen_s(&console, "CONOUT$", "w", stderr);
    freopen_s(&console, "CONIN$", "r", stdin);
#endif
}

class CanvasNavigateWidget : public Fl_Widget {
private:
    SomeCamera camera;
    vector<SomeLine> lines;
    vector<SomeTriangle> triangles;

    struct {
        bool active = false;
        PointV start;
        PointV cameraStart;
        int startX;
        int startY;
    } drag;

public:
    CanvasNavigateWidget(int X, int Y, int W, int H) : Fl_Widget(X, Y, W, H) {}

    void setCamera(SomeCamera camera) {
        this->camera = camera;
    }

    void setLines(vector<SomeLine>& lines) {
        this->lines = lines;
    }

    vector<SomeLine>& getLines() {
        return this->lines;
    }

    void setTriangles(vector<SomeTriangle>& triangles) {
        this->triangles = triangles;
    }

    vector<SomeTriangle>& getTriangles() {
        return this->triangles;
    }

    void drawLine(SomeLine l) {
        fl_line(x() + camera.convertX(l.p1.x()), y() + camera.convertY(l.p1.y()), 
                x() + camera.convertX(l.p2.x()), y() + camera.convertY(l.p2.y()));
    }

    void drawTriangle(SomeTriangle t) {
        PointV p1 = t.getPoint(1);
        PointV p2 = t.getPoint(2);
        PointV p3 = t.getPoint(3);
        int x1 = x() + camera.convertX(p1.x());
        int y1 = y() + camera.convertY(p1.y());
        int x2 = x() + camera.convertX(p2.x());
        int y2 = y() + camera.convertY(p2.y());
        int x3 = x() + camera.convertX(p3.x());
        int y3 = y() + camera.convertY(p3.y());
        fl_polygon(x1, y1, x2, y2, x3, y3);
    }

    void draw() override {
        fl_color(FL_WHITE);
        fl_rectf(x(), y(), w(), h());

        fl_color(FL_BLACK);
        for (const auto& line : lines) {
            drawLine(line);
        }
        for (const auto& triangle : triangles) {
            drawTriangle(triangle);
        }
    }

    int handle(int event) override {
        switch (event) {
            case FL_PUSH: {
                int mx = Fl::event_x() - x();
                int my = Fl::event_y() - y();

                if (Fl::event_button() == FL_LEFT_MOUSE) {
                    if ((mx < w()) && (my < h())) {
                        drag.active = true;
                        drag.startX = mx;
                        drag.startY = my;
                        drag.cameraStart.x() = camera.getX();
                        drag.cameraStart.y() = camera.getY();
                        redraw();
                        return 1;
                    }
                }
                return 0;
            }
            case FL_DRAG: {
                if (drag.active) {
                    int mx = Fl::event_x() - x();
                    int my = Fl::event_y() - y();
                    double biasX = camera.uncoverX(mx) - camera.uncoverX(drag.startX);
                    double biasY = camera.uncoverY(my) - camera.uncoverY(drag.startY);

                    camera.setX(drag.cameraStart.x() - biasX);
                    camera.setY(drag.cameraStart.y() - biasY);
                    redraw();
                    return 1;
                }
                return 0;
            }
            case FL_RELEASE: {
                if (Fl::event_button() == FL_LEFT_MOUSE) {
                    drag.active = false;
                    redraw();
                    return 1;
                }
                return 0;
            }
            case FL_MOUSEWHEEL: {
                int scroll_dx = Fl::event_dy();
                if (scroll_dx < 0) {
                    camera.zoom(1.1);
                } else if (scroll_dx > 0) {
                    camera.zoom(1.0 / 1.1);
                }
                redraw();
                return 1;
            }
            default:
                return Fl_Widget::handle(event);
        }
        return 0;
    }
};

int main() {
    createConsole();
    Fl_Double_Window* window = new Fl_Double_Window(1200, 700, "Navi.");

    CanvasNavigateWidget* canvas = new CanvasNavigateWidget(5, 50, 1100, 600);
    SomeCamera camera(3.0, 1.5, 150.0, 1100, 600);
    vector<SomeLine>& lines = canvas->getLines();
    lines.push_back(SomeLine(PointV(0.0, 0.0), PointV(-1.0, -1.0)));

    canvas->setCamera(camera);

    Fl_Button* help = new Fl_Button(840, 5, 50, 40, "Help");
    help->callback(help_callback);

    window->end();
    window->show();
    return Fl::run();
}
