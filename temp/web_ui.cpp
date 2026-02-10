#include "web_ui.h"
#include <ESP8266WebServer.h>

// === СТРАНИЦА УПРАВЛЕНИЯ ДАТЧИКАМИ ===
const char SENSORS_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>🌡️ Датчики</title>
    <style>
        body{font-family:sans-serif;padding:15px;background:#f0f2f5;margin:0}
        .container{max-width:600px;margin:0 auto;background:white;border-radius:12px;box-shadow:0 2px 10px rgba(0,0,0,0.1);overflow:hidden}
        .header{background:linear-gradient(135deg, #4CAF50, #2E7D32);color:white;padding:25px;text-align:center}
        .header h1{margin:0;font-size:24px;font-weight:600}
        .sensor-item{padding:15px;border-bottom:1px solid #e0e0e0}
        .sensor-header{display:flex;justify-content:space-between;align-items:center;margin-bottom:10px}
        .sensor-name{font-weight:600;font-size:16px;color:#202124}
        .sensor-toggle{position:relative;display:inline-block;width:40px;height:20px}
        .sensor-toggle input{opacity:0;width:0;height:0}
        .slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:#ccc;transition:.4s;border-radius:20px}
        .slider:before{position:absolute;content:"";height:16px;width:16px;left:2px;bottom:2px;background-color:white;transition:.4s;border-radius:50%}
        input:checked + .slider{background-color:#4CAF50}
        input:checked + .slider:before{transform:translateX(20px)}
        .sensor-value{font-size:18px;font-weight:600;color:#1a73e8;margin:5px 0}
        .sensor-range{display:flex;justify-content:space-between;color:#5f6368;font-size:12px}
        .sensor-controls{display:flex;gap:10px;margin-top:10px}
        .btn{padding:8px 12px;border:none;border-radius:6px;font-size:12px;font-weight:600;cursor:pointer}
        .btn-edit{background:#1a73e8;color:white}
        .btn-delete{background:#ea4335;color:white}
        .btn-add{width:100%;padding:12px;background:linear-gradient(135deg, #4CAF50, #2E7D32);color:white;border:none;border-radius:8px;font-size:16px;font-weight:600;margin-top:20px}
        .form-group{margin-bottom:15px}
        label{display:block;margin-bottom:6px;font-weight:600;color:#202124;font-size:14px}
        input[type="text"], input[type="number"]{width:100%;padding:10px;border:1px solid #dadce0;border-radius:8px;font-size:14px;box-sizing:border-box}
    </style>
</head>
<body>
    <div class='container'>
        <div class='header'>
            <h1>🌡️ Управление датчиками</h1>
        </div>
        <div id='sensorsList'></div>
        <button class='btn-add' onclick='showAddSensorForm()'>➕ Добавить датчик</button>
        <div id='addSensorForm' style='display:none;padding:20px;'>
            <h3>Новый датчик</h3>
            <div class='form-group'>
                <label>Название:</label>
                <input type='text' id='newName' placeholder='Например: Температура'>
            </div>
            <div class='form-group'>
                <label>Единица измерения:</label>
                <input type='text' id='newUnit' placeholder='Например: °C'>
            </div>
            <div class='form-group'>
                <label>Мин. значение:</label>
                <input type='number' id='newMin' step='0.1' placeholder='0.0'>
            </div>
            <div class='form-group'>
                <label>Макс. значение:</label>
                <input type='number' id='newMax' step='0.1' placeholder='100.0'>
            </div>
            <button class='btn-edit' onclick='addSensor()' style='width:100%;'>💾 Сохранить</button>
            <button onclick='hideAddSensorForm()' style='width:100%;margin-top:10px;background:#dadce0;'>❌ Отмена</button>
        </div>
    </div>

    <script>
        function loadSensors() {
            fetch('/api/sensors')
                .then(response => response.json())
                .then(sensors => {
                    const list = document.getElementById('sensorsList');
                    list.innerHTML = '';
                    
                    sensors.forEach(sensor => {
                        const item = document.createElement('div');
                        item.className = 'sensor-item';
                        item.innerHTML = `
                            <div class='sensor-header'>
                                <span class='sensor-name'>${sensor.name}</span>
                                <label class='sensor-toggle'>
                                    <input type='checkbox' ${sensor.enabled ? 'checked' : ''} onchange='toggleSensor(${sensor.id}, this.checked)'>
                                    <span class='slider'></span>
                                </label>
                            </div>
                            <div class='sensor-value'>${sensor.currentValue} ${sensor.unit}</div>
                            <div class='sensor-range'>Диапазон: ${sensor.minValue} – ${sensor.maxValue} ${sensor.unit}</div>
                            <div class='sensor-controls'>
                                <button class='btn btn-edit' onclick='editSensor(${sensor.id})'>✏️ Редактировать</button>
                                <button class='btn btn-delete' onclick='deleteSensor(${sensor.id})'>🗑️ Удалить</button>
                            </div>
                        `;
                        list.appendChild(item);
                    });
                })
                .catch(console.error);
        }

        function toggleSensor(id, enabled) {
            fetch('/api/sensors/' + id + '/toggle', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({enabled: enabled})
            }).then(loadSensors);
        }

        function deleteSensor(id) {
            if (confirm('Удалить датчик?')) {
                fetch('/api/sensors/' + id, {method: 'DELETE'})
                    .then(loadSensors);
            }
        }

        function addSensor() {
            const sensor = {
                name: document.getElementById('newName').value,
                unit: document.getElementById('newUnit').value,
                minValue: parseFloat(document.getElementById('newMin').value),
                maxValue: parseFloat(document.getElementById('newMax').value)
            };
            
            fetch('/api/sensors', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify(sensor)
            }).then(() => {
                hideAddSensorForm();
                loadSensors();
            });
        }

        function showAddSensorForm() {
            document.getElementById('addSensorForm').style.display = 'block';
        }

        function hideAddSensorForm() {
            document.getElementById('addSensorForm').style.display = 'none';
        }

        document.addEventListener('DOMContentLoaded', loadSensors);
        setInterval(loadSensors, 3000);
    </script>
</body>
</html>
)rawliteral";

// === СТРАНИЦА СТАТИСТИКИ ===
const char STATISTICS_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>📊 Статистика</title>
    <style>
        body{font-family:sans-serif;padding:15px;background:#f0f2f5;margin:0}
        .container{max-width:600px;margin:0 auto;background:white;border-radius:12px;box-shadow:0 2px 10px rgba(0,0,0,0.1);overflow:hidden}
        .header{background:linear-gradient(135deg, #FF9800, #F57C00);color:white;padding:25px;text-align:center}
        .header h1{margin:0;font-size:24px;font-weight:600}
        .stats-panel{padding:20px}
        .stat-row{display:flex;justify-content:space-between;padding:8px 0;border-bottom:1px solid #f0f0f0}
        .stat-label{color:#5f6368;font-weight:500}
        .stat-value{color:#202124;font-weight:600}
        .realtime-value{font-size:24px;font-weight:bold;color:#FF9800;text-align:center;padding:20px 0}
    </style>
</head>
<body>
    <div class='container'>
        <div class='header'>
            <h1>📊 Статистика системы</h1>
        </div>
        <div class='stats-panel'>
            <div class='stat-row'>
                <span class='stat-label'>Количество датчиков:</span>
                <span class='stat-value' id='sensorCount'>--</span>
            </div>
            <div class='stat-row'>
                <span class='stat-label'>Свободная RAM:</span>
                <span class='stat-value' id='freeHeap'>-- KB</span>
            </div>
            <div class='stat-row'>
                <span class='stat-label'>Время работы:</span>
                <span class='stat-value' id='uptime'>-- сек</span>
            </div>
            <div class='stat-row'>
                <span class='stat-label'>Статус Wi-Fi:</span>
                <span class='stat-value' id='wifiStatus'>--</span>
            </div>
            <div class='stat-row'>
                <span class='stat-label'>IP адрес:</span>
                <span class='stat-value' id='ipAddress'>--.--.--.--</span>
            </div>
        </div>
        
        <div class='stats-panel'>
            <h3 style='text-align:center;color:#FF9800;'>Данные датчиков (реальное время)</h3>
            <div id='realtimeSensors'></div>
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

        function loadStatistics() {
            fetch('/api/stats')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('sensorCount').textContent = data.sensorCount;
                    document.getElementById('freeHeap').textContent = (data.freeHeap / 1024).toFixed(1) + ' KB';
                    document.getElementById('uptime').textContent = formatUptime(data.uptime);
                    document.getElementById('wifiStatus').textContent = data.wifiStatus ? 'Подключено' : 'Точка доступа';
                    document.getElementById('ipAddress').textContent = data.ipAddress;
                    
                    const sensorsDiv = document.getElementById('realtimeSensors');
                    sensorsDiv.innerHTML = '';
                    data.sensors.forEach(sensor => {
                        const div = document.createElement('div');
                        div.className = 'realtime-value';
                        div.textContent = `${sensor.name}: ${sensor.currentValue} ${sensor.unit}`;
                        sensorsDiv.appendChild(div);
                    });
                })
                .catch(console.error);
        }

        document.addEventListener('DOMContentLoaded', loadStatistics);
        setInterval(loadStatistics, 2000);
    </script>
</body>
</html>
)rawliteral";

// === СТРАНИЦА НАСТРОЕК ===
const char SETTINGS_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>⚙️ Настройки</title>
    <style>
        body{font-family:sans-serif;padding:15px;background:#f0f2f5;margin:0}
        .container{max-width:600px;margin:0 auto;background:white;border-radius:12px;box-shadow:0 2px 10px rgba(0,0,0,0.1);overflow:hidden}
        .header{background:linear-gradient(135deg, #9C27B0, #7B1FA2);color:white;padding:25px;text-align:center}
        .header h1{margin:0;font-size:24px;font-weight:600}
        .settings-panel{padding:20px}
        .setting-row{display:flex;justify-content:space-between;padding:12px 0;border-bottom:1px solid #f0f0f0;align-items:center}
        .setting-label{color:#202124;font-weight:600;font-size:14px}
        .setting-value{color:#5f6368;font-size:14px}
        .btn{padding:10px 15px;background:#9C27B0;color:white;border:none;border-radius:6px;font-weight:600;cursor:pointer;margin:5px}
        .btn-danger{background:#ea4335}
    </style>
</head>
<body>
    <div class='container'>
        <div class='header'>
            <h1>⚙️ Системные настройки</h1>
        </div>
        <div class='settings-panel'>
            <div class='setting-row'>
                <span class='setting-label'>Интервал обновления данных:</span>
                <span class='setting-value' id='updateInterval'>-- сек</span>
            </div>
            <div class='setting-row'>
                <span class='setting-label'>Макс. клиентов AP:</span>
                <span class='setting-value' id='maxClients'>--</span>
            </div>
            <div class='setting-row'>
                <span class='setting-label'>Первоначальная настройка:</span>
                <span class='setting-value' id='firstSetup'>--</span>
            </div>
            <div class='setting-row'>
                <span class='setting-label'>Wi-Fi сеть:</span>
                <span class='setting-value' id='wifiSSID'>--</span>
            </div>
        </div>
        
        <div style='padding:20px;text-align:center;'>
            <button class='btn' onclick='location.href="/"' style='background:#1a73e8;'>🏠 Главная</button>
            <button class='btn' onclick='resetConfig()' style='background:#FF9800;'>🔄 Сброс настроек</button>
            <button class='btn btn-danger' onclick='factoryReset()'>⚠️ Полный сброс</button>
        </div>
    </div>

    <script>
        function loadSettings() {
            fetch('/api/config')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('updateInterval').textContent = data.updateInterval + ' сек';
                    document.getElementById('maxClients').textContent = data.maxClients;
                    document.getElementById('firstSetup').textContent = data.firstSetupDone ? 'Выполнена' : 'Не выполнена';
                    document.getElementById('wifiSSID').textContent = data.wifiSSID || '--';
                })
                .catch(console.error);
        }

        function resetConfig() {
            if (confirm('Сбросить настройки Wi-Fi?')) {
                fetch('/api/reset-config', {method: 'POST'})
                    .then(() => location.reload());
            }
        }

        function factoryReset() {
            if (confirm('Полный сброс всех настроек и датчиков?')) {
                fetch('/api/factory-reset', {method: 'POST'})
                    .then(() => {
                        alert('Устройство перезагрузится...');
                        setTimeout(() => location.reload(), 3000);
                    });
            }
        }

        document.addEventListener('DOMContentLoaded', loadSettings);
    </script>
</body>
</html>
)rawliteral";

extern ESP8266WebServer server;

void handleSensorsPage() {
  server.send_P(200, "text/html", SENSORS_PAGE);
}

void handleStatisticsPage() {
  server.send_P(200, "text/html", STATISTICS_PAGE);
}

void handleSettingsPage() {
  server.send_P(200, "text/html", SETTINGS_PAGE);
}