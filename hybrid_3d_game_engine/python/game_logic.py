import argparse
import json
import math
import os


def rotate_y(point, angle):
    x, y, z = point
    cos_a = math.cos(angle)
    sin_a = math.sin(angle)
    return (
        x * cos_a + z * sin_a,
        y,
        -x * sin_a + z * cos_a,
    )


def load_map(path=None):
    if path is None:
        default_path = os.path.join(os.path.dirname(__file__), "..", "maps", "simple_map.json")
        path = os.path.abspath(default_path)

    if os.path.exists(path):
        with open(path, "r", encoding="utf-8") as handle:
            return json.load(handle)

    return {
        "name": "starter-room",
        "spawn": [0.0, 0.0, 0.0],
        "floor_size": 8,
        "walls": [
            {"x": -2.5, "y": 0.0, "z": -2.0, "size": 1.0},
            {"x": 2.0, "y": 0.0, "z": 1.0, "size": 1.2},
        ],
    }


def add_box(vertices, edges, center, size):
    cx, cy, cz = center
    half = size / 2.0
    points = [
        (cx - half, cy - half, cz - half),
        (cx + half, cy - half, cz - half),
        (cx + half, cy + half, cz - half),
        (cx - half, cy + half, cz - half),
        (cx - half, cy - half, cz + half),
        (cx + half, cy - half, cz + half),
        (cx + half, cy + half, cz + half),
        (cx - half, cy + half, cz + half),
    ]

    base_index = len(vertices)
    vertices.extend(points)
    edges.extend([
        [base_index + 0, base_index + 1], [base_index + 1, base_index + 2], [base_index + 2, base_index + 3], [base_index + 3, base_index + 0],
        [base_index + 4, base_index + 5], [base_index + 5, base_index + 6], [base_index + 6, base_index + 7], [base_index + 7, base_index + 4],
        [base_index + 0, base_index + 4], [base_index + 1, base_index + 5], [base_index + 2, base_index + 6], [base_index + 3, base_index + 7],
    ])


def build_frame(frame_index: int, player_position=None, map_path=None):
    scene_map = load_map(map_path)
    player = player_position or scene_map.get("spawn", [0.0, 0.0, 0.0])
    vertices = []
    edges = []

    floor_size = scene_map.get("floor_size", 8)
    add_box(vertices, edges, (0.0, -0.6, 0.0), floor_size)
    for wall in scene_map.get("walls", []):
        add_box(vertices, edges, (wall.get("x", 0.0), wall.get("y", 0.0), wall.get("z", 0.0)), wall.get("size", 1.0))

    player_size = 0.35
    add_box(vertices, edges, (player[0], player[1], player[2]), player_size)

    angle = frame_index * 0.12
    rotated = [rotate_y(point, angle) for point in vertices]
    translated = [
        (x + 0.0, y + 0.25 * math.sin(frame_index * 0.2) + 0.4, z + 3.5) for x, y, z in rotated
    ]

    return {
        "frame": frame_index,
        "screen": {"width": 80, "height": 24},
        "player": {"x": round(player[0], 3), "y": round(player[1], 3), "z": round(player[2], 3)},
        "map": {"name": scene_map.get("name", "starter-room")},
        "hud": {
            "title": "Hybrid 3D Game",
            "controls": "WASD/Arrows move, Q/E rotate, ESC exit",
        },
        "vertices": [
            {"x": round(x, 4), "y": round(y, 4), "z": round(z, 4)} for x, y, z in translated
        ],
        "edges": edges,
    }


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--frame", type=int, default=0)
    parser.add_argument("--player-x", type=float, default=0.0)
    parser.add_argument("--player-y", type=float, default=0.0)
    parser.add_argument("--player-z", type=float, default=0.0)
    parser.add_argument("--map", type=str, default=None)
    args = parser.parse_args()
    player_position = [args.player_x, args.player_y, args.player_z]
    print(json.dumps(build_frame(args.frame, player_position, args.map)))
