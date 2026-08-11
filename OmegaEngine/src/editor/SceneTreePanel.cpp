#include "editor/SceneTreePanel.h"
#include "../core/Log.h"

namespace Omega { namespace Editor {

SceneTreePanel::SceneTreePanel(int x, int y, int w, int h) 
    : Fl_Group(x, y, w, h), m_currentScene(nullptr), m_selectedEntityId(0),
      m_selectionCallback(nullptr), m_selectionUserData(nullptr) {
    
    this->begin();
    
    m_tree = new Fl_Tree(x, y + 25, w, h - 25);
    m_tree->rootlabel("Scene");
    m_tree->callback(treeCallback, this);
    m_tree->showcollapse(true);
    m_tree->showconnector(true);
    
    this->end();
}

SceneTreePanel::~SceneTreePanel() {}

void SceneTreePanel::updateFromScene(Scene::Scene* scene) {
    if (!scene) return;
    
    m_currentScene = scene;
    m_tree->clear();
    
    // إضافة الكيانات للشجرة
    auto& entities = scene->getEntities();
    for (auto& entity : entities) {
        char label[256];
        snprintf(label, sizeof(label), "Entity_%u", entity.getId());
        
        Fl_Tree_Item* item = m_tree->add(label);
        item->user_data((void*)(uintptr_t)entity.getId());
        
        // إضافة المكونات كعناصر فرعية
        auto& components = entity.getComponents();
        for (auto& comp : components) {
            char compLabel[256];
            snprintf(compLabel, sizeof(compLabel), "%s/%s", label, comp->getName().c_str());
            Fl_Tree_Item* compItem = m_tree->add(compLabel);
            compItem->user_data((void*)(uintptr_t)entity.getId());
        }
    }
    
    m_tree->redraw();
}

void SceneTreePanel::selectEntity(uint32_t entityId) {
    m_selectedEntityId = entityId;
    m_tree->redraw();
}

void SceneTreePanel::treeCallback(Fl_Tree* tree, void* data) {
    auto* panel = static_cast<SceneTreePanel*>(data);
    
    if (tree->callback_item()) {
        const char* path = tree->callback_item()->label();
        panel->onItemSelected(path);
    }
}

void SceneTreePanel::onItemSelected(const char* path) {
    // استخراج ID الكيان من المسار
    Fl_Tree_Item* item = m_tree->find_item(path);
    if (item && item->user_data()) {
        uint32_t entityId = (uint32_t)(uintptr_t)item->user_data();
        m_selectedEntityId = entityId;
        
        OMEGA_LOG_INFO("Selected entity: {}", entityId);
        
        if (m_selectionCallback) {
            m_selectionCallback(entityId, m_selectionUserData);
        }
    }
}

}} // namespace Omega::Editor
