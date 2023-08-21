#include "InspectorWindow.h"

using namespace CSEditor;

InspectorWindow::InspectorWindow() {

}

InspectorWindow::~InspectorWindow() {

}

void InspectorWindow::SetUI() {
    ImGui::Begin("Inspector");

    ImGui::Text("TEST");

    ImGui::End();
}
