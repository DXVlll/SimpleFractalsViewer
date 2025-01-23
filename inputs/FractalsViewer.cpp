#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_Button.H>
#include <FL/fl_ask.H> 
#include <FL/Fl_Draw.H>
#include <FL/Fl_File_Chooser.H> 

#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <fstream>

#include "SomeCamera.hpp"
#include "SomeTransform.hpp"
#include "SomeLine.hpp"
#include "SomeTriangle.hpp"

using namespace dxvll_space_2d;
using namespace std;

class CanvasFractalWidget;
class CanvasDrawWidget;

const double MATH_PI = 3.141592653589793238462643;

vector<SomeTransform> transforms;
//vector<PointV> origins;
int currentTransformIndex = 0;

void updateTransformFields(Fl_Widget* widget, void* userData);
void updateTransformFromFields();

Fl_Float_Input *rotAngleInput, *scaleXInput, *scaleYInput, *translateXInput, *translateYInput;
Fl_Choice *currentTransformChoice;
Fl_Spinner *transformsSpinner;
Fl_Spinner *iterationsSpinner;
long iterationsCount;

CanvasFractalWidget *fractalsView;
CanvasDrawWidget *drawStartFigureView;


void help_callback(Fl_Widget* widget, void* data) {
    fl_message("This is fractals viewer.\n"
                "1) Program makes fractals by iterativelly applying given transforms to start figure.\n"
                "2) You can change current transform by changing values 'Rotation angle', 'Scale X', 'Scale Y', 'Translate X' and 'Translate Y' \n"
                "3) You can change current transform by using 'Current Transform' dropdown.\n"
                "4) You can change number of transforms by changing 'Number of transforms' field. \n"
                "5) You can change iterations count by changing 'Iterations count' field. \n"
                "6) You can save transforms to file by using 'Save Transforms' buttton (don't forget to give '.txt'-type name to file).\n"
                "7) You can load transforms from file by using 'Load Transforms' button (You will only see .txt files).\n"
                "8) Press 'Close' to see second part of help. \n"
                "");
    fl_message("9) You can draw starting figure in left down corner. How to do it:\n"
                "9.1) Select a mode using the radio buttons:\n"
                "9.1a) Draw Lines: Left mouse button drag to draw lines.\n"
                "       Right mouse button drag to remove lines which intersects with this which was made by drag.\n"
                "9.1b) Draw Triangles: Click left mouse button three times to form a triangle. \n"
                "    Right mouse button drag to remove triangles which intersects with line made by drag.\n"
                "9.1c) Navigate: Left mouse button drag to move the canvas, scroll to zoom.\n"
                "    Right click to select placement of origin point. When program apply rotating or scaling, it use origin from here.\n"
                "10) You can navigate in big view by dragging it with left mouse button and by using mouse wheel. \n"
                "11) Author of this program - Darastin. \n");
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

Fl_Box *originPositionInfo; // text field where cusrsor position is displayed
char originPositionText[25];


class CanvasDrawWidget : public Fl_Widget {
public:
    enum Mode { DRAW_LINES, DRAW_TRIANGLES, NAVIGATE };
private:
    SomeCamera camera;
    vector<SomeLine> lines;
    vector<SomeTriangle> triangles;
    vector<PointV> drawingTriangleVertices; // Temp vertices for triangles
    PointV origin = PointV(0.0, 0.5);

    Mode currentMode = NAVIGATE;

    bool ccw(PointV A, PointV B, PointV C) { // function to answer on question:'is relation of points A-B-C counter clock wise?'
        return (C.y() - A.y()) * (B.x() - A.x()) > (B.y() - A.y()) * (C.x() - A.x());
    }

    bool intersect(const SomeLine& l1, const SomeLine& l2) {
        PointV A = l1.p1, B = l1.p2;
        PointV C = l2.p1, D = l2.p2;
        return ccw(A, C, D) != ccw(B, C, D) && ccw(A, B, C) != ccw(A, B, D);
    }


    SomeLine tempLine;
    bool drawingDeletionLine = false;

    struct {
        bool active = false;
        PointV start;
        PointV cameraStart;
        int startX, startY;
    } drag;

public:
    CanvasDrawWidget(int X, int Y, int W, int H) : Fl_Widget(X, Y, W, H), tempLine() {}

    
    void setMode(Mode newMode){
        currentMode = newMode;
    }
    void setCamera(SomeCamera camera) { this->camera = camera; }
    vector<SomeLine>& getLines() {
        return this->lines; 
    }
    vector<SomeTriangle>& getTriangles() { 
        return this->triangles; 
    }
    PointV getOrigin(){
        return origin;
    }
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

    void drawDeletionLine(const SomeLine& l) {
        fl_color(FL_RED);
        drawLine(l);
        fl_color(FL_BLACK);
    }
    void drawOrigin(){
        int cursorX = x() + camera.convertX(origin.x());
        int cursorY = y() + camera.convertY(origin.y());
        fl_color(FL_RED);
        fl_pie(cursorX-3, cursorY-3, 6, 6, 0, 360);
        return;
    }
    

    void draw() override {
        fl_color(FL_WHITE);
        fl_rectf(x(), y(), w(), h());

        fl_color(FL_BLACK);
        for (const auto& line : lines)
            drawLine(line);
        for (const auto& triangle : triangles)
            drawTriangle(triangle);
        if (drawingDeletionLine) 
            drawDeletionLine(tempLine);
        drawOrigin();
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
                    drawingTriangleVertices.push_back(newPoint);
                    if (drawingTriangleVertices.size() == 3) {
                        triangles.emplace_back(drawingTriangleVertices[0], drawingTriangleVertices[1], drawingTriangleVertices[2]);
                        drawingTriangleVertices.clear();
                    }
                    redraw();
                    return 1;
                } else if (currentMode == DRAW_TRIANGLES && Fl::event_button() == FL_RIGHT_MOUSE) {
                    drawingDeletionLine = true;
                    tempLine = SomeLine(PointV(camera.uncoverX(mx), camera.uncoverY(my)), PointV(0, 0));
                    redraw();
                    return 1;
                } else if (currentMode == DRAW_LINES && Fl::event_button() == FL_LEFT_MOUSE) {
                    drag.active = true;
                    drag.start.x() = camera.uncoverX(mx);
                    drag.start.y() = camera.uncoverY(my);
                    return 1;
                } else if (currentMode == DRAW_LINES && Fl::event_button() == FL_RIGHT_MOUSE) {
                    drawingDeletionLine = true;
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
                if (currentMode == NAVIGATE) {
                    if (drag.active){
                        drag.active = false;
                        redraw();
                        return 1;
                    } else if (Fl::event_button() == FL_RIGHT_MOUSE){
                        int mx = Fl::event_x() - x();
                        int my = Fl::event_y() - y();
                        if ((mx < w()) && (my < h()) ){
                            origin.x() = camera.uncoverX(mx);
                            origin.y() = camera.uncoverY(my);
                            //determineNearest();
                            sprintf(originPositionText, "origin:(%.1f, %.1f)", camera.uncoverX(mx), camera.uncoverY(my));
                            originPositionInfo->label(originPositionText);
                            redraw();
                            return 1;
                            
                        }
                    }
                } else if (currentMode == DRAW_LINES && Fl::event_button() == FL_LEFT_MOUSE) {
                    PointV end(camera.uncoverX(mx), camera.uncoverY(my));
                    lines.emplace_back(drag.start, end);
                    redraw();
                    return 1;
                } else if (currentMode == DRAW_LINES && Fl::event_button() == FL_RIGHT_MOUSE) {
                    lines.erase(remove_if(lines.begin(), lines.end(), [&](const SomeLine& l) {
                        return intersect(tempLine, l); // Implement `intersect` for line intersection
                    }), lines.end());
                    drawingDeletionLine = false;
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
                    drawingDeletionLine = false;
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

class CanvasFractalWidget : public Fl_Widget {
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

    typedef struct {
        vector<SomeLine> lines;
        vector<SomeTriangle> triangles;
    } PrimitivesSet;

public:
    CanvasFractalWidget(int X, int Y, int W, int H) : Fl_Widget(X, Y, W, H) {}

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
    typedef struct {
        SomeLine l;
        PointV o; // origin
    } TransformableLine;

    typedef struct TransformableTriangle{
        SomeTriangle t;
        PointV o; // origin
        //TransformableTriangle():t(PointV(), PointV(), PointV()){}
    } TransformableTriangle;

    void recalculateAndRedraw(int iterationsCount){
        PointV origin = drawStartFigureView->getOrigin();
        /*
        lines = drawStartFigureView->getLines();
        triangles = drawStartFigureView->getTriangles();
        
        vector<TransformableLine> tLines(lines.size());
        vector<TransformableTriangle> tTriangles(triangles.size());
        
        for (int i = 0; i < tTriangles.size(); ++i){
            tTriangles.at(i).t = triangles.at(i);
            tTriangles.at(i).o = origin;
        }
        for (int i = 0; i < tLines.size(); ++i){
            tLines.at(i).l = lines.at(i);
            tLines.at(i).o = origin;
        }

        //origins.resize(transforms.size(), drawStartFigureView->getOrigin());
        calcPrimitives(iterationsCount, transforms, tLines, tTriangles);
        
        lines.resize(tLines.size());
        triangles.resize(tTriangles.size());

        for (int i = 0; i < tTriangles.size(); ++i){
            triangles.at(i) = tTriangles.at(i).t;
        }
        for (int i = 0; i < tLines.size(); ++i){
            lines.at(i) = tLines.at(i).l;
        }*/
        /*long linesCount = drawStartFigureView->getLines().size();
        long trianglesCount = drawStartFigureView->getTriangles().size();
        lines.clear();
        triangles.clear();
        lines.reserve(linesCount*pow(transforms.size(), iterationsCount));
        triangles.reserve(linesCount*pow(transforms.size(), iterationsCount));
        cout << endl <<  "new try:" << endl;
        for (int i = 0; i < transforms.size(); ++i){
            vector<SomeLine> transformLines = drawStartFigureView->getLines();
            vector<SomeTriangle> transformTriangles = drawStartFigureView->getTriangles();
            calcPrimitives2(iterationsCount, transforms, transforms.at(i), transformLines, transformTriangles, origin);
            lines.insert(lines.end(), transformLines.begin(), transformLines.end());
            triangles.insert(triangles.end(), transformTriangles.begin(), transformTriangles.end());
        }
        cout << "triangles count:" << triangles.size() << endl;
        */
        vector<SomeLine> transformLines = drawStartFigureView->getLines();
        vector<SomeTriangle> transformTriangles = drawStartFigureView->getTriangles();
        //generateFractal(iterationsCount, transforms, transformLines, transformTriangles, origin);
        calcPrimitives(iterationsCount, transforms, transformLines, transformTriangles, origin);
        lines = transformLines;
        triangles = transformTriangles;

        redraw();
    }
    static void generateFractal(
        int iterations,
        const std::vector<SomeTransform>& transforms,
        std::vector<SomeLine>& lines,
        std::vector<SomeTriangle>& triangles,
        const PointV& origin) 
    {
        if (iterations < 1) {
            return;
        }

        std::vector<SomeLine> newLines;
        std::vector<SomeTriangle> newTriangles;

        for (int i = 0; i < lines.size(); ++i) {
            for (int j = 0; j < transforms.size(); ++j) {
                PointV p1 = transforms.at(j).apply(lines.at(i).p1, origin);
                PointV p2 = transforms.at(j).apply(lines.at(i).p2, origin);
                newLines.push_back(SomeLine(p1, p2));
            }
        }

        for (int i = 0; i < triangles.size(); ++i) {
            for (int j = 0; j < transforms.size(); ++j) {
                PointV p1 = transforms.at(j).apply(triangles.at(i).getPoint(1), origin);
                PointV p2 = transforms.at(j).apply(triangles.at(i).getPoint(2), origin);
                PointV p3 = transforms.at(j).apply(triangles.at(i).getPoint(3), origin);
                newTriangles.push_back(SomeTriangle(p1, p2, p3));
            }
        }

        lines = newLines;
        triangles = newTriangles;

        generateFractal(iterations - 1, transforms, lines, triangles, origin);
    }
    
    
    static void calcPrimitives(int iteration,  vector<SomeTransform> transforms,
                             vector<SomeLine> &lines, vector<SomeTriangle> &triangles, PointV origin){
        if (iteration < 1){
            return;
        }
        vector<SomeLine> newLines;
        newLines.reserve(lines.size()*transforms.size());
        vector<SomeTriangle> newTriangles;
        newTriangles.reserve(triangles.size()*transforms.size());
        //vector<SomeTransform> newTransforms = transforms;
        //cout << endl << "iteration:" << iteration << endl;
        
        for (int i = 0; i < transforms.size(); ++i){
            SomeTransform transform = transforms.at(i);
            //cout << "oldTransform (" << i << ") T:" << transform.toString(false) << endl;

            PointV bias = transform.getTranslation();
            for (int j = 0; j < lines.size(); ++j){
                //PointV origin = tLines.at(j).o;
                PointV p1 = transform.apply(lines.at(j).p1, origin);
                PointV p2 = transform.apply(lines.at(j).p2, origin);
                
                newLines.push_back(SomeLine(p1, p2));
            }
            for (int j = 0; j < triangles.size(); ++j){
                //PointV origin = tTriangles.at(j).o;
                PointV p1 = transform.apply(triangles.at(j).getPoint(1), origin);
                PointV p2 = transform.apply(triangles.at(j).getPoint(2), origin);
                PointV p3 = transform.apply(triangles.at(j).getPoint(3), origin);
                
                newTriangles.push_back(SomeTriangle(p1,p2,p3));
            }

        }
        lines = newLines;
        triangles = newTriangles;
        calcPrimitives(iteration - 1, transforms, lines, triangles, origin);

    }
    
};

void onIterationsSpinnerChange(Fl_Widget* widget, void* data){
    iterationsCount = (int)iterationsSpinner->value();
    fractalsView->recalculateAndRedraw(iterationsCount);
}
void onTransformsSpinnerChange(Fl_Widget* widget, void* userData) {
    int newSize = (int)transformsSpinner->value();
    
    if (newSize > transforms.size()) {
        for (int i = transforms.size(); i < newSize; ++i) {
            SomeTransform newTransform;
            newTransform.setRotation(0);
            newTransform.setScale(PointV(1, 1));
            newTransform.setTranslation(PointV(0, 0));
            newTransform.recalculateMatrices();
            transforms.push_back(newTransform);
        }
    } else if (newSize < transforms.size()) {
        transforms.resize(newSize);
    }

    // Update choice dropdown
    currentTransformChoice->clear();
    for (size_t i = 0; i < transforms.size(); ++i) {
        currentTransformChoice->add(("Transform " + to_string(i + 1)).c_str());
    }
    currentTransformChoice->value(0);
    currentTransformIndex = 0;

    updateTransformFields(nullptr, nullptr);
    //fractalsView->recalculateAndRedraw(iterationsCount);
}

void onCurrentTransformChange(Fl_Widget* widget, void* userData) {
    currentTransformIndex = currentTransformChoice->value();
    updateTransformFields(nullptr, nullptr);
}

void onFieldChange(Fl_Widget* widget, void* userData) {
    updateTransformFromFields();
}

void updateTransformFields(Fl_Widget* widget, void* userData) {
    if (currentTransformIndex >= 0 && currentTransformIndex < transforms.size()) {
        SomeTransform& transform = transforms[currentTransformIndex];

        rotAngleInput->value(to_string(180.0*transform.getRotation()/MATH_PI).c_str());
        PointV scale = transform.getScale();
        scaleXInput->value(to_string(scale.x()).c_str());
        scaleYInput->value(to_string(scale.y()).c_str());
        PointV translate = transform.getTranslation();
        translateXInput->value(to_string(translate.x()).c_str());
        translateYInput->value(to_string(translate.y()).c_str());
    }
}

void updateTransformFromFields() {
    if (currentTransformIndex >= 0 && currentTransformIndex < transforms.size()) {
        SomeTransform& transform = transforms[currentTransformIndex];

        double rotAngle = MATH_PI*stod(rotAngleInput->value())/180.0;
        double scaleX = stod(scaleXInput->value());
        double scaleY = stod(scaleYInput->value());
        double translateX = stod(translateXInput->value());
        double translateY = stod(translateYInput->value());

        transform.setRotation(rotAngle);
        transform.setScale(PointV(scaleX, scaleY));
        transform.setTranslation(PointV(translateX, translateY));
        transform.recalculateMatrices();

        fractalsView->recalculateAndRedraw(iterationsCount);
    }
}

void mode_callback(Fl_Widget* widget, void* data) {
    //currentMode = static_cast<Mode>(reinterpret_cast<intptr_t>(data));
    drawStartFigureView->setMode(*static_cast<CanvasDrawWidget::Mode*>(data));
}

void saveTransformsToFile(const char* filename);
void loadTransformsFromFile(const char* filename);


int main() {
    createConsole();
    //Fl_Window* window = new Fl_Window(400, 300, "Transform Manager");
    Fl_Window* window = new Fl_Window(1200, 600, "Fractals Viewer");

    fractalsView = new CanvasFractalWidget(350, 60, 600, 500);
    SomeCamera camera(0.5, 0.5, 400.0, 600, 500);
    fractalsView->setCamera(camera);

    
    // Transforms Spinner
    transformsSpinner = new Fl_Spinner(150, 20, 100, 25, "Number of Transforms:");
    transformsSpinner->type(FL_INT_INPUT);
    transformsSpinner->minimum(1);
    transformsSpinner->value(4);
    transformsSpinner->callback(onTransformsSpinnerChange);

    iterationsSpinner = new Fl_Spinner(360, 20, 100, 25, "Iterations count:");
    iterationsSpinner->type(FL_INT_INPUT);
    iterationsSpinner->minimum(0);
    iterationsSpinner->maximum(10);
    iterationsSpinner->value(1);
    iterationsSpinner->callback(onIterationsSpinnerChange);
    iterationsCount = 1;
    

    // Current Transform Dropdown
    currentTransformChoice = new Fl_Choice(130, 60, 120, 25, "Current Transform:");
    currentTransformChoice->callback(onCurrentTransformChange);

    // Rotation Angle Input
    rotAngleInput = new Fl_Float_Input(150, 100, 100, 25, "Rotation Angle:");
    rotAngleInput->callback(onFieldChange);

    // Scale Coefficients Inputs
    scaleXInput = new Fl_Float_Input(150, 140, 100, 25, "Scale X:");
    scaleXInput->callback(onFieldChange);

    scaleYInput = new Fl_Float_Input(150, 180, 100, 25, "Scale Y:");
    scaleYInput->callback(onFieldChange);

    // Translation Bias Inputs
    translateXInput = new Fl_Float_Input(150, 220, 100, 25, "Translate X:");
    translateXInput->callback(onFieldChange);

    translateYInput = new Fl_Float_Input(150, 260, 100, 25, "Translate Y:");
    translateYInput->callback(onFieldChange);

    SomeTransform t1;
    t1.setRotation(MATH_PI*(-40)/180);    
    t1.setScale(PointV(-0.5,0.5));
    t1.setTranslation(PointV(0.8,0.0));
    t1.recalculateMatrices();
    SomeTransform t2;
    t2.setRotation(0);
    t2.setScale(PointV(0.5,-0.5));
    t2.setTranslation(PointV(0.0,-0.2));
    t2.recalculateMatrices();
    SomeTransform t3;
    t3.setRotation(MATH_PI*(-45)/180);
    t3.setScale(PointV(0.3,0.3));
    t3.setTranslation(PointV(0.3,-0.5));
    t3.recalculateMatrices();
    SomeTransform t4;
    t4.setRotation(MATH_PI*200/180);
    t4.setScale(PointV(0.3,-0.3));
    t4.setTranslation(PointV(0.76,-0.35));
    t4.recalculateMatrices();
    transforms.resize(4, SomeTransform());
    transforms.at(0) = t1;
    transforms.at(1) = t2;
    transforms.at(2) = t3;
    transforms.at(3) = t4;

    onTransformsSpinnerChange(nullptr, nullptr); // Initialize with one transform
    

    drawStartFigureView = new CanvasDrawWidget(10, 330, 250, 250);
    camera = SomeCamera(0.5, 0.5, 250.0, 250, 250);
    drawStartFigureView->setCamera(camera);
    SomeTriangle startTriangle(PointV(0,0), PointV(0.5,1.0), PointV(0.0, 1.0));
    drawStartFigureView->getTriangles().push_back(startTriangle);

    Fl_Group* radioGroup = new Fl_Group(10, 290, 275, 30);
    Fl_Radio_Round_Button* navButton = new Fl_Radio_Round_Button(20, 290, 90, 30, "Navigate");
    navButton->callback(mode_callback, new CanvasDrawWidget::Mode(CanvasDrawWidget::NAVIGATE));
    navButton->set();
    Fl_Radio_Round_Button* drawLinesButton = new Fl_Radio_Round_Button(110, 290, 90, 30, "Draw Lines");
    drawLinesButton->callback(mode_callback, new CanvasDrawWidget::Mode(CanvasDrawWidget::DRAW_LINES));
    Fl_Radio_Round_Button* drawTrianglesButton = new Fl_Radio_Round_Button(210, 290, 100, 30, "Draw Triangles");
    drawTrianglesButton->callback(mode_callback, new CanvasDrawWidget::Mode(CanvasDrawWidget::DRAW_TRIANGLES));
    radioGroup->end();

    PointV origin = drawStartFigureView->getOrigin();
    sprintf(originPositionText, "origin:(%.1f, %.1f)",origin.x(), origin.y());
    originPositionInfo = new Fl_Box(5, 580, 80, 20, originPositionText);
    originPositionInfo->labelsize(10);
    
    Fl_Button* loadButton = new Fl_Button(500, 10, 110, 25, "Load Transforms");
    loadButton->callback([](Fl_Widget*, void*) {
        // Open file chooser for loading
        const char* filename = fl_file_chooser("Open Transform File", "*.txt", nullptr);
        if (filename) {
            // Call function to load transforms
            loadTransformsFromFile(filename);
        }
    });

    Fl_Button* saveButton = new Fl_Button(620, 10, 110, 25, "Save Transforms");
    saveButton->callback([](Fl_Widget*, void*) {
        // Open file chooser for saving
        const char* filename = fl_file_chooser("Save Transform File", "*.txt", nullptr);
        if (filename) {
            // Call function to save transforms
            saveTransformsToFile(filename);
        }
    });

    Fl_Button* redrawFractalButton = new Fl_Button(735, 10, 100, 25, "Redraw fractal");
    redrawFractalButton->callback([](Fl_Widget*, void*) {
        fractalsView->recalculateAndRedraw(iterationsCount);
    });

    Fl_Button* help = new Fl_Button(840, 10, 50, 40, "Help");
    help->callback(help_callback);

    window->end();
    window->show();

    fractalsView->recalculateAndRedraw(iterationsCount);
    return Fl::run();
}

void saveTransformsToFile(const char* filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto& transform : transforms) {
            file << transform.getRotation() << " "
                 << transform.getScale().x() << " "
                 << transform.getScale().y() << " "
                 << transform.getTranslation().x() << " "
                 << transform.getTranslation().y() << "\n";
        }
        file.close();
    }
}


void loadTransformsFromFile(const char* filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        transforms.clear();
        double rotation, scaleX, scaleY, translateX, translateY;
        while (file >> rotation >> scaleX >> scaleY >> translateX >> translateY) {
            SomeTransform newTransform;
            newTransform.setRotation(rotation);
            newTransform.setScale(PointV(scaleX, scaleY));
            newTransform.setTranslation(PointV(translateX, translateY));
            newTransform.recalculateMatrices();
            transforms.push_back(newTransform);
        }
        file.close();
        updateTransformFields(nullptr, nullptr);  // Refresh fields
        transformsSpinner->value(transforms.size());
        currentTransformChoice->value(0);
        onTransformsSpinnerChange(nullptr, nullptr);
        onCurrentTransformChange(nullptr, nullptr);
    }
}