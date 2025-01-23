#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Draw.H>
#include <FL/fl_ask.H>
#include <vector>
#include <cmath>
#include <algorithm>
#include "SomeCamera.hpp"
#include "SomeLine.hpp"
#include "SomeTriangle.hpp"

using namespace dxvll_space_2d;
using namespace std;

enum Mode { DRAW_LINES, DRAW_TRIANGLES, NAVIGATE };
Mode currentMode = NAVIGATE;

void help_callback(Fl_Widget* widget, void* data) {
    fl_message(
        "How to use the program:\n"
        "- Select a mode using the radio buttons:\n"
        "  1. Draw Lines: Left mouse button drag to draw lines. \n"
        "    Right mouse button drag to remove lines which intersects with this which was made by drag.\n"
        "  2. Draw Triangles: Click left mouse button three times to form a triangle. \n"
        "    Right mouse button drag to remove triangles which intersects with line made by drag.\n"
        "  3. Navigate: Drag to move the canvas, scroll to zoom.\n"
        "\n"
    );
}

bool ccw(PointV A, PointV B, PointV C) {
    return (C.y() - A.y()) * (B.x() - A.x()) > (B.y() - A.y()) * (C.x() - A.x());
}

bool intersect(const SomeLine& l1, const SomeLine& l2) {
    PointV A = l1.p1, B = l1.p2;
    PointV C = l2.p1, D = l2.p2;
    return ccw(A, C, D) != ccw(B, C, D) && ccw(A, B, C) != ccw(A, B, D);
}


class CanvasWidget : public Fl_Widget {
private:
    SomeCamera camera;
    vector<SomeLine> lines;
    vector<SomeTriangle> triangles;
    vector<PointV> triangleVertices; // Temp vertices for triangles

    SomeLine tempLine;
    bool drawingTempLine = false;

    struct {
        bool active = false;
        PointV start;
        PointV cameraStart;
        int startX, startY;
    } drag;

public:
    CanvasWidget(int X, int Y, int W, int H) : Fl_Widget(X, Y, W, H), tempLine() {}

    void setCamera(SomeCamera camera) { this->camera = camera; }
    vector<SomeLine>& getLines() { return this->lines; }
    vector<SomeTriangle>& getTriangles() { return this->triangles; }

    void drawLine(const SomeLine& l) {
        fl_line(x() + camera.convertX(l.p1.x()), y() + camera.convertY(l.p1.y()), 
                x() + camera.convertX(l.p2.x()), y() + camera.convertY(l.p2.y()));
    }

    void drawTriangle(const SomeTriangle& t) {
        PointV p1 = t.getPoint(1), p2 = t.getPoint(2), p3 = t.getPoint(3);
        fl_polygon(
            x() + camera.convertX(p1.x()), y() + camera.convertY(p1.y()), 
            x() + camera.convertX(p2.x()), y() + camera.convertY(p2.y()), 
            x() + camera.convertX(p3.x()), y() + camera.convertY(p3.y()));
    }

    void drawTempLine(const SomeLine& l) {
        fl_color(FL_RED);
        drawLine(l);
        fl_color(FL_BLACK);
    }

    void draw() override {
        fl_color(FL_WHITE);
        fl_rectf(x(), y(), w(), h());

        fl_color(FL_BLACK);
        for (const auto& line : lines) drawLine(line);
        for (const auto& triangle : triangles) drawTriangle(triangle);
        if (drawingTempLine) drawTempLine(tempLine);
    }

    int handle(int event) override {
        int mx = Fl::event_x() - x(), my = Fl::event_y() - y();

        switch (event) {
            case FL_PUSH: {
                if (currentMode == NAVIGATE) {
                    if (Fl::event_button() == FL_LEFT_MOUSE) {
                        drag.active = true;
                        drag.startX = mx;
                        drag.startY = my;
                        drag.cameraStart.x() = camera.getX();
                        drag.cameraStart.y() = camera.getY();
                        redraw();
                        return 1;
                    }
                } else if (currentMode == DRAW_TRIANGLES && Fl::event_button() == FL_LEFT_MOUSE) {
                    PointV newPoint(camera.uncoverX(mx), camera.uncoverY(my));
                    triangleVertices.push_back(newPoint);
                    if (triangleVertices.size() == 3) {
                        triangles.emplace_back(triangleVertices[0], triangleVertices[1], triangleVertices[2]);
                        triangleVertices.clear();
                    }
                    redraw();
                    return 1;
                } else if (currentMode == DRAW_TRIANGLES && Fl::event_button() == FL_RIGHT_MOUSE) {
                    drawingTempLine = true;
                    tempLine = SomeLine(PointV(camera.uncoverX(mx), camera.uncoverY(my)), PointV(0, 0));
                    redraw();
                    return 1;
                } else if (currentMode == DRAW_LINES && Fl::event_button() == FL_LEFT_MOUSE) {
                    drag.active = true;
                    drag.start.x() = camera.uncoverX(mx);
                    drag.start.y() = camera.uncoverY(my);
                    return 1;
                } else if (currentMode == DRAW_LINES && Fl::event_button() == FL_RIGHT_MOUSE) {
                    drawingTempLine = true;
                    tempLine = SomeLine(PointV(camera.uncoverX(mx), camera.uncoverY(my)), PointV(0, 0));
                    redraw();
                    return 1;
                }
                return 0;
            }
            case FL_DRAG: {
                if (currentMode == NAVIGATE && drag.active) {
                    PointV bias(
                        camera.uncoverX(mx) - camera.uncoverX(drag.startX), 
                        camera.uncoverY(my) - camera.uncoverY(drag.startY));
                    camera.setX(drag.cameraStart.x() - bias.x());
                    camera.setY(drag.cameraStart.y() - bias.y());
                    redraw();
                    return 1;
                } else if (currentMode == DRAW_LINES && Fl::event_button() == FL_RIGHT_MOUSE) {
                    tempLine.p2.x() = camera.uncoverX(mx);
                    tempLine.p2.y() = camera.uncoverY(my);
                    redraw();
                    return 1;
                } else if (currentMode == DRAW_TRIANGLES && Fl::event_button() == FL_RIGHT_MOUSE) {
                    tempLine.p2.x() = camera.uncoverX(mx);
                    tempLine.p2.y() = camera.uncoverY(my);
                    redraw();
                    return 1;
                }
                return 0;
            }
            case FL_RELEASE: {
                if (currentMode == NAVIGATE && drag.active) {
                    drag.active = false;
                    redraw();
                    return 1;
                } else if (currentMode == DRAW_LINES && Fl::event_button() == FL_LEFT_MOUSE) {
                    PointV end(camera.uncoverX(mx), camera.uncoverY(my));
                    lines.emplace_back(drag.start, end);
                    redraw();
                    return 1;
                } else if (currentMode == DRAW_LINES && Fl::event_button() == FL_RIGHT_MOUSE) {
                    lines.erase(remove_if(lines.begin(), lines.end(), [&](const SomeLine& l) {
                        return intersect(tempLine, l); // Implement `intersect` for line intersection
                    }), lines.end());
                    drawingTempLine = false;
                    redraw();
                    return 1;
                } else if (currentMode == DRAW_TRIANGLES && Fl::event_button() == FL_RIGHT_MOUSE) {
                    // Delete all triangles intersecting the temporary red line
                    triangles.erase(
                        remove_if(triangles.begin(), triangles.end(), [&](const SomeTriangle& t) {
                            SomeLine edges[] = {
                                SomeLine(t.getPoint(1), t.getPoint(2)),
                                SomeLine(t.getPoint(2), t.getPoint(3)),
                                SomeLine(t.getPoint(3), t.getPoint(1))
                            };
                            return any_of(begin(edges), end(edges), [&](const SomeLine& edge) {
                                return intersect(tempLine, edge);
                            });
                        }),
                        triangles.end()
                    );
                    drawingTempLine = false;
                    redraw();
                    return 1;
                }
                return 0;
            }
            case FL_MOUSEWHEEL: {
                if (currentMode == NAVIGATE) {
                    int scroll_dx = Fl::event_dy();
                    camera.zoom(scroll_dx < 0 ? 1.1 : 1.0 / 1.1);
                    redraw();
                    return 1;
                }
                return 0;
            }
            default:
                return Fl_Widget::handle(event);
        }
    }
};

void mode_callback(Fl_Widget* widget, void* data) {
    currentMode = static_cast<Mode>(reinterpret_cast<intptr_t>(data));
}

int main() {
    Fl_Double_Window* window = new Fl_Double_Window(1200, 700, "Navi");
    CanvasWidget* canvas = new CanvasWidget(5, 50, 1100, 600);
    SomeCamera camera(3.0, 1.5, 150.0, 1100, 600);
    canvas->setCamera(camera);

    Fl_Button* help = new Fl_Button(840, 5, 50, 40, "Help");
    help->callback(help_callback);

    Fl_Group* radioGroup = new Fl_Group(900, 5, 300, 40);
    Fl_Radio_Round_Button* navButton = new Fl_Radio_Round_Button(910, 5, 90, 40, "Navigate");
    navButton->callback(mode_callback, reinterpret_cast<void*>(NAVIGATE));
    navButton->set();
    Fl_Radio_Round_Button* drawLinesButton = new Fl_Radio_Round_Button(1000, 5, 90, 40, "Draw Lines");
    drawLinesButton->callback(mode_callback, reinterpret_cast<void*>(DRAW_LINES));
    Fl_Radio_Round_Button* drawTrianglesButton = new Fl_Radio_Round_Button(1090, 5, 100, 40, "Draw Triangles");
    drawTrianglesButton->callback(mode_callback, reinterpret_cast<void*>(DRAW_TRIANGLES));
    radioGroup->end();

    window->end();
    window->show();
    return Fl::run();
}
