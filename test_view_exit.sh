#!/bin/bash
# Test script to verify animctl view auto-exit feature

cd /home/gkd2162/xv6-riscv

# Kill any existing QEMU
pkill -9 qemu 2>/dev/null

# Start QEMU with proper I/O redirection
(
  # Wait for boot
  sleep 5
  
  # Start animation
  echo "animctl start"
  sleep 1
  
  # Enter view mode for 3 seconds
  (echo "animctl view"; sleep 3) &
  VIEW_PID=$!
  
  sleep 4
  
  # Stop animation (should cause view to exit)
  echo "animctl stop"
  
  sleep 2
  
  # Check if view process exited
  if ps -p $VIEW_PID > /dev/null 2>&1; then
    echo "ERROR: View process still running!"
  else
    echo "SUCCESS: View process exited when animation stopped"
  fi
  
  sleep 1
  echo "exit"
) | timeout 30 make qemu 2>&1 | grep -E "Animation|view|exit|ERROR|SUCCESS"
