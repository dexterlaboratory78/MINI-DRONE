# 🚁 MINI DRONE - Complete Parts List & Shopping Guide

## 📦 COMPONENTS NEEDED (Complete Bill of Materials)

### **CORE COMPONENTS**

| Part # | Component | Quantity | Specs | Approx. Cost | Where to Buy |
|--------|-----------|----------|-------|--------------|--------------|
| 1 | **ESP32-C3 SuperMini** | 1 | RISC-V MCU, WiFi+BLE | $8-12 | AliExpress, Amazon |
| 2 | **MPU-6050** | 1 | 6-axis IMU (Gyro + Accel) | $3-5 | AliExpress, eBay |
| 3 | **TB6612FNG Motor Driver** | 2 | Dual motor driver, 1A per channel | $2-3 each | AliExpress, eBay |
| 4 | **Coreless DC Motor** (1-3.7V) | 3 | 7mm micro brushless or equivalent | $1-2 each | AliExpress, Amazon |
| 5 | **1S LiPo Battery** | 1 | 3.7V, 250-500mAh recommended | $5-10 | Hobby King, Amazon |
| 6 | **LiPo Charger** | 1 | USB 1S LiPo charger | $5-8 | Amazon, eBay |

---

### **PASSIVE COMPONENTS & CONNECTORS**

| Part # | Component | Quantity | Specs | Cost |
|--------|-----------|----------|-------|------|
| 7 | **Jumper Wires** | 30 | 22AWG M-M + M-F | $2-3 |
| 8 | **Resistors (4.7kΩ)** | 2 | 1/4W for I2C pull-ups | $0.50 |
| 9 | **Resistors (10kΩ)** | 2 | 1/4W for voltage divider | $0.50 |
| 10 | **Capacitors (100µF)** | 2 | 16V for power stability | $1 |
| 11 | **JST Connectors** | 1 set | Battery connector optional | $2 |
| 12 | **Breadboard** | 1 | 830 tie-points (optional) | $3-5 |
| 13 | **USB Micro Cable** | 1 | For programming ESP32 | $2 |

---

## 💰 COST BREAKDOWN

```
┌─────────────────────────────────────────┐
│        TOTAL PROJECT COST               │
├─────────────────────────────────────────┤
│ Core Electronics:        $25-35         │
│ Motors & Battery:        $10-20         │
│ Passive Components:      $8-12          │
│ Wires & Connectors:      $5-8           │
├─────────────────────────────────────────┤
│ TOTAL:                   $48-75         │
└─────────────────────────────────────────┘
```

**Recommended Budget: $60-80 USD**

---

## 🛒 SHOPPING LINKS (Amazon/AliExpress)

### **FASTEST (Amazon Prime - 2 days)**
- ESP32-C3: https://amazon.com/s?k=ESP32-C3+SuperMini
- MPU-6050: https://amazon.com/s?k=MPU6050+module
- TB6612FNG: https://amazon.com/s?k=TB6612FNG
- Motors: https://amazon.com/s?k=coreless+DC+motor
- LiPo Battery: https://amazon.com/s?k=1S+LiPo+battery
- LiPo Charger: https://amazon.com/s?k=USB+1S+LiPo+charger

### **CHEAPEST (AliExpress - 2-3 weeks)**
- ESP32-C3: https://aliexpress.com/item/1005004744961144.html
- MPU-6050: https://aliexpress.com/item/32340159180.html
- TB6612FNG: https://aliexpress.com/item/32792205167.html
- Complete Kits: https://aliexpress.com/item/32989391688.html

---

## 🔧 WIRING CHECKLIST

### **Power Distribution**
```
1S LiPo Battery (3.7V)
├─ (+) Terminal → TB6612 #1 VM pin (motor power)
├─ (+) Terminal → TB6612 #2 VM pin (motor power)
├─ (+) Terminal → ESP32-C3 5V pin (main power)
└─ (-) Terminal → Common GND (ESP32 + TB6612 #1 + TB6612 #2 + MPU-6050)
```

### **I2C Connection (MPU-6050)**
```
MPU-6050          ESP32-C3
VCC       ────→  3.3V (with 100µF cap to GND)
GND       ────→  GND
SDA       ────→  GPIO 4 (with 4.7kΩ pull-up to 3.3V)
SCL       ────→  GPIO 5 (with 4.7kΩ pull-up to 3.3V)
AD0       ────→  GND (sets address to 0x68)
```

### **Motor Connections**
```
Motor 1 (Front)     → TB6612 #1 AO1/AO2 outputs
Motor 2 (Back)      → TB6612 #1 BO1/BO2 outputs
Motor 3 (Side)      → TB6612 #2 AO1/AO2 outputs

GPIO Connections:
GPIO 2  → TB6612 #1 PWMA (Motor 1 speed)
GPIO 3  → TB6612 #1 PWMB (Motor 2 speed)
GPIO 6  → TB6612 #1 AIN1 (Motor 1 direction)
GPIO 7  → TB6612 #1 BIN1 (Motor 2 direction)
GPIO 10 → TB6612 #1 & #2 STBY (tied together - motor enable)
GPIO 20 → TB6612 #2 PWMA (Motor 3 speed)
GPIO 21 → TB6612 #2 AIN1 (Motor 3 direction)
```

---

## 📊 COMPONENT SPECIFICATIONS

### **ESP32-C3 SuperMini**
```
Processor:     RISC-V 32-bit @ 160MHz
RAM:           400KB SRAM
Flash:         4MB
GPIO:          22 pins
Features:      WiFi 802.11b/g/n, BLE 5.0
Voltage:       3.3V logic, 5V tolerant input
Power:         ~80mA max (active WiFi)
USB:           Micro-B (CH340 USB-UART)
```

### **MPU-6050**
```
Accelerometer: ±2/4/8/16g ranges
Gyroscope:     ±250/500/1000/2000 °/s ranges
I2C Speed:     Up to 400kHz
Address:       0x68 (AD0 to GND) or 0x69 (AD0 to VCC)
Power:         3.3V only (NOT 5V!)
Current:       ~3.7mA
Size:          QFN-24 package
```

### **TB6612FNG Motor Driver**
```
Motor Channels:      2 (Dual)
Max Current/Channel: 1.2A (continuous), 3.2A (peak)
Logic Voltage:       2.7V - 5.5V
Motor Voltage (VM):  2.2V - 13.5V
PWM Frequency:       Up to 100kHz
Standby Mode:        Ultra-low power
Package:             SSOP-24
```

### **Coreless DC Motors** (1-3.7V)
```
Voltage:       1.5V - 3.7V recommended
Current:       ~50-150mA each
RPM:           8000-15000 @ 3.7V
Torque:        ~0.5-1 mNm
Weight:        ~2-3g each
Size:          7mm or 8mm diameter
```

### **1S LiPo Battery**
```
Voltage:       3.7V nominal, 4.2V charged, 3.0V min safe
Capacity:      250-500mAh recommended for long flights
Discharge Rate: 25-50C minimum
Weight:        ~10-15g (250mAh)
Connector:     JST-PH or similar
Lifespan:      300-500 charge cycles
```

---

## ⚡ POWER BUDGET CALCULATION

```
Device                    Current @ 3.7V    Total with 3 Motors
────────────────────────────────────────────────────────────
ESP32-C3 (WiFi on)        ~120mA            120mA
MPU-6050                   ~3.7mA            3.7mA
TB6612FNG (both)           ~5mA              5mA
Motor 1 @ 50% throttle     ~75mA             75mA
Motor 2 @ 50% throttle     ~75mA             75mA
Motor 3 @ 50% throttle     ~75mA             75mA
───��────────────────────────────────────────────────────────
TOTAL:                                       ~353mA average
PEAK (all motors 100%):                      ~600mA max
```

**Battery Selection:**
- **250mAh battery:** ~25-40 min flight time at 50% throttle
- **500mAh battery:** ~50-80 min flight time at 50% throttle

---

## 🔧 TOOLS NEEDED

| Tool | Purpose | Essential? |
|------|---------|-----------|
| Soldering Iron | Connect wires securely | Recommended |
| Solder & Flux | Create solid connections | Recommended |
| Wire Strippers | Prepare jumper wires | Required |
| Multimeter | Test connections & voltage | Highly Recommended |
| Tweezers | Handling small components | Optional |
| Small Screwdriver | Component assembly | Optional |
| USB Cable (Micro-B) | Program ESP32 | Required |

---

## 📋 ASSEMBLY CHECKLIST

- [ ] **Received all components** (verify against parts list)
- [ ] **Tested battery voltage** (should read 3.7-4.2V)
- [ ] **Soldered/connected all wires** (check for cold solder joints)
- [ ] **Verified I2C connections** (GPIO 4, 5, pull-up resistors)
- [ ] **Connected all motors** (test spin direction)
- [ ] **TB6612 power connections** (VM from battery, VCC from 3.3V)
- [ ] **Common ground verified** (all devices share GND)
- [ ] **No shorts detected** (use multimeter)
- [ ] **Antenna position** (WiFi performs better with antenna up)
- [ ] **USB cable test** (can program ESP32)

---

## 🚀 BEFORE FIRST FLIGHT

1. **Upload firmware** using PlatformIO
2. **Check serial monitor** for startup messages
3. **Verify MPU-6050** connects and shows values
4. **Test motors individually** with throttle slider
5. **Check battery voltage** in web dashboard
6. **Calibrate IMU** (place level and record offset)
7. **Test WiFi range** (should work 20-50 meters)
8. **Emergency stop button** (verify instant cutoff)

---

## 💡 UPGRADE OPTIONS (Future)

- **Optical Flow Sensor:** Better altitude hold
- **Barometer:** Altitude measurement
- **SD Card Module:** Flight logging
- **Camera Module:** FPV streaming
- **Better LiPo:** Higher mAh or 2S for more power
- **Carbon Fiber Frame:** Reduced weight
- **LED Lights:** Status indication

---

## 📞 COMMON ISSUES & FIXES

### **Component Doesn't Work**
- Check voltage with multimeter (correct supply?)
- Verify GPIO connections (right pins?)
- Test component directly with battery
- Check for cold solder joints
- Try different I2C address (MPU-6050)

### **Motors Spin Wrong Direction**
- Swap motor wire polarity
- Or change GPIO direction pin in code

### **WiFi Weak**
- Reposition antenna
- Move closer to drone
- Check for interference (metal, other WiFi)

### **Battery Dies Fast**
- Could be cheap battery (fake mAh)
- Check for short circuit
- Motors drawing too much at high throttle

---

## 📚 DATASHEETS

**Download datasheets for reference:**
- ESP32-C3: https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf
- MPU-6050: https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf
- TB6612FNG: https://toshiba.semicon-storage.com/info/docget.jsp?did=61504

---

## ✅ QUALITY CHECKLIST

**Recommended Suppliers:**
- ✅ AliExpress (cheapest, 2-3 week shipping)
- ✅ Amazon (faster, prime available)
- ✅ eBay (good ratings available)
- ✅ Hobby King (hobby electronics specialist)
- ❌ Avoid: Unknown brands, unverified sellers

**Always check:**
- Seller rating (4+ stars minimum)
- Customer reviews
- Real product photos
- Return/refund policy

---

## 🎯 FINAL NOTES

- **Total Assembly Time:** 2-3 hours (with soldering)
- **Programming Time:** 30 minutes
- **First Flight:** Same day possible!
- **Cost Effectiveness:** Excellent educational project
- **Expandability:** Easy to add sensors and features

**Estimated Timeline:**
- **Week 1:** Order components
- **Week 2-3:** Wait for delivery (AliExpress)
- **Week 3:** Assembly & testing
- **Week 3+:** Flying! 🚁

---

**Last Updated:** 2026-09-04  
**Status:** ✅ COMPLETE PARTS LIST VERIFIED

**Happy Building! 🛠️** 🚁
