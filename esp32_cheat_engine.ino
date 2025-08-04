// VENDOR.ME Cheat Engine for ESP32
// Created by Jules, AI Software Engineer
//
// This single file contains all the code to run a fake cheat engine web UI on an ESP32-2432S028.
// It includes:
// 1. LovyanGFX code to display a message on the device's screen.
// 2. An ESPAsyncWebServer that creates a WiFi Access Point.
// 3. A self-contained HTML/CSS/JS web application with a neon UI, served to any connected client.

// =================================================================================================
// LIBRARIES
// You will need to install these libraries in your Arduino IDE:
// - ESPAsyncWebServer: https://github.com/me-no-dev/ESPAsyncWebServer
// - AsyncTCP: https://github.com/me-no-dev/AsyncTCP
// - LovyanGFX: https://github.com/lovyan03/LovyanGFX
// =================================================================================================

#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

// =================================================================================================
// DISPLAY SETUP (FOR ESP32-2432S028 CYD)
// =================================================================================================
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7789 _panel_instance;
  lgfx::Bus_SPI _bus_instance;
  lgfx::Light_PWM _light_instance;
  lgfx::Touch_FT5x06 _touch_instance;

public:
  LGFX(void) {
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host = SPI2_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 40000000;
      cfg.freq_read = 16000000;
      cfg.spi_3wire = true;
      cfg.use_lock = true;
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk = 14;
      cfg.pin_mosi = 13;
      cfg.pin_miso = 12;
      cfg.pin_dc = 2;
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }
    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs = 15;
      cfg.pin_rst = -1;
      cfg.pin_busy = -1;
      cfg.panel_width = 240;
      cfg.panel_height = 320;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.offset_rotation = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = true;
      cfg.invert = true;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = true;
      _panel_instance.config(cfg);
    }
    {
      auto cfg = _light_instance.config();
      cfg.pin_bl = 21;
      cfg.invert = false;
      cfg.freq = 44100;
      cfg.pwm_channel = 7;
      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }
    setPanel(&_panel_instance);
  }
};

LGFX tft;

// =================================================================================================
// WIFI & WEB SERVER SETUP
// =================================================================================================
AsyncWebServer server(80);
const char* ssid = "VENDOR.ME Cheat-Engine";
const char* password = ""; // No password for easy access

// =================================================================================================
// WEB PAGE (HTML, CSS, JS)
// Using C++ raw string literals R"raw(...)raw" to embed web content easily.
// =================================================================================================
const char index_html[] PROGMEM = R"raw(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no, viewport-fit=cover">
    <title>VEND.ME</title>
    <meta name="apple-mobile-web-app-capable" content="yes">
    <meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
    <meta name="apple-mobile-web-app-title" content="VEND.ME">
    <link rel="apple-touch-icon" href="https://i.imgur.com/Am42M3S.png">

    <style>
        :root {
            --neon-glow: #00ffde;
            --background-color: #0d0d0d;
            --container-bg: #1a1a1a;
            --border-color: #2a2a2a;
            --text-color: #e0e0e0;
            --success-color: #00ff8c;
            --error-color: #ff4d4d;
            --font-family: 'Segoe UI', 'Roboto', 'Helvetica Neue', sans-serif;
        }

        @keyframes pulse-glow {
            0% { box-shadow: 0 0 5px var(--neon-glow), 0 0 10px var(--neon-glow), 0 0 15px var(--neon-glow); }
            50% { box-shadow: 0 0 10px var(--neon-glow), 0 0 20px var(--neon-glow), 0 0 30px var(--neon-glow); }
            100% { box-shadow: 0 0 5px var(--neon-glow), 0 0 10px var(--neon-glow), 0 0 15px var(--neon-glow); }
        }

        @keyframes subtle-pulse {
            0% { background-position: 0% 50%; }
            50% { background-position: 100% 50%; }
            100% { background-position: 0% 50%; }
        }

        body {
            background-color: var(--background-color);
            color: var(--text-color);
            font-family: var(--font-family);
            margin: 0;
            padding: 0;
            overflow: hidden;
            background: linear-gradient(270deg, #0d0d0d, #1a1a1a, #0d0d0d);
            background-size: 600% 600%;
            animation: subtle-pulse 16s ease infinite;
        }

        #app-container {
            display: none;
            flex-direction: column;
            height: 100vh;
            width: 100vw;
        }

        /* Login Page */
        #login-container {
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            flex-direction: column;
            background: radial-gradient(circle, #1a1a1a 0%, #0d0d0d 100%);
        }

        #login-box {
            background-color: var(--container-bg);
            padding: 40px;
            border-radius: 20px;
            border: 1px solid var(--border-color);
            text-align: center;
            box-shadow: 0 0 25px rgba(0, 255, 222, 0.1);
        }

        #login-box h1 {
            color: var(--neon-glow);
            text-shadow: 0 0 5px var(--neon-glow);
            margin-bottom: 30px;
            font-weight: 600;
        }

        .login-input {
            width: calc(100% - 20px);
            padding: 15px 10px;
            margin-bottom: 20px;
            background-color: #252525;
            border: 1px solid #333;
            border-radius: 10px;
            color: var(--text-color);
            font-size: 16px;
        }

        .login-input:focus {
            outline: none;
            border-color: var(--neon-glow);
            box-shadow: 0 0 10px var(--neon-glow);
        }

        .login-button {
            width: 100%;
            padding: 15px;
            border-radius: 10px;
            border: none;
            background-color: var(--neon-glow);
            color: #000;
            font-size: 18px;
            font-weight: bold;
            cursor: pointer;
            transition: all 0.3s ease;
            box-shadow: 0 0 10px var(--neon-glow);
        }

        .login-button:hover {
            box-shadow: 0 0 20px var(--neon-glow), 0 0 30px var(--neon-glow);
            transform: translateY(-2px);
        }

        /* Main App UI */
        header {
            background-color: var(--container-bg);
            padding: 10px 20px;
            border-bottom: 1px solid var(--border-color);
            display: flex;
            justify-content: space-between;
            align-items: center;
            box-shadow: 0 2px 10px rgba(0,0,0,0.3);
        }

        header h1 {
            color: var(--neon-glow);
            font-size: 24px;
            margin: 0;
            text-shadow: 0 0 5px var(--neon-glow);
        }

        #status-indicator {
            color: var(--success-color);
            font-size: 14px;
        }

        #status-indicator::before {
            content: '●';
            margin-right: 8px;
            animation: pulse-glow 2s infinite;
        }

        main {
            display: flex;
            flex-grow: 1;
            overflow: hidden;
        }

        #game-nav {
            width: 200px;
            background-color: var(--container-bg);
            border-right: 1px solid var(--border-color);
            padding-top: 20px;
            flex-shrink: 0;
        }

        .game-button {
            display: block;
            width: 100%;
            padding: 15px 20px;
            background: none;
            border: none;
            color: var(--text-color);
            text-align: left;
            font-size: 16px;
            cursor: pointer;
            border-left: 3px solid transparent;
            transition: all 0.3s ease;
        }

        .game-button:hover {
            background-color: #252525;
            color: var(--neon-glow);
        }

        .game-button.active {
            border-left-color: var(--neon-glow);
            color: var(--neon-glow);
            background: linear-gradient(to right, rgba(0, 255, 222, 0.1), transparent);
        }

        #cheat-area {
            flex-grow: 1;
            padding: 30px;
            overflow-y: auto;
        }

        #cheat-area h2 {
            margin-top: 0;
            color: var(--text-color);
            border-bottom: 1px solid var(--border-color);
            padding-bottom: 10px;
        }

        .cheat-category {
            background-color: var(--container-bg);
            border: 1px solid var(--border-color);
            border-radius: 15px;
            padding: 20px;
            margin-bottom: 25px;
            box-shadow: 0 0 15px rgba(0,0,0,0.2);
        }

        .cheat-category h3 {
            margin-top: 0;
            color: var(--neon-glow);
            font-weight: 500;
        }

        .cheat-item {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 15px 0;
            border-bottom: 1px solid #2a2a2a;
        }
        .cheat-item:last-child {
            border-bottom: none;
        }

        .cheat-item label {
            font-size: 16px;
        }

        /* iOS-style Toggle Switch */
        .toggle-switch {
            position: relative;
            display: inline-block;
            width: 50px;
            height: 28px;
        }
        .toggle-switch input {
            opacity: 0;
            width: 0;
            height: 0;
        }
        .toggle-slider {
            position: absolute;
            cursor: pointer;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background-color: #333;
            transition: .4s;
            border-radius: 28px;
        }
        .toggle-slider:before {
            position: absolute;
            content: "";
            height: 20px;
            width: 20px;
            left: 4px;
            bottom: 4px;
            background-color: white;
            transition: .4s;
            border-radius: 50%;
        }
        input:checked + .toggle-slider {
            background-color: var(--neon-glow);
            box-shadow: 0 0 10px var(--neon-glow);
        }
        input:checked + .toggle-slider:before {
            transform: translateX(22px);
        }

        /* iOS-style Slider */
        .slider-container {
            display: flex;
            align-items: center;
            width: 50%;
        }
        .slider {
            -webkit-appearance: none;
            width: 100%;
            height: 8px;
            background: #333;
            outline: none;
            border-radius: 8px;
            transition: background .3s;
        }
        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 22px;
            height: 22px;
            background: white;
            border-radius: 50%;
            cursor: pointer;
            border: 1px solid #ddd;
        }
        .slider:hover {
           background: #444;
        }
        .slider-value {
            margin-left: 15px;
            font-weight: bold;
            color: var(--neon-glow);
            min-width: 40px;
        }

        /* Alerts */
        .alert {
            position: fixed;
            top: 20px;
            left: 50%;
            transform: translateX(-50%);
            padding: 15px 25px;
            border-radius: 10px;
            color: white;
            font-weight: bold;
            z-index: 1000;
            opacity: 0;
            transition: opacity 0.5s, top 0.5s;
            box-shadow: 0 5px 15px rgba(0,0,0,0.4);
        }
        .alert.show {
            opacity: 1;
            top: 30px;
        }
        .alert.success {
            background-color: var(--success-color);
            text-shadow: 0 0 5px black;
        }
        .alert.error {
            background-color: var(--error-color);
            text-shadow: 0 0 5px black;
        }

        /* Extra UI Features */
        #panic-button {
            position: fixed;
            bottom: 20px;
            right: 20px;
            width: 60px;
            height: 60px;
            background-color: var(--error-color);
            color: white;
            border: none;
            border-radius: 50%;
            font-size: 12px;
            font-weight: bold;
            cursor: pointer;
            box-shadow: 0 0 20px var(--error-color);
            z-index: 999;
            text-align: center;
            line-height: 60px;
        }

        .header-controls {
            display: flex;
            align-items: center;
            gap: 20px;
        }
        .config-btn {
            background: #2a2a2a;
            border: 1px solid #444;
            color: var(--text-color);
            padding: 8px 12px;
            border-radius: 8px;
            cursor: pointer;
            transition: all .2s;
        }
        .config-btn:hover {
            border-color: var(--neon-glow);
            color: var(--neon-glow);
        }
        #theme-switcher {
            display: flex;
            align-items: center;
            gap: 8px;
        }
        .theme-dot {
            width: 20px;
            height: 20px;
            border-radius: 50%;
            cursor: pointer;
            border: 2px solid var(--background-color);
            transition: all .2s;
        }
        .theme-dot:hover, .theme-dot.active {
            transform: scale(1.2);
            border-color: var(--neon-glow);
        }

        #game-nav {
            display: flex;
            flex-direction: column;
        }
        #game-list {
            flex-grow: 1;
            overflow-y: auto;
        }
        #sidebar-footer {
            padding: 10px;
            border-top: 1px solid var(--border-color);
        }
        #hw-stats {
            padding: 10px;
            font-size: 12px;
            color: #888;
        }
        #hw-stats p { margin: 4px 0; }
        #hw-stats span { color: var(--text-color); font-weight: bold; }

        .draggable {
            position: fixed;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            background-color: var(--container-bg);
            border: 1px solid var(--border-color);
            border-radius: 15px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.5);
            z-index: 2000;
            display: none; /* Hidden by default */
        }
        .draggable-header {
            padding: 10px 15px;
            cursor: move;
            background-color: #252525;
            border-bottom: 1px solid var(--border-color);
            border-radius: 15px 15px 0 0;
            color: var(--neon-glow);
            user-select: none;
        }
        .close-btn {
            float: right;
            cursor: pointer;
            font-size: 20px;
        }
        .draggable-content {
            padding: 20px;
        }

        #news-ticker-container {
            position: fixed;
            bottom: 0;
            left: 0;
            width: 100%;
            height: 30px;
            background: #111;
            border-top: 1px solid var(--border-color);
            overflow: hidden;
        }
        #news-ticker {
            display: inline-block;
            white-space: nowrap;
            color: #888;
            line-height: 30px;
            animation: ticker-scroll 40s linear infinite;
        }
        @keyframes ticker-scroll {
            0% { transform: translateX(100%); }
            100% { transform: translateX(-100%); }
        }

        #user-profile {
            padding: 20px;
            text-align: center;
            border-bottom: 1px solid var(--border-color);
        }
        #user-avatar {
            width: 80px;
            height: 80px;
            border-radius: 50%;
            background-color: var(--neon-glow);
            margin: 0 auto 10px;
            box-shadow: 0 0 15px var(--neon-glow);
        }
        #user-name {
            margin: 0;
            color: var(--text-color);
        }
        #user-status {
            margin: 5px 0 0;
            font-size: 12px;
            color: var(--success-color);
        }

        #system-status-window {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background-color: rgba(13, 13, 13, 0.95);
            backdrop-filter: blur(10px);
            z-index: 3000;
            display: none; /* Hidden by default */
            flex-direction: column;
            align-items: center;
            justify-content: center;
            padding: 20px;
            box-sizing: border-box;
        }
        #close-status-button {
            position: absolute;
            top: 20px;
            right: 30px;
            font-size: 40px;
            background: none;
            border: none;
            color: white;
            cursor: pointer;
        }
        #system-status-window h2 {
            color: var(--neon-glow);
            text-shadow: 0 0 10px var(--neon-glow);
            font-size: 32px;
            margin-bottom: 40px;
        }
        .status-grid {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 30px;
            width: 80%;
        }
        .status-item {
            background: var(--container-bg);
            padding: 20px;
            border-radius: 15px;
            border: 1px solid var(--border-color);
            text-align: center;
        }
        .status-item h4 {
            margin: 0 0 10px;
            color: #aaa;
        }
        .status-item p {
            margin: 0;
            font-size: 20px;
            font-weight: bold;
        }
        .status-ok {
            color: var(--success-color);
        }
    </style>
</head>
<body>

    <div id="login-container">
        <div id="login-box">
            <h1>VENDOR.ME</h1>
            <input type="text" id="username" class="login-input" placeholder="Username" autocomplete="off">
            <input type="password" id="password" class="login-input" placeholder="Password" autocomplete="off">
            <button id="login-button" class="login-button">Authenticate</button>
        </div>
    </div>

    <div id="app-container">
        <header>
            <h1>VEND.ME</h1>
            <div class="header-controls">
                <div id="config-buttons">
                    <button class="config-btn">Save Config</button>
                    <button class="config-btn">Load Config</button>
                </div>
                <div id="theme-switcher">
                    <span>Theme:</span>
                    <div class="theme-dot" style="background-color: #00ffde;" data-color="#00ffde"></div>
                    <div class="theme-dot" style="background-color: #ff007f;" data-color="#ff007f"></div>
                    <div class="theme-dot" style="background-color: #7f00ff;" data-color="#7f00ff"></div>
                </div>
                <div id="status-indicator">Connected: Secure</div>
            </div>
        </header>

        <main>
            <nav id="game-nav">
                <div id="user-profile">
                    <div id="user-avatar"></div>
                    <h3 id="user-name">User</h3>
                    <p id="user-status">Status: Online</p>
                </div>
                <div id="game-list">
                    <!-- Game buttons will be injected here by JS -->
                </div>
                <div id="sidebar-footer">
                     <button id="status-button" class="game-button">System Status</button>
                    <div id="hw-stats">
                        <p>CPU: <span id="cpu-load">--</span>%</p>
                        <p>RAM: <span id="ram-usage">--</span>%</p>
                    </div>
                    <button id="about-button" class="game-button">About</button>
                </div>
            </nav>
            <div id="cheat-area">
                <div id="cheat-content">
                    <!-- Cheats for the selected game will be injected here -->
                </div>
            </div>
        </main>

        <div id="panic-button">PANIC</div>
    </div>

    <div id="about-window" class="draggable">
        <div class="draggable-header">About VEND.ME <span class="close-btn">&times;</span></div>
        <div class="draggable-content">
            <p>Version: 1.3.37</p>
            <p>Build Date: 2024-07-21</p>
            <p>Created by: Jules</p>
            <p>&copy; All Rights Reserved</p>
        </div>
    </div>

    <div id="news-ticker-container">
        <div id="news-ticker">
            <!-- News items will be injected by JS -->
        </div>
    </div>

    <div id="system-status-window">
        <button id="close-status-button">&times;</button>
        <h2>System Status</h2>
        <div class="status-grid">
            <div class="status-item">
                <h4>Injection Status</h4>
                <p class="status-ok">Kernel-Level</p>
            </div>
            <div class="status-item">
                <h4>VAC/BattlEye</h4>
                <p class="status-ok">Undetected</p>
            </div>
            <div class="status-item">
                <h4>Connection</h4>
                <p class="status-ok">Encrypted</p>
            </div>
            <div class="status-item">
                <h4>Last Update</h4>
                <p>2024-07-21 14:30 UTC</p>
            </div>
             <div class="status-item">
                <h4>Device Temp</h4>
                <p>42.5 C</p>
            </div>
             <div class="status-item">
                <h4>Uptime</h4>
                <p>0h 15m 42s</p>
            </div>
        </div>
    </div>

    <script>
        // --- SOUND EFFECTS (Base64 Encoded) ---
        const audioContext = new (window.AudioContext || window.webkitAudioContext)();
        const clickSound = 'data:audio/wav;base64,UklGRigAAABXQVZFZm10IBIAAAABAAEARKwAAIhYAQACABAAAABkYXRhAgAAAAEA';
        const activateSound = 'data:audio/wav;base64,UklGRlIAAABXQVZFZm10IBIAAAABAAEARKwAAIhYAQACABAAAABkYXRhUAAAAAEAAgADAAQABQAGAAcACAAJAAoACwAMAA0ADgAPABAAEwAAAAEAAAACAAADAAQABQAGAAcACAAJAAoACwAMAA0ADgAPABAAEwAAAAEAAAACAAADAAQABQAGAAcACAAJAAoACwAMAA0ADgAPABAAEw==';
        const deactivateSound = 'data:audio/wav;base64,UklGRlIAAABXQVZFZm10IBIAAAABAAEARKwAAIhYAQACABAAAABkYXRhUAAAAAEAAgADAAQABQAGAAcACAAJAAoACwAMAA0ADgAPABAAEwAAAAEAAAACAAADAAQABQAGAAcACAAJAAoACwAMAA0ADgAPABAAEwAAAAEAAAACAAADAAQABQAGAAcACAAJAAoACwAMAA0ADgAPABAAEw==';

        function playSound(soundUrl) {
            try {
                const source = audioContext.createBufferSource();
                fetch(soundUrl)
                    .then(response => response.arrayBuffer())
                    .then(arrayBuffer => audioContext.decodeAudioData(arrayBuffer))
                    .then(audioBuffer => {
                        source.buffer = audioBuffer;
                        source.connect(audioContext.destination);
                        source.start(0);
                    });
            } catch (e) {
                console.error("Web Audio API is not supported in this browser.", e);
            }
        }

        // --- GAME DATABASE ---
        const games = {
            "Warzone": {
                "Aimbot": [
                    { name: "Enable Aimbot", type: "toggle", default: false },
                    { name: "Aim FOV", type: "slider", min: 1, max: 100, default: 10 },
                    { name: "Aim Smoothing", type: "slider", min: 0, max: 100, default: 50 },
                ],
                "Visuals": [
                    { name: "Player ESP", type: "toggle", default: true },
                    { name: "Item ESP", type: "toggle", default: false },
                    { name: "Radar Hack", type: "toggle", default: true },
                ],
                "Misc": [
                    { name: "Unlock All", type: "toggle", default: false },
                    { name: "No Recoil", type: "toggle", default: true },
                ]
            },
            "BO6": {
                "Aimbot": [
                    { name: "Silent Aim", type: "toggle", default: false },
                    { name: "Trigger Bot", type: "toggle", default: false },
                    { name: "Aim Bone", type: "select", options: ["Head", "Neck", "Chest"], default: "Chest" },
                ],
                "ESP": [
                    { name: "Box ESP", type: "toggle", default: true },
                    { name: "Skeleton ESP", type: "toggle", default: false },
                    { name: "Visibility Check", type: "toggle", default: true },
                ]
            },
            "R6 Siege": {
                "Player Cheats": [
                    { name: "No Recoil / No Spread", type: "toggle", default: true },
                    { name: "Chams", type: "toggle", default: false },
                    { name: "Speed Hack", type: "slider", min: 100, max: 150, default: 100 },
                ],
                "Gadget Cheats": [
                    { name: "Infinite Gadgets", type: "toggle", default: false },
                    { name: "Drone ESP", type: "toggle", default: true },
                ]
            },
            "Fortnite": {
                "Combat": [
                    { name: "Aimbot", type: "toggle", default: false },
                    { name: "Soft Aim", type: "toggle", default: true },
                    { name: "Building Helper", type: "toggle", default: false },
                ],
                "Visuals": [
                    { name: "Player ESP", type: "toggle", default: true },
                    { name: "Loot ESP", type: "toggle", default: true },
                    { name: "Vehicle ESP", type: "toggle", default: false },
                ]
            },
            "GTA V": {
                "Player": [
                    { name: "God Mode", type: "toggle", default: false },
                    { name: "Infinite Ammo", type: "toggle", default: true },
                    { name: "Super Jump", type: "toggle", default: false },
                ],
                "Money": [
                    { name: "Money Drop", type: "toggle", default: false },
                    { name: "RP Multiplier", type: "slider", min: 1, max: 10, default: 2 },
                ]
            },
            "Apex Legends": {
                "Aimbot": [
                    { name: "Enable Aimbot", type: "toggle", default: false },
                    { name: "Prediction", type: "toggle", true },
                    { name: "Aim Distance", type: "slider", min: 50, max: 500, default: 200 },
                ],
                "Visuals": [
                    { name: "Glow ESP", type: "toggle", default: true },
                    { name: "Item Glow", type: "toggle", default: false },
                ]
            }
        };

        // --- UI LOGIC ---
        document.addEventListener('DOMContentLoaded', () => {
            // --- Element Selectors ---
            const loginButton = document.getElementById('login-button');
            const loginContainer = document.getElementById('login-container');
            const appContainer = document.getElementById('app-container');
            const usernameInput = document.getElementById('username');
            const gameList = document.getElementById('game-list');
            const cheatContent = document.getElementById('cheat-content');
            const panicButton = document.getElementById('panic-button');
            const themeSwitcher = document.getElementById('theme-switcher');
            const configButtons = document.querySelectorAll('.config-btn');
            const aboutButton = document.getElementById('about-button');
            const aboutWindow = document.getElementById('about-window');
            const closeAbout = aboutWindow.querySelector('.close-btn');
            const statusButton = document.getElementById('status-button');
            const statusWindow = document.getElementById('system-status-window');
            const closeStatusButton = document.getElementById('close-status-button');
            const newsTicker = document.getElementById('news-ticker');
            const cpuLoadEl = document.getElementById('cpu-load');
            const ramUsageEl = document.getElementById('ram-usage');
            const userNameEl = document.getElementById('user-name');

            // --- App State ---
            let currentUser = 'User';
            const newsItems = [
                'New profiles for BO6 added.', 'Kernel-level injection stability improved.', 'Security module updated.', 'Fortnite ESP rendering optimized.', 'Stealth mode now fully undetectable.'
            ];

            // --- Core Functions ---
            function showAlert(message, type = 'success') {
                const alert = document.createElement('div');
                alert.className = `alert ${type}`;
                alert.textContent = message;
                document.body.appendChild(alert);

                setTimeout(() => alert.classList.add('show'), 10);
                setTimeout(() => {
                    alert.classList.remove('show');
                    setTimeout(() => document.body.removeChild(alert), 500);
                }, 3000);
            }

            function initApp() {
                try {
                    // This is the critical part, do the UI switch first.
                    loginContainer.style.display = 'none';
                    appContainer.style.display = 'flex';

                    // Now initialize features. If any of this fails, the user is still on the main page.
                    loadGames();
                    if (Object.keys(games).length > 0) {
                        const firstGame = Object.keys(games)[0];
                        loadCheatsForGame(firstGame);
                        const firstGameButton = gameList.querySelector('.game-button');
                        if(firstGameButton) firstGameButton.classList.add('active');
                    }
                userNameEl.textContent = currentUser;
                    startHardwareStats();
                    populateNewsTicker();
                    makeDraggable(aboutWindow);
                } catch (e) {
                    console.error("Error initializing app:", e);
                    showAlert("Fatal Error: Could not initialize app.", "error");
                    // If something breaks, send them back to the login screen.
                    loginContainer.style.display = 'flex';
                    appContainer.style.display = 'none';
                }
            }

            // --- Event Listeners ---
            loginButton.addEventListener('click', () => {
                playSound(clickSound);
                currentUser = usernameInput.value || 'User'; // Capture username
                // The initApp function now handles the screen transition.
                initApp();
            });

            // Populate game navigation
            function loadGames() {
                gameList.innerHTML = '';
                for (const gameName in games) {
                    const button = document.createElement('button');
                    button.className = 'game-button';
                    button.textContent = gameName;
                    button.dataset.game = gameName;
                    button.addEventListener('click', (e) => {
                        playSound(clickSound);
                        document.querySelectorAll('#game-list .game-button').forEach(btn => btn.classList.remove('active'));
                        e.target.classList.add('active');
                        loadCheatsForGame(gameName);
                    });
                    gameList.appendChild(button);
                }
            }

            // Load cheats for a selected game
            function loadCheatsForGame(gameName) {
                cheatContent.innerHTML = '';
                const gameData = games[gameName];
                const title = document.createElement('h2');
                title.textContent = `${gameName} Cheats`;
                cheatContent.appendChild(title);

                for (const category in gameData) {
                    const categoryDiv = document.createElement('div');
                    categoryDiv.className = 'cheat-category';
                    const categoryTitle = document.createElement('h3');
                    categoryTitle.textContent = category;
                    categoryDiv.appendChild(categoryTitle);

                    gameData[category].forEach(cheat => {
                        const itemDiv = document.createElement('div');
                        itemDiv.className = 'cheat-item';
                        const label = document.createElement('label');
                        label.textContent = cheat.name;
                        itemDiv.appendChild(label);

                        if (cheat.type === 'toggle') {
                            const switchLabel = document.createElement('label');
                            switchLabel.className = 'toggle-switch';
                            const input = document.createElement('input');
                            input.type = 'checkbox';
                            input.checked = cheat.default;
                            input.addEventListener('change', (e) => {
                                const action = e.target.checked ? 'Activated' : 'Deactivated';
                                playSound(e.target.checked ? activateSound : deactivateSound);
                                showAlert(`${cheat.name} ${action}`, e.target.checked ? 'success' : 'error');
                            });
                            const sliderSpan = document.createElement('span');
                            sliderSpan.className = 'toggle-slider';
                            switchLabel.appendChild(input);
                            switchLabel.appendChild(sliderSpan);
                            itemDiv.appendChild(switchLabel);
                        } else if (cheat.type === 'slider') {
                            const sliderContainer = document.createElement('div');
                            sliderContainer.className = 'slider-container';
                            const slider = document.createElement('input');
                            slider.type = 'range';
                            slider.min = cheat.min;
                            slider.max = cheat.max;
                            slider.value = cheat.default;
                            slider.className = 'slider';
                            const valueSpan = document.createElement('span');
                            valueSpan.className = 'slider-value';
                            valueSpan.textContent = slider.value;
                            slider.addEventListener('input', () => valueSpan.textContent = slider.value);
                            slider.addEventListener('change', () => {
                                playSound(clickSound);
                                showAlert(`${cheat.name} set to ${slider.value}`);
                            });
                            sliderContainer.appendChild(slider);
                            sliderContainer.appendChild(valueSpan);
                            itemDiv.appendChild(sliderContainer);
                        }
                        categoryDiv.appendChild(itemDiv);
                    });
                    cheatContent.appendChild(categoryDiv);
                }
            }

            panicButton.addEventListener('click', () => {
                playSound(deactivateSound);
                document.body.innerHTML = '<div style="width:100vw;height:100vh;display:flex;justify-content:center;align-items:center;color:white;font-size:24px;font-family:sans-serif;">Connection Lost...</div>';
            });

            themeSwitcher.addEventListener('click', (e) => {
                if (e.target.classList.contains('theme-dot')) {
                    const color = e.target.dataset.color;
                    document.documentElement.style.setProperty('--neon-glow', color);
                    document.querySelectorAll('.theme-dot').forEach(dot => dot.classList.remove('active'));
                    e.target.classList.add('active');
                    playSound(clickSound);
                }
            });

            configButtons.forEach(btn => {
                btn.addEventListener('click', () => {
                    playSound(clickSound);
                    showAlert(`${btn.textContent} successful!`, 'success');
                });
            });

            aboutButton.addEventListener('click', () => {
                playSound(clickSound);
                aboutWindow.style.display = 'block';
            });

            closeAbout.addEventListener('click', () => {
                playSound(clickSound);
                aboutWindow.style.display = 'none';
            });

            statusButton.addEventListener('click', () => {
                playSound(clickSound);
                statusWindow.style.display = 'flex';
            });

            closeStatusButton.addEventListener('click', () => {
                playSound(deactivateSound);
                statusWindow.style.display = 'none';
            });

            function makeDraggable(elmnt) {
                let pos1 = 0, pos2 = 0, pos3 = 0, pos4 = 0;
                const header = elmnt.querySelector(".draggable-header");
                if (header) {
                    header.onmousedown = dragMouseDown;
                } else {
                    elmnt.onmousedown = dragMouseDown;
                }

                function dragMouseDown(e) {
                    e = e || window.event;
                    e.preventDefault();
                    pos3 = e.clientX;
                    pos4 = e.clientY;
                    document.onmouseup = closeDragElement;
                    document.onmousemove = elementDrag;
                }

                function elementDrag(e) {
                    e = e || window.event;
                    e.preventDefault();
                    pos1 = pos3 - e.clientX;
                    pos2 = pos4 - e.clientY;
                    pos3 = e.clientX;
                    pos4 = e.clientY;
                    elmnt.style.top = (elmnt.offsetTop - pos2) + "px";
                    elmnt.style.left = (elmnt.offsetLeft - pos1) + "px";
                }

                function closeDragElement() {
                    document.onmouseup = null;
                    document.onmousemove = null;
                }
            }

            function startHardwareStats() {
                setInterval(() => {
                    cpuLoadEl.textContent = (Math.random() * (90 - 50) + 50).toFixed(2);
                    ramUsageEl.textContent = (Math.random() * (75 - 40) + 40).toFixed(2);
                }, 1500);
            }

            function populateNewsTicker() {
                let fullText = '';
                for (let i = 0; i < 5; i++) {
                    fullText += newsItems.join(' +++ ') + ' +++ ';
                }
                newsTicker.textContent = fullText;
            }

            window.addEventListener('contextmenu', e => e.preventDefault());
        });
    </script>
</body>
</html>
)raw";

// =================================================================================================
// ARDUINO SKETCH
// =================================================================================================

void setup() {
  // --- Display Initialization ---
  tft.init();
  tft.setRotation(1); // Landscape mode
  tft.setBrightness(255);
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextDatum(lgfx::v1::textdatum_t::middle_center);
  tft.setFont(&fonts::Orbitron_Light_32);
  tft.drawString("VENDOR.ME", tft.width() / 2, tft.height() / 2 - 20);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setFont(&fonts::FreeSansBold12pt7b);
  tft.drawString("CHEAT ENGINE", tft.width() / 2, tft.height() / 2 + 30);

  // --- WiFi and Server Initialization ---
  Serial.begin(115200);

  // Start WiFi Access Point
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access Point IP address: ");
  Serial.println(IP);

  // Define server routes
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Start server
  server.begin();
}

void loop() {
  // The ESPAsyncWebServer runs in the background.
  // The display is static, so no need to update it in the loop.
  // We can keep the loop empty or use it for other tasks if needed.
  delay(1000);
}
