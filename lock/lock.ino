#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Adafruit_Fingerprint.h>
#include <ArduinoJson.h>
#include "html.h"

// ============================================================================
// CONFIGURATION SECTION
// ============================================================================

// WiFi Configuration
const char* ssid = "lockofdorf";
const char* password = "";

// Login credentials
String webUsername = "thenuki";
String webPassword = "12345678";

// Access code
String accessCode = "1234";

// Hardware pins
const int solenoidPin = 4;
const int keypadPins[] = {13, 12, 14, 27, 26, 25, 33, 32};
const int numButtons = 8;

// Timing configuration
const unsigned long AUTO_LOCK_DELAY = 10000; // 10 seconds

// Logging configuration
const int MAX_LOG_ENTRIES = 20;

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

WebServer server(80);
HardwareSerial mySerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Keypad state
bool buttonStates[numButtons] = {false};
String enteredCode = "";

// Lock state
bool accessGranted = false;
String lastAccessMessage = "System Ready";
bool isLocked = true;
unsigned long unlockTime = 0;

// Access log
String accessLog[MAX_LOG_ENTRIES];
int logIndex = 0;

// System statistics
int accessCount = 0;

// ============================================================================
// INITIALIZATION FUNCTIONS
// ============================================================================

void setup() {
  Serial.begin(115200);
  
  initializeHardware();
  initializeFingerprint();
  initializeAccessLog();
  setupWiFi();
  setupWebServer();
  
  addToLog("System started");
  Serial.println("System ready!");
  Serial.println("Connect to WiFi: " + String(ssid));
  Serial.println("Then visit: http://lock.local");
}

void initializeHardware() {
  // Initialize keypad pins
  for (int i = 0; i < numButtons; i++) {
    pinMode(keypadPins[i], INPUT_PULLUP);
  }
  
  // Initialize solenoid pin
  pinMode(solenoidPin, OUTPUT);
  digitalWrite(solenoidPin, LOW);
  
  // Initialize fingerprint sensor serial
  mySerial.begin(57600, SERIAL_8N1, 16, 17);
}

void initializeFingerprint() {
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
    addToLog("Fingerprint sensor initialized");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    addToLog("Fingerprint sensor not found");
  }
  }
  
void initializeAccessLog() {
  for (int i = 0; i < MAX_LOG_ENTRIES; i++) {
    accessLog[i] = "";
  }
}

void setupWiFi() {
  Serial.println("Setting up WiFi Access Point...");
  
  // Configure Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(
    IPAddress(192, 168, 4, 1),    // IP address
    IPAddress(192, 168, 4, 1),    // Gateway
    IPAddress(255, 255, 255, 0)   // Subnet mask
  );
  
  // Start Access Point
  bool apStarted = WiFi.softAP(ssid, password);
  
  if (apStarted) {
    Serial.println("Access Point started successfully!");
    Serial.print("Network Name (SSID): ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(strlen(password) > 0 ? password : "No password");
    Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
    Serial.print("MAC Address: ");
    Serial.println(WiFi.softAPmacAddress());
    
    // Print connection instructions
    Serial.println("\nCONNECTION INSTRUCTIONS:");
    Serial.println("1. On your phone/computer, look for WiFi network: " + String(ssid));
    Serial.println("2. Connect to this network");
    Serial.println("3. Open browser and go to: http://192.168.4.1");
    Serial.println("4. Or try: http://lock.local");
    Serial.println("\nIf connection fails, restart the device and try again.");
    
  } else {
    Serial.println("Failed to start Access Point!");
    Serial.println("Trying to restart WiFi...");
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    Serial.println("Retry - Access Point IP: " + WiFi.softAPIP().toString());
  }
  
  // Setup mDNS
  if (!MDNS.begin("lock")) {
    Serial.println("mDNS responder failed to start");
    Serial.println("You can still access via IP: http://192.168.4.1");
  } else {
    Serial.println("mDNS responder started");
    Serial.println("You can access at: http://lock.local");
  }
  
  // Print network status
  Serial.println("\nNETWORK STATUS:");
  Serial.println("Connected devices: " + String(WiFi.softAPgetStationNum()));
  Serial.println("Access Point running: " + String(WiFi.getMode() == WIFI_AP ? "YES" : "NO"));
}

void setupWebServer() {
  // Main pages
  server.on("/", handleDashboard);
  server.on("/dashboard", handleDashboard);
  
  // Lock control
  server.on("/check-code", handleCheckCode);
  server.on("/status", handleStatus);
  server.on("/toggle-lock", handleToggleLock);
  
  // Configuration
  server.on("/update-code", handleUpdateCode);
  server.on("/update-password", handleUpdatePassword);
  server.on("/enroll-fingerprint", handleEnrollFingerprint);
  
  // System management
  server.on("/get-logs", handleGetLogs);
  server.on("/stats", handleStats);
  server.on("/restart", handleRestart);
  server.on("/network-test", handleNetworkTest);
  server.on("/test", handleTest);
  
  // Enhanced functions
  server.on("/test-fingerprint", handleTestFingerprint);
  server.on("/clear-logs", handleClearLogs);
  server.on("/emergency-lock", handleEmergencyLock);
  server.on("/reset-settings", handleResetSettings);
  server.on("/export-logs", handleExportLogs);
  
  server.begin();
  Serial.println("HTTP server started");
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

void addToLog(String message) {
  String timestamp = String(millis() / 1000) + "s";
  String logEntry = "[" + timestamp + "] " + message;
  accessLog[logIndex] = logEntry;
  logIndex = (logIndex + 1) % MAX_LOG_ENTRIES;
}

void unlockDoor() {
  digitalWrite(solenoidPin, HIGH);
  isLocked = false;
  unlockTime = millis();
  lastAccessMessage = "Door Unlocked - Access Granted!";
  Serial.println(lastAccessMessage);
  addToLog("Door unlocked");
}

void lockDoor() {
  digitalWrite(solenoidPin, LOW);
  isLocked = true;
  lastAccessMessage = "Door Locked - Secure";
  Serial.println(lastAccessMessage);
  addToLog("Door locked");
}

// Enhanced statistics tracking
void incrementAccessCount() {
  accessCount++;
  Serial.println("Access count: " + String(accessCount));
}

// ============================================================================
// WEB PAGE HANDLERS
// ============================================================================

void handleDashboard() {
  Serial.println("Dashboard requested");
  
  String html = "<!DOCTYPE html><html><head><title>Smart Lock</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<style>";
  html += "body{font-family:Arial;background:#667eea;margin:0;padding:10px;color:#333;}";
  html += ".container{max-width:800px;margin:0 auto;}";
  html += ".header{background:#fff;padding:15px;border-radius:10px;margin-bottom:10px;text-align:center;}";
  html += ".header h1{margin:0;color:#667eea;}";
  html += ".theme-btn{background:#667eea;color:#fff;border:none;padding:8px 15px;border-radius:5px;cursor:pointer;margin:5px;}";
  html += ".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(300px,1fr));gap:10px;}";
  html += ".card{background:#fff;padding:15px;border-radius:10px;box-shadow:0 2px 5px rgba(0,0,0,0.1);}";
  html += ".card h3{margin-top:0;color:#667eea;border-bottom:2px solid #667eea;padding-bottom:5px;}";
  html += ".status{text-align:center;background:#667eea;color:#fff;padding:20px;border-radius:10px;}";
  html += ".status h2{font-size:40px;margin:10px 0;}";
  html += ".toggle{display:flex;justify-content:center;align-items:center;gap:10px;margin:15px 0;}";
  html += ".switch{position:relative;width:60px;height:30px;}";
  html += ".switch input{display:none;}";
  html += ".slider{position:absolute;top:0;left:0;right:0;bottom:0;background:#ccc;border-radius:30px;transition:0.3s;}";
  html += ".slider:before{position:absolute;content:'';height:22px;width:22px;left:4px;bottom:4px;background:#fff;border-radius:50%;transition:0.3s;}";
  html += "input:checked + .slider{background:#27ae60;}";
  html += "input:checked + .slider:before{transform:translateX(30px);}";
  html += "input[type=text],input[type=password]{width:100%;padding:8px;margin:5px 0;border:1px solid #ddd;border-radius:5px;box-sizing:border-box;}";
  html += "button{background:#667eea;color:#fff;border:none;padding:10px 15px;border-radius:5px;cursor:pointer;margin:5px;}";
  html += "button:hover{background:#5a6fd8;}";
  html += ".success{background:#27ae60;}";
  html += ".warning{background:#f39c12;}";
  html += ".danger{background:#e74c3c;}";
  html += ".info{background:#3498db;}";
  html += ".logs{max-height:150px;overflow-y:auto;background:#f8f9fa;padding:10px;border-radius:5px;margin:10px 0;border:1px solid #ddd;}";
  html += ".log{background:#fff;padding:5px;margin:2px 0;border-radius:3px;font-size:12px;border-left:3px solid #667eea;}";
  html += ".msg{padding:8px;border-radius:5px;margin:5px 0;display:none;}";
  html += ".msg.success{background:#d4edda;color:#155724;}";
  html += ".msg.error{background:#f8d7da;color:#721c24;}";
  html += ".msg.info{background:#d1ecf1;color:#0c5460;}";
  html += ".dark{background:#1a1a1a;color:#fff;}";
  html += ".dark .header,.dark .card{background:#2d2d2d;color:#fff;}";
  html += ".dark input{border-color:#555;background:#333;color:#fff;}";
  html += "</style></head><body>";
  
  html += "<div class='container'>";
  html += "<div class='header'>";
  html += "<h1>Smart Lock Control</h1>";
  html += "<button class='theme-btn' onclick='toggleTheme()'>Toggle Theme</button>";
  html += "</div>";
  
  html += "<div class='grid'>";
  
  // Status Card
  html += "<div class='card status'>";
  html += "<h2>" + String(isLocked ? "LOCKED" : "UNLOCKED") + "</h2>";
  html += "<p>" + lastAccessMessage + "</p>";
  html += "<div class='toggle'>";
  html += "<span>LOCKED</span>";
  html += "<label class='switch'><input type='checkbox' id='toggle' " + String(isLocked ? "" : "checked") + " onchange='toggleLock()'><span class='slider'></span></label>";
  html += "<span>UNLOCKED</span></div>";
  html += "<div>Uptime: <span id='uptime'>" + String(millis() / 60000) + "</span> min | Connections: <span id='conn'>" + String(WiFi.softAPgetStationNum()) + "</span></div>";
  html += "</div>";
  
  // Control Card
  html += "<div class='card'>";
  html += "<h3>Access Control</h3>";
  html += "<input type='text' id='code' placeholder='New 4-digit code' maxlength='4'>";
  html += "<input type='password' id='pass' placeholder='New password'>";
  html += "<button onclick='updateCode()'>Update Code</button>";
  html += "<button onclick='updatePass()'>Update Password</button>";
  html += "<div id='msg1' class='msg'></div>";
  html += "<div id='msg2' class='msg'></div>";
  html += "</div>";
  
  // Actions Card
  html += "<div class='card'>";
  html += "<h3>Actions</h3>";
  html += "<button class='success' onclick='enrollFinger()'>Enroll Fingerprint</button>";
  html += "<button class='info' onclick='testFinger()'>Test Fingerprint</button>";
  html += "<button class='warning' onclick='clearLogs()'>Clear Logs</button>";
  html += "<button class='danger' onclick='emergency()'>Emergency Lock</button>";
  html += "<button onclick='restart()'>Restart System</button>";
  html += "<div id='msg3' class='msg'></div>";
  html += "</div>";
  
  // Logs Card
  html += "<div class='card'>";
  html += "<h3>Access Logs</h3>";
  html += "<div class='logs' id='logs'>Loading...</div>";
  html += "<button onclick='refresh()'>Refresh</button>";
  html += "</div>";
  
  html += "</div></div>";
  
  // Compact JavaScript
  html += "<script>";
  html += "let dark=false;if(localStorage.getItem('theme')==='dark'){document.body.classList.add('dark');dark=true;}";
  html += "function toggleTheme(){dark=!dark;document.body.classList.toggle('dark');localStorage.setItem('theme',dark?'dark':'light');}";
  html += "function toggleLock(){fetch('/toggle-lock?unlock='+document.getElementById('toggle').checked).then(()=>updateStatus());}";
  html += "function updateStatus(){fetch('/status').then(r=>r.json()).then(d=>{document.querySelector('h2').textContent=d.locked?'LOCKED':'UNLOCKED';document.getElementById('toggle').checked=!d.locked;document.getElementById('uptime').textContent=Math.floor(d.uptime/60);});}";
  html += "function updateCode(){const c=document.getElementById('code').value;if(c.length===4&&!isNaN(c)){fetch('/update-code?code='+c).then(r=>r.text()).then(d=>{show('msg1',d,'success');document.getElementById('code').value='';});}else{show('msg1','Invalid code','error');}}";
  html += "function updatePass(){const p=document.getElementById('pass').value;if(p.length>=6){fetch('/update-password?password='+p).then(r=>r.text()).then(d=>{show('msg2',d,'success');document.getElementById('pass').value='';});}else{show('msg2','Password too short','error');}}";
  html += "function enrollFinger(){fetch('/enroll-fingerprint').then(r=>r.text()).then(d=>show('msg3',d,'info'));}";
  html += "function testFinger(){fetch('/test-fingerprint').then(r=>r.text()).then(d=>show('msg3',d,'info'));}";
  html += "function clearLogs(){if(confirm('Clear all logs?')){fetch('/clear-logs').then(r=>r.text()).then(d=>{show('msg3',d,'info');refresh();});}}";
  html += "function emergency(){if(confirm('Emergency lock?')){fetch('/emergency-lock').then(()=>{show('msg3','Emergency locked!','error');updateStatus();});}}";
  html += "function restart(){if(confirm('Restart system?')){fetch('/restart');}}";
  html += "function refresh(){fetch('/get-logs').then(r=>r.json()).then(d=>{document.getElementById('logs').innerHTML=d.logs.map(l=>'<div class=log>'+l+'</div>').join('');});}";
  html += "function show(id,msg,type){const e=document.getElementById(id);e.textContent=msg;e.className='msg '+type;e.style.display='block';setTimeout(()=>e.style.display='none',3000);}";
  html += "setInterval(updateStatus,3000);";
  html += "document.getElementById('code').addEventListener('input',function(e){this.value=this.value.replace(/[^0-9]/g,'').slice(0,4);});";
  html += "updateStatus();refresh();";
  html += "</script></body></html>";
  
  server.send(200, "text/html", html);
  Serial.println("Dashboard sent successfully");
}

// ============================================================================
// API HANDLERS
// ============================================================================

// Enhanced function handlers
void handleTestFingerprint() {
  Serial.println("Testing fingerprint sensor...");
  addToLog("Fingerprint test initiated via web interface");
  
  if (finger.verifyPassword()) {
    server.send(200, "text/plain", "Fingerprint sensor is working correctly!");
  } else {
    server.send(200, "text/plain", "Fingerprint sensor not detected or not working!");
  }
}

void handleClearLogs() {
  for (int i = 0; i < MAX_LOG_ENTRIES; i++) {
    accessLog[i] = "";
  }
  logIndex = 0;
  addToLog("Access logs cleared via web interface");
  server.send(200, "text/plain", "All access logs have been cleared successfully!");
}

void handleEmergencyLock() {
  lockDoor();
  accessCount = 0;
  addToLog("Emergency lock activated via web interface");
  server.send(200, "text/plain", "Emergency lock activated! System is now secured.");
}

void handleResetSettings() {
  accessCode = "1234";
  webPassword = "12345678";
  accessCount = 0;
  addToLog("System settings reset to default via web interface");
  server.send(200, "text/plain", "All settings have been reset to default values!");
}

void handleExportLogs() {
  String logs = "";
  for (int i = 0; i < MAX_LOG_ENTRIES; i++) {
    int index = (logIndex + i) % MAX_LOG_ENTRIES;
    if (accessLog[index].length() > 0) {
      logs += accessLog[index] + "\n";
    }
  }
  addToLog("Access logs exported via web interface");
  server.send(200, "text/plain", logs);
}

void handleCheckCode() {
  String code = server.arg("code");
  DynamicJsonDocument doc(256);
  
  if (code == accessCode) {
    unlockDoor();
    accessCount++;
    addToLog("Access granted via web interface code");
    doc["success"] = true;
    doc["message"] = "Access granted!";
          } else {
    lastAccessMessage = "Access Denied - Wrong code";
    addToLog("Access denied - Invalid code via web interface");
    doc["success"] = false;
    doc["message"] = "Invalid code";
  }
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleStatus() {
  DynamicJsonDocument doc(512);
  doc["message"] = lastAccessMessage;
  doc["locked"] = isLocked;
  doc["uptime"] = millis() / 1000;
  doc["accessCount"] = accessCount;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleToggleLock() {
  String unlockStr = server.arg("unlock");
  bool unlock = (unlockStr == "true");
  
  if (unlock) {
    unlockDoor();
    accessCount++;
    addToLog("Door unlocked via web interface");
          } else {
    lockDoor();
    addToLog("Door locked via web interface");
  }
  
  server.send(200, "text/plain", "OK");
}

void handleUpdateCode() {
  String newCode = server.arg("code");
  if (newCode.length() == 4 && newCode.toInt() > 0) {
    accessCode = newCode;
    server.send(200, "text/plain", "Access code updated successfully!");
    addToLog("Access code changed via web interface");
      } else {
    server.send(400, "text/plain", "Invalid code! Must be 4 digits.");
  }
}

void handleUpdatePassword() {
  String newPassword = server.arg("password");
  if (newPassword.length() >= 6) {
    webPassword = newPassword;
    server.send(200, "text/plain", "Password updated successfully!");
    addToLog("Admin password changed via web interface");
      } else {
    server.send(400, "text/plain", "Password must be at least 6 characters.");
  }
}

void handleEnrollFingerprint() {
  Serial.println("Starting fingerprint enrollment...");
  Serial.println("Please place your finger on the sensor...");
  server.send(200, "text/plain", "Fingerprint enrollment started. Check serial monitor for instructions.");
  addToLog("Fingerprint enrollment started via web interface");
}

void handleGetLogs() {
  DynamicJsonDocument doc(2048);
  JsonArray logs = doc.createNestedArray("logs");
  
  for (int i = 0; i < MAX_LOG_ENTRIES; i++) {
    int index = (logIndex + i) % MAX_LOG_ENTRIES;
    if (accessLog[index].length() > 0) {
      logs.add(accessLog[index]);
    }
  }
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleStats() {
  DynamicJsonDocument doc(512);
  doc["uptime"] = millis() / 1000;
  doc["locked"] = isLocked;
  doc["message"] = lastAccessMessage;
  doc["accessCount"] = accessCount;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleRestart() {
  server.send(200, "text/plain", "System restarting...");
  addToLog("System restart initiated via web interface");
  delay(1000);
  ESP.restart();
}

void handleNetworkTest() {
  DynamicJsonDocument doc(1024);
  
  doc["ap_running"] = (WiFi.getMode() == WIFI_AP);
  doc["ip_address"] = WiFi.softAPIP().toString();
  doc["mac_address"] = WiFi.softAPmacAddress();
  doc["ssid"] = ssid;
  doc["connected_devices"] = WiFi.softAPgetStationNum();
  doc["uptime"] = millis() / 1000;
  doc["free_heap"] = ESP.getFreeHeap();
  doc["chip_model"] = ESP.getChipModel();
  doc["chip_revision"] = ESP.getChipRevision();
  doc["accessCount"] = accessCount;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleTest() {
  Serial.println("Test endpoint called");
  server.send(200, "text/html", 
    "<!DOCTYPE html><html><head><title>Test</title></head><body>"
    "<h1>ESP32 Web Server Test</h1>"
    "<p>Server is working!</p>"
    "<p>Free Heap: " + String(ESP.getFreeHeap()) + " bytes</p>"
    "<p>Uptime: " + String(millis() / 1000) + " seconds</p>"
    "<p>Access Count: " + String(accessCount) + "</p>"
    "<p><a href='/'>Go to Dashboard</a></p>"
    "</body></html>"
  );
}

// ============================================================================
// INPUT PROCESSING FUNCTIONS
// ============================================================================

void checkKeypad() {
  static unsigned long lastDebounceTime = 0;
  static bool lastButtonStates[numButtons] = {false};
  
  for (int i = 0; i < numButtons; i++) {
    bool currentState = digitalRead(keypadPins[i]) == LOW;
    
    if (currentState != lastButtonStates[i]) {
      lastDebounceTime = millis();
    }
    
    if ((millis() - lastDebounceTime) > 50) {
      if (currentState != buttonStates[i]) {
        buttonStates[i] = currentState;
        
        if (buttonStates[i]) {
          processKeypadPress(i);
        }
      }
    }
    
    lastButtonStates[i] = currentState;
  }
}

void processKeypadPress(int buttonIndex) {
  int buttonNumber = buttonIndex + 1;  // Assuming buttons labeled 1-8
          Serial.println("Button " + String(buttonNumber) + " pressed");
          addToLog("Keypad button " + String(buttonNumber) + " pressed");
          
          if (enteredCode.length() < 4) {
            enteredCode += String(buttonNumber);
            Serial.println("Current code: " + enteredCode);
          }
          
          if (enteredCode.length() == 4) {
            if (enteredCode == accessCode) {
              unlockDoor();
      accessCount++;
              addToLog("Access granted via keypad code");
            } else {
              lastAccessMessage = "Access Denied - Wrong keypad code";
              Serial.println(lastAccessMessage);
              addToLog("Access denied - Invalid keypad code");
            }
            enteredCode = "";
  }
}

void checkFingerprint() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return;
  
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return;
  
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    lastAccessMessage = "Fingerprint not recognized";
    Serial.println(lastAccessMessage);
    addToLog("Access denied - Fingerprint not recognized");
    return;
  }
  
  unlockDoor();
  accessCount++;
  addToLog("Access granted via fingerprint - ID #" + String(finger.fingerID));
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  server.handleClient();
  checkKeypad();
  checkFingerprint();
  
  // Auto-lock after delay
  if (!isLocked && millis() - unlockTime > AUTO_LOCK_DELAY) {
    lockDoor();
  }
  
  // Monitor WiFi status every 30 seconds
  static unsigned long lastWiFiCheck = 0;
  if (millis() - lastWiFiCheck > 30000) {
    monitorWiFiStatus();
    lastWiFiCheck = millis();
  }
  
  delay(100);
}

void monitorWiFiStatus() {
  static unsigned long lastConnectionCount = 0;
  unsigned long currentConnections = WiFi.softAPgetStationNum();
  
  if (currentConnections != lastConnectionCount) {
    Serial.println("Connected devices: " + String(currentConnections));
    lastConnectionCount = currentConnections;
  }
  
  // Check if AP is still running
  if (WiFi.getMode() != WIFI_AP) {
    Serial.println("Access Point disconnected! Attempting restart...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    Serial.println("Access Point restarted: " + WiFi.softAPIP().toString());
    addToLog("WiFi Access Point restarted");
  }
}