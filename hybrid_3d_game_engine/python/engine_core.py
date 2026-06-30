import json
import math
import os


class SceneNode:
    def __init__(self, node_id, name, kind, position=(0.0, 0.0, 0.0), props=None):
        self.id = node_id
        self.name = name
        self.kind = kind
        self.position = list(position)
        self.props = props or {}
        self.children = []

    def add_child(self, child):
        self.children.append(child)
        return child

    def to_dict(self):
        return {
            "id": self.id,
            "name": self.name,
            "kind": self.kind,
            "position": self.position,
            "props": self.props,
            "children": [child.to_dict() for child in self.children],
        }


class Lightweight3DEngine:
    def __init__(self):
        self.root = SceneNode("root", "WorldRoot", "root")
        self.camera = {"x": 0.0, "y": 0.0, "z": 0.0, "fov": 70.0}
        self.entities = []
        self.resources = {"meshes": [], "materials": []}

    def add_entity(self, node):
        self.entities.append(node)
        self.root.add_child(node)
        return node

    def update(self, dt=0.016):
        for entity in self.entities:
            if entity.kind == "player":
                entity.position[0] += 0.0
            if entity.kind == "enemy":
                entity.position[0] += 0.001

    def serialize(self, path):
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, "w", encoding="utf-8") as handle:
            json.dump(self.root.to_dict(), handle, indent=2)

    def deserialize(self, path):
        if not os.path.exists(path):
            return
        with open(path, "r", encoding="utf-8") as handle:
            data = json.load(handle)
        self.root = SceneNode(data["id"], data["name"], data["kind"])
        self._load_children(self.root, data.get("children", []))

    def _load_children(self, parent, children):
        for child in children:
            node = SceneNode(child["id"], child["name"], child["kind"], child.get("position", (0.0, 0.0, 0.0)), child.get("props", {}))
            parent.add_child(node)
            self._load_children(node, child.get("children", []))
