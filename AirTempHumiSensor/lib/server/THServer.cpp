#include "THServer.h"

AsyncWebServer server(80);
AsyncEventSource events("/events");

String pageContent()
{
    String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Air Temperature and Humidity Sensor</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">

    <style>
        :root {
            --bg: #0f172a;
            --card-bg: #020617;
            --accent: #38bdf8;
            --accent-soft: rgba(56,189,248,0.2);
            --text-main: #e5e7eb;
            --text-muted: #9ca3af;
            --temp-color: #f97316;
            --hum-color: #22c55e;
            --shadow-soft: 0 18px 45px rgba(15,23,42,0.65);
            --radius-lg: 18px;
        }

        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
        }

        body {
            min-height: 100vh;
            font-family: system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
            background: radial-gradient(circle at top, #1f2937 0, #020617 40%, #000 100%);
            color: var(--text-main);
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 20px;
        }

        .wrapper {
            width: 100%;
            max-width: 900px;
        }

        .card {
            background: linear-gradient(135deg, rgba(15,23,42,0.96), rgba(15,23,42,0.85));
            border-radius: 24px;
            padding: 26px 28px 24px;
            box-shadow: var(--shadow-soft);
            border: 1px solid rgba(148,163,184,0.25);
            backdrop-filter: blur(18px);
        }

        .header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 18px;
            gap: 10px;
        }

        .title-group h1 {
            font-size: 1.2rem;
            letter-spacing: 0.04em;
            text-transform: uppercase;
            color: #e5e7eb;
        }

        .title-group p {
            font-size: 0.78rem;
            color: var(--text-muted);
            margin-top: 4px;
        }

        .chip {
            font-size: 0.72rem;
            padding: 4px 10px;
            border-radius: 999px;
            background: var(--accent-soft);
            color: var(--accent);
            border: 1px solid rgba(56,189,248,0.5);
            display: inline-flex;
            align-items: center;
            gap: 6px;
            white-space: nowrap;
        }

        .dot {
            width: 7px;
            height: 7px;
            border-radius: 999px;
            background: #22c55e;
            box-shadow: 0 0 0 3px rgba(34,197,94,0.25);
        }

        .content-row {
            display: flex;
            gap: 18px;
            flex-wrap: wrap;
        }

        .metric-card {
            flex: 1 1 220px;
            background: radial-gradient(circle at top left, rgba(148,163,184,0.22), transparent 55%);
            border-radius: var(--radius-lg);
            padding: 16px 16px 14px;
            border: 1px solid rgba(148,163,184,0.35);
            position: relative;
            overflow: hidden;
        }

        .metric-card::before {
            content: "";
            position: absolute;
            inset: 0;
            background: linear-gradient(135deg, rgba(15,23,42,0.5), transparent 40%);
            pointer-events: none;
        }

        .metric-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 8px;
            position: relative;
            z-index: 1;
        }

        .metric-label {
            font-size: 0.78rem;
            text-transform: uppercase;
            letter-spacing: 0.12em;
            color: var(--text-muted);
        }

        .metric-icon {
            width: 26px;
            height: 26px;
            border-radius: 999px;
            display: inline-flex;
            align-items: center;
            justify-content: center;
            font-size: 0.9rem;
            background: rgba(15,23,42,0.9);
            border: 1px solid rgba(148,163,184,0.55);
        }

        .metric-value {
            font-size: 2.1rem;
            font-weight: 600;
            letter-spacing: 0.03em;
            margin-bottom: 2px;
            position: relative;
            z-index: 1;
        }

        .metric-value small {
            font-size: 0.9rem;
            font-weight: 400;
            color: var(--text-muted);
            margin-left: 4px;
        }

        .metric-sub {
            font-size: 0.78rem;
            color: var(--text-muted);
            position: relative;
            z-index: 1;
        }

        .metric-bar {
            position: absolute;
            inset-inline: 0;
            bottom: 0;
            height: 3px;
            opacity: 0.85;
        }

        .metric-bar.temp {
            background: linear-gradient(90deg, rgba(249,115,22,0.15), var(--temp-color));
        }

        .metric-bar.hum {
            background: linear-gradient(90deg, rgba(34,197,94,0.15), var(--hum-color));
        }

        .footer-row {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-top: 18px;
            font-size: 0.75rem;
            color: var(--text-muted);
        }

        .last-update {
            display: inline-flex;
            align-items: center;
            gap: 6px;
        }

        .last-update span {
            font-variant-numeric: tabular-nums;
        }

        .mini-pill {
            padding: 3px 8px;
            border-radius: 999px;
            border: 1px solid rgba(148,163,184,0.45);
            background: rgba(15,23,42,0.8);
        }

        @media (max-width: 540px) {
            .card {
                padding: 18px 16px 16px;
            }

            .metric-value {
                font-size: 1.7rem;
            }

            .header {
                flex-direction: column;
                align-items: flex-start;
            }
        }

        /* offline state for the chip (greyed out) */
        .chip.sensor-offline {
            background: rgba(156,163,175,0.08); /* subtle grey background */
            color: #e5e7eb; /* keep text readable */
            border-color: rgba(156,163,175,0.35);
        }

        .chip.sensor-offline #status-dot {
            background: #9ca3af; /* grey dot */
            box-shadow: 0 0 0 3px rgba(156,163,175,0.25);
        }
    </style>

    <script>
        function updateTimestamp() {
            var el = document.getElementById('last-update');
            if (!el) return;
            var now = new Date();
            var hh = String(now.getHours()).padStart(2, '0');
            var mm = String(now.getMinutes()).padStart(2, '0');
            var ss = String(now.getSeconds()).padStart(2, '0');
            el.innerText = hh + ':' + mm + ':' + ss;
        }

        var source = new EventSource('/events');
        source.onmessage = function(event) {
            var data = JSON.parse(event.data);

            var tempEl = document.getElementById('temperature');
            var humEl = document.getElementById('humidity');
            var chip = document.getElementById('status-chip');
            var chipLabel = document.getElementById('chip-label');

            // Default: treat missing status as OK for backward compatibility
            var status = data.status || 'ok';

            if (status === 'offline') {
                if (chip) {
                    chip.classList.remove('sensor-ok');
                    chip.classList.add('sensor-offline');
                }
                if (chipLabel) {
                    chipLabel.innerText = 'Offline';
                }

                return;
            } else {
                if (chip) {
                    chip.classList.remove('sensor-offline');
                    chip.classList.add('sensor-ok');
                }
                if (chipLabel) {
                    chipLabel.innerText = 'Live data';
                }
            }

            // Only update values if they are real numbers
            if (typeof data.temperature === 'number' && !isNaN(data.temperature)) {
                tempEl.innerText = data.temperature.toFixed(2);
            }
            if (typeof data.humidity === 'number' && !isNaN(data.humidity)) {
                humEl.innerText = data.humidity.toFixed(2);
            }

            document.getElementById('temperature-unit').innerText = '°C';
            document.getElementById('humidity-unit').innerText = '%';

            updateTimestamp();
        };
    </script>

</head>
<body>
    <div class="wrapper">
        <div class="card">
            <div class="header">
                <div class="title-group">
                    <h1>Environment Monitor</h1>
                    <p>Live air temperature and humidity</p>
                </div>
                <div class="chip" id="status-chip">
                    <span class="dot" id="status-dot"></span>
                    <span id="chip-label">Live&nbsp;data</span>
                </div>
            </div>

            <div class="content-row">
                <!-- Temperature card -->
                <div class="metric-card">
                    <div class="metric-header">
                        <div class="metric-label">Temperature</div>
                        <div class="metric-icon">🌡️</div>
                    </div>
                    <div class="metric-value" style="color: var(--temp-color);">
                        <span id="temperature">--</span>
                        <small id="temperature-unit">°C</small>
                    </div>
                    <div class="metric-sub">
                        Indoor air temperature
                    </div>
                    <div class="metric-bar temp"></div>
                </div>

                <!-- Humidity card -->
                <div class="metric-card">
                    <div class="metric-header">
                        <div class="metric-label">Humidity</div>
                        <div class="metric-icon">💧</div>
                    </div>
                    <div class="metric-value" style="color: var(--hum-color);">
                        <span id="humidity">--</span>
                        <small id="humidity-unit">%</small>
                    </div>
                    <div class="metric-sub">
                        Relative humidity
                    </div>
                    <div class="metric-bar hum"></div>
                </div>
            </div>

            <div class="footer-row">
                <div class="last-update">
                    <span>Last update:</span>
                    <span id="last-update">--:--:--</span>
                </div>
                <div class="mini-pill">
                    Sensor: SHT10
                </div>
            </div>
        </div>
    </div>
</body>
</html>
)rawliteral";
    return html;
}

void initServer()
{
    server.addHandler(&events);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", pageContent()); });
    server.begin();
}

void updatePageContent(float temperature, float humidity)
{
    String jsonData = "{";
    jsonData += "\"status\":\"ok\",";
    jsonData += "\"temperature\":" + String(temperature, 2) + ",";
    jsonData += "\"humidity\":" + String(humidity, 2);
    jsonData += "}";
    events.send(jsonData.c_str(), "message", millis());
}

void sensorOffline()
{
    String jsonData = "{";
    jsonData += "\"status\":\"offline\",";
    jsonData += "\"temperature\":null,";
    jsonData += "\"humidity\":null";
    jsonData += "}";
    events.send(jsonData.c_str(), "message", millis());
}
