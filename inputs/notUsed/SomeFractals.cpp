#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Draw.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Menu_Item.H>

// #include <FL/FL_line.H>
#include <vector>
#include <iostream>
#include "SomeCamera.hpp"
#include "SomeLine.hpp"
#include "SomeTriangle.hpp"
#include <FL/fl_ask.H> 
#define VERY_BIG_DOUBLE 9999999999999
using namespace std;


Fl_Box *cursorPositionInfo; // text field where cusrsor position is displayed
char cursorPositionText[25];
Fl_Box *distanceToNearestInfo; // text field where distance from cursor to nearest serpynski point position is displayed
char distanceToNearestText[41];
bool markNearest = false; // Will we draw green point on nearest serpynski point?

Fl_Spinner *iterationsSelector; // field to select iterations count
Fl_Choice *modeSelector; // dropdown menu to select mode (we have 3 modes: lines, zoom in, triangles, zoom in and triangles, zoom out).

char mode = 0; // 0 means lines, zoom in; 1 means triangles, zoom in; 2 means triangles, zoom out;

Point2d pointA(-5.0, 5.0);
Point2d pointB(5.0, 5.0);
Point2d pointC(0.0, -5.0);

// function that returns set of lines to draw serpynski triangle
vector<SomeLine> getSerpynskyLines(Point2d p1, Point2d p2, Point2d p3, int iterationsCount);
// function that returns set of triangles to draw serpynski triangle
vector<SomeTriangle> getSerpynskyTrianglesZoomIn(Point2d p1, Point2d p2, Point2d p3, int iterationsCount, bool firstCall = true);
// function that returns set of triangles to draw serpynski triangle (to see the whole picture, you will need to zoom out)
void getSerpynskyTrianglesZoomOut(Point2d p1, Point2d p2, Point2d p3, int iterationsCount, vector<SomeTriangle> &vect);

void help_callback (Fl_Widget* widget, void* data) {// when we click 'help' button, we call this function
    fl_message("This is Serpinsky triangles viewer.\n"
                "1) You should see triangle.\n"
                "2) You can navigate by dragging this triangle with left mouse button and by using wheel.\n"
                "3) By clickling right mouse button near triangle you can put cursor.\n"
                "4) Buttons setA, setB, setC moves one of initial triangle points into cursor position.\n"
                "5) You can change iterations count in 'iterations' field.\n"
                "6) You can change mode by using 'mode' dropdown menu.\n"
                "7) You can see distance from cursor to nearest Serpynsky set point (with given iterations count)\n"
                "(we assume that initial points are not parts of Serpynksi set)\n"
                "8) Author of this program - Darastin.\n");
    return;
}

class CanvasWidget : public Fl_Widget {// class that draws main part of program.
private:// actually, it works pretty simple: we have array of lines and triangles which we draw.
    SomeCamera camera;
    vector<SomeLine> lines;
    vector<SomeTriangle> triangles;

    struct {
        bool active = false;
        Point2d start;
        Point2d cameraStart;
        int startX;
        int startY;
    } drag; // struct to allow drag triangle view with mouse button.
    Point2d myCursor; // here we save position of our custom cursor: red point which you can see in program.
    Point2d nearestSerpynskyPoint;

public:
    CanvasWidget(int X, int Y, int W, int H) : Fl_Widget(X, Y, W, H), myCursor(0.0, 0.0), nearestSerpynskyPoint(0.0, 0.0) {}

    void setCamera(SomeCamera camera){
        this->camera = camera;
    }
    void setLines(vector<SomeLine> &lines){
        this->lines = lines;
    }
    vector<SomeLine> &getLines(){
        return this->lines;
    }
    void setTriangles(vector<SomeTriangle> &triangles){
        this->triangles = triangles;
    }
    vector<SomeTriangle> &getTriangles(){
        return this->triangles;
    }
    Point2d getCursor(){
        return myCursor;
    }

    void drawLine(SomeLine l){
        fl_line(x() + camera.convertX(l.p1.x), y() + camera.convertY(l.p1.y), x() + camera.convertX(l.p2.x), y() + camera.convertY(l.p2.y));
        return;
    }
    void drawTriangle(SomeTriangle t){
        Point2d p1 = t.getPoint(1);
        Point2d p2 = t.getPoint(2);
        Point2d p3 = t.getPoint(3);
        int x1 = x() + camera.convertX(p1.x);
        int y1 = y() + camera.convertY(p1.y);
        int x2 = x() + camera.convertX(p2.x);
        int y2 = y() + camera.convertY(p2.y);
        int x3 = x() + camera.convertX(p3.x);
        int y3 = y() + camera.convertY(p3.y);
        fl_polygon(x1, y1, x2, y2, x3, y3);
        return;
    }
    void drawCursor(){
        int cursorX = x() + camera.convertX(myCursor.x);
        int cursorY = y() + camera.convertY(myCursor.y);
        fl_color(FL_RED);
        fl_pie(cursorX-3, cursorY-3, 6, 6, 0, 360);
        
        if (markNearest && nearestSerpynskyPoint != myCursor){
            int nearestX = x() + camera.convertX(nearestSerpynskyPoint.x);
            int nearestY = y() + camera.convertY(nearestSerpynskyPoint.y);
            fl_color(FL_GREEN);
            fl_pie(nearestX-3, nearestY-3, 6, 6, 0, 360);
        }
        return;
    }
    void determineNearest(){ // function to determine nearest serpinski point (from our cursor)
        double minDistance = VERY_BIG_DOUBLE; // actually, we just go through all points and select one with minimal distance.
        bool changed = false;
        for (int i = 0; i < lines.size(); ++i){ // go through lines
            double curDist = (myCursor - lines.at(i).p1).getDistanceToZero();
            
            if ( curDist < minDistance){ // check start of line
                if (lines.at(i).p1 != pointA && lines.at(i).p1 != pointB && lines.at(i).p1 != pointC){ // initial points we don't count as serpynski points.
                    minDistance = curDist;
                    nearestSerpynskyPoint = lines.at(i).p1;
                    changed = true;
                }
            }
            curDist = (myCursor - lines.at(i).p2).getDistanceToZero();
            if ( curDist < minDistance){ // check end of line
                if (lines.at(i).p2 != pointA && lines.at(i).p2 != pointB && lines.at(i).p2 != pointC){ // initial points we don't count as serpynski points.
                    minDistance = curDist;
                    nearestSerpynskyPoint = lines.at(i).p2;
                    changed = true;
                }
            }
        }
        if (!changed)
            for (int i = 0; i < triangles.size(); ++i){// go through triangles
                for (int j = 1; j <= 3; ++j){// go through traingles points
                    double curDist = (myCursor - triangles.at(i).getPoint(j)).getDistanceToZero();
                    if ( curDist < minDistance){
                        if (triangles.at(i).getPoint(j) != pointA && triangles.at(i).getPoint(j) != pointB && triangles.at(i).getPoint(j) != pointC){ // initial points we don't count as serpynski points.
                            minDistance = curDist;
                            nearestSerpynskyPoint = triangles.at(i).getPoint(j);
                            changed = true;
                        }
                    }
                }
            }

        if (changed){
            sprintf(distanceToNearestText, "distance to nearest:%.2f", minDistance);
        } else {
            sprintf(distanceToNearestText, "distance to nearest:?.??");
            nearestSerpynskyPoint = myCursor;
        }
        distanceToNearestInfo->label(distanceToNearestText);
        
        
        
    }
    void draw() override {
        fl_color(FL_WHITE);
        
        fl_rectf(x(), y(), w(), h());

        fl_color(FL_BLACK);
        for (int i = 0; i < lines.size(); ++i){ // draw all lines
            drawLine(lines.at(i));
        }
        if (triangles.size() > 0){ // draw first triangle
            drawTriangle(triangles.at(0));
            if (mode == 1){ // if we in mode triangles, zoom in - first triangle black and all other triangles white.
                fl_color(FL_WHITE);
            }
        }
        for (int i = 1; i < triangles.size(); ++i){ // draw all other triangles
            drawTriangle(triangles.at(i));
        }
        drawCursor();


    }
    static void iterationsSelector_callback(Fl_Widget* widget, void* data){// when we select iteration, we call this function
        CanvasWidget *canvas = (CanvasWidget *)data;
        canvas->calcPictureAndRedraw((int)iterationsSelector->value());
    }
    static void modeSelector_callback(Fl_Widget* widget, void* data){// when we select mode, we call this function
        CanvasWidget *canvas = (CanvasWidget *)data;
        mode = (int)modeSelector->value();
        canvas->calcPictureAndRedraw((int)iterationsSelector->value());
    }
    void calcPictureAndRedraw(int iterations) { // calculate triangles/lines and redraw them
        
        if (mode == 2){
            this->lines.clear();
            this->triangles.clear();
            getSerpynskyTrianglesZoomOut(pointA, pointB, pointC, iterations, this->triangles);
            determineNearest();
            redraw();
            return;
        } else if (mode == 1){
            this->lines.clear();
            this->triangles = getSerpynskyTrianglesZoomIn(pointA, pointB, pointC, iterations);
            determineNearest();
            redraw();
            return;
        }
        this->triangles.clear();
        this->lines = getSerpynskyLines(pointA, pointB, pointC, iterations);
        determineNearest();
        redraw();
        return;
    }
    int handle(int event) override { // this function to handle inputs from mouse.
        switch (event) {
            case FL_PUSH: {
                int mx = Fl::event_x() - x();
                int my = Fl::event_y() - y();
                

                if (Fl::event_button() == FL_LEFT_MOUSE) {
                    if ((mx < w()) && (my < h()) ){
                        drag.active = true;
                        drag.startX = mx;
                        drag.startY = my;
                        //drag.start.x = camera.uncoverX(mx);
                        //drag.start.y = camera.uncoverY(my);
                        drag.cameraStart.x = camera.getX();
                        drag.cameraStart.y = camera.getY();
                        redraw();
                        return 1;
                    }
                }
                
                return 0;
            } case FL_DRAG: {
                if (drag.active){
                    int mx = Fl::event_x() - x();
                    int my = Fl::event_y() - y();
                    double biasX = (camera.uncoverX(mx)) - camera.uncoverX(drag.startX);
                    double biasY = (camera.uncoverY(my)) - camera.uncoverY(drag.startY);
                    
                    
                    
                    camera.setX(drag.cameraStart.x - biasX);
                    camera.setY(drag.cameraStart.y - biasY);
                    redraw();
                    return 1;
                }
                return 0;
            } case FL_RELEASE: {
                int mx = Fl::event_x() - x();
                int my = Fl::event_y() - y();

                if (Fl::event_button() == FL_LEFT_MOUSE) {
                    

                    if ((mx < w()) && (my < h()) ){
                        drag.active = false;
                        redraw();
                        return 1;
                        
                    }
                } else if (Fl::event_button() == FL_RIGHT_MOUSE){
                    if ((mx < w()) && (my < h()) ){
                        myCursor.x = camera.uncoverX(mx);
                        myCursor.y = camera.uncoverY(my);
                        determineNearest();
                        sprintf(cursorPositionText, "Cursor:(%.1f, %.1f)", camera.uncoverX(mx), camera.uncoverY(my));
                        cursorPositionInfo->label(cursorPositionText);
                        redraw();
                        return 1;
                        
                    }
                    return 1;
                }
                return 0;
            } case FL_MOUSEWHEEL:{
                
                int scroll_dx = Fl::event_dy(); // Vertical scroll amount
                if (scroll_dx < 0) {
                    this->camera.zoom(1.1);
                    // Scrolling up
                    //fl_message("Scrolling Up!");
                } else if (scroll_dx > 0) {
                    this->camera.zoom(1.0/1.1);
                    // Scrolling down
                    //fl_message("Scrolling Down!");
                }
                redraw();
                return 1; // Event handled
            }default:
                return Fl_Widget::handle(event); // Default handling
        }
        return 0;
    }
};
void getSerpynskyTrianglesZoomOut(Point2d p1, Point2d p2, Point2d p3, int iterationsCount, vector<SomeTriangle> &vect){
    
    if (vect.size() == 0){
        vect.push_back(SomeTriangle(p1, p2, p3));
    }
    if (iterationsCount == 0){
        return;
    }
    Point2d bias1 = p1 - p3;
    Point2d bias2 = p2 - p3;
    vector<SomeTriangle> copy1 = vect;
    vector<SomeTriangle> copy2 = vect;
    for (int i = 0; i < vect.size(); ++i){
        copy1.at(i).shift(bias1);
        copy2.at(i).shift(bias2);
    }
    vect.insert(vect.end(), copy1.begin(), copy1.end());
    vect.insert(vect.end(), copy2.begin(), copy2.end());
    getSerpynskyTrianglesZoomOut(p1 + bias1, p2 + bias2, p3, iterationsCount - 1, vect);

}
vector<SomeTriangle> getSerpynskyTrianglesZoomIn(Point2d p1, Point2d p2, Point2d p3, int iterationsCount, bool firstCall){
    vector<SomeTriangle> result;

    if (firstCall){
        result.push_back(SomeTriangle(p1, p2, p3));
    }
    if (iterationsCount == 0){
        return result;
    }
    
    Point2d s1, s2, s3;
    s1.x = (p1.x + p2.x)/2.0;
    s1.y = (p1.y + p2.y)/2.0;

    s2.x = (p2.x + p3.x)/2.0;
    s2.y = (p2.y + p3.y)/2.0;

    s3.x = (p1.x + p3.x)/2.0;
    s3.y = (p1.y + p3.y)/2.0;
    result.push_back(SomeTriangle(s1, s2, s3));

    //vector<SomeTriangle> result;
    vector<SomeTriangle> temp;
    temp = getSerpynskyTrianglesZoomIn(p1, s1, s3, iterationsCount - 1, false);
    result.insert(result.end(), temp.begin(), temp.end());
    temp = getSerpynskyTrianglesZoomIn(s3, s2, p3, iterationsCount - 1, false);
    result.insert(result.end(), temp.begin(), temp.end());
    temp = getSerpynskyTrianglesZoomIn(s1, p2, s2, iterationsCount - 1, false);
    result.insert(result.end(), temp.begin(), temp.end());
    return result;
}

vector<SomeLine> getSerpynskyLines(Point2d p1, Point2d p2, Point2d p3, int iterationsCount){
    if (iterationsCount == 0){
        vector<SomeLine> result;
        result.push_back(SomeLine(p1, p2));
        result.push_back(SomeLine(p2, p3));
        result.push_back(SomeLine(p3, p1));
        return result;
    } else {
        Point2d s1, s2, s3;
        s1.x = (p1.x + p2.x)/2.0;
        s1.y = (p1.y + p2.y)/2.0;

        s2.x = (p2.x + p3.x)/2.0;
        s2.y = (p2.y + p3.y)/2.0;

        s3.x = (p1.x + p3.x)/2.0;
        s3.y = (p1.y + p3.y)/2.0;
        vector<SomeLine> result;
        vector<SomeLine> temp;
        temp = getSerpynskyLines(p1, s1, s3, iterationsCount - 1);
        result.insert(result.end(), temp.begin(), temp.end());
        temp = getSerpynskyLines(s3, s2, p3, iterationsCount - 1);
        result.insert(result.end(), temp.begin(), temp.end());
        temp = getSerpynskyLines(s1, p2, s2, iterationsCount - 1);
        result.insert(result.end(), temp.begin(), temp.end());
        return result;
    }
    
}



void setPointA (Fl_Widget* widget, void* data);
void setPointB (Fl_Widget* widget, void* data);
void setPointC (Fl_Widget* widget, void* data);


int main() {
    
    
    

    // Create a window
    Fl_Double_Window *window = new Fl_Double_Window(410, 400, "Serpinsky triangles viewer.");
    

    CanvasWidget *canvas = new CanvasWidget(5, 50, 400, 300);
    SomeCamera camera(0.0, 0.0, 10.0, 400, 300);
    vector<SomeLine> &lines = canvas->getLines();
    lines = getSerpynskyLines(pointA, pointB, pointC, 0);
    canvas->setCamera(camera);
    
    
    sprintf(cursorPositionText, "Cursor:(0.0, 0.0)");
    cursorPositionInfo = new Fl_Box(5, 25, 80, 20, cursorPositionText);
    cursorPositionInfo->labelsize(10);
    
    sprintf(distanceToNearestText, "distance to nearest:?");
    distanceToNearestInfo = new Fl_Box(200, 350, 150, 20, distanceToNearestText);
    distanceToNearestInfo->labelsize(10);


    Fl_Button *buttonSetPointA = new Fl_Button(160, 5, 50, 40, "Set A");
    Fl_Button *buttonSetPointB = new Fl_Button(215, 5, 50, 40, "Set B");
    Fl_Button *buttonSetPointC = new Fl_Button(270, 5, 50, 40, "Set C");
    buttonSetPointA->callback(setPointA, canvas);
    buttonSetPointB->callback(setPointB, canvas);
    buttonSetPointC->callback(setPointC, canvas);

    Fl_Button *help = new Fl_Button(330, 5, 50, 40, "Help");
    help->callback(help_callback);

    modeSelector = new Fl_Choice(50, 350, 150, 25, "Mode:");
    modeSelector->add("Lines, zoom in.");
    modeSelector->add("Triangles, zoom in.");
    modeSelector->add("Triangles, zoom out.");
    modeSelector->value(0);
    modeSelector->callback(canvas->modeSelector_callback, canvas);
    

    iterationsSelector = new Fl_Spinner(70, 5, 80, 20, "Iterations:");
    iterationsSelector->range(0, 12);
    iterationsSelector->step(1);
    iterationsSelector->value(0); 
    iterationsSelector->callback(canvas->iterationsSelector_callback, canvas);
    canvas->calcPictureAndRedraw((int)iterationsSelector->value());

    window->end();
    window->show();
    return Fl::run();
}


void setPointA (Fl_Widget* widget, void* data) {
    CanvasWidget *canvas = (CanvasWidget *)data;
    pointA = canvas->getCursor();
    canvas->calcPictureAndRedraw(iterationsSelector->value());
}
void setPointB (Fl_Widget* widget, void* data) {
    CanvasWidget *canvas = (CanvasWidget *)data;
    pointB = canvas->getCursor();
    canvas->calcPictureAndRedraw(iterationsSelector->value());
}
void setPointC (Fl_Widget* widget, void* data) {
    CanvasWidget *canvas = (CanvasWidget *)data;
    pointC = canvas->getCursor();
    canvas->calcPictureAndRedraw(iterationsSelector->value());
}