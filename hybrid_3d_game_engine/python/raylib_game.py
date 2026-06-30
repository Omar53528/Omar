import json
import math
import os
import sys

try:
    import raylib as rl
except Exception:  # pragma: no cover
    rl = None


def default_state(path=None):
    return {
        "player": {"x": 0.0, "y": 0.0, "health": 100},
        "enemies": [
            {"x": 2.5, "y": 1.8, "health": 45, "kind": "enemy"},
            {"x": -2.2, "y": -1.6, "health": 55, "kind": "enemy"},
        ],
        "trees": [
            {"x": 2.0, "y": 1.2, "kind": "tree"},
            {"x": -2.0, "y": -1.6, "kind": "tree"},
        ],
        "terrain": [
            {"x": 0.0, "y": 0.0, "kind": "grass"},
            {"x": 1.0, "y": 0.0, "kind": "grass"},
            {"x": 0.0, "y": 1.0, "kind": "grass"},
        ],
        "score": 0,
        "message": "Raylib hybrid world",
        "camera": {"x": 0.0, "y": 0.0, "zoom": 1.0},
        "inventory": [{"name": "health_potion", "qty": 1}],
        "quests": [{"id": "q1", "title": "Explore the world", "done": False}],
        "particles": [],
        "save_path": path or os.path.join(os.path.dirname(__file__), "..", "saves", "game_save_raylib.json"),
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
        target = os.path.join(os.path.dirname(__file__), "..", "saves", "game_save_raylib.json")
    if os.path.exists(target):
        with open(target, "r", encoding="utf-8") as handle:
            return json.load(handle)
    return default_state(target)


def clamp(value, minimum, maximum):
    return max(minimum, min(maximum, value))


def step_game(state):
    player = state["player"]
    for enemy in state["enemies"]:
        dx = player["x"] - enemy["x"]
        dy = player["y"] - enemy["y"]
        dist = math.hypot(dx, dy)
        if dist < 0.01:
            continue
        enemy["x"] += (dx / dist) * 0.03
        enemy["y"] += (dy / dist) * 0.03
        if abs(player["x"] - enemy["x"]) < 0.35 and abs(player["y"] - enemy["y"]) < 0.35:
            player["health"] = max(0, player["health"] - 2)
            state["message"] = "Enemy hit you"

    state["camera"]["x"] = player["x"] * 0.2
    state["camera"]["y"] = player["y"] * 0.2

    for particle in state.get("particles", []):
        particle["life"] = max(0, particle["life"] - 1)
    state["particles"] = [p for p in state.get("particles", []) if p["life"] > 0]

    if state.get("quests") and not state["quests"][0].get("done", False) and state["score"] > 0:
        state["quests"][0]["done"] = True
        state["message"] = "Quest complete"

    return state


def move_player(state, dx, dy, sprint=False):
    player = state["player"]
    speed = 0.12 if sprint else 0.06
    new_x = player["x"] + dx * speed
    new_y = player["y"] + dy * speed
    for tree in state["trees"]:
        if abs(new_x - tree["x"]) < 0.35 and abs(new_y - tree["y"]) < 0.35:
            state["message"] = "Tree block"
            return state
    player["x"] = clamp(new_x, -4.5, 4.5)
    player["y"] = clamp(new_y, -4.5, 4.5)
    state["score"] += 1
    state["message"] = "Player moved"
    return state


def run_game(path=None):
    if rl is None:
        print("raylibpy is not installed")
        return 1

    state = load_state(path)
    screen_width = 960
    screen_height = 640
    rl.InitWindow(screen_width, screen_height, b"Hybrid Raylib Game")
    rl.SetTargetFPS(60)

    while not rl.WindowShouldClose():
        step_game(state)

        if rl.IsKeyDown(rl.KEY_RIGHT) or rl.IsKeyDown(rl.KEY_D):
            move_player(state, 1, 0, sprint=rl.IsKeyDown(rl.KEY_LEFT_SHIFT))
        if rl.IsKeyDown(rl.KEY_LEFT) or rl.IsKeyDown(rl.KEY_A):
            move_player(state, -1, 0, sprint=rl.IsKeyDown(rl.KEY_LEFT_SHIFT))
        if rl.IsKeyDown(rl.KEY_UP) or rl.IsKeyDown(rl.KEY_W):
            move_player(state, 0, -1, sprint=rl.IsKeyDown(rl.KEY_LEFT_SHIFT))
        if rl.IsKeyDown(rl.KEY_DOWN) or rl.IsKeyDown(rl.KEY_S):
            move_player(state, 0, 1, sprint=rl.IsKeyDown(rl.KEY_LEFT_SHIFT))
        if rl.IsKeyPressed(rl.KEY_P):
            state["particles"].append({"x": state["player"]["x"], "y": state["player"]["y"], "life": 12})
        if rl.IsKeyPressed(rl.KEY_F5):
            save_state(state)
        if rl.IsKeyPressed(rl.KEY_F6):
            state = load_state(path)
        if rl.IsKeyPressed(rl.KEY_ESCAPE):
            break

        rl.BeginDrawing()
        rl.ClearBackground((14, 24, 37, 255))

        for tile in state["terrain"]:
            x = int(250 + tile["x"] * 90 + state["camera"]["x"] * 80)
            y = int(220 + tile["y"] * 90 + state["camera"]["y"] * 80)
            rl.DrawRectangle(x, y, 70, 70, rl.GREEN)

        for tree in state["trees"]:
            x = int(250 + tree["x"] * 90 + state["camera"]["x"] * 80)
            y = int(220 + tree["y"] * 90 + state["camera"]["y"] * 80)
            rl.DrawCircle(x, y, 24, rl.DARKGREEN)
            rl.DrawRectangle(x - 6, y + 16, 12, 32, rl.BROWN)

        for enemy in state["enemies"]:
            x = int(250 + enemy["x"] * 90 + state["camera"]["x"] * 80)
            y = int(220 + enemy["y"] * 90 + state["camera"]["y"] * 80)
            rl.DrawCircle(x, y, 18, rl.RED)

        player = state["player"]
        px = int(250 + player["x"] * 90 + state["camera"]["x"] * 80)
        py = int(220 + player["y"] * 90 + state["camera"]["y"] * 80)
        rl.DrawCircle(px, py, 16, rl.SKYBLUE)

        for particle in state.get("particles", []):
            pxp = int(250 + particle["x"] * 90 + state["camera"]["x"] * 80)
            pyp = int(220 + particle["y"] * 90 + state["camera"]["y"] * 80)
            rl.DrawCircle(pxp, pyp, 3, rl.YELLOW)

        rl.DrawText(b"Hybrid Raylib Engine", 24, 20, 24, rl.WHITE)
        rl.DrawText(f"Score: {state['score']} | HP: {player['health']}".encode(), 24, 50, 20, rl.LIGHTGRAY)
        rl.DrawText(state["message"].encode(), 24, 78, 20, rl.GOLD)
        rl.DrawText(b"WASD/Arrows move | Shift sprint | P particles | F5 save | F6 load", 24, 108, 18, rl.LIGHTGRAY)

        rl.EndDrawing()

    rl.CloseWindow()
    return 0


if __name__ == "__main__":
    sys.exit(run_game())
