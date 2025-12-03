#ifndef WEB_PAGE_H
#define WEB_PAGE_H

#include <Arduino.h>

const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>B2A Gateway Configuration</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { 
            font-family: Arial, sans-serif; 
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: #333;
            padding: 20px;
        }
        .container { max-width: 1200px; margin: 0 auto; }
        h1 { 
            color: white; 
            text-align: center; 
            margin-bottom: 20px;
        }
        .tabs {
            display: flex;
            background: white;
            border-radius: 10px 10px 0 0;
            overflow: hidden;
        }
        .tab {
            flex: 1;
            padding: 15px;
            background: #f0f0f0;
            border: none;
            cursor: pointer;
            font-size: 14px;
            font-weight: bold;
        }
        .tab.active { background: white; color: #667eea; }
        .tab-content {
            display: none;
            background: white;
            padding: 20px;
            border-radius: 0 0 10px 10px;
            max-height: 70vh;
            overflow-y: auto;
        }
        .tab-content.active { display: block; }
        .card {
            background: #f8f8f8;
            padding: 15px;
            margin: 10px 0;
            border-radius: 8px;
            border-left: 4px solid #667eea;
        }
        .card h3 { 
            color: #667eea; 
            margin-bottom: 10px;
            font-size: 18px;
        }
        input, select {
            padding: 8px;
            margin: 5px 0;
            border: 1px solid #ddd;
            border-radius: 4px;
            font-size: 14px;
        }
        input[type="number"], input[type="text"] {
            width: 100%;
        }
        select { width: 100%; }
        input[type="checkbox"] {
            width: auto;
            margin-right: 5px;
        }
        button {
            background: #667eea;
            color: white;
            padding: 10px 20px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            margin: 5px;
            font-size: 14px;
        }
        button:hover { background: #5568d3; }
        button.danger { background: #e74c3c; }
        button.danger:hover { background: #c0392b; }
        button.success { background: #27ae60; }
        button.success:hover { background: #229954; }
        .status { 
            padding: 10px; 
            margin: 10px 0; 
            border-radius: 5px;
            text-align: center;
            font-weight: bold;
        }
        .status.success { background: #2ecc71; color: white; }
        .status.error { background: #e74c3c; color: white; }
        label { 
            display: block; 
            margin-top: 10px; 
            font-weight: bold;
            font-size: 13px;
        }
        .inline-group {
            display: flex;
            gap: 10px;
            align-items: center;
        }
        .inline-group label {
            margin: 0;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin: 10px 0;
        }
        th, td {
            padding: 10px;
            text-align: left;
            border-bottom: 1px solid #ddd;
        }
        th {
            background: #667eea;
            color: white;
        }
        .grid-2 {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 10px;
        }
        @media (max-width: 768px) {
            .grid-2 { grid-template-columns: 1fr; }
            .tabs { flex-wrap: wrap; }
            .tab { flex: 1 0 50%; }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🚀 B2A CAN Gateway Configuration</h1>
        <div style="text-align:center; margin-bottom:15px;">
            <a href="/monitor" style="color:white; background:#27ae60; padding:10px 20px; border-radius:5px; text-decoration:none; display:inline-block;">
                📊 Live Monitor
            </a>
        </div>
        
        <div class="tabs">
            <button class="tab active" onclick="showTab('dashboard')">📊 Dashboard</button>
            <button class="tab" onclick="showTab('outputs')">⚡ Outputs</button>
            <button class="tab" onclick="showTab('sensors')">🌡️ Sensors</button>
            <button class="tab" onclick="showTab('routing')">🔄 Routing</button>
            <button class="tab" onclick="showTab('system')">⚙️ System</button>
        </div>
        
        <!-- Dashboard Tab -->
        <div id="dashboard" class="tab-content active">
            <h2>System Status</h2>
            <div class="card">
                <p><strong>Uptime:</strong> <span id="uptime">0s</span></p>
                <p><strong>Free Heap:</strong> <span id="freeHeap">0 KB</span></p>
                <p><strong>CAN1 TX/RX:</strong> <span id="can1Stats">0 / 0</span></p>
                <p><strong>CAN2 TX/RX:</strong> <span id="can2Stats">0 / 0</span></p>
            </div>
            
            <h3>Outputs Status</h3>
            <div class="card">
                <p><strong>AUX 1:</strong> <span id="aux1">OFF</span></p>
                <p><strong>AUX 2:</strong> <span id="aux2">OFF</span></p>
                <p><strong>AUX 3:</strong> <span id="aux3">OFF</span></p>
                <p><strong>AUX 4:</strong> <span id="aux4">OFF</span></p>
            </div>
            
            <h3>Sensors</h3>
            <div class="card">
                <p><strong>Analog 1:</strong> <span id="analog1">0.00V</span></p>
                <p><strong>Analog 2:</strong> <span id="analog2">0.00V</span></p>
                <p><strong>Analog 3:</strong> <span id="analog3">0.00V</span></p>
                <p><strong>Temp 1:</strong> <span id="temp1">0°C</span></p>
                <p><strong>Temp 2:</strong> <span id="temp2">0°C</span></p>
            </div>
        </div>
        
        <!-- Outputs Tab -->
        <div id="outputs" class="tab-content">
            <h2>Output Configuration</h2>
            
            <div class="card">
                <h3>AUX 1 Configuration</h3>
                <label>Trigger Mode:</label>
                <select id="aux1_mode" onchange="updateAuxFields(1)">
                    <option value="0">OFF</option>
                    <option value="1">CAN Trigger</option>
                    <option value="2">Digital Input</option>
                    <option value="3">Always ON</option>
                </select>
                <div id="aux1_can" style="display:none;">
                    <label>CAN ID (hex):</label>
                    <input type="text" id="aux1_canid" placeholder="0x100" value="0x100">
                    <label>Data Byte (0-7):</label>
                    <input type="number" id="aux1_byte" min="0" max="7" value="0">
                    <label>Data Value (0-255):</label>
                    <input type="number" id="aux1_value" min="0" max="255" value="0">
                </div>
                <div id="aux1_din" style="display:none;">
                    <label>Digital Input:</label>
                    <select id="aux1_dinput">
                        <option value="1">DIN 1</option>
                        <option value="2">DIN 2</option>
                        <option value="3">DIN 3</option>
                        <option value="4">DIN 4</option>
                        <option value="5">DIN 5</option>
                    </select>
                </div>
            </div>
            
            <div class="card">
                <h3>AUX 2 Configuration</h3>
                <label>Trigger Mode:</label>
                <select id="aux2_mode" onchange="updateAuxFields(2)">
                    <option value="0">OFF</option>
                    <option value="1">CAN Trigger</option>
                    <option value="2">Digital Input</option>
                    <option value="3">Always ON</option>
                </select>
                <div id="aux2_can" style="display:none;">
                    <label>CAN ID (hex):</label>
                    <input type="text" id="aux2_canid" placeholder="0x100" value="0x100">
                    <label>Data Byte (0-7):</label>
                    <input type="number" id="aux2_byte" min="0" max="7" value="0">
                    <label>Data Value (0-255):</label>
                    <input type="number" id="aux2_value" min="0" max="255" value="0">
                </div>
                <div id="aux2_din" style="display:none;">
                    <label>Digital Input:</label>
                    <select id="aux2_dinput">
                        <option value="1">DIN 1</option>
                        <option value="2">DIN 2</option>
                        <option value="3">DIN 3</option>
                        <option value="4">DIN 4</option>
                        <option value="5">DIN 5</option>
                    </select>
                </div>
            </div>
            
            <div class="card">
                <h3>PWM 1 Configuration</h3>
                <label>Trigger Mode:</label>
                <select id="pwm1_mode" onchange="updatePwmFields(1)">
                    <option value="0">OFF</option>
                    <option value="1">CAN Trigger</option>
                    <option value="2">Digital Input</option>
                    <option value="3">Always ON</option>
                </select>
                <label>Frequency (Hz):</label>
                <input type="number" id="pwm1_freq" value="1000" min="1" max="40000">
                <div id="pwm1_can" style="display:none;">
                    <label>CAN ID (hex):</label>
                    <input type="text" id="pwm1_canid" placeholder="0x100" value="0x100">
                    <label>Data Byte (0-7):</label>
                    <input type="number" id="pwm1_byte" min="0" max="7" value="0">
                    <label>Scale (byte to %):</label>
                    <input type="number" id="pwm1_scale" step="0.1" value="0.4">
                </div>
                <div id="pwm1_alwayson" style="display:none;">
                    <label>Duty Cycle (%):</label>
                    <input type="number" id="pwm1_duty" min="0" max="100" value="50">
                </div>
            </div>
            
            <button class="success" onclick="saveOutputs()">💾 Save Output Configuration</button>
        </div>
        
        <!-- Sensors Tab -->
        <div id="sensors" class="tab-content">
            <h2>Sensor Broadcasting</h2>
            
            <div class="card">
                <h3>Analog Input 1</h3>
                <div class="inline-group">
                    <input type="checkbox" id="analog1_en">
                    <label>Enable Broadcasting</label>
                </div>
                <label>CAN ID (hex):</label>
                <input type="text" id="analog1_id" placeholder="0x200" value="0x200">
                <label>Start Byte (0-6):</label>
                <input type="number" id="analog1_byte" min="0" max="6" value="0">
                <label>Scale:</label>
                <input type="number" id="analog1_scale" step="0.1" value="100">
                <label>Offset:</label>
                <input type="number" id="analog1_offset" step="0.1" value="0">
            </div>
            
            <div class="card">
                <h3>Analog Input 2</h3>
                <div class="inline-group">
                    <input type="checkbox" id="analog2_en">
                    <label>Enable Broadcasting</label>
                </div>
                <label>CAN ID (hex):</label>
                <input type="text" id="analog2_id" placeholder="0x201" value="0x201">
                <label>Start Byte (0-6):</label>
                <input type="number" id="analog2_byte" min="0" max="6" value="0">
                <label>Scale:</label>
                <input type="number" id="analog2_scale" step="0.1" value="100">
                <label>Offset:</label>
                <input type="number" id="analog2_offset" step="0.1" value="0">
            </div>
            
            <div class="card">
                <h3>Analog Input 3</h3>
                <div class="inline-group">
                    <input type="checkbox" id="analog3_en">
                    <label>Enable Broadcasting</label>
                </div>
                <label>CAN ID (hex):</label>
                <input type="text" id="analog3_id" placeholder="0x202" value="0x202">
                <label>Start Byte (0-6):</label>
                <input type="number" id="analog3_byte" min="0" max="6" value="0">
                <label>Scale:</label>
                <input type="number" id="analog3_scale" step="0.1" value="100">
                <label>Offset:</label>
                <input type="number" id="analog3_offset" step="0.1" value="0">
            </div>
            
            <div class="card">
                <h3>Temperature Sensor 1</h3>
                <div class="inline-group">
                    <input type="checkbox" id="temp1_en">
                    <label>Enable Broadcasting</label>
                </div>
                <label>CAN ID (hex):</label>
                <input type="text" id="temp1_id" placeholder="0x210" value="0x210">
                <label>Start Byte (0-6):</label>
                <input type="number" id="temp1_byte" min="0" max="6" value="0">
                <label>Scale:</label>
                <input type="number" id="temp1_scale" step="0.1" value="10">
            </div>
            
            <div class="card">
                <h3>Temperature Sensor 2</h3>
                <div class="inline-group">
                    <input type="checkbox" id="temp2_en">
                    <label>Enable Broadcasting</label>
                </div>
                <label>CAN ID (hex):</label>
                <input type="text" id="temp2_id" placeholder="0x211" value="0x211">
                <label>Start Byte (0-6):</label>
                <input type="number" id="temp2_byte" min="0" max="6" value="0">
                <label>Scale:</label>
                <input type="number" id="temp2_scale" step="0.1" value="10">
            </div>
            
            <div class="card">
                <h3>Broadcast Settings</h3>
                <label>Broadcast Interval (ms):</label>
                <input type="number" id="sensor_interval" min="50" max="5000" value="100">
            </div>
            
            <button class="success" onclick="saveSensors()">💾 Save Sensor Configuration</button>
        </div>
        
        <!-- Routing Tab -->
        <div id="routing" class="tab-content">
            <h2>CAN Message Routing</h2>
            
            <div class="card">
                <h3>➕ Add New Route</h3>
                <label>Direction:</label>
                <select id="new_direction">
                    <option value="0">CAN1 → CAN2</option>
                    <option value="1">CAN2 → CAN1</option>
                    <option value="2">Bidirectional</option>
                </select>
                <label>Source ID (hex):</label>
                <input type="text" id="new_srcid" placeholder="0x100" value="0x100">
                <label>Destination ID (hex):</label>
                <input type="text" id="new_dstid" placeholder="0x100" value="0x100">
                <div class="inline-group">
                    <input type="checkbox" id="new_remap">
                    <label>Remap ID (use Destination ID)</label>
                </div>
                <button onclick="addRoute()">Add Route</button>
            </div>
            
            <h3>Current Routes</h3>
            <table id="routeTable">
                <thead>
                    <tr>
                        <th>Enabled</th>
                        <th>Direction</th>
                        <th>Source ID</th>
                        <th>Dest ID</th>
                        <th>Action</th>
                    </tr>
                </thead>
                <tbody id="routeList">
                    <tr><td colspan="5">No routes configured</td></tr>
                </tbody>
            </table>
            
            <button class="success" onclick="saveRoutes()">💾 Save Routing Rules</button>
        </div>
        
        <!-- System Tab -->
        <div id="system" class="tab-content">
            <h2>System Settings</h2>
            
            <div class="card">
                <h3>CAN Bus Configuration</h3>
                <label>CAN1 Bitrate:</label>
                <select id="can1Bitrate">
                    <option value="125000">125 kbps</option>
                    <option value="250000">250 kbps</option>
                    <option value="500000" selected>500 kbps</option>
                    <option value="1000000">1 Mbps</option>
                </select>
                
                <label>CAN2 Bitrate:</label>
                <select id="can2Bitrate">
                    <option value="125000">125 kbps</option>
                    <option value="250000">250 kbps</option>
                    <option value="500000" selected>500 kbps</option>
                    <option value="1000000">1 Mbps</option>
                </select>
                
                <div class="inline-group">
                    <input type="checkbox" id="can1_term">
                    <label>CAN1 120Ω Termination</label>
                </div>
                <div class="inline-group">
                    <input type="checkbox" id="can2_term">
                    <label>CAN2 120Ω Termination</label>
                </div>
            </div>
            
            <button class="success" onclick="saveSystem()">💾 Save System Settings</button>
            <button class="danger" onclick="resetConfig()">🔄 Factory Reset</button>
        </div>
        
        <div id="statusMsg" class="status" style="display:none;"></div>
    </div>
    
    <script>
        let currentConfig = {};
        
        function showTab(tabName) {
            document.querySelectorAll('.tab-content').forEach(el => el.classList.remove('active'));
            document.querySelectorAll('.tab').forEach(el => el.classList.remove('active'));
            document.getElementById(tabName).classList.add('active');
            event.target.classList.add('active');
            
            if (tabName !== 'dashboard') loadConfig();
        }
        
        function loadStatus() {
            fetch('/api/status')
                .then(r => r.json())
                .then(data => {
                    document.getElementById('uptime').textContent = data.uptime + 's';
                    document.getElementById('freeHeap').textContent = (data.free_heap / 1024).toFixed(1) + ' KB';
                    document.getElementById('can1Stats').textContent = data.can1_tx + ' / ' + data.can1_rx;
                    document.getElementById('can2Stats').textContent = data.can2_tx + ' / ' + data.can2_rx;
                    
                    for (let i = 0; i < 4; i++) {
                        document.getElementById('aux' + (i+1)).textContent = data.aux[i] ? 'ON ✓' : 'OFF';
                    }
                    
                    document.getElementById('analog1').textContent = data.analog[0].toFixed(2) + 'V';
                    document.getElementById('analog2').textContent = data.analog[1].toFixed(2) + 'V';
                    document.getElementById('analog3').textContent = data.analog[2].toFixed(2) + 'V';
                    document.getElementById('temp1').textContent = data.temp1.toFixed(1) + '°C';
                    document.getElementById('temp2').textContent = data.temp2.toFixed(1) + '°C';
                });
        }
        
        function loadConfig() {
            fetch('/api/get_config')
                .then(r => r.json())
                .then(data => {
                    currentConfig = data;
                    
                    // System settings
                    if (document.getElementById('can1Bitrate')) {
                        document.getElementById('can1Bitrate').value = data.can1_bitrate || 500000;
                        document.getElementById('can2Bitrate').value = data.can2_bitrate || 500000;
                        document.getElementById('can1_term').checked = data.can1_term || false;
                        document.getElementById('can2_term').checked = data.can2_term || false;
                    }
                    
                    // AUX outputs
                    for (let i = 0; i < 4; i++) {
                        if (data.aux && data.aux[i] && document.getElementById('aux' + (i+1) + '_mode')) {
                            document.getElementById('aux' + (i+1) + '_mode').value = data.aux[i].mode;
                            document.getElementById('aux' + (i+1) + '_canid').value = '0x' + data.aux[i].can_id.toString(16).toUpperCase();
                            document.getElementById('aux' + (i+1) + '_byte').value = data.aux[i].data_byte;
                            document.getElementById('aux' + (i+1) + '_value').value = data.aux[i].data_value;
                            document.getElementById('aux' + (i+1) + '_dinput').value = data.aux[i].digital_input;
                            updateAuxFields(i+1);
                        }
                    }
                    
                    // PWM outputs
                    for (let i = 0; i < 2; i++) {
                        if (data.pwm && data.pwm[i] && document.getElementById('pwm' + (i+1) + '_mode')) {
                            document.getElementById('pwm' + (i+1) + '_mode').value = data.pwm[i].mode;
                            document.getElementById('pwm' + (i+1) + '_canid').value = '0x' + data.pwm[i].can_id.toString(16).toUpperCase();
                            document.getElementById('pwm' + (i+1) + '_byte').value = data.pwm[i].data_byte;
                            document.getElementById('pwm' + (i+1) + '_scale').value = data.pwm[i].scale;
                            document.getElementById('pwm' + (i+1) + '_freq').value = data.pwm[i].frequency;
                            document.getElementById('pwm' + (i+1) + '_duty').value = data.pwm[i].always_duty;
                            updatePwmFields(i+1);
                        }
                    }
                    
                    // Sensor broadcasts
                    for (let i = 0; i < 3; i++) {
                        if (data.analog_broadcast && data.analog_broadcast[i] && document.getElementById('analog' + (i+1) + '_en')) {
                            document.getElementById('analog' + (i+1) + '_en').checked = data.analog_broadcast[i].enabled;
                            document.getElementById('analog' + (i+1) + '_id').value = '0x' + data.analog_broadcast[i].can_id.toString(16).toUpperCase();
                            document.getElementById('analog' + (i+1) + '_byte').value = data.analog_broadcast[i].start_byte;
                            document.getElementById('analog' + (i+1) + '_scale').value = data.analog_broadcast[i].scale;
                        }
                    }
                    
                    // Temperature broadcasts
                    for (let i = 0; i < 2; i++) {
                        if (data.temp_broadcast && data.temp_broadcast[i] && document.getElementById('temp' + (i+1) + '_en')) {
                            document.getElementById('temp' + (i+1) + '_en').checked = data.temp_broadcast[i].enabled;
                            document.getElementById('temp' + (i+1) + '_id').value = '0x' + data.temp_broadcast[i].can_id.toString(16).toUpperCase();
                            document.getElementById('temp' + (i+1) + '_byte').value = data.temp_broadcast[i].start_byte;
                            document.getElementById('temp' + (i+1) + '_scale').value = data.temp_broadcast[i].scale;
                        }
                    }
                    
                    // Routing table
                    let tbody = document.getElementById('routeList');
                    if (tbody && data.routes && data.routes.length > 0) {
                        tbody.innerHTML = ''; // Clear existing
                        
                        data.routes.forEach(route => {
                            let directionText = route.direction === 0 ? 'CAN1 → CAN2' : 
                                               route.direction === 1 ? 'CAN2 → CAN1' : 'Bidirectional';
                            let srcId = '0x' + route.src_id.toString(16).toUpperCase();
                            let dstId = '0x' + route.dst_id.toString(16).toUpperCase();
                            
                            let row = tbody.insertRow();
                            row.innerHTML = `
                                <td><input type="checkbox" ${route.enabled ? 'checked' : ''}></td>
                                <td>${directionText}</td>
                                <td>${srcId}</td>
                                <td>${dstId}</td>
                                <td><button class="danger" onclick="this.closest('tr').remove()">Delete</button></td>
                            `;
                        });
                    }
                    
                    console.log('Config loaded successfully');
                })
                .catch(err => console.error('Failed to load config:', err));
        }
        
        function updateAuxFields(num) {
            let mode = document.getElementById('aux' + num + '_mode').value;
            document.getElementById('aux' + num + '_can').style.display = (mode == '1') ? 'block' : 'none';
            document.getElementById('aux' + num + '_din').style.display = (mode == '2') ? 'block' : 'none';
        }
        
        function updatePwmFields(num) {
            let mode = document.getElementById('pwm' + num + '_mode').value;
            document.getElementById('pwm' + num + '_can').style.display = (mode == '1') ? 'block' : 'none';
            document.getElementById('pwm' + num + '_alwayson').style.display = (mode == '3') ? 'block' : 'none';
        }
        
        function saveOutputs() {
            console.log('saveOutputs called');
            let config = {
                aux: [],
                pwm: []
            };
            
            // Collect AUX data
            for (let i = 1; i <= 4; i++) {
                let canidField = document.getElementById('aux' + i + '_canid');
                if (!canidField) {
                    console.error('aux' + i + '_canid not found');
                    continue;
                }
                let canid = canidField.value;
                console.log('AUX' + i + ' canid:', canid);
                config.aux.push({
                    mode: parseInt(document.getElementById('aux' + i + '_mode').value),
                    can_id: parseInt(canid, 16),
                    data_byte: parseInt(document.getElementById('aux' + i + '_byte').value),
                    data_value: parseInt(document.getElementById('aux' + i + '_value').value),
                    digital_input: parseInt(document.getElementById('aux' + i + '_dinput').value)
                });
            }
            
            // Collect PWM data
            for (let i = 1; i <= 2; i++) {
                let canidField = document.getElementById('pwm' + i + '_canid');
                if (!canidField) {
                    console.error('pwm' + i + '_canid not found');
                    continue;
                }
                let canid = canidField.value;
                console.log('PWM' + i + ' canid:', canid);
                config.pwm.push({
                    mode: parseInt(document.getElementById('pwm' + i + '_mode').value),
                    can_id: parseInt(canid, 16),
                    data_byte: parseInt(document.getElementById('pwm' + i + '_byte').value),
                    scale: parseFloat(document.getElementById('pwm' + i + '_scale').value),
                    frequency: parseInt(document.getElementById('pwm' + i + '_freq').value),
                    always_duty: parseInt(document.getElementById('pwm' + i + '_duty').value)
                });
            }
            
            console.log('Sending config:', config);
            
            fetch('/api/set_config', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(config)
            })
            .then(r => r.text())
            .then(() => {
                showStatus('Output configuration saved!', 'success');
                saveToFlash();
            })
            .catch(err => {
                console.error('Save failed:', err);
                showStatus('Save failed: ' + err, 'error');
            });
        }
        
        function saveSensors() {
            console.log('saveSensors called');
            let config = {
                analog_broadcast: [],
                temp_broadcast: []
            };
            
            // Collect analog sensor configs
            for (let i = 1; i <= 3; i++) {
                let idField = document.getElementById('analog' + i + '_id');
                if (!idField) {
                    console.error('analog' + i + '_id not found');
                    continue;
                }
                let canid = idField.value;
                console.log('Analog' + i + ' id:', canid);
                config.analog_broadcast.push({
                    enabled: document.getElementById('analog' + i + '_en').checked,
                    can_id: parseInt(canid, 16),
                    start_byte: parseInt(document.getElementById('analog' + i + '_byte').value),
                    scale: parseFloat(document.getElementById('analog' + i + '_scale').value)
                });
            }
            
            // Collect temp sensor configs
            for (let i = 1; i <= 2; i++) {
                let idField = document.getElementById('temp' + i + '_id');
                if (!idField) {
                    console.error('temp' + i + '_id not found');
                    continue;
                }
                let canid = idField.value;
                console.log('Temp' + i + ' id:', canid);
                config.temp_broadcast.push({
                    enabled: document.getElementById('temp' + i + '_en').checked,
                    can_id: parseInt(canid, 16),
                    start_byte: parseInt(document.getElementById('temp' + i + '_byte').value),
                    scale: parseFloat(document.getElementById('temp' + i + '_scale').value)
                });
            }
            
            console.log('Sending config:', config);
            
            fetch('/api/set_config', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(config)
            })
            .then(r => r.text())
            .then(() => {
                showStatus('Sensor configuration saved!', 'success');
                saveToFlash();
            })
            .catch(err => {
                console.error('Save failed:', err);
                showStatus('Save failed: ' + err, 'error');
            });
        }
        
        function saveRoutes() {
            let config = {
                routes: []
            };
            
            let tbody = document.getElementById('routeList');
            if (!tbody) {
                console.error('routeList table not found');
                return;
            }
            
            for (let i = 0; i < tbody.rows.length; i++) {
                let row = tbody.rows[i];
                if (row.cells.length < 5) continue; // Skip empty row placeholder
                
                let enabled = row.cells[0].querySelector('input[type="checkbox"]').checked;
                let directionText = row.cells[1].textContent;
                let direction = directionText.includes('→ CAN2') ? 0 : (directionText.includes('→ CAN1') ? 1 : 2);
                let srcId = row.cells[2].textContent;
                let dstId = row.cells[3].textContent;
                
                config.routes.push({
                    enabled: enabled,
                    direction: direction,
                    src_id: parseInt(srcId, 16),
                    dst_id: parseInt(dstId, 16),
                    remap_id: srcId !== dstId
                });
            }
            
            console.log('Saving routes:', config);
            
            fetch('/api/set_config', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(config)
            })
            .then(r => {
                console.log('set_config response:', r.status);
                return r.text();
            })
            .then(msg => {
                console.log('set_config result:', msg);
                console.log('Calling saveToFlash...');
                showStatus('Routing rules saved!', 'success');
                saveToFlash();
            })
            .catch(err => {
                console.error('Save failed:', err);
                showStatus('Save failed: ' + err, 'error');
            });
        }
        
        function saveSystem() {
            let config = {
                can1_bitrate: parseInt(document.getElementById('can1Bitrate').value),
                can2_bitrate: parseInt(document.getElementById('can2Bitrate').value),
                can1_term: document.getElementById('can1_term').checked,
                can2_term: document.getElementById('can2_term').checked
            };
            
            console.log('Saving system config:', config);
            
            fetch('/api/set_config', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(config)
            })
            .then(r => {
                console.log('set_config response:', r.status);
                return r.text();
            })
            .then(msg => {
                console.log('set_config result:', msg);
                showStatus('System settings saved!', 'success');
                saveToFlash();
            })
            .catch(err => {
                console.error('Save failed:', err);
                showStatus('Save failed: ' + err, 'error');
            });
        }
        
        function saveToFlash() {
            console.log('saveToFlash called');
            fetch('/api/save_config', { method: 'POST' })
                .then(r => {
                    console.log('save_config response:', r.status);
                    return r.text();
                })
                .then(msg => console.log('Saved to flash:', msg))
                .catch(err => console.error('Flash save error:', err));
        }
        
        function addRoute() {
            let tbody = document.getElementById('routeList');
            if (tbody.rows[0].cells.length === 1) tbody.innerHTML = '';
            
            let row = tbody.insertRow();
            row.innerHTML = `
                <td><input type="checkbox" checked></td>
                <td>${document.getElementById('new_direction').selectedOptions[0].text}</td>
                <td>${document.getElementById('new_srcid').value}</td>
                <td>${document.getElementById('new_dstid').value}</td>
                <td><button class="danger" onclick="this.closest('tr').remove()">Delete</button></td>
            `;
        }
        
        function resetConfig() {
            if (confirm('⚠️ Reset to factory defaults? This cannot be undone!')) {
                fetch('/api/reset_config', { method: 'POST' })
                    .then(r => r.text())
                    .then(msg => {
                        showStatus('Configuration reset! Reloading...', 'success');
                        setTimeout(() => location.reload(), 2000);
                    });
            }
        }
        
        function showStatus(msg, type) {
            let el = document.getElementById('statusMsg');
            el.textContent = msg;
            el.className = 'status ' + type;
            el.style.display = 'block';
            setTimeout(() => el.style.display = 'none', 3000);
        }
        
        setInterval(loadStatus, 2000);
        loadStatus();
        
        // Load config after page fully loads
        window.addEventListener('load', function() {
            setTimeout(loadConfig, 500);
        });
    </script>
</body>
</html>
)rawliteral";

#endif