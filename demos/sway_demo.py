#!/usr/bin/env python3
import json
import subprocess
import threading
import shlex

def run_json(cmd: str):
	p = subprocess.run(shlex.split(cmd), capture_output=True, text=True, check=True)
	return json.loads(p.stdout)

def focused_output_name():
	outputs = run_json("swaymsg -r -t get_outputs")
	for o in outputs:
		if o.get("focused"):
			return o["name"]
	return outputs[0]["name"] if outputs else None

def workspaces():
	return run_json("swaymsg -r -t get_workspaces")

def render_state():
	out = focused_output_name()
	ws = workspaces()
	on_out = [w for w in ws if w.get("output") == out]

	on_out.sort(key=lambda w: (w.get("num", 9999), w.get("name", "")))

	labels = []
	for w in on_out:
		name = w.get("name", "?")
		if w.get("focused"):
			labels.append(f"[{name}]")
		elif w.get("visible"):
			labels.append(f"({name})")
		else:
			labels.append(name)

	print("\n--- SWAY STATE ---")
	print(f"Focused output: {out}")
	print(f"Workspace count on output: {len(on_out)}")
	print("Workspaces:", " ".join(labels) if labels else "(none)")
	print("------------------\n")

def switch_workspace(target: str):
	# target can be "1", "2", or workspace name
	subprocess.run(["swaymsg", "workspace", target], check=False)

def subscribe_loop():
	# -m keeps swaymsg running and streaming events
	cmd = ["swaymsg", "-m", "-r", "-t", "subscribe", '["workspace","output"]']
	with subprocess.Popen(cmd, stdout=subprocess.PIPE, text=True, bufsize=1) as p:
		for line in p.stdout:
			line = line.strip()
			if not line:
				continue
			try:
				_event = json.loads(line)  # parse to validate
			except json.JSONDecodeError:
				continue
			render_state()

def main():
	render_state()

	t = threading.Thread(target=subscribe_loop, daemon=True)
	t.start()

	print("Type a workspace number/name to switch, or 'q' to quit.")
	while True:
		s = input("> ").strip()
		if s.lower() in {"q", "quit", "exit"}:
			break
		if s:
			switch_workspace(s)

if __name__ == "__main__":
	main()
