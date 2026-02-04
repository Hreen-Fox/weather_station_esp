#ifndef WEB_PAGES_H
#define WEB_PAGES_H

// Главная страница - Статус
const char STATUS_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>🏠 ESP8266 - Статус</title>
    <style>
        body{font-family:sans-serif;padding:7px;margin:7px;background:#f5f5f5}
        .container{max-width:650px;background:white;padding:14px;border-radius:10px;display:flex;flex-direction:column;gap:14px}
        .panel{background:#e8f4fd;padding:16px;border-radius:10px;display:flex;flex-direction:column;gap:16px}
        .btn-nav{text-align:center;display:flex;flex-wrap:wrap;gap:7px;justify-content:center}
        .btn-nav a{display:inline-block;padding:8px 16px;background:#1a73e8;color:white;text-decoration:none;border-radius:5px}
        h2{color:#1a73e8;text-align:center;margin:0;font-size:1.4rem}
        .panel-title{margin:0;padding-bottom:10px;border-bottom:1px solid #d0e8fc;font-weight:bold}
        .data-row{display:flex;justify-content:space-between;padding:12px 0;border-bottom:1px solid #d8ecfb}
        .data-row:last-child{border-bottom:none}
        .data-label{font-weight:bold;color:#5f6368}
        .data-value{color:#202124;font-weight:bold}
        .sensor-item{padding:10px;background:white;border-radius:5px;border:1px solid #d0e8fc}
        .sensor-name{font-weight:bold;color:#1a73e8}
        .sensor-value{font-size:1.2rem;font-weight:bold;color:#202124}
        .status-indicator{display:inline-block;width:10px;height:10px;border-radius:50%;margin-right:8px}
        .status-online{background:#34a853}
        .status-offline{background:#ea4335}
        .last-update{text-align:center;color:#666;padding:10px 0}
    </style>
</head>
<body>
    <div class='container'>
        <div class="btn-nav">
            <a href="/">Статус</a>
            <a href="/attributes">Характеристики</a>
            <a href="/sensors">Датчики</a>
            <a href="/settings">Настройки</a>
        </div>
        
        <h2>🏠 Статус устройства</h2>
        
        <div class='panel'>
            <h3 class="panel-title">Системная информация</h3>
            <div class='data-row'><span class='data-label'>Оперативная память:</span><span class='data-value' id="ram">-- KB / -- KB</span></div>
            <div class='data-row'><span class='data-label'>Флеш-память (FS):</span><span class='data-value' id="flash">-- KB / -- KB</span></div>
            <div class='data-row'><span class='data-label'>Время работы:</span><span class='data-value' id="uptime">-- ч -- мин</span></div>
            <div class='data-row'><span class='data-label'>Интервал обновления:</span><span class='data-value' id="interval">-- сек</span></div>
        </div>
        
        <div class='panel'>
            <h3 class="panel-title">Wi-Fi статус</h3>
            <div class='data-row'><span class='data-label'>Режим работы:</span><span class='data-value' id="wifiMode">--</span></div>
            <div class='data-row'><span class='data-label'>Подключение:</span><span class='data-value'><span class="status-indicator" id="wifiStatus"></span><span id="wifiStatusText">--</span></span></div>
            <div class='data-row'><span class='data-label'>IP адрес:</span><span class='data-value' id="ipAddress">--</span></div>
        </div>
        
        <div class='panel'>
            <h3 class="panel-title">Подключенные пользователи</h3>
            <div class='data-row'><span class='data-label'>Количество:</span><span class='data-value' id="clients">-- из --</span></div>
            <div id="macList"></div>
        </div>
        
        <div class='panel'>
            <h3 class="panel-title">Данные датчиков</h3>
            <div id="sensorsContainer"></div>
        </div>
        
        <div class="last-update" id="lastUpdate">Последнее обновление: --</div>
    </div>
    
    <script>
        function formatUptime(seconds) {
            const hours = Math.floor(seconds / 3600);
            const minutes = Math.floor((seconds % 3600) / 60);
            return `${hours} ч ${minutes} мин`;
        }
        
        function updateStatus() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    // Системная информация
                    document.getElementById('ram').textContent = `${(data.freeHeap/1024).toFixed(1)} KB / ${(data.totalHeap/1024).toFixed(1)} KB`;
                    document.getElementById('flash').textContent = `${data.fsUsed} KB / ${data.fsTotal} KB`;
                    document.getElementById('uptime').textContent = formatUptime(data.uptime);
                    document.getElementById('interval').textContent = `${data.updateInterval} сек`;
                    
                    // Wi-Fi статус
                    document.getElementById('wifiMode').textContent = data.wifiMode;
                    const wifiOnline = data.wifiConnected;
                    const statusIndicator = document.getElementById('wifiStatus');
                    const statusText = document.getElementById('wifiStatusText');
                    if (wifiOnline) {
                        statusIndicator.className = 'status-indicator status-online';
                        statusText.textContent = data.ssid || 'Подключено';
                    } else {
                        statusIndicator.className = 'status-indicator status-offline';
                        statusText.textContent = 'Не подключено';
                    }
                    document.getElementById('ipAddress').textContent = data.ipAddress;
                    
                    // Подключенные пользователи
                    document.getElementById('clients').textContent = `${data.connectedClients} из ${data.maxClients}`;
                    const macList = document.getElementById('macList');
                    macList.innerHTML = '';
                    if (data.macAddresses && data.macAddresses.length > 0) {
                        data.macAddresses.forEach(mac => {
                            const div = document.createElement('div');
                            div.className = 'data-row';
                            div.innerHTML = `<span class='data-label'>MAC:</span><span class='data-value'>${mac}</span>`;
                            macList.appendChild(div);
                        });
                    }
                    
                    // Датчики
                    const sensorsContainer = document.getElementById('sensorsContainer');
                    sensorsContainer.innerHTML = '';
                    if (data.sensors) {
                        Object.keys(data.sensors).forEach(name => {
                            const value = data.sensors[name];
                            const sensorDiv = document.createElement('div');
                            sensorDiv.className = 'sensor-item';
                            sensorDiv.innerHTML = `
                                <div class="sensor-name">${name}</div>
                                <div class="sensor-value">${value}</div>
                            `;
                            sensorsContainer.appendChild(sensorDiv);
                        });
                    }
                    
                    // Время обновления
                    const now = new Date();
                    document.getElementById('lastUpdate').textContent = `Последнее обновление: ${now.toLocaleTimeString()}`;
                })
                .catch(error => console.error('Ошибка:', error));
        }
        
        setInterval(updateStatus, 2000);
        updateStatus();
    </script>
</body>
</html>
)rawliteral";

// Страница характеристик
const char ATTRIBUTES_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>📊 ESP8266 - Характеристики</title>
    <style>
        body{font-family:sans-serif;padding:7px;margin:7px;background:#f5f5f5}
        .container{max-width:650px;background:white;padding:14px;border-radius:10px;display:flex;flex-direction:column;gap:14px}
        .panel{background:#e8f4fd;padding:16px;border-radius:10px;display:flex;flex-direction:column;gap:16px}
        .btn-nav{text-align:center;display:flex;flex-wrap:wrap;gap:7px;justify-content:center}
        .btn-nav a{display:inline-block;padding:8px 16px;background:#1a73e8;color:white;text-decoration:none;border-radius:5px}
        h2{color:#1a73e8;text-align:center;margin:0;font-size:1.4rem}
        .panel-title{margin:0;padding-bottom:10px;border-bottom:1px solid #d0e8fc;font-weight:bold}
        .data-row{display:flex;justify-content:space-between;padding:8px 0}
        .data-label{font-weight:bold;color:#5f6368;min-width:140px;padding-right:12px;text-align:right}
        .data-value{color:#202124}
    </style>
</head>
<body>
    <div class='container'>
        <div class="btn-nav">
            <a href="/">Статус</a>
            <a href="/attributes">Характеристики</a>
            <a href="/sensors">Датчики</a>
            <a href="/settings">Настройки</a>
        </div>
        
        <h2>📊 Характеристики устройства</h2>
        
        <div class='panel'>
            <h3 class="panel-title">🔧 Аппаратные характеристики ESP8266</h3>
            <div class='data-row'><span class='data-label'>Микроконтроллер:</span><span class='data-value'>ESP8266EX (Tensilica L106 32-bit RISC)</span></div>
            <div class='data-row'><span class='data-label'>Тактовая частота:</span><span class='data-value'>80 МГц (макс. 160 МГц)</span></div>
            <div class='data-row'><span class='data-label'>Оперативная память:</span><span class='data-value'>80 КБ пользовательской + 32 КБ инструкций</span></div>
            <div class='data-row'><span class='data-label'>Флеш-память:</span><span class='data-value'>4 МБ (встроенная на модуле)</span></div>
            <div class='data-row'><span class='data-label'>Рабочее напряжение:</span><span class='data-value'>3.3 В</span></div>
            <div class='data-row'><span class='data-label'>Потребление тока:</span><span class='data-value'>До 215 мА (при передаче Wi-Fi)</span></div>
            <div class='data-row'><span class='data-label'>Размеры:</span><span class='data-value'>24 × 16 мм</span></div>
            <div class='data-row'><span class='data-label'>Рабочая температура:</span><span class='data-value'>-40°C до +125°C</span></div>
        </div>
        
        <div class='panel'>
            <h3 class="panel-title">📶 Wi-Fi и сетевые возможности</h3>
            <div class='data-row'><span class='data-label'>Стандарт:</span><span class='data-value'>IEEE 802.11 b/g/n</span></div>
            <div class='data-row'><span class='data-label'>Частота:</span><span class='data-value'>2.4 ГГц</span></div>
            <div class='data-row'><span class='data-label'>Режимы работы:</span><span class='data-value'>Station (STA), Access Point (AP), STA+AP</span></div>
            <div class='data-row'><span class='data-label'>Макс. скорость:</span><span class='data-value'>72.2 Мбит/с</span></div>
            <div class='data-row'><span class='data-label'>Одновременные подключения:</span><span class='data-value'>до 4-5 клиентов</span></div>
        </div>
        
        <div class='panel'>
            <h3 class="panel-title">🔌 Порты и интерфейсы</h3>
            <div class='data-row'><span class='data-label'>GPIO:</span><span class='data-value'>11 доступных из 17 физических пинов</span></div>
            <div class='data-row'><span class='data-label'>ADC:</span><span class='data-value'>1 канал, 10-битный, диапазон 0-1 В</span></div>
            <div class='data-row'><span class='data-label'>UART:</span><span class='data-value'>2 (UART0: TX/RX, UART1: только TX)</span></div>
            <div class='data-row'><span class='data-label'>SPI:</span><span class='data-value'>2 (HSPI и VSPI)</span></div>
            <div class='data-row'><span class='data-label'>I²C:</span><span class='data-value'>Программный (через любые GPIO)</span></div>
        </div>
        
        <div class='panel'>
            <h3 class="panel-title">⚙️ Конфигурация прошивки</h3>
            <div class='data-row'><span class='data-label'>Flash Size:</span><span class='data-value'>4MB (FS:3MB OTA:~0KB)</span></div>
            <div class='data-row'><span class='data-label'>CPU Frequency:</span><span class='data-value'>80 MHz</span></div>
            <div class='data-row'><span class='data-label'>IwIP Variant:</span><span class='data-value'>v2 Lower Memory</span></div>
            <div class='data-row'><span class='data-label'>MMU:</span><span class='data-value'>32KB cache + 32KB IRAM (balanced)</span></div>
            <div class='data-row'><span class='data-label'>Built-in Led:</span><span class='data-value'>GPIO2</span></div>
        </div>
        
        <div class='panel'>
            <h3 class="panel-title">🎯 Особенности проекта</h3>
            <div class='data-row'><span class='data-label'>Файловая система:</span><span class='data-value'>LittleFS, 3 МБ</span></div>
            <div class='data-row'><span class='data-label'>Веб-интерфейс:</span><span class='data-value'>HTML/CSS/JS с реальным временем</span></div>
            <div class='data-row'><span class='data-label'>Автоматическое подключение:</span><span class='data-value'>К указанному Wi-Fi</span></div>
            <div class='data-row'><span class='data-label'>Мобильное приложение:</span><span class='data-value'>Поддержка JSON API</span></div>
        </div>
    </div>
</body>
</html>
)rawliteral";

// Страница датчиков
const char SENSORS_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>🔌 ESP8266 - Датчики</title>
    <style>
        body{font-family:sans-serif;padding:7px;margin:7px;background:#f5f5f5}
        .container{max-width:650px;background:white;padding:14px;border-radius:10px;display:flex;flex-direction:column;gap:14px}
        .panel{background:#e8f4fd;padding:16px;border-radius:10px;display:flex;flex-direction:column;gap:16px}
        .btn-nav{text-align:center;display:flex;flex-wrap:wrap;gap:7px;justify-content:center}
        .btn-nav a{display:inline-block;padding:8px 16px;background:#1a73e8;color:white;text-decoration:none;border-radius:5px}
        h2{color:#1a73e8;text-align:center;margin:0;font-size:1.4rem}
        .panel-title{margin:0;padding-bottom:10px;border-bottom:1px solid #d0e8fc;font-weight:bold}
        .sensor-card{background:white;padding:15px;border-radius:8px;border:1px solid #d0e8fc;display:flex;flex-direction:column;gap:10px}
        .sensor-header{display:flex;justify-content:space-between;align-items:center}
        .sensor-name{font-weight:bold;color:#1a73e8;font-size:1.1rem}
        .toggle-switch{position:relative;display:inline-block;width:40px;height:20px}
        .toggle-switch input{opacity:0;width:0;height:0}
        .slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:#ccc;transition:.4s;border-radius:20px}
        .slider:before{position:absolute;content:"";height:16px;width:16px;left:2px;bottom:2px;background-color:white;transition:.4s;border-radius:50%}
        input:checked + .slider{background-color:#1a73e8}
        input:checked + .slider:before{transform:translateX(20px)}
        .sensor-details{display:grid;grid-template-columns:1fr 1fr;gap:8px;font-size:0.9rem}
        .detail-label{color:#5f6368;font-weight:bold}
        .detail-value{color:#202124;text-align:right}
        .form-group{display:flex;flex-direction:column;gap:8px}
        label{font-weight:bold;color:#5f6368}
        input,select,button{padding:10px;border:1px solid #ddd;border-radius:5px;font-size:0.9rem}
        button{background:#1a73e8;color:white;cursor:pointer;font-weight:bold;border:none}
        button:hover{background:#0d47a1}
        .action-buttons{display:flex;gap:10px;margin-top:10px}
        .btn-delete{background:#ea4335}
        .btn-delete:hover{background:#d32f2f}
        .loading{text-align:center;color:#666;padding:20px 0}
    </style>
</head>
<body>
    <div class='container'>
        <div class="btn-nav">
            <a href="/">Статус</a>
            <a href="/attributes">Характеристики</a>
            <a href="/sensors">Датчики</a>
            <a href="/settings">Настройки</a>
        </div>
        
        <h2>🔌 Управление датчиками</h2>
        
        <div class='panel'>
            <div id="sensorsLoading" class="loading">Загрузка датчиков...</div>
            <div id="sensorsContainer"></div>
            <button onclick="addSensor()" style="margin-top:15px;">➕ Добавить датчик</button>
        </div>
    </div>
    
    <script>
        function createSensorCard(sensor) {
            const card = document.createElement('div');
            card.className = 'sensor-card';
            card.dataset.id = sensor.id;
            
            card.innerHTML = `
                <div class="sensor-header">
                    <div class="sensor-name">${sensor.name}</div>
                    <label class="toggle-switch">
                        <input type="checkbox" ${sensor.enabled ? 'checked' : ''} onchange="toggleSensor(${sensor.id}, this.checked)">
                        <span class="slider"></span>
                    </label>
                </div>
                <div class="sensor-details">
                    <div><span class='detail-label'>Тип:</span></div>
                    <div><span class='detail-value'>${sensor.type}</span></div>
                    <div><span class='detail-label'>Единицы:</span></div>
                    <div><span class='detail-value'>${sensor.unit}</span></div>
                    <div><span class='detail-label'>Диапазон:</span></div>
                    <div><span class='detail-value'>${sensor.minVal} - ${sensor.maxVal}</span></div>
                    <div><span class='detail-label'>Пин:</span></div>
                    <div><span class='detail-value'>${sensor.pin}</span></div>
                </div>
                <div class="action-buttons">
                    <button onclick="editSensor(${sensor.id})">✏️ Редактировать</button>
                    <button class="btn-delete" onclick="deleteSensor(${sensor.id})">🗑️ Удалить</button>
                </div>
            `;
            return card;
        }
        
        function loadSensors() {
            const loadingDiv = document.getElementById('sensorsLoading');
            const container = document.getElementById('sensorsContainer');
            
            loadingDiv.style.display = 'block';
            container.innerHTML = '';
            
            fetch('/api/sensors/list')
                .then(response => response.json())
                .then(data => {
                    loadingDiv.style.display = 'none';
                    if (data.sensors && data.sensors.length > 0) {
                        data.sensors.forEach(sensor => {
                            const card = createSensorCard(sensor);
                            container.appendChild(card);
                        });
                    } else {
                        container.innerHTML = '<div style="text-align:center;color:#666;">Нет датчиков</div>';
                    }
                })
                .catch(error => {
                    loadingDiv.textContent = 'Ошибка загрузки';
                    console.error('Ошибка:', error);
                });
        }
        
        function toggleSensor(id, enabled) {
            fetch(`/api/sensors/toggle/${id}`, {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({enabled: enabled})
            }).catch(console.error);
        }
        
        function deleteSensor(id) {
            if (confirm('Удалить датчик?')) {
                fetch(`/api/sensors/delete/${id}`, {method: 'DELETE'})
                    .then(() => loadSensors())
                    .catch(console.error);
            }
        }
        
        function editSensor(id) {
            alert('Редактирование пока не реализовано');
        }
        
        function addSensor() {
            alert('Добавление пока не реализовано');
        }
        
        document.addEventListener('DOMContentLoaded', loadSensors);
    </script>
</body>
</html>
)rawliteral";

// Страница настроек
const char SETTINGS_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>⚙️ ESP8266 - Настройки</title>
    <style>
        body{font-family:sans-serif;padding:7px;margin:7px;background:#f5f5f5}
        .container{max-width:650px;background:white;padding:14px;border-radius:10px;display:flex;flex-direction:column;gap:14px}
        .panel{background:#e8f4fd;padding:16px;border-radius:10px;display:flex;flex-direction:column;gap:16px}
        .btn-nav{text-align:center;display:flex;flex-wrap:wrap;gap:7px;justify-content:center}
        .btn-nav a{display:inline-block;padding:8px 16px;background:#1a73e8;color:white;text-decoration:none;border-radius:5px}
        h2{color:#1a73e8;text-align:center;margin:0;font-size:1.4rem}
        .panel-title{margin:0;padding-bottom:10px;border-bottom:1px solid #d0e8fc;font-weight:bold}
        .form-group{display:flex;flex-direction:column;gap:8px}
        label{font-weight:bold;color:#5f6368}
        input,select,button{padding:10px;border:1px solid #ddd;border-radius:5px;font-size:0.9rem}
        button{background:#1a73e8;color:white;cursor:pointer;font-weight:bold;border:none}
        button:hover{background:#0d47a1}
        .config-section{background:#f8f9fa;padding:12px;border-radius:8px;border-left:3px solid #1a73e8;margin-top:10px}
    </style>
</head>
<body>
    <div class='container'>
        <div class="btn-nav">
            <a href="/">Статус</a>
            <a href="/attributes">Характеристики</a>
            <a href="/sensors">Датчики</a>
            <a href="/settings">Настройки</a>
        </div>
        
        <h2>⚙️ Настройки системы</h2>
        
        <div class='panel'>
            <form id="settingsForm">
                <div class="config-section">
                    <h4>🌐 Wi-Fi настройки</h4>
                    <div class='form-group'>
                        <label for='ssid'>SSID роутера:</label>
                        <input type='text' id='ssid' name='ssid' placeholder='Имя сети'>
                    </div>
                    <div class='form-group'>
                        <label for='password'>Пароль:</label>
                        <input type='password' id='password' name='password' placeholder='Пароль'>
                    </div>
                </div>
                
                <div class="config-section">
                    <h4>⏱️ Интервалы</h4>
                    <div class='form-group'>
                        <label for='updateInterval'>Интервал обновления данных (сек):</label>
                        <input type='number' id='updateInterval' name='updateInterval' min='1' max='3600' value='5'>
                    </div>
                </div>
                
                <div class="config-section">
                    <h4>👥 Подключения</h4>
                    <div class='form-group'>
                        <label for='maxClients'>Макс. клиентов к AP:</label>
                        <input type='number' id='maxClients' name='maxClients' min='1' max='4' value='1'>
                    </div>
                </div>
                
                <button type='submit' style="margin-top:20px;">💾 Сохранить настройки</button>
            </form>
        </div>
    </div>
    
    <script>
        function loadSettings() {
            fetch('/api/settings')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('ssid').value = data.ssid || '';
                    document.getElementById('password').value = data.password || '';
                    document.getElementById('updateInterval').value = data.updateInterval || 5;
                    document.getElementById('maxClients').value = data.maxClients || 1;
                })
                .catch(console.error);
        }
        
        document.getElementById('settingsForm').addEventListener('submit', function(e) {
            e.preventDefault();
            const formData = new FormData(this);
            const settings = {
                ssid: formData.get('ssid'),
                password: formData.get('password'),
                updateInterval: parseInt(formData.get('updateInterval')),
                maxClients: parseInt(formData.get('maxClients'))
            };
            
            fetch('/api/settings', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify(settings)
            })
            .then(response => {
                if (response.ok) {
                    alert('✅ Настройки сохранены!');
                } else {
                    throw new Error('Ошибка сохранения');
                }
            })
            .catch(error => {
                alert('❌ Ошибка сохранения');
                console.error('Ошибка:', error);
            });
        });
        
        document.addEventListener('DOMContentLoaded', loadSettings);
    </script>
</body>
</html>
)rawliteral";

#endif