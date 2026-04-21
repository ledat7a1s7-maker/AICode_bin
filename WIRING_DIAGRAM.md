# 🔌 WIRING DIAGRAM - Smart Trash Can

## Complete Hardware Connection Guide

---

## 1️⃣ POWER DISTRIBUTION SYSTEM

### Overview
```
                    Battery (7.4V - 11.1V LiPo)
                              │
                    ┌─────────┴─────────┐
                    │                   │
              ┌─────┴─────┐      ┌─────┴─────┐
              │  LM2596   │      │  LM2596   │
              │   #1      │      │   #2      │
              └─────┬─────┘      └─────┬─────┘
                    │ 5V               │ 5V
                    │                  │
              ┌─────┴─────┐      ┌─────┴─────┐
              │ ESP32-CAM │      │ Arduino   │
              │   VCC     │      │   VIN     │
              └───────────┘      └───────────┘
                    
        Battery (direct) ─────────┐
                                  │
                          ┌───────┴───────┐
                          │               │
                    ┌─────┴─────┐   ┌─────┴─────┐
                    │  L298N    │   │  L298N    │
                    │   #1      │   │   #2      │
                    │  (Left)   │   │  (Right)  │
                    └───────────┘   └───────────┘
```

### LM2596 Setup Instructions

**CRITICAL**: Set output voltage BEFORE connecting to circuits!

1. Connect LM2596 input to battery
2. Connect multimeter to output terminals
3. Adjust potentiometer screw until multimeter reads **5.0V**
4. Double-check voltage
5. NOW connect to ESP32-CAM or Arduino

**Specifications**:
- Input: 7.4V - 11.1V (battery)
- Output: 5V, up to 2A
- Efficiency: ~90%

---

## 2️⃣ ESP32-CAM CONNECTIONS

### Pin Mapping
```
ESP32-CAM Module:
┌─────────────────────────────┐
│                             │
│  [Camera Connector]         │
│                             │
│  VCC  ────────────→ 5V      │
│  GND  ────────────→ GND     │
│  TX (GPIO1) ──────→ Arduino RX (Pin 0)
│  RX (GPIO3) ──────→ Arduino TX (Pin 1)
│                             │
│  Note: Disconnect TX/RX     │
│  when uploading code!       │
│                             │
└─────────────────────────────┘
```

### Power Requirements
- Voltage: 5V (from LM2596)
- Current: ~500mA (with camera active)
- **WARNING**: Do NOT use 3.3V - camera won't work!

---

## 3️⃣ ARDUINO UNO CONNECTIONS

### To L298N #1 (Left Side Motors)

```
Arduino Uno              L298N #1
┌─────────────┐         ┌──────────────┐
│             │         │              │
│  Pin 5  ────┼────────→│  IN1         │
│  Pin 6  ────┼────────→│  IN2         │
│  Pin 9  ────┼────────→│  IN3         │
│  Pin 10 ────┼────────→│  IN4         │
│             │         │              │
│             │         │  ENA ──→ 5V  │
│             │         │  ENB ──→ 5V  │
│             │         │              │
│             │         │  OUT1 ──→    │┐
│             │         │  OUT2 ──→    │┘ Left Front Motor
│             │         │              │
│             │         │  OUT3 ──→    │┐
│             │         │  OUT4 ──→    │┘ Left Rear Motor
│             │         │              │
└─────────────┘         └──────────────┘
```

### To L298N #2 (Right Side Motors)

```
Arduino Uno              L298N #2
┌─────────────┐         ┌──────────────┐
│             │         │              │
│  Pin 11 ────┼────────→│  IN1         │
│  Pin 12 ────┼────────→│  IN2         │
│  Pin 3  ────┼────────→│  IN3         │
│  Pin 2  ────┼────────→│  IN4         │
│             │         │              │
│             │         │  ENA ──→ 5V  │
│             │         │  ENB ──→ 5V  │
│             │         │              │
│             │         │  OUT1 ──→    │┐
│             │         │  OUT2 ──→    │┘ Right Front Motor
│             │         │              │
│             │         │  OUT3 ──→    │┐
│             │         │  OUT4 ──→    │┘ Right Rear Motor
│             │         │              │
└─────────────┘         └──────────────┘
```

### Pin Function Table

| Arduino Pin | L298N Input | Motor Controlled | Direction Control |
|-------------|-------------|------------------|-------------------|
| 5           | IN1         | Left Front       | FORWARD/REVERSE   |
| 6           | IN2         | Left Front       | FORWARD/REVERSE   |
| 9           | IN3         | Left Rear        | FORWARD/REVERSE   |
| 10          | IN4         | Left Rear        | FORWARD/REVERSE   |
| 11          | IN1         | Right Front      | FORWARD/REVERSE   |
| 12          | IN2         | Right Front      | FORWARD/REVERSE   |
| 3           | IN3         | Right Rear       | FORWARD/REVERSE   |
| 2           | IN4         | Right Rear       | FORWARD/REVERSE   |

**Note**: ENA and ENB are connected to 5V for full speed always. For PWM speed control, connect to Arduino PWM pins instead.

---

## 4️⃣ L298N TO MOTORS

### Motor Connection Diagram

```
L298N Board (each):
┌─────────────────────────────────┐
│                                 │
│  12V ───→ Battery +             │
│  GND ───→ Battery - (common)    │
│  5V  ───→ NOT USED (output)     │
│                                 │
│  Motor A:                       │
│    OUT1 ───→ Motor wire 1       │
│    OUT2 ───→ Motor wire 2       │
│                                 │
│  Motor B:                       │
│    OUT3 ───→ Motor wire 1       │
│    OUT4 ───→ Motor wire 2       │
│                                 │
│  Logic Control:                 │
│    IN1, IN2, IN3, IN4           │
│    (from Arduino)               │
│                                 │
└─────────────────────────────────┘
```

### If Motor Spins Wrong Direction

**Option 1**: Swap motor wires on L298N terminals
```
Before: OUT1 → Red wire, OUT2 → Black wire
After:  OUT1 → Black wire, OUT2 → Red wire
```

**Option 2**: Swap IN1/IN2 in code (software fix)

---

## 5️⃣ COMMON GROUND CONNECTION

### ⚠️ CRITICAL: ALL GROUNDS MUST BE CONNECTED ⚠️

```
                    GND Bus Point
                         │
            ┌────────────┼────────────┐
            │            │            │
            │            │            │
        ┌───┴───┐    ┌───┴───┐    ┌───┴───┐
        │ESP32  │    │Arduino│    │Battery│
        │ GND   │    │ GND   │    │  GND  │
        └───────┘    └───────┘    └───────┘
                              ┌───┴───┐
                              │L298N#1│
                              │ L298N#2│
                              └───────┘
```

**How to implement**:
1. Use a breadboard or terminal block as GND bus
2. Connect ALL ground wires to this point
3. Or daisy-chain: ESP32 GND → Arduino GND → Battery GND → L298N GND

**WARNING**: Without common ground, communication will fail!

---

## 6️⃣ COMPLETE WIRING CHECKLIST

### Before Powering On - CHECK EVERYTHING!

#### Power System
- [ ] LM2596 #1 output = 5.0V (measured with multimeter)
- [ ] LM2596 #2 output = 5.0V (measured with multimeter)
- [ ] Battery voltage correct (7.4V or 11.1V)
- [ ] All GND connections secure

#### ESP32-CAM
- [ ] VCC connected to 5V (from LM2596 #1)
- [ ] GND connected to common GND
- [ ] TX (GPIO1) connected to Arduino Pin 0 (RX)
- [ ] RX (GPIO3) connected to Arduino Pin 1 (TX)
- [ ] TX/RX will be disconnected during upload

#### Arduino Uno
- [ ] VIN connected to 5V (from LM2596 #2)
- [ ] GND connected to common GND
- [ ] Pin 5 → L298N #1 IN1
- [ ] Pin 6 → L298N #1 IN2
- [ ] Pin 9 → L298N #1 IN3
- [ ] Pin 10 → L298N #1 IN4
- [ ] Pin 11 → L298N #2 IN1
- [ ] Pin 12 → L298N #2 IN2
- [ ] Pin 3 → L298N #2 IN3
- [ ] Pin 2 → L298N #2 IN4

#### L298N #1 (Left Motors)
- [ ] 12V terminal → Battery +
- [ ] GND terminal → Common GND
- [ ] ENA jumper → 5V
- [ ] ENB jumper → 5V
- [ ] OUT1, OUT2 → Left Front Motor
- [ ] OUT3, OUT4 → Left Rear Motor

#### L298N #2 (Right Motors)
- [ ] 12V terminal → Battery +
- [ ] GND terminal → Common GND
- [ ] ENA jumper → 5V
- [ ] ENB jumper → 5V
- [ ] OUT1, OUT2 → Right Front Motor
- [ ] OUT3, OUT4 → Right Rear Motor

---

## 7️⃣ TESTING PROCEDURE

### Step-by-Step Power On

1. **Double-check all connections** (use checklist above)
2. **Elevate robot** (wheels not touching ground)
3. **Connect battery** to power distribution
4. **Verify LEDs**:
   - ESP32-CAM LED should blink
   - Arduino power LED should be on
   - L298N power LED should be on
5. **Open Serial Monitor** (115200 baud)
6. **Check ESP32-CAM output**:
   ```
   === ESP32-CAM WiFi Bridge ===
   Camera initialized successfully
   WiFi connected!
   IP Address: http://192.168.1.100
   HTTP server started on port 80
   ```
7. **Check Arduino output**:
   ```
   === Arduino Mecanum Control Ready ===
   Waiting for commands from ESP32-CAM...
   ```

### Motor Direction Test

1. Open browser, go to: `http://[ESP32-IP]/command?cmd=FORWARD`
2. Observe all 4 wheels
3. **All wheels should spin FORWARD**
4. If any wheel spins backward:
   - Note which wheel
   - Swap that motor's wires on L298N
   - Test again
5. Repeat for all directions:
   - `BACKWARD` - all wheels reverse
   - `LEFT` - robot strafes left
   - `RIGHT` - robot strafes right
   - `ROTATE_LEFT` - spins counter-clockwise
   - `ROTATE_RIGHT` - spins clockwise
   - `STOP` - all wheels stop

---

## 8️⃣ TROUBLESHOOTING

### ESP32-CAM Not Working
- **No power**: Check 5V at VCC pin
- **Camera fail**: Check ribbon cable connection
- **No WiFi**: Check SSID/password, ensure 2.4GHz network
- **No serial output**: Check baud rate (115200)

### Motors Not Working
- **No movement**: Check L298N power LED
- **One motor not working**: Check wire connections
- **Wrong direction**: Swap motor wires
- **Weak movement**: Check battery voltage

### Communication Issues
- **Commands not working**: Check TX/RX cross-connection
- **Intermittent**: Check loose wires
- **No response**: Verify common ground

---

## 9️⃣ SAFETY WARNINGS

⚠️ **IMPORTANT**:
1. **NEVER** short circuit battery terminals
2. **ALWAYS** set LM2596 voltage BEFORE connecting circuits
3. **VERIFY** all connections before powering on
4. **KEEP** robot elevated during testing
5. **HAVE** someone ready to disconnect battery
6. **DO NOT** touch motors while running
7. **USE** proper gauge wires for motor current
8. **SECURE** battery to prevent movement

---

## 🔟 WIRE SPECIFICATIONS

### Recommended Wire Gauge
- **Power lines** (Battery → L298N): 18-20 AWG
- **Logic lines** (Arduino → L298N): 22-24 AWG (jumper wires OK)
- **Signal lines** (ESP32 ↔ Arduino): 22-24 AWG
- **Camera power**: 20-22 AWG

### Wire Length
- Keep power wires as short as possible
- Signal wires can be longer (up to 50cm)
- Avoid running power and signal wires parallel

---

**Wiring Complete Checklist**: ✅

Once all checks pass, your system is ready for software testing!

Next step: Upload firmware and test each component individually.
