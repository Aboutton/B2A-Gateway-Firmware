# B2A CAN Gateway - Quick Start Guide

## 🚀 Getting Started in 5 Minutes

### Step 1: Install Arduino IDE Libraries (5 min)
Open Arduino IDE → Tools → Manage Libraries, search and install:
- ArduinoJson
- WebSockets (by Markus Sattler)
- Adafruit_ADS1X15
- ACAN2517FD

### Step 2: Board Setup
1. Tools → Board → ESP32S3 Dev Module
2. Tools → Upload Speed → 921600
3. Tools → USB CDC On Boot → Enabled

### Step 3: Upload Firmware
1. Open `Gateway_Firmware.ino`
2. Click Upload button
3. Wait for "Done uploading" message

### Step 4: Connect & Configure
1. **Find WiFi network**: `B2A-GW-XXXXXXXX`
2. **Password**: `B2A12345`
3. **Open browser**: `http://192.168.4.1`
4. **Configure your outputs and routing**
5. **Click "Save Configuration"**

---

## 📱 Quick Configuration Examples

### Example 1: Turn ON AUX1 when engine starts
**Outputs Tab:**
- AUX1 Mode: `Always ON`
- Done! AUX1 will be ON as soon as gateway powers up

### Example 2: Control fan speed from CAN
**Outputs Tab:**
- PWM1 Mode: `CAN Trigger`
- CAN ID: `0x100`
- Data Byte: `2`
- Scale: `0.4` (byte 0-255 → 0-100%)
- Frequency: `25000` Hz

### Example 3: Forward all messages from ECU to dashboard
**Routing Tab:**
- Click "Add Rule"
- Direction: `CAN1 → CAN2`
- Source ID: `0x100`
- Destination ID: `0x100` (keep same)
- Enable: ✓

---

## 🔧 System Settings

### CAN Bus Settings
**System Tab:**
- CAN1 Bitrate: `500000` (500 kbps) - typical for modern ECUs
- CAN2 Bitrate: `250000` (250 kbps) - typical for older vehicles
- Termination: Enable if gateway is at end of CAN bus

### WiFi Settings (optional)
- SSID: Change from default
- Password: Change from default `B2A12345`

---

## 📊 Dashboard View

The dashboard shows real-time status:
- **Uptime**: How long gateway has been running
- **CAN Stats**: TX/RX message counts
- **AUX Outputs**: ✓ = ON, ✗ = OFF
- **Sensors**: Current voltage and temperature readings

**Tip**: Dashboard auto-refreshes every 2 seconds

---

## 🛠️ Troubleshooting

| Problem | Solution |
|---------|----------|
| Can't find WiFi | Check Serial Monitor for correct SSID |
| Can't access webpage | Try `http://192.168.4.1` instead of mDNS |
| AUX not working | Test with "Always ON" mode first |
| No CAN traffic | Check termination resistors enabled |
| Config not saving | Click "Save Configuration" button |

---

## 💡 Pro Tips

1. **Test outputs safely**: Start with "Always ON" mode to verify hardware before using CAN triggers
2. **Monitor Serial**: Keep Serial Monitor open (115200 baud) to see CAN messages in real-time
3. **Save often**: Click "Save Configuration" after every change
4. **Bluetooth monitoring**: Connect via Bluetooth to monitor while driving (safer than WiFi)
5. **Backup config**: Take screenshot of your configuration before major changes

---

## 📞 Need Help?

**Serial Monitor Commands:**
- Shows all CAN RX messages in real-time
- Shows configuration loading status
- Shows any errors or warnings

**Factory Reset:**
System Tab → Factory Reset button → Confirm

**Pin Reference:**
See `config.h` for all pin assignments

---

## ⚡ Next Steps

1. ✅ Upload firmware
2. ✅ Connect to WiFi
3. ✅ Configure one output (test with Always ON)
4. ✅ Add CAN routing rule
5. ✅ Test with real vehicle
6. ✅ Fine-tune scaling and timing
7. ✅ Save final configuration

**Happy engine swapping! 🏁**
