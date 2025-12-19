#!/usr/bin/env python3
# Lightweight helper: boot xv6, send a command, capture output, then exit.
import subprocess, time, os, sys, signal

# Ensure fs.img exists (make fs.img is idempotent)
subprocess.run(["make", "fs.img"], check=True)

# Launch QEMU via make qemu
qemu = subprocess.Popen(["make", "qemu"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
print("QEMU PID:", qemu.pid)

# Give the system time to boot to shell prompt
time.sleep(3)

# Send animctl start
cmd = b"animctl start\n"
print("Sending:", cmd)
try:
    qemu.stdin.write(cmd)
    qemu.stdin.flush()
except Exception as e:
    print("Failed to write to QEMU stdin:", e)

# Wait for a bit to let kernel log appear
time.sleep(4)

# read available output
out = b""
try:
    # non-blocking read attempt
    qemu.stdout.flush()
    while True:
        chunk = qemu.stdout.read(4096)
        if not chunk:
            break
        out += chunk
except Exception:
    pass

text = out.decode('utf-8', 'replace')
print("----- QEMU output (tail) -----")
print(text[-800:])
print("------------------------------")

# terminate QEMU
try:
    # Try gentle termination
    qemu.terminate()
    time.sleep(1)
    if qemu.poll() is None:
        os.kill(qemu.pid, signal.SIGKILL)
except Exception as e:
    print("Failed to terminate QEMU:", e)

sys.exit(0)
