# MINI DRONE - Wiring Diagram (ESP32-C3)

## 📌 PIN LAYOUT - ESP32-C3 SuperMini

```
┌─────────────────────────────────────┐
│     ESP32-C3 PINOUT                 │
│                                     │
│  USB  [GND][5V][3V3]              │
│       ┌─────────────────┐          │
│    0  │                 │  21       │
│    1  │                 │  20       │
│    2  │   ESP32-C3      │  10       │
│    3  │   MINI          │  GND      │
│    4  │                 │  VCC(3V3) │
│    5  │                 │  GND      │
│    6  │                 │           │
│    7  │                 │           │
│   10  │                 │           │
│  GND  │                 │           │
│  5V   └─────────────────┘           │
│  GND                                │
└─────────────────────────────────────┘
```

---

## 🔌 CONNECTION DIAGRAM

### ⚡ POWER CONNECTIONS
```
1S LiPo Battery (3.7-4.2V)
├─ (+) RED  ──→ TB6612 VM (BOTH drivers)
├─ (-) BLACK ──→ ESP32 GND + TB6612 GND + MPU-6050 GND (Common Ground)
└─ (+) RED ──→ ESP32 5V pin (NOT 3V3!)
```

### 🧭 MPU-6050 (I2C IMU Sensor)
```
MPU-6050 Pin          ESP32-C3 Pin       Wire Color
─────────────────────────────────────────────────
VCC                   3V3                RED
GND                   GND                BLACK
SDA                   GPIO 4             YELLOW
SCL                   GPIO 5             GREEN
AD0                   GND                (Pull address to 0x68)
```

### 🔋 MOTOR DRIVER #1 (TB6612FNG) - Motors 1 & 2
```
TB6612 Pin            ESP32-C3 Pin       Function
─────────────────────────────────────────────────
VM                    LiPo + (3.7V)      Motor Power
GND                   Common GND         Ground
VCC                   3V3                Logic Power
STBY                  GPIO 10            Standby Enable
AIN1                  GPIO 6             Motor 1 Direction
AIN2                  GND                Motor 1 Direction Fixed
PWMA                  GPIO 2             Motor 1 Speed (PWM)
BIN1                  GPIO 7             Motor 2 Direction
BIN2                  GND                Motor 2 Direction Fixed
PWMB                  GPIO 3             Motor 2 Speed (PWM)
AO1 ─────→ Motor 1 (+) lead
AO2 ─────→ Motor 1 (-) lead
BO1 ─────→ Motor 2 (+) lead
BO2 ─────→ Motor 2 (-) lead
```

### 🔋 MOTOR DRIVER #2 (TB6612FNG) - Motor 3
```
TB6612 Pin            ESP32-C3 Pin       Function
─────────────────────────────────────────────────
VM                    LiPo + (3.7V)      Motor Power
GND                   Common GND         Ground
VCC                   3V3                Logic Power
STBY                  GPIO 10            Standby Enable (tied with driver #1)
AIN1                  GPIO 21            Motor 3 Direction
AIN2                  GND                Motor 3 Direction Fixed
PWMA                  GPIO 20            Motor 3 Speed (PWM)
BIN1                  GND                Unused - pull to GND
BIN2                  GND                Unused - pull to GND
PWMB                  GND                Unused - pull to GND
AO1 ─────→ Motor 3 (+) lead
AO2 ─────→ Motor 3 (-) lead
```

### 🚀 DC MOTORS (3x Coreless)
```
Motor 1 (Front)
├─ (+) lead ──→ TB6612 #1 AO1
└─ (-) lead ──→ TB6612 #1 AO2

Motor 2 (Back)
├─ (+) lead ──→ TB6612 #1 BO1
└─ (-) lead ──→ TB6612 #1 BO2

Motor 3 (Side)
├─ (+) lead ──→ TB6612 #2 AO1
└─ (-) lead ──→ TB6612 #2 AO2
```

---

## 📊 COMPLETE WIRING SUMMARY TABLE

| Component | Pin Type | ESP32 Pin | Color | Notes |
|-----------|----------|-----------|-------|-------|
| **Power** |
| LiPo + | 5V IN | 5V | RED | Direct to 5V input |
| LiPo - | GND | GND | BLACK | Common ground |
| **MPU-6050** |
| VCC | 3V3 | 3V3 | RED | Logic power only |
| GND | GND | GND | BLACK | Common |
| SDA | I2C | GPIO 4 | YELLOW | I2C Data |
| SCL | I2C | GPIO 5 | GREEN | I2C Clock |
| AD0 | GND | GND | - | Address 0x68 |
| **Motor 1** |
| PWM | PWM | GPIO 2 | ORANGE | Speed control |
| DIR | GPIO | GPIO 6 | BLUE | Direction control |
| **Motor 2** |
| PWM | PWM | GPIO 3 | ORANGE | Speed control |
| DIR | GPIO | GPIO 7 | BLUE | Direction control |
| **Motor 3** |
| PWM | PWM | GPIO 20 | ORANGE | Speed control |
| DIR | GPIO | GPIO 21 | BLUE | Direction control |
| **Motor Enable** |
| STBY | GPIO | GPIO 10 | PURPLE | Both drivers tied |

---

## ✅ CHECKLIST BEFORE POWERING ON

- [ ] All GND connections soldered and secure
- [ ] LiPo battery connected to 5V (NOT 3V3!)
- [ ] MPU-6050 VCC to 3V3 (NOT 5V!)
- [ ] Motors spinning smoothly with no shorts
- [ ] All pull-up resistors on TB6612 STBY pins
- [ ] No loose wires near motor connections
- [ ] USB cable connected for serial monitoring
- [ ] WiFi SSID visible on phone (MINI-DRONE)

---

## 🔧 ERROR TROUBLESHOOTING

### ❌ MPU-6050 Not Found
```
Error Message: "MPU-6050 NOT FOUND at address 0x68"

FIXES:
1. Check GPIO 4 (SDA) and GPIO 5 (SCL) connections
2. Verify 3.3V power to VCC pin (use multimeter)
3. Check that GND is properly connected
4. Try I2C address 0x69 (change AD0 to 3V3)
5. Check for cold solder joints on I2C lines
6. Pull-up resistors: Add 4.7kΩ between SDA-3V3 and SCL-3V3 if not built-in
```

### ❌ Motors Not Spinning
```
Error Message: "Motor spins too slow / doesn't spin"

FIXES:
1. Check LiPo voltage (minimum 3.3V for single cell)
2. Verify GPIO pins 2, 3, 20 are outputting PWM
3. Check TB6612 VM getting battery voltage (use multimeter)
4. Verify STBY pin (GPIO 10) is connected and HIGH when armed
5. Check motor leads polarity - swap if needed
6. Test TB6612 with direct battery connection (bypass ESP32)
```

### ❌ Cannot Connect to WiFi
```
Error Message: "WiFi connection failed"

FIXES:
1. Check antenna connection on ESP32-C3
2. Restart ESP32 and wait 3 seconds
3. Use phone WiFi scanner - SSID should show as "MINI-DRONE"
4. Default password: "123456789"
5. If not visible: ESP32 may be in deep sleep, press RESET button
6. Check serial monitor for WiFi errors
```

### ❌ Battery Voltage Shows 0V
```
Error Message: "Battery: 0.00V"

FIXES:
1. Voltage divider not implemented - ADC pin (GPIO 0) needs setup
2. Add 10kΩ + 10kΩ voltage divider from battery
3. Connect center tap to GPIO 0 (ADC input)
4. For now, ignore battery reading (future enhancement)
```

---

## 📡 HOW TO USE

1. **Connect to WiFi:**
   - Phone/Computer: Scan WiFi networks
   - SSID: `MINI-DRONE`
   - Password: `123456789`

2. **Open Web Dashboard:**
   - URL: `http://192.168.4.1` or `http://192.168.4.1:80`

3. **Control the Drone:**
   - ARM DRONE button (enables motors)
   - DISARM button (stops motors)
   - Throttle slider (0-255 speed)
   - EMERGENCY STOP (instant motor cutoff)

4. **Monitor Telemetry:**
   - Pitch/Roll angles from IMU
   - Battery voltage
   - Connection status
   - Error messages if any

---

## 🚀 QUICK START COMMANDS (Serial Monitor)

After upload, open Serial Monitor (115200 baud):

```
[STARTUP] MINI DRONE Flight Controller v1.0
[MOTOR] ✅ Motor pins configured
[IMU] ✅ MPU-6050 connected successfully
[WiFi] ✅ Access Point created - IP: 192.168.4.1
[WEB] ✅ Web server started on port 80
```

If you see errors, check the troubleshooting section above.

---

**Last Updated:** 2026-09-04
**Version:** 1.0
**Status:** ✅ READY FOR FLIGHT
