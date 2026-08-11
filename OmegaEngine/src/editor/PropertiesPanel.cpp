#include "editor/PropertiesPanel.h"
#include "../core/Log.h"
#include <cmath>
#include <cstdio>

namespace Omega { namespace Editor {

PropertiesPanel::PropertiesPanel(int x, int y, int w, int h) 
    : Fl_Group(x, y, w, h), m_currentEntity(nullptr) {
    
    this->begin();
    
    // عنوان اللوحة
    Fl_Box* title = new Fl_Box(FL_NO_BOX, x, y, w() - 20, 25, "Properties");
    title->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    title->labelfont(FL_BOLD);
    
    createTransformSection();
    
    this->end();
}

PropertiesPanel::~PropertiesPanel() {}

void PropertiesPanel::createTransformSection() {
    int y = 30;
    int labelW = 60;
    int inputW = (w() - 20 - labelW) / 3;
    
    // Position
    new Fl_Box(FL_NO_BOX, 10, y, w() - 20, 20, "Position:");
    y += 20;
    
    m_posX = new Fl_Input(10, y, inputW, 25, "X:");
    m_posX->value("0");
    m_posX->callback(valueChangedCallback, this);
    
    m_posY = new Fl_Input(10 + inputW + 5, y, inputW, 25, "Y:");
    m_posY->value("0");
    m_posY->callback(valueChangedCallback, this);
    
    m_posZ = new Fl_Input(10 + (inputW + 5) * 2, y, inputW, 25, "Z:");
    m_posZ->value("0");
    m_posZ->callback(valueChangedCallback, this);
    
    y += 30;
    
    // Rotation
    new Fl_Box(FL_NO_BOX, 10, y, w() - 20, 20, "Rotation:");
    y += 20;
    
    m_rotX = new Fl_Input(10, y, inputW, 25, "X:");
    m_rotX->value("0");
    m_rotX->callback(valueChangedCallback, this);
    
    m_rotY = new Fl_Input(10 + inputW + 5, y, inputW, 25, "Y:");
    m_rotY->value("0");
    m_rotY->callback(valueChangedCallback, this);
    
    m_rotZ = new Fl_Input(10 + (inputW + 5) * 2, y, inputW, 25, "Z:");
    m_rotZ->value("0");
    m_rotZ->callback(valueChangedCallback, this);
    
    y += 30;
    
    // Scale
    new Fl_Box(FL_NO_BOX, 10, y, w() - 20, 20, "Scale:");
    y += 20;
    
    m_scaleX = new Fl_Input(10, y, inputW, 25, "X:");
    m_scaleX->value("1");
    m_scaleX->callback(valueChangedCallback, this);
    
    m_scaleY = new Fl_Input(10 + inputW + 5, y, inputW, 25, "Y:");
    m_scaleY->value("1");
    m_scaleY->callback(valueChangedCallback, this);
    
    m_scaleZ = new Fl_Input(10 + (inputW + 5) * 2, y, inputW, 25, "Z:");
    m_scaleZ->value("1");
    m_scaleZ->callback(valueChangedCallback, this);
}

void PropertiesPanel::displayEntity(Scene::Entity* entity) {
    if (!entity) return;
    
    m_currentEntity = entity;
    
    // تحديث قيم Transform
    auto transform = entity->getTransform();
    
    char buf[32];
    snprintf(buf, sizeof(buf), "%.2f", transform.position.x);
    m_posX->value(buf);
    snprintf(buf, sizeof(buf), "%.2f", transform.position.y);
    m_posY->value(buf);
    snprintf(buf, sizeof(buf), "%.2f", transform.position.z);
    m_posZ->value(buf);
    
    snprintf(buf, sizeof(buf), "%.2f", transform.rotation.x);
    m_rotX->value(buf);
    snprintf(buf, sizeof(buf), "%.2f", transform.rotation.y);
    m_rotY->value(buf);
    snprintf(buf, sizeof(buf), "%.2f", transform.rotation.z);
    m_rotZ->value(buf);
    
    snprintf(buf, sizeof(buf), "%.2f", transform.scale.x);
    m_scaleX->value(buf);
    snprintf(buf, sizeof(buf), "%.2f", transform.scale.y);
    m_scaleY->value(buf);
    snprintf(buf, sizeof(buf), "%.2f", transform.scale.z);
    m_scaleZ->value(buf);
    
    redraw();
}

void PropertiesPanel::clear() {
    m_currentEntity = nullptr;
    m_posX->value("0");
    m_posY->value("0");
    m_posZ->value("0");
    m_rotX->value("0");
    m_rotY->value("0");
    m_rotZ->value("0");
    m_scaleX->value("1");
    m_scaleY->value("1");
    m_scaleZ->value("1");
    redraw();
}

void PropertiesPanel::applyChanges() {
    if (!m_currentEntity) return;
    
    Scene::Transform transform;
    transform.position.x = atof(m_posX->value());
    transform.position.y = atof(m_posY->value());
    transform.position.z = atof(m_posZ->value());
    transform.rotation.x = atof(m_rotX->value());
    transform.rotation.y = atof(m_rotY->value());
    transform.rotation.z = atof(m_rotZ->value());
    transform.scale.x = atof(m_scaleX->value());
    transform.scale.y = atof(m_scaleY->value());
    transform.scale.z = atof(m_scaleZ->value());
    
    m_currentEntity->setTransform(transform);
    OMEGA_LOG_INFO("Applied transform changes to entity");
}

void PropertiesPanel::valueChangedCallback(Fl_Widget* w, void* data) {
    auto* panel = static_cast<PropertiesPanel*>(data);
    panel->onValueChanged();
}

void PropertiesPanel::onValueChanged() {
    applyChanges();
}

}} // namespace Omega::Editor
