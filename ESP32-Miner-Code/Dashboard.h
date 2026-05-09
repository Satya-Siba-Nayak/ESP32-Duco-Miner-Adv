// Dashboard.h
#ifndef DASHBOARD_H
#define DASHBOARD_H

const char WEBSITE[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>@@DEVICE@@ | Core Dashboard</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <link href="https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;700&family=Inter:wght@400;600;800&display=swap" rel="stylesheet">
    <style>
        :root {
            --bg: #050507; --surface: #0e0e11; --border: #222228;
            --text: #f4f4f5; --text-dim: #a1a1aa; --text-dark: #52525b;
            --accent: #f59e0b; --accent-glow: rgba(245, 158, 11, 0.15);
            --secondary: #06b6d4; --secondary-glow: rgba(6, 182, 212, 0.1);
            --danger: #ef4444; --success: #10b981; --terminal-bg: #030305;
            --threat-color: var(--secondary-glow);
        }
        * { box-sizing: border-box; }
        body { background-color: var(--bg); color: var(--text); font-family: 'Inter', sans-serif; margin: 0; padding: 20px; background-image: radial-gradient(circle at 0% 0%, var(--accent-glow) 0%, transparent 40%), radial-gradient(circle at 100% 100%, var(--threat-color) 0%, transparent 40%); background-attachment: fixed; min-height: 100vh; transition: background-image 1s ease-in-out; }
        .container { max-width: 1250px; margin: 0 auto; display: flex; flex-direction: column; gap: 20px; }
        /* HEADER */
        .header { display: flex; justify-content: space-between; align-items: center; padding: 20px 25px; background: rgba(14, 14, 17, 0.6); backdrop-filter: blur(12px); border: 1px solid var(--border); border-radius: 16px; }
        .sys-info h1 { margin: 0; font-size: 1.4rem; font-weight: 800; display: flex; align-items: center; gap: 12px; }
        .sys-info h1 img { width: 28px; }
        .sys-info .sub { font-family: 'JetBrains Mono', monospace; color: var(--text-dim); font-size: 0.85rem; margin-top: 6px; }
        .status-badge { display: flex; align-items: center; gap: 8px; background: rgba(16, 185, 129, 0.1); color: var(--success); padding: 6px 12px; border-radius: 8px; font-family: 'JetBrains Mono', monospace; font-size: 0.8rem; border: 1px solid rgba(16, 185, 129, 0.2); }
        .pulse { width: 8px; height: 8px; background-color: var(--success); border-radius: 50%; box-shadow: 0 0 10px var(--success); animation: blink 2s infinite; }
        /* GRID */
        .grid-main { display: grid; grid-template-columns: 1fr 2.5fr 1.2fr; gap: 20px; }
        .grid-stats { display: grid; grid-template-columns: repeat(auto-fit, minmax(220px, 1fr)); gap: 20px; }
        @media (max-width: 1100px) { .grid-main { grid-template-columns: 1fr; } }
        /* CARDS */
        .card { background: rgba(14, 14, 17, 0.7); backdrop-filter: blur(10px); border: 1px solid var(--border); border-radius: 16px; padding: 24px; transition: all 0.3s ease; position: relative; overflow: hidden; }
        .card-label { color: var(--text-dim); font-size: 0.75rem; text-transform: uppercase; letter-spacing: 1.5px; margin-bottom: 12px; font-weight: 600; display: flex; justify-content: space-between; }
        .card-value { font-family: 'JetBrains Mono', monospace; font-size: 2rem; font-weight: 700; color: var(--text); }
        .card-unit { font-size: 0.9rem; color: var(--text-dim); font-weight: 400; }
        .value-accent { color: var(--accent); }
        .hashrate-card .card-value { font-size: 3.5rem; text-shadow: 0 0 20px var(--accent-glow); }
        .chart-container { width: 100%; height: 220px; position: relative; }
        .mini-chart-container { width: 100%; height: 45px; position: relative; margin-top: 12px; }
        /* PROGRESS BARS */
        .progress-bg { width: 100%; background: var(--border); border-radius: 4px; height: 6px; margin-top: 8px; overflow: hidden; }
        .progress-fill { height: 100%; background: var(--success); transition: width 0.5s ease, background 0.5s ease; }
        /* TERMINAL */
        .terminal { background: var(--terminal-bg); border: 1px solid var(--border); border-radius: 12px; padding: 15px; height: 280px; overflow-y: auto; font-family: 'JetBrains Mono', monospace; font-size: 0.75rem; color: var(--text-dim); display: flex; flex-direction: column-reverse; scrollbar-width: none; }
        .terminal::-webkit-scrollbar { display: none; }
        .term-line { margin-bottom: 6px; border-bottom: 1px solid rgba(255,255,255,0.02); padding-bottom: 4px; animation: slideIn 0.2s ease-out; }
        .term-time { color: var(--text-dark); margin-right: 8px; }
        .term-success { color: var(--success); }
        .term-warn { color: var(--accent); }
        .term-crit { color: var(--danger); font-weight: bold; }
        .subtle-btn { background: transparent; border: 1px solid var(--border); color: var(--text-dim); padding: 6px 12px; font-family: 'JetBrains Mono', monospace; font-size: 0.7rem; border-radius: 6px; cursor: pointer; transition: all 0.2s; text-transform: uppercase; letter-spacing: 1px; }
        .subtle-btn:hover { background: rgba(239, 68, 68, 0.1); border-color: var(--danger); color: var(--danger); }
        .action-btn { flex: 1; padding: 8px; text-align: center; font-weight: bold; border-radius: 6px; cursor: pointer; transition: 0.3s; }
        .data-row { display: flex; justify-content: space-between; margin-bottom: 8px; font-family: 'JetBrains Mono', monospace; font-size: 0.8rem; align-items: center; }
        .footer { text-align: center; font-family: 'JetBrains Mono', monospace; font-size: 0.75rem; color: var(--text-dim); padding: 20px 0; border-top: 1px solid var(--border); margin-top: 10px; }
        @keyframes blink { 0% { opacity: 1; } 50% { opacity: 0.4; } 100% { opacity: 1; } }
        @keyframes slideIn { from { opacity: 0; transform: translateX(-10px); } to { opacity: 1; transform: translateX(0); } }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <div class="sys-info">
                <h1><img src="https://github.com/revoxhere/duino-coin/blob/master/Resources/duco.png?raw=true" alt="Duco"> @@DEVICE@@</h1>
                <div class="sub">RIG: @@ID@@ | USER: @@USER@@ | IP: @@IP_ADDR@@</div>
            </div>
            <div class="status-badge" id="mainStatus"><div class="pulse"></div><span style="color:var(--success);">SYSTEM ONLINE</span></div>
        </div>

        <div class="grid-main">
            <div class="card hashrate-card" style="display:flex; flex-direction:column; justify-content:center;">
                <div class="card-label">Total Hashrate</div>
                <div class="card-value value-accent"><span id="hashrate">@@HASHRATE@@</span><span class="card-unit" style="font-size:1.2rem;"> kH/s</span></div>
                
                <div style="margin-top:20px; border-top: 1px solid var(--border); padding-top: 15px;">
                    <div class="data-row"><span style="color:var(--text-dim);">Thread 0:</span><span id="hr0" style="color:var(--text);">--</span></div>
                    <div class="data-row"><span style="color:var(--text-dim);">Thread 1:</span><span id="hr1" style="color:var(--text);">--</span></div>
                </div>

                <div style="margin-top:15px; border-top: 1px solid var(--border); padding-top: 15px;">
                    <div class="data-row">
                        <span style="color:var(--text-dim);">Lifetime Hashes:</span>
                        <span id="lifetime" style="color:var(--text); font-weight:bold;">0</span>
                    </div>
                    <div class="data-row" style="margin-top: 8px;">
                        <span style="color:var(--text-dim);">Est. Daily Yield:</span>
                        <span style="color:var(--accent); font-weight:bold;"><span id="dailyYield">--</span><span class="card-unit"> DUCO</span></span>
                    </div>
                </div>
            </div>

            <div class="card" style="padding: 15px 20px;">
                <div class="card-label">Hashrate Telemetry <span id="avgTime" style="color:var(--text-dark);">Avg: --s / share</span></div>
                <div class="chart-container">
                    <canvas id="hashChart"></canvas>
                </div>
            </div>

            <div class="card" style="padding: 15px;">
                <div class="card-label" style="padding-left:5px;">Matrix Log Feed</div>
                <div class="terminal" id="terminal"></div>
            </div>
        </div>

        <div class="grid-stats">
            <!-- LEFT COLUMN: MINING EFFICIENCY -->
            <div class="card">
                <div class="card-label">Mining Efficiency</div>
                <div class="card-value" style="color:var(--secondary);" id="efficiency">0.0%</div>
                <div class="progress-bg"><div class="progress-fill" id="eff-bar" style="width: 0%;"></div></div>
                <div style="font-family: 'JetBrains Mono'; font-size: 0.85rem; color: var(--text-dim); margin-top: 8px;">Acc: <span id="shares" style="color:var(--success)">0 / @@SHARES@@</span></div>
                
                <div style="margin-top: 15px; border-top: 1px solid var(--border); padding-top: 15px;">
                    <div class="card-label">Uplink Latency</div>
                    <div style="font-size: 1.5rem; font-family: 'JetBrains Mono'; color: var(--text);"><span id="ping" style="color:var(--accent)">-- ms</span></div>
                    <div class="progress-bg"><div class="progress-fill" id="ping-bar" style="width: 100%; background: var(--accent);"></div></div>
                    <div class="mini-chart-container">
                        <canvas id="pingChart"></canvas>
                    </div>
                </div>

                <div style="margin-top: 15px; border-top: 1px solid var(--border); padding-top: 15px;">
                    <div class="card-label">Validation Engine</div>
                    <div class="data-row" style="margin-top: 10px;">
                        <span style="color:var(--text-dim);">Rejected Shares:</span>
                        <span id="rejected" style="color:var(--danger); font-weight:bold; font-size: 1.1rem;">0</span>
                    </div>
                    <div class="data-row">
                        <span style="color:var(--text-dim);">Kolka Trust:</span>
                        <span id="kolka" style="color:var(--success); font-weight:bold;">100%</span>
                    </div>
                    <div class="data-row">
                        <span style="color:var(--text-dim);">Hashes / Share:</span>
                        <span id="hashRatio" style="color:var(--text);">--</span>
                    </div>
                </div>

                <!-- NEW PERFORMANCE MATRIX -->
                <div style="margin-top: 15px; border-top: 1px solid var(--border); padding-top: 15px;">
                    <div class="card-label">Performance Matrix</div>
                    <div class="data-row" style="margin-top: 10px;">
                        <span style="color:var(--text-dim);">Hashrate Stability:</span>
                        <span id="hashStability" style="color:var(--text); font-weight:bold;">--%</span>
                    </div>
                    <div class="data-row">
                        <span style="color:var(--text-dim);">Drop Rate:</span>
                        <span id="dropRate" style="color:var(--text);">--%</span>
                    </div>
                    <div class="data-row">
                        <span style="color:var(--text-dim);">Network Jitter:</span>
                        <span id="pingJitter" style="color:var(--text);">-- ms</span>
                    </div>
                </div>
            </div>

            <!-- MIDDLE COLUMN: NETWORK SIGNAL -->
            <div class="card">
                <div class="card-label">Network Signal</div>
                <div class="card-value" id="rssi" style="color:var(--success)">-- dBm</div>
                <div style="font-family: 'JetBrains Mono'; font-size: 0.85rem; color: var(--text-dim); margin-top: 8px;">Node: <span id="node">@@NODE@@</span></div>
                
                <div style="margin-top: 15px; border-top: 1px solid var(--border); padding-top: 15px;">
                    <div class="data-row">
                        <span style="color:var(--text-dim);">MAC Address:</span>
                        <span id="mac" style="color:var(--text);">--:--:--:--:--:--</span>
                    </div>
                    <div class="data-row">
                        <span style="color:var(--text-dim);">WiFi Channel:</span>
                        <span id="channel" style="color:var(--text);">--</span>
                    </div>
                    <div class="data-row">
                        <span style="color:var(--text-dim);">BSSID:</span>
                        <span id="bssid" style="color:var(--text-dark);">--</span>
                    </div>
                </div>

                <div style="margin-top: 15px; border-top: 1px solid var(--border); padding-top: 15px;">
                    <div class="card-label">System Uptime</div>
                    <div style="font-size: 1.5rem; font-family: 'JetBrains Mono'; color: var(--text);" id="uptime">Loading...</div>
                    <div style="font-family: 'JetBrains Mono'; font-size: 0.8rem; color: var(--text-dim); margin-top: 4px;">Net Diff: <span id="diff">@@DIFF@@</span></div>
                </div>

                <div style="margin-top: 15px; border-top: 1px solid var(--border); padding-top: 15px;">
                    <div class="card-label">Share Velocity</div>
                    <div style="font-size: 1.5rem; font-family: 'JetBrains Mono'; color: var(--secondary);" id="shareVelocity">--</div>
                    <div style="font-family: 'JetBrains Mono'; font-size: 0.75rem; color: var(--text-dim); margin-top: 4px;">Accepted Shares / Min</div>
                </div>

                <div style="margin-top: 15px; border-top: 1px solid var(--border); padding-top: 15px;">
                    <div class="card-label">Energy Profile</div>
                    <div class="data-row" style="margin-top: 10px;">
                        <span style="color:var(--text-dim);">Est. Consumption:</span>
                        <span id="estPower" style="color:var(--accent); font-size: 1.1rem;">-- W</span>
                    </div>
                    <div class="data-row">
                        <span style="color:var(--text-dim);">Core Voltage:</span>
                        <span style="color:var(--text);">3.30 V</span>
                    </div>
                </div>
            </div>

            <!-- RIGHT COLUMN: SILICON DIAGNOSTICS -->
            <div class="card">
                <div class="card-label">Silicon Diagnostics</div>
                <div class="card-value"><span id="memory">@@MEMORY@@</span><span class="card-unit"> B</span></div>
                <div style="font-family: 'JetBrains Mono'; font-size: 0.85rem; color: var(--text-dim); margin-top: 8px;">Free RAM Available</div>
                
                <div style="margin-top: 15px; border-top: 1px solid var(--border); padding-top: 15px;">
                    <div class="card-label">Thermal Reading</div>
                    <div class="data-row">
                        <span style="font-size: 1.5rem; font-family: 'JetBrains Mono'; color: var(--text);" id="temp">-- &deg;C</span>
                    </div>
                    <div class="progress-bg"><div class="progress-fill" id="temp-bar" style="width: 0%;"></div></div>
                </div>

                <div style="margin-top: 15px; border-top: 1px solid var(--border); padding-top: 15px;">
                    <div class="data-row"><span>Chip Model:</span><span id="chip" style="color: var(--text);">--</span></div>
                    <div class="data-row"><span>Clock Speed:</span><span id="freq" style="color: var(--text);">-- MHz</span></div>
                </div>

                <!-- NEW COMPUTE METRICS -->
                <div style="margin-top: 15px; border-top: 1px solid var(--border); padding-top: 15px;">
                    <div class="card-label">Compute Metrics</div>
                    <div class="data-row" style="margin-top: 10px;">
                        <span style="color:var(--text-dim);">Core Symmetry:</span>
                        <span id="coreSymmetry" style="color:var(--text); font-weight:bold;">--%</span>
                    </div>
                    <div class="data-row">
                        <span style="color:var(--text-dim);">Memory Health:</span>
                        <span id="memHealth" style="color:var(--text);">--</span>
                    </div>
                </div>

                <div style="margin-top: 15px; border-top: 1px solid var(--border); padding-top: 15px;">
                    <div class="card-label" id="pmLabel" style="display:flex; align-items:center;">Power Management</div>
                    <div class="data-row" style="margin-top: 10px;">
                        <span style="color:var(--text-dim);">Thermal Status:</span>
                        <span id="thermalStatus" style="color:var(--success); font-weight:bold;">NOMINAL</span>
                    </div>
                    <div style="display: flex; gap: 8px; margin-top: 12px;">
                        <button id="btnEco" class="subtle-btn action-btn" onclick="setPowerMode('eco')">ECO</button>
                        <button id="btnBal" class="subtle-btn action-btn" onclick="setPowerMode('bal')">BAL</button>
                        <button id="btnMax" class="subtle-btn action-btn" onclick="setPowerMode('max')">MAX</button>
                    </div>
                </div>

                <div style="margin-top: 15px; padding-top: 15px; display: flex; justify-content: space-between; align-items: center; border-top: 1px solid var(--border);">
                    <div style="font-family: 'JetBrains Mono'; font-size: 0.65rem; color: var(--text-dark);" id="overrideStatus">SYS_CTRL</div>
                    <button class="subtle-btn" style="padding: 4px 8px; font-size: 0.6rem;" onclick="executeReboot()">Force Reboot</button>
                </div>
            </div>
        </div>

        <div class="footer">
        Advanced Telemetry Engine<br><br>
        <span style="font-size: 0.85rem;">Made with ♡ by <a href="https://satya.is-a.dev" target="_blank" style="color: var(--accent); text-decoration: none; font-weight: bold; transition: 0.2s;">Satya</a></span><br><br>
        <a href="https://duinocoin.com" target="_blank" style="color: var(--text-dim); text-decoration: none;">duinocoin.com</a>
        </div>
    </div>

    <script>
        // Main Hashrate Chart
        const ctx = document.getElementById('hashChart').getContext('2d');
        let gradient = ctx.createLinearGradient(0, 0, 0, 220);
        gradient.addColorStop(0, 'rgba(245, 158, 11, 0.5)');
        gradient.addColorStop(1, 'rgba(245, 158, 11, 0.0)');

        const hashChart = new Chart(ctx, {
            type: 'line',
            data: { labels: [], datasets: [{ data: [], borderColor: '#f59e0b', backgroundColor: gradient, borderWidth: 2, fill: true, tension: 0.1, pointRadius: 0, pointHitRadius: 10 }] },
            options: { 
                responsive: true, maintainAspectRatio: false, 
                scales: { 
                    x: { display: false }, 
                    y: { 
                        display: true, beginAtZero: false, grace: '10%',       
                        grid: { color: '#222228', drawBorder: false }, 
                        ticks: { color: '#a1a1aa', font: { family: 'JetBrains Mono' } } 
                    } 
                }, 
                plugins: { legend: { display: false } }, animation: { duration: 0 } 
            }
        });

        // Mini Ping Chart
        const pingCtx = document.getElementById('pingChart').getContext('2d');
        const pingChart = new Chart(pingCtx, {
            type: 'line',
            data: { labels: [], datasets: [{ data: [], borderColor: '#06b6d4', borderWidth: 1.5, backgroundColor: 'rgba(6, 182, 212, 0.15)', fill: true, tension: 0.3, pointRadius: 0 }] },
            options: { 
                responsive: true, maintainAspectRatio: false, 
                scales: { x: { display: false }, y: { display: false, min: 0 } }, 
                plugins: { legend: { display: false }, tooltip: {enabled: false} }, 
                animation: { duration: 0 } 
            }
        });

        const MAX_DATAPOINTS = 35;
        const MAX_PING_POINTS = 20;
        let lastAccepted = 0;

        // JS Derived Metric Trackers
        let hashrateHistory = [];
        let pingHistory = [];

        // Terminal Logger
        function logToTerminal(msg, type = "normal") {
            const term = document.getElementById('terminal');
            const time = new Date().toLocaleTimeString('en-US', { hour12: false });
            let style = "";
            if(type === "success") style = "term-success";
            if(type === "warn") style = "term-warn";
            if(type === "crit") style = "term-crit";
            
            const line = `<div class="term-line"><span class="term-time">[${time}]</span><span class="${style}">${msg}</span></div>`;
            term.insertAdjacentHTML('afterbegin', line); 
            if(term.childElementCount > 40) term.lastElementChild.remove();
        }

        logToTerminal("System Boot. Connecting to Kolka Node...", "warn");

        function executeReboot() {
            if(confirm("CRITICAL WARNING: Force hardware reboot?")) {
                document.getElementById('overrideStatus').innerText = "EXECUTING REBOOT...";
                document.getElementById('overrideStatus').style.color = "var(--danger)";
                logToTerminal("INITIATING HARDWARE REBOOT SEQUENCE...", "crit");
                fetch('/api/reboot').then(() => {
                    let c = 5; let i = setInterval(() => {
                        document.getElementById('overrideStatus').innerText = "RECONNECT IN " + c--;
                        if(c < 0) { clearInterval(i); location.reload(); }
                    }, 1000);
                }).catch(e => { setTimeout(() => location.reload(), 5000); });
            }
        }

        // Throttle Control
        function setPowerMode(mode) {
            logToTerminal(`Engaging ${mode.toUpperCase()} power profile...`, "warn");
            fetch(`/api/throttle?mode=${mode}`)
                .then(res => res.json())
                .then(data => {
                    if (data.status === "success") {
                        logToTerminal(`Profile locked. Freq scaling applied.`, "success");
                    }
                });
        }

        function updateTelemetry() {
            fetch('/api')
                .then(res => res.json())
                .then(data => {
                    // Reset Status Badge
                    document.getElementById('mainStatus').innerHTML = `<div class="pulse"></div><span style="color:var(--success);">SYSTEM ONLINE</span>`;

                    // Main Stats
                    document.getElementById('hashrate').innerText = data.hashrate;
                    document.getElementById('hr0').innerText = data.hr0 + " kH/s";
                    document.getElementById('hr1').innerText = data.hr1 + " kH/s";
                    document.getElementById('node').innerText = data.node;
                    document.getElementById('shares').innerText = data.accepted + " / " + data.shares;
                    document.getElementById('memory').innerText = data.memory.toLocaleString();
                    document.getElementById('uptime').innerText = data.uptime;
                    document.getElementById('chip').innerText = data.chip;
                    document.getElementById('freq').innerText = data.freq;
                    
                    // Network Deep Dive
                    document.getElementById('bssid').innerText = data.bssid;
                    document.getElementById('mac').innerText = data.mac;
                    document.getElementById('channel').innerText = data.channel;

                    // Boot Boost UI Indicator
                    if (data.boot_boost === "true") {
                        document.getElementById('pmLabel').innerHTML = `Power Management <span style="color:var(--accent); font-size:0.65rem; margin-left:8px; animation: blink 1s infinite;">[BOOST ACTIVE]</span>`;
                    } else {
                        document.getElementById('pmLabel').innerHTML = `Power Management`;
                    }

                    // Power Buttons & Energy UI
                    let currentFreq = parseInt(data.freq);
                    let powerEst = ((currentFreq / 240) * 1.6 + 0.6).toFixed(2); 
                    document.getElementById('estPower').innerText = powerEst + " W";
                    
                    ['btnEco', 'btnBal', 'btnMax'].forEach(id => {
                        document.getElementById(id).style.background = "transparent";
                        document.getElementById(id).style.borderColor = "var(--border)";
                        document.getElementById(id).style.color = "var(--text-dim)";
                    });

                    if (currentFreq <= 80) {
                        document.getElementById('btnEco').style.background = "rgba(16, 185, 129, 0.2)";
                        document.getElementById('btnEco').style.borderColor = "var(--success)";
                        document.getElementById('btnEco').style.color = "var(--success)";
                    } else if (currentFreq <= 160) {
                        document.getElementById('btnBal').style.background = "rgba(6, 182, 212, 0.2)";
                        document.getElementById('btnBal').style.borderColor = "var(--secondary)";
                        document.getElementById('btnBal').style.color = "var(--secondary)";
                    } else {
                        document.getElementById('btnMax').style.background = "rgba(239, 68, 68, 0.2)";
                        document.getElementById('btnMax').style.borderColor = "var(--danger)";
                        document.getElementById('btnMax').style.color = "var(--danger)";
                    }

                    // Temperature Update
                    const tempElem = document.getElementById('temp');
                    const tempBar = document.getElementById('temp-bar');
                    const thermalStat = document.getElementById('thermalStatus');
                    
                    if (data.temp && data.temp > 0) {
                        tempElem.innerHTML = data.temp.toFixed(1) + " &deg;C";
                        let tempPct = Math.min(100, Math.max(0, ((data.temp - 30) / 50) * 100));
                        tempBar.style.width = tempPct + "%";
                        
                        if (data.temp < 50) {
                            tempElem.style.color = "var(--success)"; 
                            tempBar.style.background = "var(--success)";
                            thermalStat.innerText = "NOMINAL";
                            thermalStat.style.color = "var(--success)";
                        } else if (data.temp < 72) {
                            tempElem.style.color = "var(--accent)"; 
                            tempBar.style.background = "var(--accent)";
                            thermalStat.innerText = "ELEVATED";
                            thermalStat.style.color = "var(--accent)";
                        } else {
                            tempElem.style.color = "var(--danger)"; 
                            tempBar.style.background = "var(--danger)";
                            thermalStat.innerText = "CRITICAL";
                            thermalStat.style.color = "var(--danger)";
                        }
                    } else {
                        tempElem.innerText = "N/A";
                        tempBar.style.width = "0%";
                    }

                    // ----------------------------------------------------
                    // NEW: Performance Matrix (JS Derived)
                    // ----------------------------------------------------
                    hashrateHistory.push(data.hashrate);
                    if (hashrateHistory.length > 10) hashrateHistory.shift();
                    
                    if (hashrateHistory.length >= 3) {
                        let avgHr = hashrateHistory.reduce((a, b) => a + b, 0) / hashrateHistory.length;
                        let diffs = hashrateHistory.map(hr => Math.abs(hr - avgHr));
                        let avgDiff = diffs.reduce((a, b) => a + b, 0) / diffs.length;
                        let stability = 100 - ((avgDiff / avgHr) * 100);
                        
                        let stabElem = document.getElementById('hashStability');
                        stabElem.innerText = Math.max(0, Math.min(100, stability)).toFixed(1) + "%";
                        if (stability > 90) stabElem.style.color = "var(--success)";
                        else if (stability > 75) stabElem.style.color = "var(--accent)";
                        else stabElem.style.color = "var(--danger)";
                    }

                    let dropRate = data.shares > 0 ? ((data.shares - data.accepted) / data.shares * 100).toFixed(2) : 0;
                    document.getElementById('dropRate').innerText = dropRate + "%";

                    pingHistory.push(data.ping);
                    if (pingHistory.length > 10) pingHistory.shift();
                    if (pingHistory.length >= 2) {
                        let jitter = Math.abs(pingHistory[pingHistory.length - 1] - pingHistory[pingHistory.length - 2]);
                        document.getElementById('pingJitter').innerText = jitter + " ms";
                    }

                    // ----------------------------------------------------
                    // NEW: Compute Metrics (JS Derived)
                    // ----------------------------------------------------
                    let hr0 = parseFloat(data.hr0);
                    let hr1 = parseFloat(data.hr1);
                    const symElem = document.getElementById('coreSymmetry');
                    
                    if (hr1 > 0 && hr0 > 0) {
                        let symmetry = (Math.min(hr0, hr1) / Math.max(hr0, hr1)) * 100;
                        symElem.innerText = symmetry.toFixed(1) + "%";
                        if (symmetry > 90) symElem.style.color = "var(--success)";
                        else if (symmetry > 70) symElem.style.color = "var(--accent)";
                        else symElem.style.color = "var(--danger)";
                    } else {
                        symElem.innerText = "SINGLE THREAD";
                        symElem.style.color = "var(--text-dark)";
                    }

                    const memHealthElem = document.getElementById('memHealth');
                    if (data.memory > 100000) {
                        memHealthElem.innerText = "OPTIMAL";
                        memHealthElem.style.color = "var(--success)";
                    } else if (data.memory > 40000) {
                        memHealthElem.innerText = "FAIR";
                        memHealthElem.style.color = "var(--accent)";
                    } else {
                        memHealthElem.innerText = "CRITICAL";
                        memHealthElem.style.color = "var(--danger)";
                    }


                    // Validation Engine Data
                    let rejected = data.shares - data.accepted;
                    document.getElementById('rejected').innerText = rejected;
                    
                    let kolkaFactor = 100;
                    if (rejected > 0) kolkaFactor -= (rejected / data.shares) * 50;
                    if (data.ping > 500) kolkaFactor -= 5;
                    document.getElementById('kolka').innerText = Math.max(0, Math.floor(kolkaFactor)) + "%";

                    if (data.shares > 0) {
                        let totalHashesCalc = data.hashrate * 1000 * data.uptime_sec;
                        let ratio = Math.floor(totalHashesCalc / data.shares);
                        document.getElementById('hashRatio').innerText = ratio.toLocaleString();
                    }

                    // Velocity & Daily Yield Projection
                    if(data.accepted > 0) {
                        let avg = (data.uptime_sec / data.accepted).toFixed(1);
                        document.getElementById('avgTime').innerText = `Avg: ${avg}s / share`;
                    }
                    
                    const velElem = document.getElementById('shareVelocity');
                    const yieldElem = document.getElementById('dailyYield');
                    
                    if (data.uptime_sec > 60) {
                        let spm = (data.accepted / (data.uptime_sec / 60));
                        velElem.innerHTML = spm.toFixed(2) + ' <span class="card-unit" style="font-size:1rem;">s/m</span>';
                        velElem.style.fontSize = "1.5rem";
                        
                        let projectedDuco = (spm * 1440 * 0.000045).toFixed(2);
                        yieldElem.innerText = "~" + projectedDuco;
                    } else if (data.uptime_sec > 0) {
                        velElem.innerText = "Calibrating...";
                        velElem.style.fontSize = "1rem";
                        yieldElem.innerText = "--";
                    }

                    // Ping Colors & Bar
                    const pingElem = document.getElementById('ping');
                    const pingBar = document.getElementById('ping-bar');
                    pingElem.innerText = data.ping + " ms";
                    let pingPct = Math.max(0, 100 - (data.ping / 5)); 
                    pingBar.style.width = pingPct + "%";

                    if (data.ping < 150) { 
                        pingElem.style.color = "var(--success)"; pingBar.style.background = "var(--success)";
                        document.documentElement.style.setProperty('--threat-color', 'rgba(6, 182, 212, 0.1)'); 
                    } else if (data.ping < 400) { 
                        pingElem.style.color = "var(--accent)"; pingBar.style.background = "var(--accent)";
                        document.documentElement.style.setProperty('--threat-color', 'rgba(245, 158, 11, 0.15)'); 
                    } else { 
                        pingElem.style.color = "var(--danger)"; pingBar.style.background = "var(--danger)";
                        document.documentElement.style.setProperty('--threat-color', 'rgba(239, 68, 68, 0.25)'); 
                    }

                    // RSSI Colors
                    const rssiElement = document.getElementById('rssi');
                    rssiElement.innerText = data.rssi + " dBm";
                    if(data.rssi < -80) rssiElement.style.color = "var(--danger)";
                    else if(data.rssi < -65) rssiElement.style.color = "var(--accent)";
                    else rssiElement.style.color = "var(--success)";

                    // Efficiency
                    let eff = data.shares > 0 ? ((data.accepted / data.shares) * 100).toFixed(1) : 0.0;
                    const effElement = document.getElementById('efficiency');
                    const effBar = document.getElementById('eff-bar');
                    effElement.innerText = eff + "%";
                    effBar.style.width = eff + "%";
                    if(eff < 90 && data.shares > 10) { effElement.style.color = "var(--danger)"; effBar.style.background = "var(--danger)"; }
                    else { effElement.style.color = "var(--secondary)"; effBar.style.background = "var(--secondary)"; }

                    // Lifetime Hashes
                    let totalHashes = data.hashrate * 1000 * data.uptime_sec;
                    document.getElementById('lifetime').innerText = totalHashes.toLocaleString();

                    // Matrix Terminal Logs
                    if (data.accepted > lastAccepted && lastAccepted !== 0) {
                        logToTerminal(`[${data.user}] Block share accepted. Latency: ${data.ping}ms`, "success");
                    } else if (Math.random() > 0.85) { 
                        logToTerminal(`[Telemetry] Thread 0 & Thread 1 synced. Matrix stable.`);
                    }
                    lastAccepted = data.accepted;

                    // Chart Updates
                    const now = new Date();
                    const timeStr = now.getHours() + ':' + String(now.getMinutes()).padStart(2, '0') + ':' + String(now.getSeconds()).padStart(2, '0');
                    
                    hashChart.data.labels.push(timeStr);
                    hashChart.data.datasets[0].data.push(data.hashrate);
                    if (hashChart.data.labels.length > MAX_DATAPOINTS) {
                        hashChart.data.labels.shift();
                        hashChart.data.datasets[0].data.shift();
                    }
                    hashChart.update();

                    pingChart.data.labels.push(timeStr);
                    pingChart.data.datasets[0].data.push(data.ping);
                    if (pingChart.data.labels.length > MAX_PING_POINTS) {
                        pingChart.data.labels.shift();
                        pingChart.data.datasets[0].data.shift();
                    }
                    pingChart.update();
                })
                .catch(err => {
                    logToTerminal("Uplink failure. Awaiting telemetry...", "crit");
                    document.getElementById('mainStatus').innerHTML = `<div class="pulse" style="background:var(--danger);box-shadow: 0 0 10px var(--danger);"></div><span style="color:var(--danger);">SYSTEM OFFLINE</span>`;
                });
        }

        setInterval(updateTelemetry, 2500);
    </script>
</body>
</html>
)=====";

#endif