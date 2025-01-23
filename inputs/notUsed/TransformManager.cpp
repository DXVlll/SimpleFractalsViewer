#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_File_Chooser.H> 
#include <FL/Fl_Button.H>
#include <vector>
#include <fstream>

#include "SomeTransform.hpp"

using namespace dxvll_space_2d;

std::vector<SomeTransform> transforms;
int currentTransformIndex = 0;

void updateTransformFields(Fl_Widget* widget, void* userData);
void updateTransformFromFields();

Fl_Float_Input *rotAngleInput, *scaleXInput, *scaleYInput, *translateXInput, *translateYInput;
Fl_Choice *currentTransformChoice;
Fl_Spinner *transformsSpinner;

void onTransformsSpinnerChange(Fl_Widget* widget, void* userData) {
    int newSize = (int)transformsSpinner->value();
    if (newSize > transforms.size()) {
        for (int i = transforms.size(); i < newSize; ++i) {
            SomeTransform newTransform;
            newTransform.setRotation(0);
            newTransform.setScale(PointV(1, 1));
            newTransform.setTranslation(PointV(0, 0));
            transforms.push_back(newTransform);
        }
    } else if (newSize < transforms.size()) {
        transforms.resize(newSize);
    }

    // Update choice dropdown
    currentTransformChoice->clear();
    for (size_t i = 0; i < transforms.size(); ++i) {
        currentTransformChoice->add(("Transform " + std::to_string(i + 1)).c_str());
    }
    currentTransformChoice->value(0);
    currentTransformIndex = 0;

    updateTransformFields(nullptr, nullptr);
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

        rotAngleInput->value(std::to_string(transform.getRotation()).c_str());
        PointV scale = transform.getScale();
        scaleXInput->value(std::to_string(scale.x()).c_str());
        scaleYInput->value(std::to_string(scale.y()).c_str());
        PointV translate = transform.getTranslation();
        translateXInput->value(std::to_string(translate.x()).c_str());
        translateYInput->value(std::to_string(translate.y()).c_str());
    }
}

void updateTransformFromFields() {
    if (currentTransformIndex >= 0 && currentTransformIndex < transforms.size()) {
        SomeTransform& transform = transforms[currentTransformIndex];

        double rotAngle = std::stod(rotAngleInput->value());
        double scaleX = std::stod(scaleXInput->value());
        double scaleY = std::stod(scaleYInput->value());
        double translateX = std::stod(translateXInput->value());
        double translateY = std::stod(translateYInput->value());

        transform.setRotation(rotAngle);
        transform.setScale(PointV(scaleX, scaleY));
        transform.setTranslation(PointV(translateX, translateY));
    }
}

void saveTransformsToFile(const char* filename);
void loadTransformsFromFile(const char* filename);


int main() {
    //Fl_Window* window = new Fl_Window(400, 300, "Transform Manager");
    Fl_Window* window = new Fl_Window(1200, 600, "Transform Manager");

    // Transforms Spinner
    transformsSpinner = new Fl_Spinner(150, 20, 100, 25, "Number of Transforms:");
    transformsSpinner->type(FL_INT_INPUT);
    transformsSpinner->minimum(1);
    transformsSpinner->value(3);
    transformsSpinner->callback(onTransformsSpinnerChange);

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

    onTransformsSpinnerChange(nullptr, nullptr); // Initialize with one transform

    Fl_Button* loadButton = new Fl_Button(150, 300, 100, 25, "Load Transforms");
    loadButton->callback([](Fl_Widget*, void*) {
        // Open file chooser for loading
        const char* filename = fl_file_chooser("Open Transform File", "*.txt", nullptr);
        if (filename) {
            // Call function to load transforms
            loadTransformsFromFile(filename);
        }
    });

    Fl_Button* saveButton = new Fl_Button(150, 340, 100, 25, "Save Transforms");
    saveButton->callback([](Fl_Widget*, void*) {
        // Open file chooser for saving
        const char* filename = fl_file_chooser("Save Transform File", "*.txt", nullptr);
        if (filename) {
            // Call function to save transforms
            saveTransformsToFile(filename);
        }
    });


    window->end();
    window->show();

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
