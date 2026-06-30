import json
import os
import sys
import tempfile
import unittest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "python")))
import gui_game


class LightweightEngineTests(unittest.TestCase):
    def test_default_state_includes_lightweight_features(self):
        state = gui_game.default_state()
        self.assertIn("camera", state)
        self.assertIn("particles", state)
        self.assertIn("quests", state)
        self.assertIn("inventory", state)

    def test_move_player_respects_collision_and_updates_camera(self):
        state = gui_game.default_state()
        gui_game.move_player(state, 1, 0)
        self.assertGreaterEqual(state["camera"]["x"], 0)
        self.assertIn("camera", state)

    def test_save_and_load_round_trip(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            path = os.path.join(tmpdir, "save.json")
            state = gui_game.default_state(path)
            gui_game.save_state(state, path)
            loaded = gui_game.load_state(path)
            self.assertEqual(loaded["player"]["health"], state["player"]["health"])


if __name__ == "__main__":
    unittest.main()
