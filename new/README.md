# B2A CAN Gateway - Production Firmware

## Overview
Production-ready CAN bus gateway firmware for ESP32-S3 with:
- ✅ Bidirectional CAN routing (CAN1 ↔ CAN2) with speed conversion
- ✅ Output control (4x AUX, 2x PWM) via CAN/Digital Input/Always ON
- ✅ Sensor broadcasting (3x Analog, 2x Temp) to CAN bus
- ✅ WiFi configuration interface
- ✅ Bluetooth monitoring (mobile access)
- ✅ Flash-based configuration storage
- ✅ mDNS support (http://b2a-gateway.local)

## Features

### 1. **Output Control**
Configure AUX and PWM outputs with multiple trigger modes:
- **CAN Trigger**: Activate output based on specific CAN message data
- **Digital Input Trigger**: Control output from DIN1-5 state
- **Always ON**: Force output ON with fixed value
- **OFF**: Disable output

### 2. **Sensor Broadcasting**
Send sensor data on CAN bus:
- 3x Analog inputs (0-5V)
- 2x Temperature sensors (NTC)
- Configurable CAN ID, byte position, scaling, and offset

### 3. **CAN Message Routing**
Route messages between CAN1 and CAN2:
- Bidirectional or unidirectional
- ID remapping support
- Rate limiting
- Automatic speed conversion (e.g., 500k → 250k)

### 4. **Dual Configuration Access**
- **WiFi**: Full web interface for complete setup
- **Bluetooth**: Mobile monitoring and quick adjustments

## Installation

### Required Libraries
Install via Arduino Library Manager:
```
- ArduinoJson (v6.x)
- WebSockets by Markus Sattler
- Adafruit_ADS1X15
- ACAN2517FD
- Wire (built-in)
- SPI (built-in)
- WiFi (built-in)
- BluetoothSerial (built-in)
- ESPmDNS (built-in)
- Preferences (built-in)
```

### Board Settings
- **Board**: ESP32S3 Dev Module
- **Upload Speed**: 921600
- **USB CDC On Boot**: Enabled
- **Flash Size**: 16MB
- **Partition Scheme**: Default

### Upload Steps
1. Open `Gateway_Firmware.ino` in Arduino IDE
2. Select correct board and port
3. Upload firmware
4. Open Serial Monitor (115200 baud) to view startup

## Configuration

### First Time Setup
1. **Connect to WiFi**:
   - SSID: `B2A-GW-XXXXXXXX` (unique per device)
   - Password: `B2A12345`

2. **Access Web Interface**:
   - URL: `http://192.168.4.1` or `http://b2a-gateway.local`

3. **Configure Outputs** (Outputs Tab):
   - Select trigger mode for each AUX/PWM
   - For CAN triggers: Set ID, byte, value
   - For DIN triggers: Select digital input
   - For Always ON: Set duty cycle (PWM only)

4. **Configure Sensors** (Sensors Tab):
   - Enable/disable each sensor
   - Set CAN ID for broadcast
   - Set byte position (0-7)
   - Configure scale and offset

5. **Configure Routing** (Routing Tab):
   - Add routing rules
   - Set direction (CAN1→CAN2, CAN2→CAN1, Both)
   - Configure ID remapping if needed

6. **System Settings** (System Tab):
   - Set CAN1/CAN2 bitrates
   - Enable/disable termination resistors
   - Save configuration

### Bluetooth Access
1. Pair with device: `B2A-Gateway-BT`
2. Use serial terminal app (e.g., Serial Bluetooth Terminal)
3. Send commands:
   - `STATUS` - Get system status
   - `AUX1=ON` - Turn on AUX1
   - `PWM1=50` - Set PWM1 to 50%

## Configuration Examples

### Example 1: Fuel Pump Control via CAN
**Scenario**: Turn on fuel pump (AUX1) when engine RPM > 500

**Config**:
- AUX1 Mode: CAN Trigger
- CAN ID: `0x100` (RPM message)
- Data Byte: 2
- Data Value: 0x50 (80 in decimal = 500 RPM with scale 0.1)
- Mask: 0xFF

### Example 2: Cooling Fan via Temperature
**Scenario**: PWM fan speed based on coolant temp sensor

**Config**:
- PWM1 Mode: CAN Trigger
- CAN ID: `0x200` (temp message)
- Data Byte: 0
- Scale: 0.5 (byte value 0-200 → 0-100% duty)
- Frequency: 25000 Hz

### Example 3: Route Engine Data to Dashboard
**Scenario**: Forward engine ECU messages (CAN1 @ 500k) to dashboard (CAN2 @ 250k)

**Config**:
- Route 1: CAN1→CAN2
- Source ID: 0x100
- Dest ID: 0x100 (keep same)
- Rate Limit: 0ms (no limit)

## File Structure

```
Gateway_Firmware/
├── Gateway_Firmware.ino       # Main program
├── gateway_config.h           # Configuration structures
├── config_storage.h/cpp       # Flash storage
├── output_controller.h/cpp    # Output trigger logic
├── sensor_broadcaster.h/cpp   # Sensor CAN broadcast
├── can_router.h/cpp           # CAN routing engine
├── bluetooth_handler.h/cpp    # Bluetooth communication
├── web_handlers.h/cpp         # Web API endpoints
├── web_page.h                 # Configuration UI
├── config.h                   # Hardware pin definitions
├── io_control.h/cpp           # PCF8575 I/O control
├── sensors.h/cpp              # Analog/Temp sensors
├── can_bus.h/cpp              # MCP2517FD CAN driver
├── pwm_control.h/cpp          # PWM output control
└── websocket_handler.h/cpp    # WebSocket updates
```

## API Reference

### HTTP Endpoints
- `GET /` - Main configuration page
- `GET /api/status` - System status (JSON)
- `GET /api/get_config` - Get current configuration
- `POST /api/set_config` - Update configuration
- `POST /api/save_config` - Save to flash
- `POST /api/reset_config` - Factory reset

### WebSocket (Port 81)
Real-time status updates sent every 500ms:
```json
{
  "type": "status_update",
  "uptime": 12345,
  "can1_tx": 100,
  "can1_rx": 200,
  "aux": [true, false, true, false],
  "analog": [1.23, 2.45, 3.67],
  "temp1": 25.5,
  "temp2": 30.2
}
```

## Troubleshooting

### Issue: Can't connect to WiFi
- **Solution**: Check WiFi SSID matches chip ID
- **Reset**: Hold reset button, check Serial Monitor for SSID

### Issue: CAN messages not routing
- **Check**: Routing rules enabled
- **Check**: Correct direction (CAN1→CAN2)
- **Check**: Source ID matches incoming message

### Issue: Output not activating
- **Check**: Trigger mode configured correctly
- **Check**: CAN ID and byte position match message
- **Check**: Data value/mask correct
- **Test**: Set to "Always ON" mode to verify hardware

### Issue: Bluetooth not connecting
- **Check**: Bluetooth enabled in System Settings
- **Check**: Device name: `B2A-Gateway-BT`
- **Reset**: Power cycle gateway

## Safety Notes

⚠️ **Important Safety Considerations**:
- Always test outputs with low-power loads first
- Verify CAN routing rules before connecting to vehicle
- Use appropriate fuses on all outputs
- Monitor system temperature during operation
- Keep firmware updated

## Factory Reset

**Via Web Interface**:
1. Go to System Settings tab
2. Click "Factory Reset"
3. Confirm action

**Via Serial**:
Send command in Serial Monitor at 115200 baud (not implemented in this version, can be added)

## Version History

### v1.0.0 (Current)
- Initial production release
- WiFi + Bluetooth configuration
- Bidirectional CAN routing
- Output control (CAN/DIN/Always ON)
- Sensor broadcasting
- Flash configuration storage

## Support

For issues, questions, or feature requests:
- GitHub: [Your Repository]
- Email: [Your Email]

## License

[Your License Here]

---

**Built with ❤️ for automotive enthusiasts**
