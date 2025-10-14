import matplotlib.pyplot as plt
import numpy as np
import os

# === CONFIG ===
DATA_FILE = "dataTest1072025.txt"   # your data file
OUTPUT_DIR = "plots"
os.makedirs(OUTPUT_DIR, exist_ok=True)

# === Try both encodings automatically ===
def read_lines_auto(filename):
    for enc in ("utf-8", "utf-16", "utf-16le", "utf-16be"):
        try:
            with open(filename, "r", encoding=enc) as f:
                return f.readlines()
        except UnicodeError:
            continue
    raise ValueError("Unable to read file: unsupported encoding")

lines = read_lines_auto(DATA_FILE)

# === Initialize storage ===
timestamps = []
magnetometer, orientation, acceleration, ang_velocity, ang_accel = [], [], [], [], []
failed_times = []

# === Parsing ===
for i, raw_line in enumerate(lines, start=1):
    line = raw_line.strip().replace('\x00', '')
    if not line:
        continue

    parts = line.split('|')
    if len(parts) != 7:
        # If malformed, try to extract start/stop if possible
        try:
            t_start = int(parts[0]) if parts and parts[0].isdigit() else None
        except Exception:
            t_start = None
        if t_start:
            failed_times.append(t_start)
        print(f"⚠️ Skipping malformed line {i}: {line[:50]}...")
        continue

    try:
        t_start = int(parts[0])
        t_stop = int(parts[-1])
        timestamps.append((t_start + t_stop) / 2)  # midpoint
        parse_xyz = lambda s: list(map(float, s.split(',')))
        magnetometer.append(parse_xyz(parts[1]))
        orientation.append(parse_xyz(parts[2]))
        acceleration.append(parse_xyz(parts[3]))
        ang_velocity.append(parse_xyz(parts[4]))
        ang_accel.append(parse_xyz(parts[5]))
    except Exception:
        # If parsing fails, mark approximate time
        try:
            t_start = int(parts[0])
            failed_times.append(t_start)
        except Exception:
            pass
        print(f"⚠️ Error parsing line {i}: {line[:50]}")

# === Convert to arrays ===
timestamps = np.array(timestamps)
groups = {
    "magnetometer": np.array(magnetometer),
    "orientation": np.array(orientation),
    "acceleration": np.array(acceleration),
    "angular_velocity": np.array(ang_velocity),
    "angular_acceleration": np.array(ang_accel),
}

# === Plot helper ===
def plot_group(data, title, ylabel, filename):
    plt.figure(figsize=(10, 6))
    if data.size > 0:
        plt.plot(timestamps, data[:, 0], label='X')
        plt.plot(timestamps, data[:, 1], label='Y')
        plt.plot(timestamps, data[:, 2], label='Z')
    else:
        plt.text(0.5, 0.5, "No data parsed", ha='center', va='center')

    # draw red vertical lines for failed times
    for t in failed_times:
        plt.axvline(x=t, color='r', linestyle='--', linewidth=0.8, alpha=0.7)

    plt.title(title)
    plt.xlabel("Time (ms)")
    plt.ylabel(ylabel)
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(os.path.join(OUTPUT_DIR, filename))
    plt.close()

# === Generate plots ===
plot_group(groups["magnetometer"], "Magnetometer", "Magnetic Field (units)", "magnetometer.png")
plot_group(groups["orientation"], "Orientation (Pitch, Yaw, Roll)", "Degrees", "orientation.png")
plot_group(groups["acceleration"], "Linear Acceleration", "m/s²", "acceleration.png")
plot_group(groups["angular_velocity"], "Angular Velocity", "rad/s", "angular_velocity.png")
plot_group(groups["angular_acceleration"], "Angular Acceleration", "rad/s²", "angular_acceleration.png")

print(f"✅ Plots saved in '{OUTPUT_DIR}' with red lines for {len(failed_times)} failed entries.")
