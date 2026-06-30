import os
import subprocess
import sys

root = os.path.dirname(os.path.abspath(__file__))
script = os.path.join(root, "python", "engine_ui.py")

if __name__ == "__main__":
    try:
        subprocess.run([sys.executable, script], cwd=root, check=True)
    except Exception as exc:
        print(f"Failed to launch engine editor: {exc}")
        sys.exit(1)
