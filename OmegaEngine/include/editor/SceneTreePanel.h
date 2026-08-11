#ifndef SCENE_TREE_PANEL_H
#define SCENE_TREE_PANEL_H

#include <FL/Fl.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Group.H>
#include "../scene/Scene.h"

namespace Omega { namespace Editor {

/**
 * @brief لوحة شجرة المشهد لعرض الكيانات بشكل هرمي
 */
class SceneTreePanel : public Fl_Group {
public:
    SceneTreePanel(int x, int y, int w, int h);
    ~SceneTreePanel();

    // تحديث الشجرة من المشهد الحالي
    void updateFromScene(Scene::Scene* scene);
    
    // تحديد كيان
    void selectEntity(uint32_t entityId);
    uint32_t getSelectedEntityId() const { return m_selectedEntityId; }

    // استدعاء عند التحديد
    using SelectionCallback = void(*)(uint32_t entityId, void* userData);
    void setSelectionCallback(SelectionCallback cb, void* userData) {
        m_selectionCallback = cb;
        m_selectionUserData = userData;
    }

private:
    static void treeCallback(Fl_Tree* tree, void* data);
    void onItemSelected(const char* path);

    Fl_Tree* m_tree;
    Scene::Scene* m_currentScene;
    uint32_t m_selectedEntityId;
    
    SelectionCallback m_selectionCallback;
    void* m_selectionUserData;
};

}} // namespace Omega::Editor

#endif // SCENE_TREE_PANEL_H
