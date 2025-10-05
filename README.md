# lock

A smart lock system powered by ESP32 featuring fingerprint access, keypad entry, solenoid lock control, a modern web dashboard, and robust access management.

---

## Hardware Connections

### 1. Fingerprint Sensor (A608)

| ESP32 Pin | Fingerprint Sensor |
|-----------|-------------------|
| 3.3V      | V+                |
| GPIO16    | TX                |
| GPIO17    | RX                |
| GND       | GND               |

### 2. Keypad (8 Buttons)

| ESP32 Pin | Keypad Button |
|-----------|--------------|
| GPIO13    | Button 1     |
| GPIO12    | Button 2     |
| GPIO14    | Button 3     |
| GPIO27    | Button 4     |
| GPIO26    | Button 5     |
| GPIO25    | Button 6     |
| GPIO33    | Button 7     |
| GPIO32    | Button 8     |

### 3. Solenoid Lock (via Relay)

| ESP32 Pin | Relay Module     |
|-----------|-----------------|
| GPIO4     | IN1 (Solenoid)  |
| 3.3V      | VCC             |
| GND       | GND             |

---

## Features

- **Theme Toggle:** Light/Dark mode switching
- **Real-time Updates:** Status updates every 3 seconds
- **Animated Lock Control:** Smooth toggle switch for lock/unlock
- **Access Control:** Update codes and passwords
- **Fingerprint Functions:** Enroll and test fingerprints
- **Emergency Lock:** Immediate security lock
- **System Management:** Restart system remotely
- **Access Logs:** View and clear logs of all access attempts and system events
- **Message System:** Success/error notifications
- **Professional UI:** Clean, modern, responsive design

### Interactive Features

- **Live Status Updates:** Dashboard auto-refresh every 3 seconds
- **Theme Persistence:** Remembers your light/dark preference
- **Input Validation:** Only allows 4-digit codes and passwords with 6+ characters
- **Color-coded Buttons:** Green (success), Red (danger), Blue (info)
- **Smooth Animations:** Animated toggle switch transitions
- **Responsive Design:** Works on mobile and desktop
- **Notification System:** Auto-hiding success/error messages

### Professional Interface

- Clean white cards on purple background
- Adaptive grid layout for any screen size
- Proper spacing and readable typography
- Hover effects on buttons for better UX
- Message notifications with auto-hide

---

## 🚀 How to Use

1. **Upload the code** to your ESP32.
2. **Connect the hardware** as described above.
3. **Connect to WiFi:** `lockofdorf`
4. **Open your browser** and go to: [http://192.168.4.1](http://192.168.4.1)
5. **Use the Dashboard:** All features will load instantly.

---

## Dashboard Features

- **Toggle Theme:** Light/dark mode switcher
- **Lock Control:** Animated toggle for lock/unlock
- **Update Settings:** Change access codes and passwords
- **Fingerprint Management:** Enroll new fingerprints, test recognition
- **Emergency Features:** Lockdown and system restart
- **View Logs:** See all access attempts and events

---

The compact web dashboard is optimized for the ESP32, combining robust security, full feature set, and a professional, user-friendly interface.

---
