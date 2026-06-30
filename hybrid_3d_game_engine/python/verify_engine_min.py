import os, sys
base = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.insert(0, os.path.join(base, "python"))
from engine_core import Lightweight3DEngine, SceneNode
engine = Lightweight3DEngine()
engine.add_entity(SceneNode('p1', 'Player', 'player', (1,0,0)))
engine.add_entity(SceneNode('e1', 'Enemy', 'enemy', (2,0,0)))
print('entities', len(engine.entities))
print('tree-root-children', len(engine.root.children))
path = os.path.join(base, "saves", "verify_scene.json")
engine.serialize(path)
print('saved', os.path.exists(path))
