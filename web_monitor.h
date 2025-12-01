#ifndef WEB_MONITOR_H
#define WEB_MONITOR_H

#include <Arduino.h>

const char MONITOR_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>B2A Gateway - Live Monitor</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { 
            font-family: 'Courier New', monospace;
            background: #1e1e1e;
            color: #d4d4d4;
            padding: 15px;
        }
        .header {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 20px;
            border-radius: 10px;
            margin-bottom: 20px;
            text-align: center;
        }
        .header h1 { font-size: 24px; margin-bottom: 5px; }
        .header .subtitle { font-size: 14px; opacity: 0.9; }
        .grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 15px;
            margin-bottom: 15px;
        }
        .panel {
            background: #252526;
            border: 1px solid #3e3e42;
            border-radius: 8px;
            padding: 15px;
        }
        .panel-title {
            color: #4ec9b0;
            font-size: 16px;
            font-weight: bold;
            margin-bottom: 12px;
            padding-bottom: 8px;
            border-bottom: 1px solid #3e3e42;
        }
        .status-item {
            display: flex;
            justify-content: space-between;
            padding: 8px 0;
            border-bottom: 1px solid #2d2d30;
            font-size: 14px;
        }
        .status-item:last-child { border-bottom: none; }
        .label { color: #9cdcfe; }
        .value { 
            color: #ce9178;
            font-weight: bold;
        }
        .value.on { color: #4ec9b0; }
        .value.off { color: #d16969; }
        .indicator {
            display: inline-block;
            width: 10px;
            height: 10px;
            border-radius: 50%;
            margin-right: 5px;
        }
        .indicator.on { background: #4ec9b0; box-shadow: 0 0 8px #4ec9b0; }
        .indicator.off { background: #d16969; }
        .traffic-panel {
            background: #252526;
            border: 1px solid #3e3e42;
            border-radius: 8px;
            padding: 15px;
            margin-top: 15px;
        }
        .can-message {
            background: #1e1e1e;
            padding: 8px;
            margin: 5px 0;
            border-left: 3px solid #4ec9b0;
            border-radius: 3px;
            font-size: 12px;
            font-family: 'Courier New', monospace;
        }
        .can-message.bus2 { border-left-color: #c586c0; }
        .byte { 
            display: inline-block;
            padding: 2px 4px;
            margin: 0 2px;
            background: #3e3e42;
            border-radius: 3px;
        }
        .byte-changed {
            background: #d7ba7d;
            color: #1e1e1e;
            font-weight: bold;
        }
        .route-list {
            max-height: 200px;
            overflow-y: auto;
        }
        .route-item {
            padding: 8px;
            margin: 5px 0;
            background: #1e1e1e;
            border-radius: 4px;
            font-size: 12px;
        }
        .route-item.active { border-left: 3px solid #4ec9b0; }
        .route-item.inactive { border-left: 3px solid #d16969; opacity: 0.5; }
        .controls {
            display: flex;
            gap: 10px;
            margin-bottom: 15px;
        }
        button {
            background: #0e639c;
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 14px;
            font-family: inherit;
        }
        button:hover { background: #1177bb; }
        button.danger { background: #d16969; }
        button.danger:hover { background: #e07a7a; }
        #traffic { 
            max-height: 300px;
            overflow-y: auto;
            margin-top: 10px;
        }
        .timestamp { 
            color: #858585;
            font-size: 11px;
        }
        a {
            color: #4ec9b0;
            text-decoration: none;
        }
        a:hover { text-decoration: underline; }
    </style>
</head>
<body>
    <div class="header">
        <h1>🚀 B2A Gateway - Live Monitor</h1>
        <div class="subtitle">Real-time System Status | <a href="/" style="color:white;">⚙️ Configuration</a></div>
    </div>

    <div class="controls">
        <button onclick="toggleTraffic()">⏸️ Pause Traffic</button>
        <button onclick="clearTraffic()">🗑️ Clear Traffic</button>
        <button class="danger" onclick="location.reload()">🔄 Refresh</button>
    </div>

    <div class="grid">
        <!-- System Status -->
        <div class="panel">
            <div class="panel-title">💻 System Status</div>
            <div class="status-item">
                <span class="label">Uptime:</span>
                <span class="value" id="uptime">0s</span>
            </div>
            <div class="status-item">
                <span class="label">Free Heap:</span>
                <span class="value" id="heap">0 KB</span>
            </div>
            <div class="status-item">
                <span class="label">PCF8575:</span>
                <span class="value" id="pcf">-</span>
            </div>
            <div class="status-item">
                <span class="label">ADS1115:</span>
                <span class="value" id="ads">-</span>
            </div>
        </div>

        <!-- CAN Status -->
        <div class="panel">
            <div class="panel-title">🚌 CAN Bus Status</div>
            <div class="status-item">
                <span class="label">CAN1 TX:</span>
                <span class="value" id="can1tx">0</span>
            </div>
            <div class="status-item">
                <span class="label">CAN1 RX:</span>
                <span class="value" id="can1rx">0</span>
            </div>
            <div class="status-item">
                <span class="label">CAN2 TX:</span>
                <span class="value" id="can2tx">0</span>
            </div>
            <div class="status-item">
                <span class="label">CAN2 RX:</span>
                <span class="value" id="can2rx">0</span>
            </div>
        </div>

        <!-- Digital Inputs -->
        <div class="panel">
            <div class="panel-title">🔌 Digital Inputs</div>
            <div class="status-item">
                <span class="label">DIN 1:</span>
                <span class="value"><span class="indicator off" id="din1"></span><span id="din1txt">OFF</span></span>
            </div>
            <div class="status-item">
                <span class="label">DIN 2:</span>
                <span class="value"><span class="indicator off" id="din2"></span><span id="din2txt">OFF</span></span>
            </div>
            <div class="status-item">
                <span class="label">DIN 3:</span>
                <span class="value"><span class="indicator off" id="din3"></span><span id="din3txt">OFF</span></span>
            </div>
            <div class="status-item">
                <span class="label">DIN 4:</span>
                <span class="value"><span class="indicator off" id="din4"></span><span id="din4txt">OFF</span></span>
            </div>
            <div class="status-item">
                <span class="label">DIN 5:</span>
                <span class="value"><span class="indicator off" id="din5"></span><span id="din5txt">OFF</span></span>
            </div>
        </div>

        <!-- Analog Inputs -->
        <div class="panel">
            <div class="panel-title">📈 Analog Inputs</div>
            <div class="status-item">
                <span class="label">Analog 1:</span>
                <span class="value" id="analog1">0.00V</span>
            </div>
            <div class="status-item">
                <span class="label">Analog 2:</span>
                <span class="value" id="analog2">0.00V</span>
            </div>
            <div class="status-item">
                <span class="label">Analog 3:</span>
                <span class="value" id="analog3">0.00V</span>
            </div>
        </div>

        <!-- Temperature Sensors -->
        <div class="panel">
            <div class="panel-title">🌡️ Temperature</div>
            <div class="status-item">
                <span class="label">Temp 1:</span>
                <span class="value" id="temp1">0.0°C</span>
            </div>
            <div class="status-item">
                <span class="label">Temp 2:</span>
                <span class="value" id="temp2">0.0°C</span>
            </div>
        </div>

        <!-- AUX Outputs -->
        <div class="panel">
            <div class="panel-title">⚡ AUX Outputs</div>
            <div class="status-item">
                <span class="label">AUX 1:</span>
                <span class="value"><span class="indicator off" id="aux1"></span><span id="aux1txt">OFF</span></span>
            </div>
            <div class="status-item">
                <span class="label">AUX 2:</span>
                <span class="value"><span class="indicator off" id="aux2"></span><span id="aux2txt">OFF</span></span>
            </div>
            <div class="status-item">
                <span class="label">AUX 3:</span>
                <span class="value"><span class="indicator off" id="aux3"></span><span id="aux3txt">OFF</span></span>
            </div>
            <div class="status-item">
                <span class="label">AUX 4:</span>
                <span class="value"><span class="indicator off" id="aux4"></span><span id="aux4txt">OFF</span></span>
            </div>
        </div>
    </div>

    <!-- Active Routes -->
    <div class="traffic-panel">
        <div class="panel-title">🔄 Active CAN Routes</div>
        <div class="route-list" id="routeList">
            <div class="route-item inactive">No routes configured</div>
        </div>
    </div>

    <!-- CAN Traffic -->
    <div class="traffic-panel">
        <div class="panel-title">📡 Live CAN Traffic (Last 50 messages)</div>
        <div id="traffic"></div>
    </div>

    <script>
        let ws;
        let trafficPaused = false;
        let canMessages = {};
        let canMessagesPrev = {};

        function connectWebSocket() {
            ws = new WebSocket('ws://' + window.location.hostname + ':81');
            
            ws.onopen = () => {
                console.log('WebSocket connected');
                loadStatus();
            };
            
            ws.onmessage = (evt) => {
                let data = JSON.parse(evt.data);
                if (data.type === 'status_update') {
                    updateStatus(data);
                } else if (data.type === 'can_rx' && !trafficPaused) {
                    displayCANMessage(data);
                }
            };
            
            ws.onclose = () => {
                console.log('WebSocket disconnected');
                setTimeout(connectWebSocket, 3000);
            };
        }

        function loadStatus() {
            fetch('/api/status')
                .then(r => r.json())
                .then(data => updateStatus(data));
            
            fetch('/api/get_config')
                .then(r => r.json())
                .then(data => displayRoutes(data))
                .catch(() => {});
        }

        function updateStatus(data) {
            document.getElementById('uptime').textContent = data.uptime + 's';
            document.getElementById('heap').textContent = (data.free_heap / 1024).toFixed(1) + ' KB';
            document.getElementById('pcf').textContent = data.pcf_available ? '✓ OK' : '✗ N/A';
            document.getElementById('ads').textContent = data.ads_available ? '✓ OK' : '✗ N/A';
            
            document.getElementById('can1tx').textContent = data.can1_tx;
            document.getElementById('can1rx').textContent = data.can1_rx;
            document.getElementById('can2tx').textContent = data.can2_tx;
            document.getElementById('can2rx').textContent = data.can2_rx;
            
            // Digital inputs
            for (let i = 0; i < 5; i++) {
                let state = data.digital[i];
                document.getElementById('din' + (i+1)).className = 'indicator ' + (state ? 'on' : 'off');
                document.getElementById('din' + (i+1) + 'txt').textContent = state ? 'ON' : 'OFF';
            }
            
            // Analog
            document.getElementById('analog1').textContent = data.analog[0].toFixed(2) + 'V';
            document.getElementById('analog2').textContent = data.analog[1].toFixed(2) + 'V';
            document.getElementById('analog3').textContent = data.analog[2].toFixed(2) + 'V';
            
            // Temperature
            document.getElementById('temp1').textContent = data.temp1.toFixed(1) + '°C';
            document.getElementById('temp2').textContent = data.temp2.toFixed(1) + '°C';
            
            // AUX outputs
            for (let i = 0; i < 4; i++) {
                let state = data.aux[i];
                document.getElementById('aux' + (i+1)).className = 'indicator ' + (state ? 'on' : 'off');
                document.getElementById('aux' + (i+1) + 'txt').textContent = state ? 'ON' : 'OFF';
            }
        }

        function displayRoutes(config) {
            if (!config.routes || config.routes.length === 0) return;
            
            let html = '';
            for (let route of config.routes) {
                let dir = ['CAN1→CAN2', 'CAN2→CAN1', 'CAN1↔CAN2'][route.direction];
                let status = route.enabled ? 'active' : 'inactive';
                html += `<div class="route-item ${status}">
                    ${route.enabled ? '✓' : '✗'} ${dir} | 
                    0x${route.src_id.toString(16).toUpperCase()} → 
                    0x${route.dst_id.toString(16).toUpperCase()}
                    ${route.remap_id ? ' (Remap)' : ''}
                </div>`;
            }
            document.getElementById('routeList').innerHTML = html;
        }

        function displayCANMessage(msg) {
            let key = `${msg.bus}_${msg.id}`;
            canMessages[key] = msg;
            
            if (Object.keys(canMessages).length > 50) {
                let keys = Object.keys(canMessages);
                delete canMessages[keys[0]];
            }
            
            updateCANDisplay();
        }

        function updateCANDisplay() {
            let html = '';
            let sorted = Object.values(canMessages).sort((a, b) => {
                if (a.bus !== b.bus) return a.bus - b.bus;
                return a.id - b.id;
            });
            
            for (let m of sorted) {
                let prev = canMessagesPrev[`${m.bus}_${m.id}`];
                let dataStr = '';
                
                for (let i = 0; i < m.data.length; i++) {
                    let byteHex = m.data[i].toString(16).padStart(2, '0').toUpperCase();
                    let changed = prev && prev.data[i] !== m.data[i];
                    dataStr += `<span class="byte ${changed ? 'byte-changed' : ''}">${byteHex}</span>`;
                }
                
                let time = new Date(m.timestamp).toLocaleTimeString();
                html += `<div class="can-message bus${m.bus}">
                    <span class="timestamp">${time}</span> | 
                    <strong>Bus ${m.bus}</strong> | 
                    ID: 0x${m.id.toString(16).toUpperCase()} | 
                    DLC: ${m.dlc} | 
                    ${dataStr}
                </div>`;
            }
            
            canMessagesPrev = {...canMessages};
            document.getElementById('traffic').innerHTML = html;
        }

        function toggleTraffic() {
            trafficPaused = !trafficPaused;
            event.target.textContent = trafficPaused ? '▶️ Resume Traffic' : '⏸️ Pause Traffic';
        }

        function clearTraffic() {
            canMessages = {};
            canMessagesPrev = {};
            document.getElementById('traffic').innerHTML = '';
        }

        connectWebSocket();
        setInterval(loadStatus, 2000);
    </script>
</body>
</html>
)rawliteral";

#endif
