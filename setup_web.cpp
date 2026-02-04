#include "setup_web.h"
#include "config_manager.h"
#include <ESP8266WiFi.h>
#include <LittleFS.h>

ESP8266WebServer server(80);

const char SETUP_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>🔧 Настройка ESP8266</title>
    <style>
        body{font-family:sans-serif;padding:15px;background:#f0f2f5;margin:0}
        .container{max-width:600px;margin:0 auto;background:white;border-radius:12px;box-shadow:0 2px 10px rgba(0,0,0,0.1);overflow:hidden}
        .header{background:linear-gradient(135deg, #1a73e8, #0d47a1);color:white;padding:25px;text-align:center}
        .header h1{margin:0;font-size:24px;font-weight:600}
        .status-panel{padding:20px;border-bottom:1px solid #e0e0e0}
        .status-row{display:flex;justify-content:space-between;padding:8px 0;font-size:14px}
        .status-label{color:#5f6368;font-weight:500}
        .status-value{color:#202124;font-weight:600}
        .networks-panel{padding:20px;border-bottom:1px solid #e0e0e0}
        .panel-title{font-size:18px;font-weight:600;color:#1a73e8;margin-bottom:15px;padding-bottom:10px;border-bottom:2px solid #e8f4fd}
        .network-item{background:#f8f9fa;padding:12px;border-radius:8px;margin-bottom:10px;border:1px solid #e0e0e0;cursor:pointer}
        .network-item:hover{background:#e8f4fd;border-color:#1a73e8}
        .network-name{font-weight:600;color:#202124;font-size:16px}
        .network-rssi{color:#5f6368;font-size:13px;margin-top:4px}
        .network-security{display:inline-block;background:#e8f4fd;color:#1a73e8;padding:2px 6px;border-radius:4px;font-size:11px;margin-left:8px}
        .form-panel{padding:20px}
        .form-group{margin-bottom:15px}
        label{display:block;margin-bottom:6px;font-weight:600;color:#202124;font-size:14px}
        input{width:100%;padding:12px;border:1px solid #dadce0;border-radius:8px;font-size:14px;box-sizing:border-box}
        input:focus{outline:none;border-color:#1a73e8;box-shadow:0 0 0 2px rgba(26, 115, 232, 0.2)}
        button{width:100%;padding:14px;background:linear-gradient(135deg, #1a73e8, #0d47a1);color:white;border:none;border-radius:8px;font-size:16px;font-weight:600;cursor:pointer}
        button:hover{background:linear-gradient(135deg, #0d47a1, #0a3a8c)}
        button:disabled{background:#dadce0;cursor:not-allowed}
        .loading{text-align:center;color:#5f6368;padding:20px;font-style:italic}
        .success{background:#d4edda;color:#155724;padding:15px;border-radius:8px;text-align:center;font-weight:600;margin:20px 0}
        .error{background:#f8d7da;color:#721c24;padding:15px;border-radius:8px;text-align:center;font-weight:600;margin:20px 0}
        .instructions{background:#e8f4fd;padding:15px;border-radius:8px;margin:20px 0;font-size:13px;color:#5f6368}
        .instructions h3{margin:0 0 10px 0;color:#1a73e8;font-size:15px}
        .instructions ol{margin:0;padding-left:20px}
        .instructions li{margin-bottom:5px}
    </style>
</head>
<body>
    <div class='container'>
        <div class='header'>
            <h1>🔧 Настройка ESP8266</h1>
        </div>
        
        <div class='status-panel'>
            <div class='panel-title'>📊 Состояние устройства</div>
            <div class='status-row'>
                <span class='status-label'>Свободная RAM:</span>
                <span class='status-value' id='freeHeap'>-- KB</span>
            </div>
            <div class='status-row'>
                <span class='status-label'>Время работы:</span>
                <span class='status-value' id='uptime'>-- сек</span>
            </div>
            <div class='status-row'>
                <span class='status-label'>IP адрес (AP):</span>
                <span class='status-value' id='ipAddressAP'>192.168.4.1</span>
            </div>
            <div class='status-row'>
                <span class='status-label'>IP адрес (STA):</span>
                <span class='status-value' id='ipAddressSTA'>--.--.--.--</span>
            </div>
        </div>
        
        <div class='networks-panel'>
            <div class='panel-title'>📶 Доступные сети</div>
            <div id='scanStatus' class='loading'>Сканирование сетей...</div>
            <div id='networksList'></div>
            <button onclick='scanNetworks()' id='scanBtn' style='margin-top:15px;'>🔄 Обновить список</button>
        </div>
        
        <div class='form-panel'>
            <div class='panel-title'>⚙️ Подключение к сети</div>
            
            <div class='instructions'>
                <h3>📋 Инструкция:</h3>
                <ol>
                    <li>Выберите сеть из списка выше ИЛИ введите вручную</li>
                    <li>Введите пароль от сети</li>
                    <li>Нажмите "Сохранить и перезагрузить"</li>
                    <li>ESP8266 подключится к вашей сети</li>
                </ol>
            </div>
            
            <form id='wifiForm'>
                <div class='form-group'>
                    <label for='ssid'>Имя сети (SSID):</label>
                    <input type='text' id='ssid' name='ssid' placeholder='Введите SSID или выберите из списка' required>
                </div>
                <div class='form-group'>
                    <label for='password'>Пароль:</label>
                    <input type='password' id='password' name='password' placeholder='Введите пароль' required>
                </div>
                <button type='submit' id='saveBtn'>💾 Сохранить и перезагрузить</button>
            </form>
            
            <div id='messageArea'></div>
        </div>
    </div>

    <script>
        function formatUptime(seconds) {
            if (seconds < 60) return seconds + ' сек';
            if (seconds < 3600) return Math.floor(seconds / 60) + ' мин';
            const hours = Math.floor(seconds / 3600);
            const minutes = Math.floor((seconds % 3600) / 60);
            return hours + ' ч ' + minutes + ' мин';
        }

        function updateStatus() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('freeHeap').textContent = (data.freeHeap / 1024).toFixed(1) + ' KB';
                    document.getElementById('uptime').textContent = formatUptime(data.uptime);
                    document.getElementById('ipAddressAP').textContent = data.ipAddressAP;
                    document.getElementById('ipAddressSTA').textContent = data.ipAddressSTA || '--.--.--.--';
                })
                .catch(console.error);
        }

        function scanNetworks() {
            const scanBtn = document.getElementById('scanBtn');
            const scanStatus = document.getElementById('scanStatus');
            const networksList = document.getElementById('networksList');
            
            scanBtn.disabled = true;
            scanStatus.textContent = 'Сканирование...';
            networksList.innerHTML = '';
            
            fetch('/api/scan')
                .then(response => response.json())
                .then(data => {
                    if (data.networks && data.networks.length > 0) {
                        scanStatus.style.display = 'none';
                        data.networks.forEach(network => {
                            const networkDiv = document.createElement('div');
                            networkDiv.className = 'network-item';
                            networkDiv.onclick = () => selectNetwork(network.ssid);
                            
                            let security = '';
                            if (network.encryption !== 0) {
                                security = '<span class="network-security">🔒 Защищена</span>';
                            }
                            
                            networkDiv.innerHTML = `
                                <div class="network-name">${network.ssid}${security}</div>
                                <div class="network-rssi">Сигнал: ${network.rssi} dBm</div>
                            `;
                            networksList.appendChild(networkDiv);
                        });
                    } else {
                        scanStatus.textContent = 'Сети не найдены';
                        scanStatus.style.display = 'block';
                    }
                })
                .catch(error => {
                    scanStatus.textContent = 'Ошибка сканирования';
                    scanStatus.style.display = 'block';
                    console.error('Ошибка:', error);
                })
                .finally(() => {
                    scanBtn.disabled = false;
                });
        }

        function selectNetwork(ssid) {
            document.getElementById('ssid').value = ssid;
            document.getElementById('password').focus();
        }

        function showMessage(text, isError = false) {
            const messageArea = document.getElementById('messageArea');
            messageArea.innerHTML = `
                <div class="${isError ? 'error' : 'success'}">${text}</div>
            `;
            setTimeout(() => {
                messageArea.innerHTML = '';
            }, 5000);
        }

        document.getElementById('wifiForm').addEventListener('submit', function(e) {
            e.preventDefault();
            
            const ssid = document.getElementById('ssid').value;
            const password = document.getElementById('password').value;
            const saveBtn = document.getElementById('saveBtn');
            
            if (!ssid.trim()) {
                showMessage('Введите имя сети', true);
                return;
            }
            
            saveBtn.disabled = true;
            saveBtn.textContent = 'Сохранение...';
            
            fetch('/api/save-wifi', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    ssid: ssid,
                    password: password
                })
            })
            .then(response => response.json())
            .then(data => {
                if (data.success) {
                    showMessage('✅ Настройки сохранены! Устройство перезагрузится через 3 секунды...');
                    setTimeout(() => {
                        window.location.href = '/';
                    }, 3000);
                } else {
                    showMessage('❌ Ошибка: ' + (data.message || 'Не удалось сохранить'), true);
                    saveBtn.disabled = false;
                    saveBtn.textContent = '💾 Сохранить и перезагрузить';
                }
            })
            .catch(error => {
                showMessage('❌ Ошибка сохранения настроек', true);
                console.error('Ошибка:', error);
                saveBtn.disabled = false;
                saveBtn.textContent = '💾 Сохранить и перезагрузить';
            });
        });

        document.addEventListener('DOMContentLoaded', function() {
            updateStatus();
            scanNetworks();
            setInterval(updateStatus, 5000);
        });
    </script>
</body>
</html>
)rawliteral";

void handleApiStatus() {
  String json = "{";
  json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"uptime\":" + String(millis() / 1000) + ",";
  json += "\"ipAddressAP\":\"" + WiFi.softAPIP().toString() + "\",";
  json += "\"ipAddressSTA\":\"" + WiFi.localIP().toString() + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

void handleScanNetworks() {
  int n = WiFi.scanNetworks();
  
  String json = "{\"networks\":[";
  
  for (int i = 0; i < n; i++) {
    if (i > 0) json += ",";
    json += "{";
    json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
    json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
    json += "\"encryption\":" + String(WiFi.encryptionType(i));
    json += "}";
  }
  
  json += "]}";
  
  server.send(200, "application/json", json);
}

void handleSaveWiFi() {
  // Получаем сырое тело запроса (JSON)
  String requestBody = server.arg("plain");
  
  if (requestBody.length() > 0) {
    // Парсим JSON вручную (простой способ без библиотек)
    int ssidStart = requestBody.indexOf("\"ssid\":\"") + 9;
    int ssidEnd = requestBody.indexOf("\"", ssidStart);
    int passStart = requestBody.indexOf("\"password\":\"") + 13;
    int passEnd = requestBody.indexOf("\"", passStart);
    
    if (ssidStart > 9 && ssidEnd > ssidStart && 
        passStart > 13 && passEnd > passStart) {
      
      String ssid = requestBody.substring(ssidStart, ssidEnd);
      String password = requestBody.substring(passStart, passEnd);
      
      ssid.toCharArray(config.wifiSSID, 32);
      password.toCharArray(config.wifiPassword, 64);
      config.firstSetupDone = true;
      config.maxClients = 1;
      config.updateInterval = 5;
      saveConfig();
      
      server.send(200, "application/json", "{\"success\":true}");
      return;
    }
  }
  
  // Если не получилось распарсить
  server.send(200, "application/json", "{\"success\":false,\"message\":\"Недостаточно параметров\"}");
}

// Добавь в существующий файл после других обработчиков
void handleApiSensors() {
  String json = getSensorDataJSON();
  server.send(200, "application/json", json);
}


void handleSetupPage() {
  server.send_P(200, "text/html", SETUP_PAGE);
}

void initSetupWebServer() {
  // ВОТ ЭТО ТЫ ХОТЕЛ — AP+STA РЕЖИМ, СУКА!
  WiFi.mode(WIFI_AP_STA);
  
  // Создаём свою точку доступа для управления
  WiFi.softAP("ESP8266_Setup", "12345678");
  
  // Если есть сохранённые настройки — подключаемся к роутеру
  if (strlen(config.wifiSSID) > 0) {
    WiFi.begin(config.wifiSSID, config.wifiPassword);
  }
  
  server.on("/", handleSetupPage);
  server.on("/api/status", handleApiStatus);
  server.on("/api/scan", handleScanNetworks);
  server.on("/api/save-wifi", HTTP_POST, handleSaveWiFi);
  // Добавь этот маршрут
  server.on("/api/sensors", handleApiSensors);
  
  server.begin();
  
  Serial.println("🌐 AP+STA режим запущен");
  Serial.println("AP: ESP8266_Setup / 12345678");
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
  if (strlen(config.wifiSSID) > 0) {
    Serial.print("Подключение к: ");
    Serial.println(config.wifiSSID);
  }
}

void handleSetupWebRequests() {
  server.handleClient();
}