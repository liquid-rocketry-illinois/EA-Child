import csv
import matplotlib.pyplot as plt

# Path to your data file
filepath = "test/log11-7.txt"

time = []
pitch = []
yaw = []
roll = []

# Store quaternion components
quat_x = []
quat_y = []
quat_z = []
quat_w = []

# Store magnetometer (you already have code for this part, but we include it for clarity)
mag_x = []
mag_y = []
mag_z = []

# Store acceleration
acc_x = []
acc_y = []
acc_z = []

# Store angular velocity (gyro)
gyro_x = []
gyro_y = []
gyro_z = []

# Store environmental data
height = []
pressure = []
temperature = []



with open(filepath, "r", errors="ignore") as f:
    for line in f:
        line = line.replace("\x00", "").strip()     # Remove null bytes
        if not line.startswith("||"):
            continue
        parts = line.split("||")
        if len(parts) < 3:
            continue

        # Main IMU block is in parts[1], env block is in parts[2]
        imu = parts[1].split("|")
        if len(imu) < 7:
            continue

        try:
            # Parse fields
            t_start = float(imu[0])
            quat = list(map(float, imu[1].split(",")))
            mag = list(map(float, imu[2].split(",")))
            pyr = list(map(float, imu[3].split(",")))  # pitch, yaw, roll
            acc = list(map(float, imu[4].split(",")))
            gyro = list(map(float, imu[5].split(",")))
            t_end = float(imu[6])

            # Store orientation values
            time.append(t_start)
            pitch.append(pyr[0])
            yaw.append(pyr[1])
            roll.append(pyr[2])

            # === Parse environment data (height, pressure, temperature) ===
            # The environment block is inside parts[2], but can have extra empty pipes
            env_section = parts[2].strip("|")  # Remove leading/trailing pipes

            if env_section:
                try:
                    env_vals = list(map(float, env_section.split(",")))
                    # height, pressure, temp
                    height.append(env_vals[0])
                    pressure.append(env_vals[1])
                    temperature.append(env_vals[2])
                except:
                    # If malformed, skip this line
                    pass

            # Store useful values
            time.append(t_start)
            pitch.append(pyr[0])
            yaw.append(pyr[1])
            roll.append(pyr[2])
            
            # Save quaternion
            quat_x.append(quat[0])
            quat_y.append(quat[1])
            quat_z.append(quat[2])
            quat_w.append(quat[3])

            # Save magnetometer
            mag_x.append(mag[0])
            mag_y.append(mag[1])
            mag_z.append(mag[2])

            # Save acceleration
            acc_x.append(acc[0])
            acc_y.append(acc[1])
            acc_z.append(acc[2])

            # Save gyroscope
            gyro_x.append(gyro[0])
            gyro_y.append(gyro[1])
            gyro_z.append(gyro[2])

            env_vals = parts[2].split("|")[1].split(",")
            height.append(float(env_vals[0]))
            pressure.append(float(env_vals[1]))
            temperature.append(float(env_vals[2]))
        except:
            continue


# ---- Graphs ----

# Combined Pitch / Yaw / Roll Plot
plt.figure()
plt.plot(time, pitch, label="Pitch")
plt.plot(time, yaw, label="Yaw")
plt.plot(time, roll, label="Roll")

plt.xlabel("Time")
plt.ylabel("Degrees")
plt.title("Pitch / Yaw / Roll vs Time")
plt.legend()   # <-- shows labels
plt.savefig("pyr_combined.png")

print("Graph saved: pyr_combined.png")

# Quaternion Components
plt.figure()
plt.plot(time, quat_w, label="w")
plt.plot(time, quat_x, label="x")
plt.plot(time, quat_y, label="y")
plt.plot(time, quat_z, label="z")
plt.xlabel("Time")
plt.ylabel("Quaternion Value")
plt.title("Quaternion Components vs Time")
plt.legend()
plt.savefig("quat_combined.png")

# Magnetometer (X, Y, Z)
plt.figure()
plt.plot(time, mag_x, label="Mag X")
plt.plot(time, mag_y, label="Mag Y")
plt.plot(time, mag_z, label="Mag Z")
plt.xlabel("Time")
plt.ylabel("Magnetic Field")
plt.title("Magnetometer vs Time")
plt.legend()
plt.savefig("mag_combined.png")

# Acceleration (X, Y, Z)
plt.figure()
plt.plot(time, acc_x, label="Acc X")
plt.plot(time, acc_y, label="Acc Y")
plt.plot(time, acc_z, label="Acc Z")
plt.xlabel("Time")
plt.ylabel("Acceleration (m/s²)")
plt.title("Acceleration vs Time")
plt.legend()
plt.savefig("acc_combined.png")

# Gyroscope (X, Y, Z)
plt.figure()
plt.plot(time, gyro_x, label="Gyro X")
plt.plot(time, gyro_y, label="Gyro Y")
plt.plot(time, gyro_z, label="Gyro Z")
plt.xlabel("Time")
plt.ylabel("Angular Velocity (rad/s)")
plt.title("Gyroscope vs Time")
plt.legend()
plt.savefig("gyro_combined.png")

# Height
plt.figure()
plt.plot(time, height)
plt.xlabel("Time")
plt.ylabel("Height")
plt.title("Height vs Time")
plt.savefig("height.png")

# Pressure
plt.figure()
plt.plot(time, pressure)
plt.xlabel("Time")
plt.ylabel("Pressure")
plt.title("Pressure vs Time")
plt.savefig("pressure.png")

# Temperature
plt.figure()
plt.plot(time, temperature)
plt.xlabel("Time")
plt.ylabel("Temperature (C)")
plt.title("Temperature vs Time")
plt.savefig("temperature.png")
