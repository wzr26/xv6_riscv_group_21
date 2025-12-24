#!/usr/bin/env python3
import subprocess
import time
import sys

def run_qemu_test():
    """Run QEMU with animation tests"""
    proc = subprocess.Popen(
        ['make', 'qemu'],
        cwd='/home/gkd2162/xv6-riscv',
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        bufsize=1
    )
    
    try:
        # Wait for boot
        time.sleep(6)
        
        # Send commands
        commands = [
            ('animctl start\n', 2),
            ('animctl view\n', 3),
            ('animctl stop\n', 2),
            ('exit\n', 1),
        ]
        
        output = []
        for cmd, delay in commands:
            print(f">> {cmd.strip()}", file=sys.stderr)
            proc.stdin.write(cmd)
            proc.stdin.flush()
            time.sleep(delay)
            
        # Read remaining output
        proc.stdin.close()
        out, _ = proc.communicate(timeout=5)
        print(out[-1000:])  # Last 1000 chars
        
        if 'stopped' in out.lower():
            print("\n✓ SUCCESS: Animation stop command worked")
        if 'exit' in out.lower() or 'exiting' in out.lower():
            print("✓ SUCCESS: View mode may have exited")
            
    except subprocess.TimeoutExpired:
        proc.kill()
        print("Test timed out")
    except Exception as e:
        print(f"Error: {e}")
        proc.kill()

if __name__ == '__main__':
    run_qemu_test()
