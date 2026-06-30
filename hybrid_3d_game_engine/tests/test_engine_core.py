import json
import os
import sys

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "python")))

from engine_core import Lightweight3DEngine, SceneNode


def test_scene_nodes_and_save_load():
    engine = Lightweight3DEngine()
    player = SceneNode("player", "Player", "player", (1.0, 0.0, 0.0), {"speed": 4.0})
    engine.add_entity(player)

    path = os.path.join(os.path.dirname(__file__), "tmp_scene.json")
    engine.serialize(path)
    assert os.path.exists(path)

    new_engine = Lightweight3DEngine()
    new_engine.deserialize(path)
    assert new_engine.root.children[0].name == "Player"

    os.remove(path)
