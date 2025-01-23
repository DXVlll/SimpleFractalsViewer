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
#include "FMatrix.hpp"
#include <ctime>

#define M_PI 3.141592653589793238462643383279502884197169399375105
#define VERY_BIG_DOUBLE 9999999999999
using namespace dxvll_space_2d;
//using namespace someDXVlll24_11_matrix;



Fl_Spinner *framesCountSelector; 
int framesCount = 50;
Fl_Spinner *durationSelector;
int duration = 3000;


Fl_Box *circleAinfo;
char circleAinfoText[50];
Fl_Box *circleBinfo;
char circleBinfoText[50];


void help_callback (Fl_Widget* widget, void* data) {// when we click 'help' button, we call this function
    fl_message("This is not implemented help.\n"
                "1) You should see ...\n"
                "...\n"
                "?) Author of this program - Darastin.\n");
    return;
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


//typedef struct SomeTransform{
//    PointV translation;
//    PointV scale;
//    double rotation;
//} SomeTransform;

typedef struct SomeCircle{
    private:
        PointV pos; // Circle's center position
        PointV dirPos; // Position to create vector from center to here
    public:             // and by that way to find radius
    PointV getPos(){
        return pos;
    }
    void setPos(const PointV& newPos) {
        pos = newPos;
    }
    PointV getDirPos() {
        return dirPos;
    }
    void setDirPos(const PointV& newDirPos) {
        dirPos = newDirPos;
    }
    double getRadius(){
        return (dirPos - pos).getDistanceToZero();
    }
    double getDirectionAngle(){
        return acos((dirPos-pos).x()/getRadius()) * (((dirPos-pos).y()<0) ? -1.0 : 1.0);
    }
    SomeCircle operator=(const SomeCircle & other){
        this->pos = other.pos;
        this->dirPos = other.dirPos;
        return (*this);
    }
} SomeCircle;


SomeTransform getTransformBetweenCircles(SomeCircle c1, SomeCircle c2){ // #HERE_stopped
    SomeTransform res;
    //res.translation = c2.getPos() - c1.getPos();
    //res.scale = PointV(c2.getRadius()/c1.getRadius(), c2.getRadius()/c1.getRadius());
    //res.rotation = c1.getDirectionAngle() - c2.getDirectionAngle();
    res.setTranslation(c2.getPos() - c1.getPos());
    res.setScale(PointV(c2.getRadius()/c1.getRadius(), c2.getRadius()/c1.getRadius()));
    res.setRotation(c1.getDirectionAngle() - c2.getDirectionAngle());
    return res;
}

SomeCircle applyTraslation(SomeCircle c, PointV translation){
    SomeCircle res;
    res.setPos(c.getPos() + translation); // we just add translation vector to our points
    res.setDirPos(c.getDirPos() + translation);
    return res;
}

SomeCircle applyScale(SomeCircle c, PointV scale, PointV origin){
    SomeCircle res = applyTraslation(c, (PointV(0,0) - origin)); // move circle to (0,0)
    //res.getPos().getVec().resize(2);
    FMatrix<double> mx(2,2,0); // create scale matrix
    mx.elem(0,0) = scale.x();
    mx.elem(1,1) = scale.y();
    PointV temp;
    vector<double> vecForMult = res.getPos().getVec();
    vecForMult.resize(2);
    temp.setVec(vecForMult*mx);// multiply our circle building points by scale matrix
    //temp.getVec().resize(2);
    res.setPos(temp);
    vecForMult = res.getDirPos().getVec();
    vecForMult.resize(2);
    temp.setVec(vecForMult*mx);
    res.setDirPos(temp);
    res = applyTraslation(res, origin); // move circle back to it's position
    return  res;
}

SomeCircle applyRotation(SomeCircle c, double rads, PointV origin){
    //cout << "mult0" << endl;
    SomeCircle res = applyTraslation(c, (PointV(0,0) - origin)); // move circle to (0,0)
    
    FMatrix<double> mx(2,2,0); // create rotation matrix
    mx.elem(0,0) = cos(rads);  mx.elem(0,1) = -sin(rads);
    mx.elem(1,0) = sin(rads); mx.elem(1,1) = cos(rads);
    PointV temp;
    //cout << "mult1" << endl;
    //res.getPos().getVec().resize(2);
    //cout << "vec1 size:" << res.getPos().getVec() << endl;
    vector<double> vecForMult = res.getPos().getVec();
    vecForMult.resize(2);
    temp.setVec(vecForMult*mx);// multiply our circle building points by rotation matrix
    res.setPos(temp);
    //res.getPos().getVec().resize(2);
    //cout << "mult2" << endl;
    vecForMult = res.getDirPos().getVec();
    vecForMult.resize(2);
    temp.setVec(vecForMult*mx);
    res.setDirPos(temp);
    res = applyTraslation(res, origin); // move circle back to it's position
    return res;
}

class CanvasWidget : public Fl_Widget {// class that draws main part of program.
private:// actually, it works pretty simple: we have array of elements which we draw.
    SomeCamera camera;
    vector<SomeLine> lines; // set of lines to draw (not used)
    vector<SomeTriangle> triangles; // set of triangles to draw (not used)
    SomeCircle circleA; // circle A
    SomeCircle circleB; // circle B
    SomeCircle anim; // circle that we draw while playing animation
    
    SomeCircle *mouseCircRef = &circleA;

    struct {
        bool activeDrag = false;
        bool activeDraw = false;
        PointV startDraw;
        PointV cameraStart;
        int startX;
        int startY;
    } drag; // struct to allow drag our view with mouse button.

public:
    CanvasWidget(int X, int Y, int W, int H) : Fl_Widget(X, Y, W, H) {
        circleA.setPos(PointV(0,0));
        circleA.setDirPos(PointV(0,1.0));

        circleB.setPos(PointV(1, 2));
        circleB.setDirPos(PointV(0,-1.5));

        anim.setPos(PointV(0,0));
        anim.setDirPos(PointV(0,0));
    }

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
    void setMouseCircleA(){
        this->mouseCircRef = &circleA;
    }
    void setMouseCircleB(){
        this->mouseCircRef = &circleB;
    }
    SomeCircle &getCircleA(){
        return circleA;
    }
    SomeCircle &getCircleB(){
        return circleB;
    }
    

    void drawLine(SomeLine l){
        fl_line(x() + camera.convertX(l.p1.x()), y() + camera.convertY(l.p1.y()), x() + camera.convertX(l.p2.x()), y() + camera.convertY(l.p2.y()));
        return;
    }
    void drawTriangle(SomeTriangle t){
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
        return;
    }

    void drawCircle(SomeCircle c){
        double radius = c.getRadius();
        PointV dirPos = c.getDirPos();
        PointV pos = c.getPos();
        double mainDirPosX = x() + camera.convertX(dirPos.x());
        double mainDirPosY = y() + camera.convertY(dirPos.y());
        int x1 = x() + camera.convertX(pos.x());
        int y1 = y() + camera.convertY(pos.y());

        fl_circle (x1, y1, camera.getScale()*radius);
        fl_circle (mainDirPosX, mainDirPosY, camera.getScale()*radius/20.0);
        return;
    }
    
    
    void draw() override {
        fl_color(FL_WHITE);
        
        fl_rectf(x(), y(), w(), h());

        fl_color(FL_BLACK);
        for (int i = 0; i < lines.size(); ++i){ // draw all lines
            drawLine(lines.at(i));
        }
        for (int i = 0; i < triangles.size(); ++i){ // draw all triangles
            drawTriangle(triangles.at(i));
        }
        
        drawCircle(circleA);
        drawCircle(circleB);
        drawCircle(anim);
        //for (int i = 0; i < circles.size(); ++i){
        //    drawCircle(circles.at(i));
        //}
        
        

    }
    

    void runTest (int duration, int framesCount){ // here I tested function getTransformBetweenCircles
        //cout << "runTest" << endl;
        SomeTransform tr = getTransformBetweenCircles(circleA, circleB);
        tr.recalculateMatrices();
        cout << tr.toString();
        cout << "--------------------------------------------\n";
        anim.setPos(tr.apply(circleA.getPos(), circleA.getPos()));
        anim.setDirPos(tr.apply(circleA.getDirPos(), circleA.getPos()));
        //cout << "we have got transform" << endl;
      //  anim = applyRotation(circleA, tr.getRotation(), circleA.getPos());
        //cout << "after rot" << endl;
      //  anim = applyScale(anim, (tr.getScale()), circleA.getPos());
        //cout << "after scale" << endl;
      //  anim = applyTraslation(anim, tr.getTranslation());
        //cout << "after translation" << endl;
        //circleA = anim;
        redraw();
    }
    void runAnimation(int duration, int framesCount){
       
        clock_t start = clock();
        SomeTransform tr = getTransformBetweenCircles(circleA, circleB);
        SomeTransform curTransform;

        anim = circleA;

        for (int i = 1; i <= framesCount; ++i){

            double t = (double)i/(double)framesCount;
            int current = (int)(clock()-start)/(CLOCKS_PER_SEC/1000);

            while (current < duration*i/framesCount){
                current = (int)(clock()-start)/(CLOCKS_PER_SEC/1000);
                Fl::check();
            }

            PointV tempScale(1.0, 1.0);
            tempScale = tempScale + (tr.getScale() - tempScale)*t;
            curTransform.setRotation(tr.getRotation()*t);
            curTransform.setScale(tempScale);
            curTransform.setTranslation(tr.getTranslation()*t);
            curTransform.recalculateMatrices();

            anim.setPos(curTransform.apply(circleA.getPos(), circleA.getPos()));
            anim.setDirPos(curTransform.apply(circleA.getDirPos(), circleA.getPos()));

          //  anim = applyRotation(circleA, tr.getRotation()*t, circleA.getPos());
          //  anim = applyScale(anim, tempScale/*(tr.scale*t)*/, circleA.getPos());
          //  anim = applyTraslation(anim, tr.getTranslation()*t);

            redraw();

        }

        redraw();
        Fl::check();
        anim.setPos(PointV(0,0));
        anim.setDirPos(PointV(0,0));
        return;
    }

    int handle(int event) override { // this function to handle inputs from mouse.
        switch (event) {
            case FL_PUSH: {
                int mx = Fl::event_x() - x();
                int my = Fl::event_y() - y();
                

                if (Fl::event_button() == FL_LEFT_MOUSE) {
                    if ((mx < w()) && (my < h()) ){
                        drag.activeDrag = true;
                        drag.startX = mx;
                        drag.startY = my;
                        drag.cameraStart.x() = camera.getX();
                        drag.cameraStart.y() = camera.getY();
                        redraw();
                        return 1;
                    }
                }

                if (Fl::event_button() == FL_RIGHT_MOUSE) {
                    if ((mx < w()) && (my < h()) ){
                        drag.activeDraw = true;

                        drag.startDraw.x() = camera.uncoverX(mx);
                        drag.startDraw.y()  = camera.uncoverY(my);

                        mouseCircRef->setPos(drag.startDraw);

                        sprintf(circleAinfoText, "Apos:(x:%.3f, y:%.3f), Adir: %.3f", getCircleA().getPos().x(), getCircleA().getPos().y(), getCircleA().getDirectionAngle());
                        circleAinfo->label(circleAinfoText);
                        sprintf(circleBinfoText, "Bpos:(x:%.3f, y:%.3f), Bdir: %.3f", getCircleB().getPos().x(), getCircleB().getPos().y(), getCircleB().getDirectionAngle());
                        circleBinfo->label(circleBinfoText);
                        redraw();
                        return 1;
                    }
                }
                
                return 0;
            } case FL_DRAG: {
                if (drag.activeDrag){
                    int mx = Fl::event_x() - x();
                    int my = Fl::event_y() - y();
                    double biasX = (camera.uncoverX(mx)) - camera.uncoverX(drag.startX);
                    double biasY = (camera.uncoverY(my)) - camera.uncoverY(drag.startY);
                    
                    camera.setX(drag.cameraStart.x() - biasX);
                    camera.setY(drag.cameraStart.y() - biasY);
                    redraw();
                    return 1;
                }
                if (drag.activeDraw){
                    int mx = Fl::event_x() - x();
                    int my = Fl::event_y() - y();
                    double dirPosX = (camera.uncoverX(mx));
                    double dirPosY = (camera.uncoverY(my));
                    mouseCircRef->setDirPos(PointV(dirPosX, dirPosY));
                    sprintf(circleAinfoText, "Apos:(x:%.3f, y:%.3f), Adir: %.3f", getCircleA().getPos().x(), getCircleA().getPos().y(), getCircleA().getDirectionAngle());
                    circleAinfo->label(circleAinfoText);
                    sprintf(circleBinfoText, "Bpos:(x:%.3f, y:%.3f), Bdir: %.3f", getCircleB().getPos().x(), getCircleB().getPos().y(), getCircleB().getDirectionAngle());
                    circleBinfo->label(circleBinfoText);
                    
                    redraw();
                    return 1;
                }
                return 0;
            } case FL_RELEASE: {
                int mx = Fl::event_x() - x();
                int my = Fl::event_y() - y();

                if (Fl::event_button() == FL_LEFT_MOUSE) {
                    

                    if ((mx < w()) && (my < h()) ){
                        drag.activeDrag = false;
                        redraw();
                        return 1;
                        
                    }
                }
                if (Fl::event_button() == FL_RIGHT_MOUSE) {
                    

                    if ((mx < w()) && (my < h()) ){
                        drag.activeDraw = false;
                        redraw();
                        return 1;
                        
                    }
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

    static void generateRandCircles_callback(Fl_Widget* widget, void* data){
        
    }
};




void framesCountSelector_callback(Fl_Widget* widget, void* data){
    framesCount = (int)framesCountSelector->value();
}
void durationSelector_callback(Fl_Widget* widget, void* data){
    duration = (int)durationSelector->value();
    cout << "duration:" << duration << endl;
}

void startAnimation_callback(Fl_Widget* widget, void* data){
    cout << "You have pressed 'startAnimation' button\n";
    CanvasWidget *canvas = (CanvasWidget *)data;
    //canvas->runTest(duration, framesCount);
    canvas->runAnimation(duration, framesCount);
    return;
}

void checkboxMouseCircle_callback(Fl_Widget* widget, void* data) {
    CanvasWidget *canvas = (CanvasWidget *)data;
    Fl_Check_Button* check = (Fl_Check_Button*)widget;
    if (check->value() == 1) {
        canvas->setMouseCircleB();
    } else {
        canvas->setMouseCircleA();
    }
    return;
}


int main() {
    createConsole();
    // Create a window
    Fl_Double_Window *window = new Fl_Double_Window(1200, 700, ". . .");
    
    cout << "Hello, world!\n";
    CanvasWidget *canvas = new CanvasWidget(5, 50, 1100, 600);
    SomeCamera camera(3.0, 1.5, 150.0, 1100, 600);
    
    Fl_Button *buttonStartAnimation = new Fl_Button(260, 5, 150, 40, "Start animation");
    buttonStartAnimation->callback(startAnimation_callback, canvas);
    
    canvas->setCamera(camera);

    Fl_Check_Button* checkboxMouseCircle = new Fl_Check_Button(410, 25, 200, 30, "Give Circle B to mouse(instead of A)");
    checkboxMouseCircle->callback(checkboxMouseCircle_callback, canvas);  // Set callback for the checkbox


    framesCountSelector = new Fl_Spinner(160, 5, 80, 20, "Frames count:");
    framesCountSelector->range(0, 360);
    framesCountSelector->step(1);
    framesCountSelector->value(50); 
    framesCountSelector->callback(framesCountSelector_callback, canvas);

    durationSelector = new Fl_Spinner(160, 25, 80, 20, "Duration(ms):");
    durationSelector->range(0, 30000);
    durationSelector->step(500);
    durationSelector->value(3000); 
    durationSelector->callback(durationSelector_callback, canvas);

    sprintf(circleAinfoText, "Apos:(x:%.3f, y:%.3f), Adir: %.3f", canvas->getCircleA().getPos().x(), canvas->getCircleA().getPos().y(), canvas->getCircleA().getDirectionAngle());

    circleAinfo = new Fl_Box(490, 0, 200, 20, circleAinfoText);
    circleAinfo->labelsize(10);

    sprintf(circleBinfoText, "Bpos:(x:%.3f, y:%.3f), Bdir: %.3f", canvas->getCircleB().getPos().x(), canvas->getCircleB().getPos().y(), canvas->getCircleB().getDirectionAngle());

    circleBinfo = new Fl_Box(490, 12, 200, 20, circleBinfoText);
    circleBinfo->labelsize(10);

    Fl_Box *circleBinfo;
    char circleBinfoText[50];


    Fl_Button *help = new Fl_Button(840, 5, 50, 40, "Help");
    help->callback(help_callback);

    window->end();
    window->show();
    return Fl::run();
}
