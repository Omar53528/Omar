import argparse
import json
import math
import os
import sys

try:
    import tkinter as tk
    from tkinter import ttk
except ImportError:  # pragma: no cover
    tk = None
    ttk = None

try:
    import winsound
except ImportError:  # pragma: no cover
    winsound = None


def default_state(path=None):
    return {
        "player": {"x": 0.0, "y": 0.0, "health": 100},
        "enemies": [
            {"x": 3.0, "y": 2.0, "health": 45, "kind": "enemy"},
            {"x": -3.0, "y": -2.0, "health": 55, "kind": "enemy"},
        ],
        "trees": [
            {"x": 2.5, "y": 1.2, "kind": "tree"},
            {"x": -2.2, "y": -1.6, "kind": "tree"},
            {"x": 1.0, "y": -2.2, "kind": "tree"},
        ],
        "terrain": [
            {"x": 0.0, "y": 0.0, "kind": "grass"},
            {"x": 1.0, "y": 0.0, "kind": "grass"},
            {"x": 0.0, "y": 1.0, "kind": "grass"},
            {"x": -1.0, "y": 0.0, "kind": "grass"},
        ],
        "score": 0,
        "message": "Welcome to the hybrid world",
        "selected_node": "player",
        "camera": {"x": 0.0, "y": 0.0, "zoom": 1.0},
        "particles": [{"x": 0.0, "y": 0.0, "life": 8}],
        "quests": [{"id": "q1", "title": "Explore the world", "done": False}],
        "inventory": [{"name": "health_potion", "qty": 1}],
        "save_path": path or os.path.join(os.path.dirname(__file__), "..", "saves", "game_save.json"),
    }


def save_state(state, path=None):
    target = path or state.get("save_path")
    if target is None:
        return None
    os.makedirs(os.path.dirname(target), exist_ok=True)
    with open(target, "w", encoding="utf-8") as handle:
        json.dump(state, handle, indent=2)
    return target


def load_state(path=None):
    target = path
    if target is None:
        target = os.path.join(os.path.dirname(__file__), "..", "saves", "game_save.json")
    if os.path.exists(target):
        with open(target, "r", encoding="utf-8") as handle:
            return json.load(handle)
    return default_state(target)


def clamp(value, minimum, maximum):
    return max(minimum, min(maximum, value))


def play_sound():
    if winsound is not None:
        try:
            winsound.Beep(900, 80)
        except Exception:
            pass


def build_scene_nodes(state):
    root = {"id": "root", "name": "WorldRoot", "kind": "root", "children": []}
    player = {"id": "player", "name": "Player", "kind": "player", "position": (state["player"]["x"], state["player"]["y"]), "children": []}
    enemies = [{"id": f"enemy-{idx}", "name": f"Enemy {idx + 1}", "kind": "enemy", "position": (enemy["x"], enemy["y"]), "children": []} for idx, enemy in enumerate(state["enemies"])]
    trees = [{"id": f"tree-{idx}", "name": f"Tree {idx + 1}", "kind": "tree", "position": (tree["x"], tree["y"]), "children": []} for idx, tree in enumerate(state["trees"])]
    terrain = [{"id": f"terrain-{idx}", "name": f"Tile {idx + 1}", "kind": "terrain", "position": (tile["x"], tile["y"]), "children": []} for idx, tile in enumerate(state["terrain"])]
    root["children"] = [player] + enemies + trees + terrain
    return root


def get_node_path(node_id, root):
    if root["id"] == node_id:
        return [root]
    for child in root.get("children", []):
        if child["id"] == node_id:
            return [root, child]
        nested = get_node_path(node_id, child)
        if nested:
            return [root] + nested
    return []


def step_game(state):
    player = state["player"]
    for enemy in state["enemies"]:
        dx = player["x"] - enemy["x"]
        dy = player["y"] - enemy["y"]
        dist = math.hypot(dx, dy)
        if dist < 0.01:
            continue
        enemy["x"] += (dx / dist) * 0.04
        enemy["y"] += (dy / dist) * 0.04

        if abs(player["x"] - enemy["x"]) < 0.35 and abs(player["y"] - enemy["y"]) < 0.35:
            player["health"] = max(0, player["health"] - 5)
            state["message"] = "Enemy hit you!"
            play_sound()

    if player["health"] <= 0:
        state["message"] = "Game over - load a save or restart"

    if state.get("particles"):
        for particle in state["particles"]:
            particle["life"] = max(0, particle["life"] - 1)
        state["particles"] = [p for p in state["particles"] if p["life"] > 0]

    if state.get("quests") and not state["quests"][0].get("done", False) and state["score"] > 0:
        state["quests"][0]["done"] = True
        state["message"] = "Quest complete: Explore the world"

    return state


def move_player(state, dx, dy, sprint=False):
    player = state["player"]
    speed = 0.7 if sprint else 0.35
    new_x = player["x"] + dx * speed
    new_y = player["y"] + dy * speed
    for tree in state["trees"]:
        if abs(new_x - tree["x"]) < 0.45 and abs(new_y - tree["y"]) < 0.45:
            state["message"] = "Tree block"
            return state
    player["x"] = clamp(new_x, -4.5, 4.5)
    player["y"] = clamp(new_y, -4.5, 4.5)
    state["score"] += 1
    state["camera"]["x"] = player["x"] * 0.25
    state["camera"]["y"] = player["y"] * 0.25
    state["camera"]["zoom"] = 1.0 + min(0.25, state["score"] / 300.0)
    state["message"] = "Player moved" if not sprint else "Player sprinted"
    state["selected_node"] = "player"
    return state


def attack_nearby_enemy(state):
    player = state["player"]
    for enemy in state["enemies"]:
        if abs(player["x"] - enemy["x"]) < 0.9 and abs(player["y"] - enemy["y"]) < 0.9:
            enemy["health"] = max(0, enemy["health"] - 20)
            state["message"] = f"Attacked {enemy['kind']}"
            play_sound()
            if enemy["health"] <= 0:
                state["score"] += 10
                state["message"] = "Enemy defeated"
            return state
    state["message"] = "No enemy nearby"
    return state


def interact_with_world(state):
    player = state["player"]
    for tree in state["trees"]:
        if abs(player["x"] - tree["x"]) < 0.8 and abs(player["y"] - tree["y"]) < 0.8:
            state["message"] = "You inspected a tree node"
            state["selected_node"] = f"tree-{state['trees'].index(tree)}"
            return state
    state["message"] = "No interactable object nearby"
    return state


class GameWindow:
    def __init__(self, root, state):
        self.root = root
        self.state = state
        self.root.title("Hybrid 3D Game - GUI")
        self.root.geometry("1120x680")
        self.root.configure(bg="#0f172a")
        self.root.minsize(1020, 620)

        self.main = ttk.Frame(self.root, padding=10)
        self.main.pack(fill=tk.BOTH, expand=True)
        self.main.grid_columnconfigure(1, weight=1)
        self.main.grid_rowconfigure(0, weight=1)

        self.left_panel = ttk.LabelFrame(self.main, text="Scene Nodes", padding=8)
        self.left_panel.grid(row=0, column=0, sticky="nsew", padx=(0, 8))
        self.left_panel.grid_columnconfigure(0, weight=1)
        self.left_panel.grid_rowconfigure(0, weight=1)

        self.center_panel = ttk.LabelFrame(self.main, text="World View", padding=8)
        self.center_panel.grid(row=0, column=1, sticky="nsew", padx=(0, 8))
        self.center_panel.grid_columnconfigure(0, weight=1)
        self.center_panel.grid_rowconfigure(0, weight=1)

        self.right_panel = ttk.LabelFrame(self.main, text="Inspector", padding=8)
        self.right_panel.grid(row=0, column=2, sticky="nsew")

        self.tree = ttk.Treeview(self.left_panel, show="tree")
        self.tree.grid(row=0, column=0, sticky="nsew")
        self.tree.bind("<<TreeviewSelect>>", self.on_node_select)

        self.canvas = tk.Canvas(self.center_panel, width=620, height=460, bg="#1e293b", highlightthickness=0)
        self.canvas.grid(row=0, column=0, sticky="nsew")

        self.toolbar = ttk.Frame(self.center_panel)
        self.toolbar.grid(row=1, column=0, sticky="ew", pady=(8, 0))
        ttk.Button(self.toolbar, text="Save", command=self.save_game).pack(side=tk.LEFT, padx=4)
        ttk.Button(self.toolbar, text="Load", command=self.load_game).pack(side=tk.LEFT, padx=4)
        ttk.Button(self.toolbar, text="Reset", command=self.reset_game).pack(side=tk.LEFT, padx=4)
        ttk.Button(self.toolbar, text="Attack", command=self.attack_action).pack(side=tk.LEFT, padx=4)
        ttk.Button(self.toolbar, text="Interact", command=self.interact_action).pack(side=tk.LEFT, padx=4)

        self.info = tk.Text(self.right_panel, width=32, height=18, bg="#111827", fg="#f8fafc", padx=8, pady=8)
        self.info.grid(row=0, column=0, sticky="nsew")
        self.info.insert(tk.END, "Advanced shortcuts:\n")
        self.info.insert(tk.END, "- WASD / Arrows: move\n")
        self.info.insert(tk.END, "- Shift + move: sprint\n")
        self.info.insert(tk.END, "- Space: attack\n")
        self.info.insert(tk.END, "- E: interact\n")
        self.info.insert(tk.END, "- F5/F6/F7: save/load/reset\n")
        self.info.insert(tk.END, "- Tab: cycle nodes\n")
        self.info.insert(tk.END, "- P: spawn light particles\n")
        self.info.configure(state=tk.DISABLED)

        self.status_var = tk.StringVar(value="Ready")
        self.status = ttk.Label(self.root, textvariable=self.status_var, anchor=tk.W)
        self.status.pack(fill=tk.X, padx=10, pady=(0, 8))

        self.root.bind("<KeyPress>", self.handle_key)
        self.root.after(140, self.tick)
        self.refresh_scene_tree()
        self.draw()

    def on_node_select(self, _event=None):
        selected = self.tree.selection()
        if selected:
            self.state["selected_node"] = selected[0]
            self.update_inspector()

    def update_inspector(self):
        node_id = self.state.get("selected_node", "player")
        player = self.state["player"]
        lines = [f"Selected node: {node_id}", f"Position: ({player['x']:.2f}, {player['y']:.2f})", f"Health: {player['health']}", f"Score: {self.state['score']}"]
        if node_id == "player":
            lines.append("Type: Player Node")
        elif node_id.startswith("enemy"):
            idx = int(node_id.split("-")[-1])
            enemy = self.state["enemies"][idx]
            lines.append(f"Type: Enemy Node | HP: {enemy['health']}")
        elif node_id.startswith("tree"):
            idx = int(node_id.split("-")[-1])
            tree = self.state["trees"][idx]
            lines.append(f"Type: Tree Node | Position: ({tree['x']:.2f}, {tree['y']:.2f})")
        else:
            lines.append("Type: World Root")
        self.info.configure(state=tk.NORMAL)
        self.info.delete("1.0", tk.END)
        self.info.insert(tk.END, "\n".join(lines))
        self.info.configure(state=tk.DISABLED)

    def refresh_scene_tree(self):
        self.tree.delete(*self.tree.get_children())
        root = build_scene_nodes(self.state)
        self._populate_tree(root, "")

    def _populate_tree(self, node, parent):
        node_id = self.tree.insert(parent, tk.END, node["id"], text=node["name"])
        for child in node.get("children", []):
            self._populate_tree(child, node_id)

    def handle_key(self, event):
        key = event.keysym.lower()
        shift = bool(event.state & 0x1)
        ctrl = bool(event.state & 0x4)

        if ctrl and key == "s":
            self.save_game()
            return
        if ctrl and key == "l":
            self.load_game()
            return
        if key == "escape":
            self.root.destroy()
            return
        if key == "tab":
            self.cycle_selected_node()
            return
        if key == "f5":
            self.save_game()
            return
        if key == "f6":
            self.load_game()
            return
        if key == "f7":
            self.reset_game()
            return
        if key == "space":
            self.attack_action()
            return
        if key == "e":
            self.interact_action()
            return
        if key == "p":
            self.state["particles"].append({"x": self.state["player"]["x"], "y": self.state["player"]["y"], "life": 10})
            self.state["message"] = "Particles spawned"
            self.draw()
            return
        if key in {"w", "up"}:
            move_player(self.state, 0, -1, sprint=shift)
        elif key in {"s", "down"}:
            move_player(self.state, 0, 1, sprint=shift)
        elif key in {"a", "left"}:
            move_player(self.state, -1, 0, sprint=shift)
        elif key in {"d", "right"}:
            move_player(self.state, 1, 0, sprint=shift)
        else:
            self.draw()
            return
        self.refresh_scene_tree()
        self.update_inspector()
        self.draw()

    def cycle_selected_node(self):
        node_ids = ["player"] + [f"enemy-{i}" for i in range(len(self.state["enemies"]))] + [f"tree-{i}" for i in range(len(self.state["trees"]))]
        current = self.state.get("selected_node", "player")
        idx = node_ids.index(current) if current in node_ids else 0
        next_idx = (idx + 1) % len(node_ids)
        self.state["selected_node"] = node_ids[next_idx]
        self.refresh_scene_tree()
        self.update_inspector()
        self.draw()

    def draw(self):
        self.canvas.delete("all")
        self.canvas.create_rectangle(0, 0, 620, 460, fill="#0f172a", outline="")

        for tile in self.state["terrain"]:
            x = 310 + tile["x"] * 70
            y = 230 + tile["y"] * 70
            self.canvas.create_rectangle(x - 30, y - 30, x + 30, y + 30, fill="#4ade80", outline="#166534")

        for tree in self.state["trees"]:
            x = 310 + tree["x"] * 70
            y = 230 + tree["y"] * 70
            self.canvas.create_oval(x - 20, y - 20, x + 20, y + 20, fill="#166534", outline="#14532d")
            self.canvas.create_rectangle(x - 6, y + 10, x + 6, y + 36, fill="#8b5a2b", outline="#6b4423")

        for enemy in self.state["enemies"]:
            x = 310 + enemy["x"] * 70
            y = 230 + enemy["y"] * 70
            self.canvas.create_oval(x - 16, y - 16, x + 16, y + 16, fill="#ef4444", outline="#b91c1c")
            self.canvas.create_text(x, y + 26, text=f"HP {enemy['health']}", fill="white", font=("Segoe UI", 8))

        player = self.state["player"]
        x = 310 + player["x"] * 70
        y = 230 + player["y"] * 70
        self.canvas.create_oval(x - 18, y - 18, x + 18, y + 18, fill="#38bdf8", outline="#0ea5e9")
        self.canvas.create_text(x, y + 28, text="Player", fill="white", font=("Segoe UI", 9))

        self.canvas.create_text(20, 20, anchor="nw", text="Hybrid world", fill="#f8fafc", font=("Segoe UI", 16, "bold"))
        self.canvas.create_text(20, 46, anchor="nw", text=f"Score: {self.state['score']} | HP: {player['health']}", fill="#cbd5e1", font=("Segoe UI", 12))
        self.canvas.create_text(20, 72, anchor="nw", text=self.state["message"], fill="#fde68a", font=("Segoe UI", 11))
        self.canvas.create_text(20, 92, anchor="nw", text=f"Quest: {self.state['quests'][0]['title']} ({'done' if self.state['quests'][0]['done'] else 'active'})", fill="#86efac", font=("Segoe UI", 10))

        for particle in self.state.get("particles", []):
            px = 310 + particle["x"] * 70
            py = 230 + particle["y"] * 70
            self.canvas.create_oval(px - 2, py - 2, px + 2, py + 2, fill="#fde68a", outline="")

        self.status_var.set(f"Selected: {self.state.get('selected_node', 'player')} | Quick actions: Space/E/P/F5/F6/F7")

    def tick(self):
        step_game(self.state)
        self.draw()
        self.root.after(180, self.tick)

    def save_game(self):
        path = save_state(self.state)
        self.state["message"] = f"Saved to {path}"
        self.refresh_scene_tree()
        self.update_inspector()
        self.draw()

    def load_game(self):
        self.state = load_state(self.state.get("save_path"))
        self.state["message"] = "Loaded from save"
        self.refresh_scene_tree()
        self.update_inspector()
        self.draw()

    def reset_game(self):
        self.state = default_state(self.state.get("save_path"))
        self.state["message"] = "Reset complete"
        self.refresh_scene_tree()
        self.update_inspector()
        self.draw()

    def attack_action(self):
        attack_nearby_enemy(self.state)
        self.refresh_scene_tree()
        self.update_inspector()
        self.draw()

    def interact_action(self):
        interact_with_world(self.state)
        self.refresh_scene_tree()
        self.update_inspector()
        self.draw()


def run_gui(path=None):
    if tk is None:
        print("tkinter is not available")
        return 1
    root = tk.Tk()
    app = GameWindow(root, load_state(path))
    root.mainloop()
    return 0


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--headless", action="store_true")
    parser.add_argument("--save", type=str, default=None)
    args = parser.parse_args()

    if args.headless:
        state = load_state(args.save)
        step_game(state)
        save_state(state, args.save)
        print(json.dumps({
            "status": "headless-ok",
            "score": state["score"],
            "hp": state["player"]["health"],
            "message": state["message"],
        }))
        return 0

    return run_gui(args.save)


if __name__ == "__main__":
    sys.exit(main())
