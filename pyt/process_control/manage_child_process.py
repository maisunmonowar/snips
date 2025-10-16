import argparse
import os
import subprocess
import sys
import time

#!/usr/bin/env python3
"""
manage_child_process.py

Usage:
    python manage_child_process.py "C:\Path\to\your.exe" [arg1 arg2 ...]

This script launches the given executable, waits 10 seconds, then tries to terminate it
gracefully and force-kill if it doesn't exit.
"""

def main():
    parser = argparse.ArgumentParser(description="Launch an EXE, wait 10s, then terminate it.")
    parser.add_argument("exe", help="Path to the executable to run")
    parser.add_argument("args", nargs=argparse.REMAINDER, help="Arguments passed to the executable")
    ns = parser.parse_args()

    exe_path = ns.exe
    if not os.path.isabs(exe_path):
        exe_path = os.path.abspath(exe_path)

    if not os.path.exists(exe_path):
        print(f"Executable not found: {exe_path}", file=sys.stderr)
        sys.exit(2)

    cmd = [exe_path] + ns.args

    creationflags = 0
    # On Windows, create a new process group so signals/termination are isolated
    if os.name == "nt":
        creationflags = subprocess.CREATE_NEW_PROCESS_GROUP

    try:
        proc = subprocess.Popen(cmd, creationflags=creationflags)
    except Exception as e:
        print(f"Failed to start process: {e}", file=sys.stderr)
        sys.exit(1)

    print(f"Launched PID {proc.pid}; waiting 10 seconds...")
    try:
        time.sleep(10)
    except KeyboardInterrupt:
        print("Interrupted while waiting; proceeding to terminate process.")

    print("Attempting graceful terminate...")
    try:
        proc.terminate()
        proc.wait(timeout=5)
        print("Process terminated gracefully.")
    except subprocess.TimeoutExpired:
        print("Process did not exit after terminate(); force-killing...")
        try:
            proc.kill()
            proc.wait(timeout=5)
            print("Process killed.")
        except Exception as e:
            print(f"Failed to kill process: {e}", file=sys.stderr)
    except Exception as e:
        print(f"Error while terminating process: {e}", file=sys.stderr)
    finally:
        if proc.poll() is None:
            # Last resort: on Windows use taskkill to remove any child processes
            if os.name == "nt":
                try:
                    subprocess.run(["taskkill", "/F", "/T", "/PID", str(proc.pid)], check=False, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                except Exception:
                    pass
        print("Done.")

if __name__ == "__main__":
    main()