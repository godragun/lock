#ifndef HTML_H
#define HTML_H

// Login Page HTML (fixed and completed)
const char* LOGIN_HTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Smart Lock Control System</title>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <style>
        body { font-family: Arial, sans-serif; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); margin: 0; padding: 20px; min-height: 100vh; }
        .container { background: white; padding: 40px; border-radius: 15px; max-width: 450px; margin: 0 auto; box-shadow: 0 20px 40px rgba(0,0,0,0.1); }
        h1 { color: #333; text-align: center; margin-bottom: 30px; }
        .form-group { margin-bottom: 20px; }
        label { display: block; margin-bottom: 8px; color: #555; font-weight: bold; }
        input { padding: 12px; width: 100%; border: 2px solid #ddd; border-radius: 5px; font-size: 16px; box-sizing: border-box; }
        button { background: #667eea; color: white; padding: 15px; border: none; border-radius: 5px; width: 100%; font-size: 16px; cursor: pointer; }
        button:hover { background: #5a6fd8; }
        .info { background: #e8f4fd; padding: 15px; border-radius: 8px; margin-top: 20px; border-left: 4px solid #667eea; }
        .links { margin-top: 20px; text-align: center; }
        .links a { color: #667eea; text-decoration: none; margin: 0 10px; }
        .links a:hover { text-decoration: underline; }
    </style>
</head>
<body>
    <div class='container'>
        <h1>🔐 Smart Lock Control System</h1>
        <form method='GET' action='/login'>
            <div class='form-group'>
                <label>👤 Username</label>
                <input type='text' name='username' placeholder='Enter username' required>
            </div>
            <div class='form-group'>
                <label>🔑 Password</label>
                <input type='password' name='password' placeholder='Enter password' required>
            </div>
            <button type='submit'>🚀 Sign In</button>
        </form>
        <div class='info'>
            <strong>📋 Default Credentials:</strong><br>
            Username: thenuki<br>Password: 12345678
        </div>
        <div class='links'>
            <a href='/dashboard'>Direct Dashboard Access (if logged in)</a>
        </div>
    </div>
</body>
</html>
)rawliteral";

// Dashboard Page HTML (fixed and completed with placeholders)
const char* DASHBOARD_HTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Smart Lock Control Dashboard</title>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <style>
        body { font-family: 'Segoe UI', Arial, sans-serif; background: #f5f7fa; margin: 0; padding: 20px; }
        .header { background: white; padding: 20px; border-radius: 10px; margin-bottom: 20px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        .header h1 { color: #333; margin: 0; font-weight: 300; }
        .header p { color: #666; margin: 5px 0 0 0; }
        .logout-btn { float: right; background: #e74c3c; color: white; padding: 8px 16px; border: none; border-radius: 5px; cursor: pointer; }
        .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; }
        .card { background: white; padding: 25px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        .card h3 { color: #333; margin-top: 0; border-bottom: 2px solid #667eea; padding-bottom: 10px; }
        .status-card { text-align: center; }
        .lock-icon { font-size: 60px; margin: 20px 0; color: %LOCK_COLOR%; }
        .status-message { background: %BG_COLOR%; color: %TEXT_COLOR%; padding: 15px; border-radius: 8px; margin: 15px 0; border-left: 4px solid %BORDER_COLOR%; }
        .toggle-container { display: flex; justify-content: center; align-items: center; gap: 20px; margin: 20px 0; }
        .toggle-switch { position: relative; width: 60px; height: 30px; }
        .toggle-switch input { display: none; }
        .slider { position: absolute; top: 0; left: 0; right: 0; bottom: 0; background: %LOCK_COLOR%; border-radius: 30px; transition: 0.3s; }
        .slider:before { position: absolute; content: ''; height: 22px; width: 22px; left: 4px; bottom: 4px; background: white; border-radius: 50%; transition: 0.3s; }
        input:checked + .slider:before { transform: translateX(30px); }
        .form-group { margin: 15px 0; }
        .form-group label { display: block; margin-bottom: 5px; color: #555; font-weight: 500; }
        .form-group input { padding: 10px; width: 100%; border: 2px solid #e1e5e9; border-radius: 5px; font-size: 14px; }
        .form-group input:focus { outline: none; border-color: #667eea; }
        button { background: linear-gradient(135deg, #667eea, #764ba2); color: white; padding: 12px 20px; border: none; border-radius: 5px; cursor: pointer; margin: 5px; transition: transform 0.2s; }
        button:hover { transform: translateY(-2px); }
        .btn-success { background: linear-gradient(135deg, #27ae60, #2ecc71); }
        .btn-warning { background: linear-gradient(135deg, #f39c12, #e67e22); }
        .btn-danger { background: linear-gradient(135deg, #e74c3c, #c0392b); }
        .stats { display: grid; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); gap: 15px; margin: 20px 0; }
        .stat { background: linear-gradient(135deg, #667eea, #764ba2); color: white; padding: 20px; border-radius: 8px; text-align: center; }
        .stat-number { font-size: 24px; font-weight: bold; margin-bottom: 5px; }
        .stat-label { font-size: 12px; opacity: 0.9; }
        .logs { max-height: 200px; overflow-y: auto; background: #f8f9fa; padding: 15px; border-radius: 5px; margin-top: 15px; }
        .log-entry { background: white; padding: 8px; margin: 5px 0; border-radius: 3px; border-left: 3px solid #667eea; font-family: monospace; font-size: 12px; }
        .message { padding: 10px; border-radius: 5px; margin: 10px 0; display: none; }
        .success { background: #d4edda; color: #155724; border: 1px solid #c3e6cb; }
        .error { background: #f8d7da; color: #721c24; border: 1px solid #f5c6cb; }
        .info { background: #d1ecf1; color: #0c5460; border: 1px solid #bee5eb; }
        @media (max-width: 768px) { .grid { grid-template-columns: 1fr; } .toggle-container { flex-direction: column; } }
    </style>
</head>
<body>
    <div class='header'>
        <h1>🔐 Smart Lock Control Dashboard</h1>
        <p>Real-time monitoring and control system</p>
        <button class='logout-btn' onclick="window.location.href='/logout'">Logout</button>
    </div>
    
    <div class='grid'>
        <!-- Status Card -->
        <div class='card status-card'>
            <h3>Lock Status</h3>
            <div class='lock-icon'>%LOCK_STATUS%</div>
            <div class='status-message'>%STATUS_MESSAGE%</div>
            <div class='toggle-container'>
                <span>🔒 LOCKED</span>
                <label class='toggle-switch'>
                    <input type='checkbox' id='lockToggle' %CHECKED% onchange='toggleLock()'>
                    <span class='slider'></span>
                </label>
                <span>🔓 UNLOCKED</span>
            </div>
            <div class='stats'>
                <div class='stat'><div class='stat-number' id='uptime'>%UPTIME%</div><div class='stat-label'>UPTIME (MIN)</div></div>
                <div class='stat'><div class='stat-number' id='connections'>%CONNECTIONS%</div><div class='stat-label'>CONNECTIONS</div></div>
            </div>
        </div>
        
        <!-- Access Code Card -->
        <div class='card'>
            <h3>🔢 Access Code Management</h3>
            <div class='form-group'>
                <label>New 4-Digit Code</label>
                <input type='text' id='newCode' maxlength='4' placeholder='Enter new code'>
            </div>
            <button onclick='updateCode()'>Update Access Code</button>
            <div id='codeMessage' class='message'></div>
        </div>
        
        <!-- Security Card -->
        <div class='card'>
            <h3>🛡️ Security Settings</h3>
            <div class='form-group'>
                <label>New Admin Password</label>
                <input type='password' id='newPassword' placeholder='Enter new password'>
            </div>
            <button onclick='updatePassword()'>🔄 Update Password</button>
            <div id='passwordMessage' class='message'></div>
        </div>
        
        <!-- Fingerprint Card -->
        <div class='card'>
            <h3>👆 Fingerprint Enrollment</h3>
            <p>Add new fingerprints for biometric access. Follow serial monitor instructions during enrollment.</p>
            <button class='btn-success' onclick='enrollFingerprint()'>➕ Start Enrollment</button>
            <div id='fingerprintMessage' class='message'></div>
        </div>
        
        <!-- System Card -->
        <div class='card'>
            <h3>⚙️ System Information</h3>
            <p><strong>WiFi Network:</strong> %SSID%</p>
            <p><strong>IP Address:</strong> 192.168.4.1</p>
            <p><strong>Free Memory:</strong> %FREE_HEAP% bytes</p>
            <button class='btn-warning' onclick='restartSystem()'>🔄 Restart System</button>
        </div>
        
        <!-- Logs Card -->
        <div class='card'>
            <h3>📝 Access Logs</h3>
            <div class='logs' id='logsContainer'>Loading logs...</div>
            <button onclick='refreshLogs()'>🔄 Refresh Logs</button>
        </div>
    </div>
    
    <script>
        function toggleLock() {
            const isUnlocked = document.getElementById('lockToggle').checked;
            fetch('/toggle-lock?unlock=' + isUnlocked).then(() => updateStatus());
        }
        function updateStatus() {
            fetch('/status').then(r => r.json()).then(data => {
                document.querySelector('.status-message').textContent = data.message;
                document.querySelector('.lock-icon').textContent = data.locked ? '🔒 LOCKED' : '🔓 UNLOCKED';
                const color = data.locked ? '#e74c3c' : '#27ae60';
                document.querySelector('.lock-icon').style.color = color;
                document.querySelector('.slider').style.background = color;
                document.getElementById('lockToggle').checked = !data.locked;
                document.getElementById('uptime').textContent = Math.floor(data.uptime / 60);
            });
        }
        function updateCode() {
            const code = document.getElementById('newCode').value;
            if (code.length === 4 && !isNaN(code)) {
                fetch('/update-code?code=' + code).then(r => r.text()).then(data => {
                    showMessage('codeMessage', data, 'success');
                    document.getElementById('newCode').value = '';
                });
            } else {
                showMessage('codeMessage', 'Please enter a valid 4-digit code', 'error');
            }
        }
        function updatePassword() {
            const pass = document.getElementById('newPassword').value;
            if (pass.length >= 6) {
                fetch('/update-password?password=' + pass).then(r => r.text()).then(data => {
                    showMessage('passwordMessage', data, 'success');
                    document.getElementById('newPassword').value = '';
                });
            } else {
                showMessage('passwordMessage', 'Password must be at least 6 characters', 'error');
            }
        }
        function enrollFingerprint() {
            fetch('/enroll-fingerprint').then(r => r.text()).then(data => {
                showMessage('fingerprintMessage', data, 'info');
            });
        }
        function refreshLogs() {
            fetch('/get-logs').then(r => r.json()).then(data => {
                const container = document.getElementById('logsContainer');
                container.innerHTML = data.logs.map(log => '<div class="log-entry">' + log + '</div>').join('');
                if (data.logs.length === 0) container.innerHTML = '<div class="log-entry">No logs available</div>';
            });
        }
        function restartSystem() {
            if (confirm('Are you sure you want to restart the system?')) {
                fetch('/restart').then(() => showMessage('fingerprintMessage', 'System restarting...', 'info'));
            }
        }
        function showMessage(id, message, type) {
            const el = document.getElementById(id);
            el.textContent = message;
            el.className = 'message ' + type;
            el.style.display = 'block';
            setTimeout(() => el.style.display = 'none', 5000);
        }
        setInterval(updateStatus, 3000);
        document.getElementById('newCode').addEventListener('input', function(e) {
            this.value = this.value.replace(/[^0-9]/g, '').slice(0, 4);
        });
        document.addEventListener('DOMContentLoaded', function() {
            updateStatus(); refreshLogs();
        });
    </script>
</body>
</html>
)rawliteral";

#endif // HTML_H