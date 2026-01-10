#include "THServer.h"
#include <math.h>   // for isnan()

AsyncWebServer server(80);
AsyncEventSource events("/events");

// -------------------------
// Server-side state storage
// -------------------------
static float lastTemp = NAN;
static float lastHum  = NAN;

static unsigned long lastOkMs = 0;        // millis() of last successful sensor read
static unsigned long errorSinceMs = 0;    // millis() when error first triggered (0 = not in error)
static String currentStatus = "offline";  // "ok" | "offline" | "error"

// Build one JSON snapshot of the current server state.
// We include:
// - status
// - temperature/humidity (numbers only when ok; otherwise null)
// - last_temperature/last_humidity (last known good values)
// - last_ok_ms, error_since_ms
// - server_ms (millis at time of building message; used by browser to estimate clock time)
static String buildStateJson()
{
    const unsigned long nowMs = millis();

    auto numOrNull = [](float v, int decimals) -> String {
        if (isnan(v)) return "null";
        return String(v, decimals);
    };

    String json = "{";

    json += "\"status\":\"" + currentStatus + "\",";
    json += "\"server_ms\":" + String(nowMs) + ",";
    json += "\"last_ok_ms\":" + String(lastOkMs) + ",";
    json += "\"error_since_ms\":" + String(errorSinceMs) + ",";

    json += "\"last_temperature\":" + numOrNull(lastTemp, 2) + ",";
    json += "\"last_humidity\":"    + numOrNull(lastHum,  2) + ",";

    if (currentStatus == "ok") {
        json += "\"temperature\":" + numOrNull(lastTemp, 2) + ",";
        json += "\"humidity\":"    + numOrNull(lastHum,  2);
    } else {
        json += "\"temperature\":null,";
        json += "\"humidity\":null";
    }

    json += "}";
    return json;
}

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

            --danger: #f87171;
            --danger-soft: rgba(248,113,113,0.14);
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
            background: rgba(156,163,175,0.08);
            color: #e5e7eb;
            border-color: rgba(156,163,175,0.35);
        }

        .chip.sensor-offline #status-dot {
            background: #9ca3af;
            box-shadow: 0 0 0 3px rgba(156,163,175,0.25);
        }

        /* error state for the chip (red-ish) */
        .chip.sensor-error {
            background: var(--danger-soft);
            color: #fecaca;
            border-color: rgba(248,113,113,0.55);
        }

        .chip.sensor-error #status-dot {
            background: var(--danger);
            box-shadow: 0 0 0 3px rgba(248,113,113,0.20);
        }

        /* -------------------------
           ERROR OVERLAY (modal-ish)
           ------------------------- */
        .overlay {
            position: fixed;
            inset: 0;
            display: none; /* hidden by default */
            align-items: center;
            justify-content: center;
            padding: 18px;
            background: rgba(0,0,0,0.65);
            backdrop-filter: blur(7px);
            z-index: 9999;
        }

        .overlay.active {
            display: flex;
        }

        .overlay-card {
            width: min(620px, 100%);
            border-radius: 22px;
            padding: 18px 18px 16px;
            background: rgba(2,6,23,0.96);
            border: 1px solid rgba(248,113,113,0.50);
            box-shadow: 0 30px 90px rgba(0,0,0,0.70);
        }

        .overlay-title {
            font-size: 1.15rem;
            font-weight: 700;
            letter-spacing: 0.02em;
        }

        .overlay-subtitle {
            margin-top: 8px;
            font-size: 0.9rem;
            color: var(--text-muted);
            line-height: 1.35;
        }

        .overlay-grid {
            margin-top: 14px;
            display: grid;
            grid-template-columns: repeat(2, minmax(0, 1fr));
            gap: 10px;
        }

        .overlay-box {
            border: 1px solid rgba(148,163,184,0.30);
            border-radius: 14px;
            padding: 10px 10px 9px;
            background: radial-gradient(circle at top left, rgba(148,163,184,0.16), transparent 60%);
        }

        .overlay-label {
            font-size: 0.72rem;
            color: var(--text-muted);
            text-transform: uppercase;
            letter-spacing: 0.12em;
            margin-bottom: 6px;
        }

        .overlay-value {
            font-variant-numeric: tabular-nums;
            font-size: 1.1rem;
        }
    </style>

    <script>
        function fmtTime(d) {
            var hh = String(d.getHours()).padStart(2, '0');
            var mm = String(d.getMinutes()).padStart(2, '0');
            var ss = String(d.getSeconds()).padStart(2, '0');
            return hh + ':' + mm + ':' + ss;
        }

        // Convert "server millis timestamps" into an estimated local time.
        // We assume "now" and "server_ms" are roughly aligned at message arrival.
        function estimateLocalTime(serverNowMs, eventMs) {
            var delta = serverNowMs - eventMs;
            return new Date(Date.now() - delta);
        }

        // Track the newest last_ok_ms we've displayed, so offline/error messages
        // don’t change the "Last update" time unless a NEW ok reading happened.
        var lastOkMsSeen = 0;

        function setLastUpdateIfNew(serverMs, lastOkMs) {
            if (typeof serverMs !== 'number') return;
            if (typeof lastOkMs !== 'number') return;
            if (lastOkMs <= 0) return;

            if (lastOkMs > lastOkMsSeen) {
                lastOkMsSeen = lastOkMs;
                var dt = estimateLocalTime(serverMs, lastOkMs);
                var el = document.getElementById('last-update');
                if (el) el.innerText = fmtTime(dt);
            }
        }

        function setValueIfNumber(id, value) {
            var el = document.getElementById(id);
            if (!el) return;
            if (typeof value === 'number' && !isNaN(value)) {
                el.innerText = value.toFixed(2);
            }
        }

        function showErrorOverlay(lastT, lastH, lastOkAt, errAt) {
            var overlay = document.getElementById('error-overlay');
            if (!overlay) return;

            var lt = document.getElementById('err-last-temp');
            var lh = document.getElementById('err-last-hum');
            var lok = document.getElementById('err-last-ok-at');
            var es  = document.getElementById('err-since');

            if (lt) lt.innerText = (typeof lastT === 'number' && !isNaN(lastT)) ? lastT.toFixed(2) : '--';
            if (lh) lh.innerText = (typeof lastH === 'number' && !isNaN(lastH)) ? lastH.toFixed(2) : '--';
            if (lok) lok.innerText = (lastOkAt instanceof Date) ? fmtTime(lastOkAt) : '--:--:--';
            if (es)  es.innerText  = (errAt instanceof Date) ? fmtTime(errAt) : '--:--:--';

            overlay.classList.add('active');
        }

        function hideErrorOverlay() {
            var overlay = document.getElementById('error-overlay');
            if (!overlay) return;
            overlay.classList.remove('active');
        }

        var source = new EventSource('/events');
        source.onmessage = function(event) { // default SSE "message" event :contentReference[oaicite:2]{index=2}
            var data = JSON.parse(event.data);

            var tempEl = document.getElementById('temperature');
            var humEl  = document.getElementById('humidity');
            var chip = document.getElementById('status-chip');
            var chipLabel = document.getElementById('chip-label');

            var status = data.status || 'ok';

            var serverMs = (typeof data.server_ms === 'number') ? data.server_ms : null;
            var lastOkMs = (typeof data.last_ok_ms === 'number') ? data.last_ok_ms : 0;
            var errMs    = (typeof data.error_since_ms === 'number') ? data.error_since_ms : 0;

            var lastOkAt = (serverMs !== null && lastOkMs > 0) ? estimateLocalTime(serverMs, lastOkMs) : null;
            var errAt    = (serverMs !== null && errMs > 0)    ? estimateLocalTime(serverMs, errMs)    : null;

            // For offline/error we still want to be able to show last known values (esp. after reload)
            var lastT = (typeof data.last_temperature === 'number') ? data.last_temperature : NaN;
            var lastH = (typeof data.last_humidity === 'number') ? data.last_humidity : NaN;

            // Update unit labels once
            var tu = document.getElementById('temperature-unit');
            var hu = document.getElementById('humidity-unit');
            if (tu) tu.innerText = '°C';
            if (hu) hu.innerText = '%';

            if (status === 'offline') {
                if (chip) {
                    chip.classList.remove('sensor-error');
                    chip.classList.remove('sensor-ok');
                    chip.classList.add('sensor-offline');
                }
                if (chipLabel) chipLabel.innerText = 'Offline';

                // Show last known values (helpful after reload)
                setValueIfNumber('temperature', lastT);
                setValueIfNumber('humidity', lastH);

                // Do NOT change "Last update" unless last_ok_ms is newer (it shouldn't be in offline)
                setLastUpdateIfNew(serverMs, lastOkMs);

                // Offline is not an error overlay
                hideErrorOverlay();
                return;
            }

            if (status === 'error') {
                if (chip) {
                    chip.classList.remove('sensor-offline');
                    chip.classList.remove('sensor-ok');
                    chip.classList.add('sensor-error');
                }
                if (chipLabel) chipLabel.innerText = 'Error';

                // Keep showing last known values behind the overlay
                setValueIfNumber('temperature', lastT);
                setValueIfNumber('humidity', lastH);

                // Do NOT change "Last update" unless last_ok_ms is newer (it won't be)
                setLastUpdateIfNew(serverMs, lastOkMs);

                showErrorOverlay(lastT, lastH, lastOkAt, errAt);
                return;
            }

            // status === 'ok'
            if (chip) {
                chip.classList.remove('sensor-offline');
                chip.classList.remove('sensor-error');
                chip.classList.add('sensor-ok');
            }
            if (chipLabel) chipLabel.innerText = 'Live data';

            // Update displayed live values
            if (typeof data.temperature === 'number' && !isNaN(data.temperature)) {
                tempEl.innerText = data.temperature.toFixed(2);
            }
            if (typeof data.humidity === 'number' && !isNaN(data.humidity)) {
                humEl.innerText = data.humidity.toFixed(2);
            }

            // Update "Last update" only when there's a NEW ok reading
            setLastUpdateIfNew(serverMs, lastOkMs);

            // Clear overlay on recovery
            hideErrorOverlay();
        };

        // Optional: connection-level issues (ESP down / WiFi drop) can be handled here
        // EventSource auto-reconnects in most browsers. :contentReference[oaicite:3]{index=3}
        source.onerror = function(e) {
            // You can set the chip to offline here if you want, but it's separate from "sensor offline".
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

    <!-- ERROR OVERLAY -->
    <div class="overlay" id="error-overlay">
        <div class="overlay-card" role="dialog" aria-modal="true" aria-label="Sensor error dialog">
            <div class="overlay-title">⚠️ Sensor error</div>
            <div class="overlay-subtitle">
                The sensors reported an error. Please check wiring / power / connections.
            </div>

            <div class="overlay-grid">
                <div class="overlay-box">
                    <div class="overlay-label">Last valid temperature</div>
                    <div class="overlay-value">
                        <span id="err-last-temp">--</span> °C
                    </div>
                </div>

                <div class="overlay-box">
                    <div class="overlay-label">Last valid humidity</div>
                    <div class="overlay-value">
                        <span id="err-last-hum">--</span> %
                    </div>
                </div>

                <div class="overlay-box">
                    <div class="overlay-label">Last valid reading at</div>
                    <div class="overlay-value" id="err-last-ok-at">--:--:--</div>
                </div>

                <div class="overlay-box">
                    <div class="overlay-label">Error detected at</div>
                    <div class="overlay-value" id="err-since">--:--:--</div>
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
    // Send a full state snapshot immediately when a browser (re)connects to /events.
    // This fixes: "reload page during error/offline shows normal view".
    events.onConnect([](AsyncEventSourceClient *client) {
        String json = buildStateJson();
        // Using the same pattern as common ESPAsyncWebServer SSE examples: onConnect + client->send :contentReference[oaicite:4]{index=4}
        client->send(json.c_str(), "message", millis(), 10000);
    });

    server.addHandler(&events);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", pageContent()); });

    server.begin();
}

void updatePageContent(float temperature, float humidity)
{
    // Successful read
    lastTemp = temperature;
    lastHum  = humidity;
    lastOkMs = millis();
    errorSinceMs = 0;
    currentStatus = "ok";

    String jsonData = buildStateJson();
    events.send(jsonData.c_str(), "message", millis());
}

void sensorOffline()
{
    // Sensor not triggered: keep lastTemp/lastHum/lastOkMs untouched
    currentStatus = "offline";

    String jsonData = buildStateJson();
    events.send(jsonData.c_str(), "message", millis());
}

void sensorError()
{
    // Sensor reading hindered: remember when error started (first time only)
    currentStatus = "error";
    if (errorSinceMs == 0) errorSinceMs = millis();

    String jsonData = buildStateJson();
    events.send(jsonData.c_str(), "message", millis());
}
