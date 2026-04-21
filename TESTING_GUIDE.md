# 🧪 TESTING GUIDE - Smart Trash Can

Complete step-by-step testing procedures for each component.

---

## 📋 TESTING PHILOSOPHY

**Test incrementally**: 
1. Test each component individually
2. Verify it works before moving to next
3. Then integrate components together

**Never skip steps** - this will save you hours of debugging!

---

## 🔧 TOOLS NEEDED

- Multimeter
- Serial Monitor (Arduino IDE)
- Web browser
- Python terminal
- Qt Creator (later)
- Screwdriver (for LM2596 adjustment)

---

## 1️⃣ TEST LM2596 VOLTAGE REGULATORS

**Time**: 10 minutes
**Priority**: CRITICAL - Do this FIRST!

### Procedure

1. **Connect LM2596 to battery**
   - Input: Battery + and -
   - Do NOT connect output yet!

2. **Set multimeter to DC voltage**

3. **Connect multimeter to output terminals**
   - Red probe → OUT+
   - Black probe → OUT-

4. **Power on battery**

5. **Adjust potentiometer**
   - Use small screwdriver
   - Turn slowly clockwise or counterclockwise
   - Watch multimeter reading

6. **Set to exactly 5.0V**
   - Target: 5.0V ± 0.1V
   - Acceptable range: 4.9V - 5.1V

7. **Power off and double-check**
   - Turn off battery
   - Turn on again
   - Verify still reads 5.0V

8. **Repeat for second LM2596**

### Expected Result
✅ Multimeter reads 5.0V

### If Wrong
❌ Reading 0V: Check input connection
❌ Reading >5V: Adjust potentiometer more
❌ Reading <5V: Adjust potentiometer more
❌ LM2596 gets hot: Disconnect immediately, check for short circuit

---

## 2️⃣ TEST ESP32-CAM

**Time**: 15 minutes
**Priority**: HIGH

### Step 2A: Upload Firmware

1. **Open Arduino IDE**
2. **Open** `esp32cam_wifi_bridge.ino`
3. **Edit WiFi credentials**:
   ```cpp
   const char* ssid = "YourWiFiName";
   const char* password = "YourPassword";
   ```
4. **Board settings**:
   - Board: "AI Thinker ESP32-CAM"
   - Partition Scheme: "Minimal SPIFFS"
   - Upload Speed: 115200
5. **Disconnect TX/RX pins**
6. **Connect FTDI programmer** to ESP32-CAM
7. **Press and hold FLASH button**
8. **Click Upload**
9. **Release FLASH button** when upload starts
10. **Wait for "Done uploading"**

### Step 2B: Test Camera

1. **Reconnect TX/RX pins**
2. **Connect power** (5V from LM2596)
3. **Open Serial Monitor**
   - Baud rate: 115200
   - Both NL & CR
4. **Press ESP32-CAM reset button**
5. **Watch output**:

**Expected Output**:
```
=== ESP32-CAM WiFi Bridge ===
Starting system...
Camera initialized successfully
Connecting to WiFi.....
WiFi connected!
IP Address: http://192.168.1.100
/stream - Video stream
/command?cmd=XXX - Motor control
HTTP server started on port 80
=== System Ready ===
```

6. **Note the IP address** (e.g., 192.168.1.100)

### Step 2C: Test Video Stream

1. **Open web browser**
2. **Go to**: `http://[YOUR-IP]/stream`
   - Example: `http://192.168.1.100/stream`
3. **You should see live video** from camera

### Expected Result
✅ Browser shows live camera feed

### If Wrong
❌ "Camera init failed": 
  - Check camera ribbon cable
  - Check 5V power supply
  - Try lower resolution in code

❌ "WiFi connection failed":
  - Check SSID and password (case-sensitive)
  - Ensure 2.4GHz WiFi (not 5GHz)
  - Move closer to router

❌ Browser can't connect:
  - Check IP address is correct
  - Ping the IP: `ping 192.168.1.100`
  - Check firewall settings

---

## 3️⃣ TEST ARDUINO UNO

**Time**: 10 minutes
**Priority**: HIGH

### Step 3A: Upload Firmware

1. **Open Arduino IDE**
2. **Open** `arduino_mecanum_control.ino`
3. **Board settings**:
   - Board: "Arduino Uno"
   - Port: Correct COM port
4. **Click Upload**
5. **Wait for "Done uploading"**

### Step 3B: Test Serial Communication

1. **Open Serial Monitor**
   - Baud rate: 115200
2. **Press Arduino reset button**

**Expected Output**:
```
=== Arduino Mecanum Control Ready ===
Waiting for commands from ESP32-CAM...
```

3. **Type command in Serial Monitor** and press Send:
   - Type: `FORWARD`
   - Click Send

**Expected Output**:
```
Executing: FORWARD
```

### Expected Result
✅ Arduino responds to commands

### If Wrong
❌ No output: Check baud rate, upload again
❌ "Unknown command": Check spelling, must be exact

---

## 4️⃣ TEST ESP32-CAM → ARDUINO COMMUNICATION

**Time**: 5 minutes
**Priority**: CRITICAL

### Setup

1. **Connect ESP32-CAM to Arduino**:
   - ESP32 TX → Arduino Pin 0 (RX)
   - ESP32 RX → Arduino Pin 1 (TX)
   - **Common GND connected**

2. **Power both devices**

### Test

1. **Open Arduino Serial Monitor**
2. **Open browser**, go to:
   `http://[ESP32-IP]/command?cmd=FORWARD`

**Expected Arduino Output**:
```
Command received: FORWARD
Executing: FORWARD
```

3. **Test all commands**:
   - `/command?cmd=BACKWARD`
   - `/command?cmd=LEFT`
   - `/command?cmd=RIGHT`
   - `/command?cmd=ROTATE_LEFT`
   - `/command?cmd=ROTRIGHT`
   - `/command?cmd=STOP`

### Expected Result
✅ Arduino receives and executes all commands

### If Wrong
❌ Arduino doesn't receive:
  - Check TX/RX cross-connection
  - Check common GND
  - Verify baud rate matches (115200)

❌ Garbled text:
  - Baud rate mismatch
  - Loose connections

---

## 5️⃣ TEST MOTOR DIRECTIONS

**Time**: 20 minutes
**Priority**: CRITICAL - Safety first!

### ⚠️ SAFETY PRECAUTIONS

- **ROBOT MUST BE ELEVATED** (wheels not touching ground)
- **Keep hands away from wheels**
- **Have someone ready to disconnect battery**
- **Test one command at a time**

### Procedure

1. **Elevate robot** on blocks or hold it

2. **Power everything on**

3. **Test FORWARD**:
   - Browser: `http://[IP]/command?cmd=FORWARD`
   - **Observe**: All 4 wheels should spin forward
   - **Duration**: 2 seconds, then send STOP

4. **If any wheel spins backward**:
   - Note which wheel (e.g., "Left Front")
   - Power OFF
   - Swap that motor's wires on L298N
   - Power ON and test again

5. **Test all directions**:

| Command | Expected Behavior |
|---------|------------------|
| FORWARD | All wheels forward |
| BACKWARD | All wheels backward |
| LEFT | Robot strafes left (special Mecanum pattern) |
| RIGHT | Robot strafes right (special Mecanum pattern) |
| ROTATE_LEFT | Robot spins counter-clockwise |
| ROTATE_RIGHT | Robot spins clockwise |
| STOP | All wheels stop immediately |

### Expected Result
✅ All directions work correctly

### If Wrong
❌ One direction wrong: Check motor wiring for that wheel
❌ All directions wrong: Check Arduino pin mapping
❌ Intermittent: Check loose wires

---

## 6️⃣ TEST PYTHON AI DETECTION

**Time**: 20 minutes
**Priority**: HIGH

### Step 6A: Install Dependencies

```bash
pip install ultralytics opencv-python numpy requests
```

**Wait for YOLOv8 model download** (~6MB on first run)

### Step 6B: Test with Camera Stream

1. **Ensure ESP32-CAM is streaming**
2. **Run test script**:
   ```bash
   cd SmartTrashCan
   python ai_detection.py --test
   ```

3. **Enter ESP32-CAM IP** when prompted:
   ```
   Enter ESP32-CAM IP (default: 192.168.1.100):
   ```

4. **Show water bottle to camera**

**Expected Output**:
```
Loading YOLOv8 model...
Model loaded successfully!
Connecting to http://192.168.1.100/stream...
Connected! Starting detection...

Frame 1: bottle at (160, 120), confidence: 0.85, area: 12000
{"detected": true, "object": "bottle", "x": 160, "y": 120, ...}

Frame 2: No object detected
{"detected": false}

Frame 3: bottle at (165, 118), confidence: 0.82, area: 12500
{"detected": true, "object": "bottle", "x": 165, "y": 118, ...}
```

### Expected Result
✅ Detects bottle with position and confidence

### If Wrong
❌ "Module not found": Run pip install again
❌ "Connection refused": Check ESP32-CAM IP
❌ No detections:
  - Lower confidence threshold to 0.3
  - Use clear plastic water bottle
  - Improve lighting
  - Hold bottle steady

❌ Slow detection (<2 FPS):
  - Use yolov8n.pt (nano model) - already configured
  - Close other programs
  - Check CPU usage

---

## 7️⃣ TEST QT APPLICATION

**Time**: 30 minutes
**Priority**: MEDIUM

### Step 7A: Build Qt Project

1. **Open Qt Creator**
2. **Open** `SmartTrashCanQt.pro`
3. **Configure project** (let it run CMake/qmake)
4. **Build** (Ctrl+B)
5. **Fix any compilation errors** (should be minimal)
6. **Run** (Ctrl+R)

### Step 7B: Test UI Components

1. **Application opens** with main window

2. **Enter ESP32-CAM IP**
   - Type IP in text field
   - Click "Connect"

3. **Camera display**:
   - Should show live video
   - Update at 5-10 FPS

4. **Test Manual Mode**:
   - Select "Manual" radio button
   - Press and hold "Forward" button
   - Robot should move forward
   - Release button - robot should stop

5. **Test all buttons**:
   - Forward, Backward, Left, Right
   - Rotate Left, Rotate Right
   - Stop

6. **Test Auto Mode**:
   - Select "Auto" radio button
   - Show bottle to camera
   - Robot should track automatically

### Expected Result
✅ All UI components work

### If Wrong
❌ Can't build: Check Qt installation, verify all files present
❌ Camera doesn't display: Check IP, test in browser first
❌ Buttons don't work: Check console output for errors

---

## 8️⃣ INTEGRATION TEST - AUTO MODE

**Time**: 30 minutes
**Priority**: HIGHEST - This is the main demo!

### Setup

1. **Place robot on floor**
2. **Clear area** (2m x 2m minimum)
3. **Power everything on**
4. **Start Qt application**
5. **Connect to ESP32-CAM**
6. **Select Auto Mode**

### Test Scenarios

#### Scenario 1: Direct Approach
1. **Place bottle** 1.5m in front of robot
2. **Robot should**:
   - Detect bottle
   - Move forward
   - Stop when close (~30cm)

✅ **Pass**: Robot reaches bottle and stops

#### Scenario 2: Left Side
1. **Place bottle** 1m to the left
2. **Robot should**:
   - Rotate left
   - Move forward
   - Stop at bottle

✅ **Pass**: Robot tracks and reaches bottle

#### Scenario 3: Right Side
1. **Place bottle** 1m to the right
2. **Robot should**:
   - Rotate right
   - Move forward
   - Stop at bottle

✅ **Pass**: Robot tracks and reaches bottle

#### Scenario 4: Lost and Found
1. **Start tracking** bottle
2. **Remove bottle** from view
3. **Robot should**:
   - Enter search mode (rotate slowly)
4. **Show bottle again**
5. **Robot should**:
   - Resume tracking

✅ **Pass**: Robot searches and re-acquires target

#### Scenario 5: Orange Detection
1. **Show orange peel** to camera
2. **Robot should**:
   - Detect "orange"
   - Track same as bottle

✅ **Pass**: Detects and tracks orange

### Expected Result
✅ All 5 scenarios pass

---

## 9️⃣ FINAL DEMO TEST

**Time**: 15 minutes
**Priority**: ULTIMATE GOAL

### Demo Script

1. **Introduction** (30 seconds)
   - Show hardware setup
   - Explain components

2. **Manual Mode Demo** (1 minute)
   - Control robot with Qt app
   - Show all 7 directions
   - Smooth, responsive control

3. **Auto Mode Demo** (2 minutes)
   - Place bottle on floor
   - Robot automatically tracks
   - Show detection on screen
   - Robot stops at correct distance

4. **Multi-object Demo** (1 minute)
   - Show bottle - tracks
   - Show orange - tracks
   - Explain AI detection

5. **Q&A**

### Success Criteria
- [ ] No crashes during demo
- [ ] Smooth movement
- [ ] Accurate detection
- [ ] Professional presentation

---

## 📊 TESTING CHECKLIST SUMMARY

| Test | Time | Status | Notes |
|------|------|--------|-------|
| LM2596 Voltage | 10 min | ⬜ | Must be 5.0V |
| ESP32-CAM Upload | 15 min | ⬜ | WiFi + Camera |
| Arduino Upload | 10 min | ⬜ | Serial commands |
| Communication | 5 min | ⬜ | ESP32 → Arduino |
| Motor Directions | 20 min | ⬜ | All 7 directions |
| Python AI | 20 min | ⬜ | Bottle detection |
| Qt Application | 30 min | ⬜ | UI + Control |
| Auto Mode | 30 min | ⬜ | Full integration |
| Final Demo | 15 min | ⬜ | Presentation ready |

**Total Testing Time**: ~2.5 hours

---

## 🐛 COMMON ISSUES & FIXES

### Problem: Robot spins in circles
**Fix**: Check Mecanum wheel orientation - wheels must be mounted correctly

### Problem: Detection too slow
**Fix**: Use smaller image size, reduce FPS

### Problem: Robot overshoots target
**Fix**: Increase NEAR_THRESHOLD_AREA in config.h

### Problem: WiFi disconnects
**Fix**: Check signal strength, move closer to router

### Problem: Motors get hot
**Fix**: Check for mechanical binding, reduce voltage

---

## ✅ READY FOR DEMO WHEN:

- [ ] All tests pass
- [ ] Demo runs 3 times in a row without failure
- [ ] Can explain every component
- [ ] Have backup video recording
- [ ] Battery fully charged
- [ ] All cables and tools packed

---

**Good luck with testing! 🎯**

Remember: Test incrementally, don't skip steps, and document any issues you find.
