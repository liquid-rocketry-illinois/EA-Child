import matplotlib.pyplot as plt

filepath = "test/log11-7.txt"   # <--- change this to your file

height = []

with open(filepath, "r", errors="ignore") as f:
    for line in f:
        line = line.replace("\x00", "").strip()
        if line == "":
            continue
        try:
            value = float(line)
            height.append(value)
        except:
            continue  # ignore non-float junk lines

# Plot
plt.figure()
plt.plot(height)
plt.xlabel("Sample Index")
plt.ylabel("Calibrated Height")
plt.title("Calibrated Height vs Sample Index")
plt.savefig("calibrated_height.png")

print("Graph saved: calibrated_height.png")
