/*
  MINI DRONE - ESP32-C3 Flight Controller
  PRIVATE VERSION WITH PASSWORD PROTECTION
  Full working code with error handling & web dashboard
  Platform: PlatformIO + Arduino
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// ==================== PIN CONFIGURATION ====================
#define SDA_PIN 4
#define SCL_PIN 5

// Motor 1 (Front)
#define M1_PWM 2
#define M1_DIR 6

// Motor 2 (Back)
#define M2_PWM 3
#define M2_DIR 7

// Motor 3 (Side)
#define M3_PWM 20
#define M3_DIR 21

// Motor Enable
#define MOTOR_STBY 10

// ==================== CONSTANTS ====================
#define BAUD_RATE 115200
#define I2C_FREQ 400000
#define PWM_FREQ 5000
#define PWM_RESOLUTION 8

// ==================== SECURITY SETTINGS ====================
// WiFi Credentials - PRIVATE ACCESS ONLY
const char* SSID = "MINI-DRONE-PRIVATE";
const char* PASSWORD = "SecurePass2026!";

// Web Dashboard Login - CHANGE THESE FOR YOUR SECURITY
const char* WEB_USERNAME = "admin";
const char* WEB_PASSWORD = "YourSecurePassword123";

// ==================== GLOBAL VARIABLES ====================
WebServer server(80);
Adafruit_MPU6050 mpu;
bool authenticated = false;
unsigned long auth_timestamp = 0;
const unsigned long AUTH_TIMEOUT = 3600000; // 1 hour

struct DroneState {
  bool armed = false;
  float pitch = 0.0;
  float roll = 0.0;
  float yaw = 0.0;
  uint8_t throttle = 0;
  float battery_voltage = 0.0;
  bool mpu_connected = false;
  bool wifi_connected = false;
};

struct ErrorStatus {
  bool mpu_error = false;
  bool motor1_error = false;
  bool motor2_error = false;
  bool motor3_error = false;
  bool battery_low = false;
  bool wifi_error = false;
  String error_message = "";
};

DroneState drone_state;
ErrorStatus error_status;

// ==================== FUNCTION DECLARATIONS ====================
void setup_wifi();
void setup_web_server();
void setup_motors();
void setup_imu();
void read_imu();
void read_battery();
void control_motor(uint8_t motor, uint8_t pwm);
void arm_drone();
void disarm_drone();
void emergency_stop();
void check_errors();
void send_telemetry();
bool check_auth();
String get_dashboard_html();

// ==================== SETUP ====================
void setup() {
  Serial.begin(BAUD_RATE);
  delay(1000);
  
  Serial.println("\n\n========================================");
  Serial.println("   MINI DRONE - FLIGHT CONTROLLER v2.0");
  Serial.println("   🔒 PRIVATE VERSION WITH SECURITY    ");
  Serial.println("========================================\n");
  
  // Initialize pins
  setup_motors();
  
  // Initialize I2C and IMU
  Wire.begin(SDA_PIN, SCL_PIN, I2C_FREQ);
  setup_imu();
  
  // Initialize WiFi
  setup_wifi();
  
  // Setup web server with auth
  setup_web_server();
  
  Serial.println("\n✅ STARTUP COMPLETE - Waiting for commands...\n");
}

// ==================== LOOP ====================
void loop() {
  // Handle web server requests
  server.handleClient();
  
  // Read sensors
  if (drone_state.mpu_connected) {
    read_imu();
  }
  
  // Read battery voltage
  read_battery();
  
  // Check for errors
  check_errors();
  
  // Send telemetry every 100ms
  static unsigned long last_telemetry = 0;
  if (millis() - last_telemetry > 100) {
    send_telemetry();
    last_telemetry = millis();
  }
  
  // Check auth timeout
  if (authenticated && (millis() - auth_timestamp > AUTH_TIMEOUT)) {
    authenticated = false;
    Serial.println("[AUTH] Session expired - login required");
  }
  
  delay(10);
}

// ==================== AUTHENTICATION ====================
bool check_auth() {
  if (!authenticated) {
    if (server.hasHeader("Authorization")) {
      String auth = server.header("Authorization");
      if (auth.startsWith("Basic ")) {
        String encoded = auth.substring(6);
        if (encoded.indexOf(WEB_USERNAME) >= 0 && encoded.indexOf(WEB_PASSWORD) >= 0) {
          authenticated = true;
          auth_timestamp = millis();
          Serial.println("[AUTH] ✅ User authenticated successfully");
          return true;
        }
      }
    }
    
    server.setHeader("WWW-Authenticate", "Basic realm=\"MINI-DRONE Private Access\"");
    server.send(401, "application/json", "{\"error\":\"Unauthorized - Login required\"}");
    Serial.println("[AUTH] ❌ Unauthorized access attempt");
    return false;
  }
  
  auth_timestamp = millis();
  return true;
}

// ==================== MOTOR SETUP ====================
void setup_motors() {
  Serial.println("[MOTOR] Initializing motor pins...");
  
  pinMode(M1_PWM, OUTPUT);
  pinMode(M1_DIR, OUTPUT);
  pinMode(M2_PWM, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  pinMode(M3_PWM, OUTPUT);
  pinMode(M3_DIR, OUTPUT);
  pinMode(MOTOR_STBY, OUTPUT);
  
  ledcSetup(0, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(1, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(2, PWM_FREQ, PWM_RESOLUTION);
  
  ledcAttachPin(M1_PWM, 0);
  ledcAttachPin(M2_PWM, 1);
  ledcAttachPin(M3_PWM, 2);
  
  digitalWrite(MOTOR_STBY, LOW);
  
  Serial.println("[MOTOR] ✅ Motor pins configured");
  Serial.println("[MOTOR] Motors DISABLED (standby mode)");
}

// ==================== IMU SETUP ====================
void setup_imu() {
  Serial.println("[IMU] Initializing MPU-6050...");
  
  if (!mpu.begin(0x68, &Wire)) {
    Serial.println("[IMU] ❌ ERROR: MPU-6050 NOT FOUND at address 0x68");
    Serial.println("[IMU] TROUBLESHOOTING:");
    Serial.println("     1. Check SDA (GPIO 4) and SCL (GPIO 5) connections");
    Serial.println("     2. Check 3.3V power to MPU-6050 VCC pin");
    Serial.println("     3. Check GND connection to MPU-6050");
    error_status.mpu_error = true;
    drone_state.mpu_connected = false;
    return;
  }
  
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  Serial.println("[IMU] ✅ MPU-6050 connected successfully");
  drone_state.mpu_connected = true;
  error_status.mpu_error = false;
}

// ==================== WIFI SETUP ====================
void setup_wifi() {
  Serial.println("[WiFi] Starting PRIVATE WiFi Access Point...");
  Serial.print("[WiFi] SSID: ");
  Serial.println(SSID);
  Serial.print("[WiFi] Password: ");
  Serial.println(PASSWORD);
  Serial.println("[WiFi] ⚠️  PRIVATE MODE - Password required for access");
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID, PASSWORD);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("[WiFi] ✅ Private Access Point created - IP: ");
  Serial.println(IP);
  Serial.println("[WiFi] Only authorized users can connect and login");
  
  drone_state.wifi_connected = true;
}

// ==================== WEB SERVER SETUP ====================
void setup_web_server() {
  Serial.println("[WEB] Setting up SECURE web server...");
  
  server.on("/", HTTP_GET, []() {
    if (!check_auth()) return;
    server.send(200, "text/html", get_dashboard_html());
  });
  
  server.on("/api/status", HTTP_GET, []() {
    if (!check_auth()) return;
    
    DynamicJsonDocument doc(1024);
    doc["armed"] = drone_state.armed;
    doc["pitch"] = drone_state.pitch;
    doc["roll"] = drone_state.roll;
    doc["yaw"] = drone_state.yaw;
    doc["throttle"] = drone_state.throttle;
    doc["battery"] = drone_state.battery_voltage;
    doc["mpu_connected"] = drone_state.mpu_connected;
    doc["wifi_connected"] = drone_state.wifi_connected;
    
    doc["errors"]["mpu"] = error_status.mpu_error;
    doc["errors"]["motor1"] = error_status.motor1_error;
    doc["errors"]["motor2"] = error_status.motor2_error;
    doc["errors"]["motor3"] = error_status.motor3_error;
    doc["errors"]["battery_low"] = error_status.battery_low;
    doc["errors"]["message"] = error_status.error_message;
    
    String json;
    serializeJson(doc, json);
    server.send(200, "application/json", json);
  });
  
  server.on("/api/arm", HTTP_GET, []() {
    if (!check_auth()) return;
    arm_drone();
    server.send(200, "application/json", "{\"status\":\"armed\"}");
  });
  
  server.on("/api/disarm", HTTP_GET, []() {
    if (!check_auth()) return;
    disarm_drone();
    server.send(200, "application/json", "{\"status\":\"disarmed\"}");
  });
  
  server.on("/api/throttle", HTTP_GET, []() {
    if (!check_auth()) return;
    if (server.hasArg("value")) {
      drone_state.throttle = constrain(server.arg("value").toInt(), 0, 255);
      if (drone_state.armed) {
        control_motor(1, drone_state.throttle);
        control_motor(2, drone_state.throttle);
        control_motor(3, drone_state.throttle);
      }
      server.send(200, "application/json", "{\"throttle\":" + String(drone_state.throttle) + "}");
    } else {
      server.send(400, "application/json", "{\"error\":\"missing value parameter\"}");
    }
  });
  
  server.on("/api/emergency", HTTP_GET, []() {
    if (!check_auth()) return;
    emergency_stop();
    server.send(200, "application/json", "{\"status\":\"emergency stop activated\"}");
  });
  
  server.begin();
  Serial.println("[WEB] ✅ SECURE web server started on port 80");
}

// ==================== MOTOR CONTROL ====================
void control_motor(uint8_t motor, uint8_t pwm) {
  if (!drone_state.armed) return;
  
  pwm = constrain(pwm, 0, 255);
  
  switch(motor) {
    case 1:
      digitalWrite(M1_DIR, HIGH);
      ledcWrite(0, pwm);
      error_status.motor1_error = false;
      break;
    case 2:
      digitalWrite(M2_DIR, HIGH);
      ledcWrite(1, pwm);
      error_status.motor2_error = false;
      break;
    case 3:
      digitalWrite(M3_DIR, HIGH);
      ledcWrite(2, pwm);
      error_status.motor3_error = false;
      break;
  }
}

void arm_drone() {
  if (error_status.mpu_error) {
    Serial.println("[ARM] ❌ Cannot arm: MPU-6050 not connected");
    error_status.error_message = "MPU-6050 disconnected";
    return;
  }
  
  if (error_status.battery_low) {
    Serial.println("[ARM] ❌ Cannot arm: Battery voltage too low");
    error_status.error_message = "Battery low";
    return;
  }
  
  drone_state.armed = true;
  digitalWrite(MOTOR_STBY, HIGH);
  Serial.println("[ARM] ✅ DRONE ARMED - Motors enabled");
}

void disarm_drone() {
  drone_state.armed = false;
  drone_state.throttle = 0;
  
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  digitalWrite(MOTOR_STBY, LOW);
  
  Serial.println("[DISARM] ✅ DRONE DISARMED - Motors disabled");
}

void emergency_stop() {
  drone_state.armed = false;
  drone_state.throttle = 0;
  
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  digitalWrite(MOTOR_STBY, LOW);
  
  Serial.println("[EMERGENCY] 🚨 EMERGENCY STOP ACTIVATED!");
  error_status.error_message = "Emergency stop activated";
}

// ==================== SENSOR READING ====================
void read_imu() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  drone_state.pitch = atan2(a.acceleration.y, a.acceleration.z) * 57.2958;
  drone_state.roll = atan2(a.acceleration.x, a.acceleration.z) * 57.2958;
  drone_state.yaw += g.gyro.z * 0.01;
}

void read_battery() {
  float adc_value = analogRead(0) * (3.3 / 4095.0) * 2.0;
  drone_state.battery_voltage = adc_value;
  
  if (drone_state.battery_voltage < 3.0 && drone_state.battery_voltage > 0) {
    error_status.battery_low = true;
    error_status.error_message = "Battery low: " + String(drone_state.battery_voltage, 2) + "V";
  } else if (drone_state.battery_voltage >= 3.2) {
    error_status.battery_low = false;
  }
}

// ==================== ERROR CHECKING ====================
void check_errors() {
  if (drone_state.mpu_connected && !mpu.begin(0x68, &Wire)) {
    error_status.mpu_error = true;
    drone_state.mpu_connected = false;
    error_status.error_message = "MPU-6050 lost connection";
  }
  
  static unsigned long last_error_print = 0;
  if (millis() - last_error_print > 5000 && error_status.error_message != "") {
    Serial.print("[ERROR] ");
    Serial.println(error_status.error_message);
    last_error_print = millis();
  }
}

// ==================== TELEMETRY ====================
void send_telemetry() {
  static unsigned long counter = 0;
  counter++;
  
  if (counter % 10 == 0) {
    Serial.print("[TELEMETRY] Pitch: ");
    Serial.print(drone_state.pitch, 2);
    Serial.print("° | Roll: ");
    Serial.print(drone_state.roll, 2);
    Serial.print("° | Throttle: ");
    Serial.print(drone_state.throttle);
    Serial.print(" | Battery: ");
    Serial.print(drone_state.battery_voltage, 2);
    Serial.print("V | Armed: ");
    Serial.println(drone_state.armed ? "YES" : "NO");
  }
}

// ==================== HTML DASHBOARD ====================
String get_dashboard_html() {
  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>🔒 MINI DRONE - Private Access</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body { background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%); font-family: Arial, sans-serif; color: #fff; padding: 20px; }
    .container { max-width: 1000px; margin: 0 auto; }
    h1 { text-align: center; margin-bottom: 30px; font-size: 2.5em; }
    .lock-badge { display: inline-block; background: #ff6b6b; color: white; padding: 5px 15px; border-radius: 20px; font-size: 0.9em; margin-left: 10px; }
    .status-box { background: rgba(0,0,0,0.3); border: 2px solid #00ff00; border-radius: 10px; padding: 20px; margin-bottom: 20px; }
    .error-box { background: rgba(255,0,0,0.2); border: 2px solid #ff3333; border-radius: 10px; padding: 15px; margin-bottom: 20px; }
    .error-box.hidden { display: none; }
    .info { display: grid; grid-template-columns: 1fr 1fr; gap: 15px; }
    .info-item { background: rgba(0,0,0,0.4); padding: 15px; border-radius: 5px; }
    .info-label { font-size: 0.9em; color: #aaa; }
    .info-value { font-size: 1.8em; color: #00ff00; font-weight: bold; }
    .controls { display: grid; grid-template-columns: 1fr 1fr; gap: 15px; margin-bottom: 20px; }
    button { padding: 15px 30px; font-size: 1.1em; border: none; border-radius: 5px; cursor: pointer; font-weight: bold; transition: all 0.3s; }
    .btn-arm { background: #00ff00; color: #000; } .btn-arm:hover { background: #00dd00; }
    .btn-disarm { background: #ff9900; color: #000; } .btn-disarm:hover { background: #ff7700; }
    .btn-emergency { background: #ff3333; color: #fff; grid-column: 1/-1; } .btn-emergency:hover { background: #ff0000; }
    .slider-container { background: rgba(0,0,0,0.3); border: 2px solid #00ff00; border-radius: 10px; padding: 20px; margin-bottom: 20px; }
    .slider-label { font-size: 1.2em; margin-bottom: 10px; }
    input[type="range"] { width: 100%; height: 8px; border-radius: 5px; background: #00ff00; outline: none; }
    .throttle-value { font-size: 1.5em; color: #00ff00; text-align: center; margin-top: 10px; }
  </style>
</head>
<body>
  <div class="container">
    <h1>🚁 MINI DRONE Flight Controller <span class="lock-badge">🔒 PRIVATE</span></h1>
    
    <div class="error-box" id="errorBox">
      <strong>⚠️ ERROR:</strong> <span id="errorMessage"></span>
    </div>
    
    <div class="status-box">
      <h2>System Status</h2>
      <div class="info">
        <div class="info-item"><div class="info-label">Status</div><div class="info-value" id="armed">DISARMED</div></div>
        <div class="info-item"><div class="info-label">Battery Voltage</div><div class="info-value" id="battery">0.0V</div></div>
        <div class="info-item"><div class="info-label">Pitch (°)</div><div class="info-value" id="pitch">0.0</div></div>
        <div class="info-item"><div class="info-label">Roll (°)</div><div class="info-value" id="roll">0.0</div></div>
      </div>
    </div>
    
    <div class="controls">
      <button class="btn-arm" onclick="armDrone()">🔓 ARM DRONE</button>
      <button class="btn-disarm" onclick="disarmDrone()">🔒 DISARM</button>
      <button class="btn-emergency" onclick="emergencyStop()">⚠️ EMERGENCY STOP</button>
    </div>
    
    <div class="slider-container">
      <div class="slider-label">Throttle Control</div>
      <input type="range" id="throttleSlider" min="0" max="255" value="0" oninput="setThrottle(this.value)">
      <div class="throttle-value"><span id="throttleValue">0</span> / 255</div>
    </div>
  </div>

  <script>
    const API_BASE = 'http://' + window.location.hostname;
    
    function updateStatus() {
      fetch(API_BASE + '/api/status').then(r => r.json()).then(data => {
        document.getElementById('armed').textContent = data.armed ? '✅ ARMED' : '⏸️ DISARMED';
        document.getElementById('battery').textContent = data.battery.toFixed(2) + 'V';
        document.getElementById('pitch').textContent = data.pitch.toFixed(1);
        document.getElementById('roll').textContent = data.roll.toFixed(1);
        const errorBox = document.getElementById('errorBox');
        if (data.errors.message) {
          errorBox.classList.remove('hidden');
          document.getElementById('errorMessage').textContent = data.errors.message;
        } else {
          errorBox.classList.add('hidden');
        }
      }).catch(e => console.error('Error:', e));
    }
    
    function armDrone() { fetch(API_BASE + '/api/arm').then(() => updateStatus()); }
    function disarmDrone() { fetch(API_BASE + '/api/disarm').then(() => updateStatus()); }
    function emergencyStop() { if (confirm('🚨 Emergency stop - Continue?')) fetch(API_BASE + '/api/emergency').then(() => updateStatus()); }
    function setThrottle(value) { document.getElementById('throttleValue').textContent = value; fetch(API_BASE + '/api/throttle?value=' + value).then(() => updateStatus()); }
    
    setInterval(updateStatus, 500);
    updateStatus();
  </script>
</body>
</html>
)rawliteral";
}
