import json
import os
import sys

try:
    import tkinter as tk
    from tkinter import ttk
except Exception:  # pragma: no cover
    tk = None
    ttk = None

from engine_core import Lightweight3DEngine, SceneNode


class EngineEditor:
    def __init__(self, root):
        self.root = root
        self.root.title("Lightweight 3D Game Engine")
        self.root.geometry("960x620")
        self.engine = Lightweight3DEngine()
        self._build_ui()
        self._populate_scene()

    def _build_ui(self):
        self.main = ttk.Frame(self.root, padding=8)
        self.main.pack(fill=tk.BOTH, expand=True)

        self.left = ttk.LabelFrame(self.main, text="Scene Tree", padding=8)
        self.left.pack(side=tk.LEFT, fill=tk.Y, padx=(0, 8))
        self.tree = ttk.Treeview(self.left, show="tree")
        self.tree.pack(fill=tk.BOTH, expand=True)

        self.right = ttk.Frame(self.main)
        self.right.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)

        self.toolbar = ttk.Frame(self.right)
        self.toolbar.pack(fill=tk.X, pady=(0, 8))
        ttk.Button(self.toolbar, text="Add Player", command=self.add_player).pack(side=tk.LEFT, padx=4)
        ttk.Button(self.toolbar, text="Add Enemy", command=self.add_enemy).pack(side=tk.LEFT, padx=4)
        ttk.Button(self.toolbar, text="Add Cube", command=self.add_cube).pack(side=tk.LEFT, padx=4)
        ttk.Button(self.toolbar, text="Save Scene", command=self.save_scene).pack(side=tk.LEFT, padx=4)

        self.inspector = ttk.LabelFrame(self.right, text="Inspector", padding=8)
        self.inspector.pack(fill=tk.BOTH, expand=True)
        self.inspector_text = tk.Text(self.inspector, wrap=tk.WORD, height=16, width=42)
        self.inspector_text.pack(fill=tk.BOTH, expand=True)

        self.log = ttk.LabelFrame(self.right, text="Engine Log", padding=8)
        self.log.pack(fill=tk.BOTH, expand=True, pady=(8, 0))
        self.log_text = tk.Text(self.log, height=8, width=42)
        self.log_text.pack(fill=tk.BOTH, expand=True)
        self.log_text.insert(tk.END, "Lightweight 3D engine ready\n")

    def _populate_scene(self):
        self.tree.delete(*self.tree.get_children())
        self._add_tree_node(self.engine.root, "")

    def _add_tree_node(self, node, parent):
        tree_id = self.tree.insert(parent, tk.END, text=node.name)
        for child in node.children:
            self._add_tree_node(child, tree_id)

    def add_player(self):
        node = SceneNode("player_1", "Player", "player", (0.0, 0.0, 0.0), {"speed": 3.0})
        self.engine.add_entity(node)
        self._populate_scene()
        self._log("Added Player node")

    def add_enemy(self):
        node = SceneNode("enemy_1", "Enemy", "enemy", (2.0, 0.0, 0.0), {"ai": "chase"})
        self.engine.add_entity(node)
        self._populate_scene()
        self._log("Added Enemy node")

    def add_cube(self):
        node = SceneNode("cube_1", "Cube", "mesh", (0.0, 1.0, 0.0), {"mesh": "cube", "material": "simple"})
        self.engine.add_entity(node)
        self._populate_scene()
        self._log("Added Cube node")

    def save_scene(self):
        path = os.path.join(os.path.dirname(__file__), "..", "saves", "engine_scene.json")
        self.engine.serialize(path)
        self._log(f"Saved scene to {path}")

    def _log(self, message):
        self.log_text.insert(tk.END, message + "\n")
        self.log_text.see(tk.END)


def run_editor():
    if tk is None:
        print("tkinter is not available")
        return 1
    root = tk.Tk()
    app = EngineEditor(root)
    root.mainloop()
    return 0


if __name__ == "__main__":
    sys.exit(run_editor())
