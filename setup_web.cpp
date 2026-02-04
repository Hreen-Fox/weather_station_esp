#include "sensor_generator.h"  // ДОБАВЬ ЭТУ СТРОКУ
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
        <span class='status-label'>Версия прошивки:</span>
        <span class='status-value'>1.0.0</span>
    </div>
</div>

<div class='status-panel'>
    <div class='panel-title'>📶 Статус подключения</div>
    <div class='status-row'>
        <span class='status-label'>Статус:</span>
        <span class='status-value' id='wifiStatus'>Не подключено</span>
    </div>
    <div class='status-row'>
        <span class='status-label'>Сеть (SSID):</span>
        <span class='status-value' id='wifiSSID'>--</span>
    </div>
    <div class='status-row'>
        <span class='status-label'>IP адрес (STA):</span>
        <span class='status-value' id='ipAddressSTA'>--.--.--.--</span>
    </div>
    <div class='status-row'>
        <span class='status-label'>MAC адрес (STA):</span>
        <span class='status-value' id='macAddressSTA'>--:--:--:--:--:--</span>
    </div>
    <div class='status-row'>
        <span class='status-label'>Уровень сигнала:</span>
        <span class='status-value' id='wifiRSSI'>-- dBm</span>
    </div>
    <div class='status-row'>
        <span class='status-label'>Канал:</span>
        <span class='status-value' id='wifiChannel'>--</span>
    </div>
    <div class='status-row'>
        <span class='status-label'>IP адрес (AP):</span>
        <span class='status-value' id='ipAddressAP'>192.168.4.1</span>
    </div>
    <div class='status-row'>
        <span class='status-label'>MAC адрес (AP):</span>
        <span class='status-value' id='macAddressAP'>--:--:--:--:--:--</span>
    </div>
</div>

<div class='status-panel'>
    <div class='panel-title'>💾 Сохраненные настройки</div>
    <div class='status-row'>
        <span class='status-label'>Сеть (SSID):</span>
        <span class='status-value' id='savedSSID'>--</span>
    </div>
    <div class='status-row'>
        <span class='status-label'>Интервал обновления:</span>
        <span class='status-value' id='savedInterval'>-- сек</span>
    </div>
    <div class='status-row'>
        <span class='status-label'>Макс. клиентов:</span>
        <span class='status-value' id='savedClients'>--</span>
    </div>
    <div class='status-row'>
        <span class='status-label'>Первоначальная настройка:</span>
        <span class='status-value' id='savedSetup'>--</span>
    </div>
    <div style='text-align:center; margin-top:15px;'>
        <a href='/config.dat' style='color:#1a73e8; text-decoration:underline; font-size:14px;'>
            📄 Скачать файл конфигурации
        </a>
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
                fetch('/api/config')
    .then(response => response.json())
    .then(data => {
        document.getElementById('savedSSID').textContent = data.wifiSSID || '--';
        document.getElementById('savedInterval').textContent = (data.updateInterval || '--') + ' сек';
        document.getElementById('savedClients').textContent = data.maxClients || '--';
        document.getElementById('savedSetup').textContent = data.firstSetupDone ? 'Выполнена' : 'Не выполнена';
    })
    .catch(() => {
        // Если API недоступно, оставляем прочерки
    });
    fetch('/api/status')
        .then(response => response.json())
        .then(data => {
            // Системная информация
            document.getElementById('freeHeap').textContent = (data.freeHeap / 1024).toFixed(1) + ' KB';
            document.getElementById('uptime').textContent = formatUptime(data.uptime);
            document.getElementById('ipAddressAP').textContent = data.ipAddressAP;
            
            // Статус подключения
            const statusElement = document.getElementById('wifiStatus');
            const ssidElement = document.getElementById('wifiSSID');
            const ipSTAElement = document.getElementById('ipAddressSTA');
            const macSTAElement = document.getElementById('macAddressSTA');
            const rssiElement = document.getElementById('wifiRSSI');
            const channelElement = document.getElementById('wifiChannel');
            const macAPElement = document.getElementById('macAddressAP');
            
            if (data.wifiConnected) {
                statusElement.textContent = 'Подключено';
                statusElement.style.color = '#34a853';
                ssidElement.textContent = data.ssid;
                ipSTAElement.textContent = data.ipAddressSTA;
                macSTAElement.textContent = data.macAddressSTA;
                rssiElement.textContent = data.rssi + ' dBm';
                channelElement.textContent = data.channel;
            } else {
                statusElement.textContent = 'Не подключено';
                statusElement.style.color = '#ea4335';
                ssidElement.textContent = '--';
                ipSTAElement.textContent = '--.--.--.--';
                macSTAElement.textContent = '--:--:--:--:--:--';
                rssiElement.textContent = '-- dBm';
                channelElement.textContent = '--';
            }
            
            macAPElement.textContent = data.macAddressAP;
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

/**
 * Отправка файла конфигурации для скачивания
 */
void handleConfigFile() {
  if (!LittleFS.begin()) {
    server.send(500, "text/plain", "LittleFS error");
    return;
  }
  
  if (LittleFS.exists("/config.dat")) {
    File file = LittleFS.open("/config.dat", "r");
    if (file) {
      server.streamFile(file, "application/octet-stream");
      file.close();
    } else {
      server.send(404, "text/plain", "File not found");
    }
  } else {
    server.send(404, "text/plain", "Config file not exists");
  }
}

/**
 * Получение расширенной системной информации и статуса подключения
 */
void handleApiStatus() {
  String json = "{";
  
  // Системная информация
  json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"uptime\":" + String(millis() / 1000) + ",";
  json += "\"ipAddressAP\":\"" + WiFi.softAPIP().toString() + "\",";
  
  // Информация о подключении к Wi-Fi
  if (WiFi.status() == WL_CONNECTED) {
    json += "\"wifiConnected\":true,";
    json += "\"ssid\":\"" + String(WiFi.SSID()) + "\",";
    json += "\"ipAddressSTA\":\"" + WiFi.localIP().toString() + "\",";
    json += "\"macAddressSTA\":\"" + WiFi.macAddress() + "\",";
    json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
    json += "\"channel\":" + String(WiFi.channel());
  } else {
    json += "\"wifiConnected\":false,";
    json += "\"ssid\":\"\",";
    json += "\"ipAddressSTA\":\"--.--.--.--\",";
    json += "\"macAddressSTA\":\"--:--:--:--:--:--\",";
    json += "\"rssi\":0,";
    json += "\"channel\":0";
  }
  
  // MAC-адрес AP режима
  json += ",\"macAddressAP\":\"" + WiFi.softAPmacAddress() + "\"";
  
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
  String requestBody = server.arg("plain");
  
  if (requestBody.length() > 0) {
    // Правильный парсинг JSON
    int ssidStart = requestBody.indexOf("\"ssid\":\"");
    int ssidEnd = requestBody.indexOf("\"", ssidStart + 9);
    int passStart = requestBody.indexOf("\"password\":\"");
    int passEnd = requestBody.indexOf("\"", passStart + 13);
    
    if (ssidStart != -1 && ssidEnd > ssidStart + 8 && 
        passStart != -1 && passEnd > passStart + 12) {
      
      // Извлекаем SSID (начинаем после "ssid":"")
      String ssid = requestBody.substring(ssidStart + 8, ssidEnd);
      String password = requestBody.substring(passStart + 12, passEnd);
      
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
/**
 * Получение сохраненных настроек системы
 */
void handleApiConfig() {
  String json = "{";
  json += "\"wifiSSID\":\"" + String(config.wifiSSID) + "\",";
  json += "\"maxClients\":" + String(config.maxClients) + ",";
  json += "\"updateInterval\":" + String(config.updateInterval) + ",";
  json += "\"firstSetupDone\":" + String(config.firstSetupDone ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}
void initSetupWebServer() {
  Serial.println("=== НАЧАЛО ДИАГНОСТИКИ ===");
  
  loadConfig();
  
  if (strlen(config.wifiSSID) > 0) {
    Serial.print("SSID: '");
    Serial.print(config.wifiSSID);
    Serial.println("'");
    
    Serial.print("Длина пароля: ");
    Serial.println(strlen(config.wifiPassword));
    
    // Выводим первые и последние символы пароля (без полного вывода)
    if (strlen(config.wifiPassword) > 0) {
      Serial.print("Первый символ пароля: '");
      Serial.print(config.wifiPassword[0]);
      Serial.println("'");
      
      if (strlen(config.wifiPassword) > 1) {
        Serial.print("Последний символ пароля: '");
        Serial.print(config.wifiPassword[strlen(config.wifiPassword)-1]);
        Serial.println("'");
      }
    }
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(config.wifiSSID, config.wifiPassword);
    
    Serial.println("Ожидание подключения (макс. 30 сек)...");
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
      Serial.print(".");
      delay(1000);
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n✅ УСПЕШНО ПОДКЛЮЧЕНО!");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("\n❌ ОШИБКА ПОДКЛЮЧЕНИЯ");
      Serial.print("Код ошибки: ");
      Serial.println(WiFi.status());
      
      // Сканируем сети для проверки наличия нужной
      Serial.println("Доступные сети:");
      int n = WiFi.scanNetworks();
      bool found = false;
      for (int i = 0; i < n; i++) {
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        if (WiFi.SSID(i) == config.wifiSSID) {
          Serial.print(" ← НАША СЕТЬ!");
          found = true;
        }
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.println(" dBm)");
      }
      
      if (!found) {
        Serial.println("⚠️ НАША СЕТЬ НЕ НАЙДЕНА В СПИСКЕ!");
      }
    }
  } else {
    Serial.println("❌ НЕТ СОХРАНЕННЫХ НАСТРОЕК");
    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP8266_Setup", "12345678");
    Serial.print("Точка доступа: ");
    Serial.println(WiFi.softAPIP());
  }
  
  Serial.println("=== КОНЕЦ ДИАГНОСТИКИ ===");
  
  server.on("/", handleSetupPage);
  server.on("/api/status", handleApiStatus);
  server.on("/api/scan", handleScanNetworks);
  server.on("/api/save-wifi", HTTP_POST, handleSaveWiFi);
  server.on("/api/sensors", handleApiSensors);
  server.on("/api/config", handleApiConfig);
  server.on("/config.dat", handleConfigFile);
  server.begin();
}

void handleSetupWebRequests() {
  server.handleClient();
}