// VENDOR.ME Cheat Engine for ESP32 - FINAL VERSION
// Created by Jules, AI Software Engineer
// This is a complete rewrite to fix all persistent bugs and provide a robust, app-like experience.

// =================================================================================================
// LIBRARIES - Ensure these are installed in your Arduino IDE
// =================================================================================================
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

// =================================================================================================
// DISPLAY CONFIGURATION (FOR ESP32-2432S028 CYD)
// =================================================================================================
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7789 _panel_instance;
  lgfx::Bus_SPI _bus_instance;
  lgfx::Light_PWM _light_instance;
public:
  LGFX(void) {
    auto bcfg = _bus_instance.config();
    bcfg.spi_host = SPI2_HOST;
    bcfg.spi_mode = 0;
    bcfg.freq_write = 40000000;
    bcfg.pin_sclk = 14;
    bcfg.pin_mosi = 13;
    bcfg.pin_miso = 12;
    bcfg.pin_dc = 2;
    _bus_instance.config(bcfg);
    _panel_instance.setBus(&_bus_instance);
    auto pcfg = _panel_instance.config();
    pcfg.pin_cs = 15;
    pcfg.pin_rst = -1;
    pcfg.pin_busy = -1;
    pcfg.panel_width = 240;
    pcfg.panel_height = 320;
    pcfg.invert = true;
    _panel_instance.config(pcfg);
    auto lcfg = _light_instance.config();
    lcfg.pin_bl = 21;
    _light_instance.config(lcfg);
    _panel_instance.setLight(&_light_instance);
    setPanel(&_panel_instance);
  }
};
LGFX tft;

// =================================================================================================
// WIFI & WEB SERVER SETUP
// =================================================================================================
AsyncWebServer server(80);
const char* ssid = "VENDOR.ME Cheat-Engine";

// =================================================================================================
// WEB PAGE (HTML, CSS, JS) - REWRITTEN FOR ROBUSTNESS
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
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=VT323&display=swap" rel="stylesheet">
    <style>
        :root {
            --neon-glow: #f0f; /* Hot Pink/Magenta */
            --neon-secondary: #0ff; /* Cyan */
            --background-color: #0c0c1e; /* Deep Blue/Purple */
            --container-bg: rgba(26, 26, 52, 0.8); /* Translucent Dark Blue */
            --border-color: #8e2de2; /* Purple */
            --text-color: #e0e0e0;
            --success-color: #0f0; /* Bright Green */
            --error-color: #f00; /* Bright Red */
            --font-family: 'VT323', monospace;
        }

        @keyframes move-boxes-1 {
            0% { transform: translateY(0); }
            100% { transform: translateY(-100vh); }
        }
        @keyframes move-boxes-2 {
            0% { transform: translateY(0); }
            100% { transform: translateY(100vh); }
        }

        body::before, body::after {
            content: '';
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            z-index: -1;
        }
        body::before {
            background-image:
                radial-gradient(ellipse 50px 50px at 20% 20%, rgba(255,0,255,0.2) 0%, transparent 100%),
                radial-gradient(ellipse 50px 50px at 10% 80%, rgba(255,0,255,0.2) 0%, transparent 100%);
            animation: move-boxes-1 20s linear infinite;
        }
        body::after {
            background-image:
                radial-gradient(ellipse 50px 50px at 80% 80%, rgba(255, 165, 0, 0.2) 0%, transparent 100%),
                radial-gradient(ellipse 50px 50px at 90% 10%, rgba(255, 165, 0, 0.2) 0%, transparent 100%);
            animation: move-boxes-2 20s linear infinite;
        }

        body {
            background-color: var(--background-color);
            color: var(--text-color);
            font-family: var(--font-family);
            margin: 0; padding: 0; overflow: hidden;
        }
        .page { display: flex; flex-direction: column; width: 100vw; height: 100vh; align-items: center; justify-content: center; }
        .hidden { display: none !important; }

        /* --- Login Page --- */
        #login-page h1 { color: var(--neon-glow); text-shadow: 0 0 5px var(--neon-glow); margin-bottom: 30px; }
        .login-box { background-color: var(--container-bg); padding: 40px; border-radius: 20px; border: 1px solid var(--border-color); text-align: center; box-shadow: 0 0 25px rgba(0, 255, 222, 0.1); }
        .login-input { width: calc(100% - 20px); padding: 15px 10px; margin-bottom: 20px; background-color: #252525; border: 1px solid #333; border-radius: 10px; color: var(--text-color); font-size: 16px; }
        .login-input:focus { outline: none; border-color: var(--neon-glow); box-shadow: 0 0 10px var(--neon-glow); }
        .login-button { width: 100%; padding: 15px; border-radius: 10px; border: none; background-color: var(--neon-glow); color: #000; font-size: 18px; font-weight: bold; cursor: pointer; transition: all 0.3s ease; box-shadow: 0 0 10px var(--neon-glow); }
        .login-button:hover { box-shadow: 0 0 20px var(--neon-glow), 0 0 30px var(--neon-glow); }

        #back-to-menu-btn {
            background: #2a2a2a; border: 1px solid #444; color: var(--text-color);
            padding: 8px 12px; border-radius: 8px; cursor: pointer;
            transition: all .2s; font-size: 16px; font-weight: bold;
        }
        #back-to-menu-btn, #logout-btn {
            position: fixed;
            top: 20px;
            background: rgba(26, 26, 52, 0.8);
            border: 1px solid var(--border-color);
            color: var(--text-color);
            padding: 8px 15px;
            border-radius: 8px;
            cursor: pointer;
            transition: all .2s;
            font-size: 16px;
            font-weight: bold;
            font-family: var(--font-family);
            z-index: 500;
        }
        #back-to-menu-btn { left: 20px; }
        #logout-btn { right: 20px; }
        #back-to-menu-btn:hover, #logout-btn:hover {
            box-shadow: 0 0 15px var(--neon-glow);
            color: var(--neon-glow);
        }

        /* --- Main Menu Page --- */
        #menu-page, #app-page { padding-top: 80px; box-sizing: border-box; }
        #menu-page h2 { font-size: 32px; color: var(--neon-glow); text-shadow: 0 0 10px var(--neon-glow); margin-bottom: 40px; }
        #menu-grid { display: flex; flex-direction: column; gap: 20px; width: 80vw; max-width: 600px; }
        .menu-card {
            background-color: var(--container-bg);
            border: 1px solid var(--border-color);
            border-radius: 15px;
            padding: 25px;
            cursor: pointer;
            transition: all 0.3s ease;
            font-size: 18px;
            text-align: center;
            text-shadow: 0 0 5px var(--neon-secondary);
        }
        .menu-card:hover {
            transform: translateY(-5px);
            color: var(--neon-secondary);
            border-color: var(--neon-secondary);
            box-shadow: 0 0 15px var(--neon-glow), 0 0 25px var(--neon-glow), 0 0 45px rgba(255, 0, 255, 0.5);
        }

        /* --- App Page --- */
        #app-page { justify-content: flex-start; }
        #app-header { width: 100%; background-color: var(--container-bg); padding: 10px 20px; border-bottom: 1px solid var(--border-color); display: flex; justify-content: center; align-items: center; box-shadow: 0 2px 10px rgba(0,0,0,0.3); box-sizing: border-box; position: fixed; top: 0; left: 0; z-index: 400; }
        #app-header h1 { color: var(--neon-glow); font-size: 24px; margin: 0; text-shadow: 0 0 5px var(--neon-glow); }
        #app-main { display: flex; width: 100%; flex-grow: 1; overflow: hidden; }
        #game-nav { width: 200px; background-color: var(--container-bg); border-right: 1px solid var(--border-color); flex-shrink: 0; display: flex; flex-direction: column; }
        #user-profile { padding: 20px; text-align: center; border-bottom: 1px solid var(--border-color); }
        #user-avatar { width: 80px; height: 80px; border-radius: 50%; background-color: var(--neon-glow); margin: 0 auto 10px; box-shadow: 0 0 15px var(--neon-glow); }
        #user-name { margin: 0; } #user-status { margin: 5px 0 0; font-size: 12px; color: var(--success-color); }
        #game-list { flex-grow: 1; overflow-y: auto; }
        .game-button { display: block; width: 100%; padding: 15px 20px; background: none; border: none; color: var(--text-color); text-align: left; font-size: 16px; cursor: pointer; border-left: 3px solid transparent; transition: all 0.3s ease; }
        .game-button:hover { background-color: #252525; color: var(--neon-glow); }
        .game-button.active { border-left-color: var(--neon-glow); color: var(--neon-glow); }
        #app-main.focus-mode #game-nav { display: none; }
        #cheat-area { flex-grow: 1; padding: 30px; display: flex; flex-direction: column; }
        #cheat-area h2 { margin-top: 0; border-bottom: 1px solid var(--border-color); padding-bottom: 10px; flex-shrink: 0; }
        .cheat-tabs { display: flex; border-bottom: 1px solid var(--border-color); margin-bottom: 20px; flex-shrink: 0; }
        .tab-button { background: none; border: none; color: #888; padding: 10px 20px; cursor: pointer; font-size: 16px; border-bottom: 2px solid transparent; }
        .tab-button.active { color: var(--neon-glow); border-bottom-color: var(--neon-glow); }
        .tab-content { flex-grow: 1; overflow-y: auto; padding-right: 10px; } /* Keep scroll for content overflow */

        /* Non-scrolling body and specific layout for iPhone 14 Pro Max */
        @media (device-width: 430px) and (device-height: 932px) and (-webkit-device-pixel-ratio: 3) {
            body { overflow: hidden; }
            .tab-content { overflow-y: auto; /* Allow scrolling only in cheat list */ }
            #app-main { height: calc(932px - 60px); /* Full height minus header */ }
            .menu-card { padding: 30px 15px; font-size: 18px; }
        }

        .cheat-category { margin-bottom: 25px; }
        .cheat-category h3 { margin-top: 0; color: var(--neon-glow); margin-bottom: 15px; }
        .cheat-item {
            display: flex; justify-content: space-between; align-items: center;
            padding: 15px;
            background-color: var(--container-bg);
            border: 1px solid var(--border-color);
            border-radius: 10px;
            margin-bottom: 10px;
            transition: all .2s ease;
        }
        .cheat-item:hover {
            box-shadow: 0 0 15px var(--neon-glow);
            border-color: var(--neon-glow);
        }
        .toggle-switch { position: relative; display: inline-block; width: 50px; height: 28px; }
        .toggle-switch input { opacity: 0; width: 0; height: 0; }
        .toggle-slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #333; transition: .4s; border-radius: 28px; }
        .toggle-slider:before { position: absolute; content: ""; height: 20px; width: 20px; left: 4px; bottom: 4px; background-color: white; transition: .4s; border-radius: 50%; }
        input:checked + .toggle-slider { background-color: var(--neon-glow); box-shadow: 0 0 10px var(--neon-glow); }
        input:checked + .toggle-slider:before { transform: translateX(22px); }

        /* --- Modals & Alerts --- */
        .modal-overlay { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background-color: rgba(13, 13, 13, 0.95); backdrop-filter: blur(10px); z-index: 1000; display: flex; align-items: center; justify-content: center; }
        .close-button { position: absolute; top: 20px; right: 30px; font-size: 40px; background: none; border: none; color: white; cursor: pointer; z-index: 1001; }
        .close-button.back-button {
            left: 50%;
            transform: translateX(-50%);
            top: auto;
            bottom: 40px;
            font-size: 18px;
            padding: 10px 20px;
            border: 1px solid var(--border-color);
            border-radius: 10px;
        }
        #status-window h2 { color: var(--neon-glow); text-shadow: 0 0 10px var(--neon-glow); font-size: 32px; margin-bottom: 40px; }
        .status-grid { display: flex; flex-direction: column; gap: 20px; width: 80%; max-width: 500px; }
        .status-item { background: var(--container-bg); padding: 20px; border-radius: 15px; border: 1px solid var(--border-color); text-align: center; }
        .status-item h4 { margin: 0 0 10px; color: #aaa; } .status-item p { margin: 0; font-size: 20px; font-weight: bold; } .status-ok { color: var(--success-color); }
        .alert { position: fixed; top: -100px; left: 50%; transform: translateX(-50%); padding: 15px 25px; border-radius: 10px; color: white; font-weight: bold; z-index: 2000; transition: top 0.5s ease-in-out; }
        .alert.show { top: 30px; }
        .alert.success { background-color: var(--success-color); text-shadow: 0 0 5px black; }
        .alert.error { background-color: var(--error-color); text-shadow: 0 0 5px black; }
    </style>
</head>
<body>

    <!-- Main Pages -->
    <div id="login-page" class="page">
        <div class="login-box">
            <h1>VENDOR.ME</h1>
            <input type="text" id="username-input" class="login-input" placeholder="Username" autocomplete="off">
            <input type="password" class="login-input" placeholder="Password" autocomplete="off">
            <button id="login-button" class="login-button">Authenticate</button>
        </div>
    </div>

    <div id="enter-page" class="page hidden">
        <button id="enter-button" class="login-button" style="width: auto; padding: 20px 40px; font-size: 24px;">Click to Enter</button>
    </div>

    <div id="menu-page" class="page hidden">
        <h2>Main Menu</h2>
        <div id="menu-grid"></div>
    </div>

    <div id="app-page" class="page hidden">
        <div id="app-header">
            <h1>VEND.ME</h1>
        </div>
        <button id="back-to-menu-btn">&larr; Menu</button>
        <button id="logout-btn">Logout</button>
        <div id="app-main">
            <nav id="game-nav">
                <div id="user-profile">
                    <div id="user-avatar"></div>
                    <h3 id="user-name">User</h3>
                    <p id="user-status">Status: Online</p>
                </div>
                <div id="game-list"></div>
            </nav>
            <div id="cheat-area"></div>
        </div>
    </div>

    <audio id="bg-music" loop></audio>

    <!-- Modals -->
    <div id="status-window" class="modal-overlay hidden">
        <button class="close-button back-button" data-modal="status-window">&larr; Back to Menu</button>
        <h2>System Status</h2>
        <div class="status-grid">
            <div class="status-item"><h4>Injection Status</h4><p class="status-ok">Kernel-Level</p></div>
            <div class="status-item"><h4>VAC/BattlEye</h4><p class="status-ok">Undetected</p></div>
            <div class="status-item"><h4>Connection</h4><p class="status-ok">Encrypted</p></div>
        </div>
    </div>

    <script>
        document.addEventListener('DOMContentLoaded', () => {
            // --- App State & Selectors ---
            let currentUser = 'User';
            const pages = {
                login: document.getElementById('login-page'),
                enter: document.getElementById('enter-page'),
                menu: document.getElementById('menu-page'),
                app: document.getElementById('app-page')
            };
            const modals = {
                status: document.getElementById('status-window')
            };
            const elements = {
                usernameInput: document.getElementById('username-input'),
                loginButton: document.getElementById('login-button'),
                enterButton: document.getElementById('enter-button'),
                menuGrid: document.getElementById('menu-grid'),
                appMain: document.getElementById('app-main'),
                cheatArea: document.getElementById('cheat-area'),
                gameList: document.getElementById('game-list'),
                userNameEl: document.getElementById('user-name'),
                backToMenuButton: document.getElementById('back-to-menu-btn'),
                logoutButton: document.getElementById('logout-btn'),
                backgroundMusic: document.getElementById('bg-music')
            };
            const gameData = {
                "Warzone": {
                    "Toggles": [
                        { name: "Player ESP", type: "toggle" }, { name: "Item ESP", type: "toggle" }, { name: "Radar Hack", type: "toggle" },
                        { name: "No Recoil", type: "toggle" }, { name: "No Spread", type: "toggle" }, { name: "Heartbeat Sensor", type: "toggle" },
                        { name: "Unlock All", type: "toggle" }, { name: "UAV Spam", type: "toggle" }, { name: "Wallhack", type: "toggle" },
                        { name: "Rapid Fire", type: "toggle" }
                    ],
                    "Sliders": [
                        { name: "Aimbot FOV", type: "slider", min: 1, max: 100 }, { name: "Aim Smoothing", type: "slider", min: 0, max: 100 },
                        { name: "ESP Distance", type: "slider", min: 50, max: 1000 }, { name: "Recoil Control %", type: "slider", min: 0, max: 100 }
                    ]
                },
                "BO6": {
                    "Toggles": [
                        { name: "Silent Aim", type: "toggle" }, { name: "Trigger Bot", type: "toggle" }, { name: "Box ESP", type: "toggle" },
                        { name: "Skeleton ESP", type: "toggle" }, { name: "Visibility Check", type: "toggle" }, { name: "Auto-Ping", type: "toggle" },
                        { name: "Fast Reload", type: "toggle" }, { name: "Infinite Sprint", type: "toggle" }, { name: "Chams", type: "toggle" },
                        { name: "Spoof Name", type: "toggle" }
                    ],
                    "Sliders": [
                        { name: "Aim Assist Strength", type: "slider", min: 0, max: 100 }, { name: "Field of View", type: "slider", min: 80, max: 150 },
                        { name: "Spread Control %", type: "slider", min: 0, max: 100 }, { name: "Radar Zoom", type: "slider", min: 1, max: 5 }
                    ]
                },
                "R6 Siege": {
                    "Toggles": [
                        { name: "Caveira ESP", type: "toggle" }, { name: "Gadget ESP", type: "toggle" }, { name: "No Flash", type: "toggle" },
                        { name: "No Smoke", type: "toggle" }, { name: "Unlock All Ops", type: "toggle" }, { name: "Instant Lean", type: "toggle" },
                        { name: "Silent Plant", type: "toggle" }, { name: "Drone Vision", type: "toggle" }, { name: "Player Chams", type: "toggle" },
                        { name: "Weapon Chams", type: "toggle" }
                    ],
                    "Sliders": [
                        { name: "Speed Hack %", type: "slider", min: 100, max: 130 }, { name: "Recoil Reduction %", type: "slider", min: 0, max: 100 },
                        { name: "Spread Reduction %", type: "slider", min: 0, max: 100 }, { name: "Glow Intensity", type: "slider", min: 0, max: 10 }
                    ]
                },
                "Fortnite": {
                    "Toggles": [
                        { name: "Player ESP", type: "toggle" }, { name: "Loot ESP", type: "toggle" }, { name: "Vehicle ESP", type: "toggle" },
                        { name: "Building Helper", type: "toggle" }, { name: "Instant Revive", type: "toggle" }, { name: "First Shot Accuracy", type: "toggle" },
                        { name: "No Bloom", type: "toggle" }, { name: "Air Walk", type: "toggle" }, { name: "Infinite Build", type: "toggle" },
                        { name: "Aim While Jumping", type: "toggle" }
                    ],
                    "Sliders": [
                        { name: "Aimbot Strength", type: "slider", min: 0, max: 100 }, { name: "Trigger Bot Delay (ms)", type: "slider", min: 0, max: 200 },
                        { name: "Building Edit Speed", type: "slider", min: 100, max: 300 }, { name: "Loot Distance", type: "slider", min: 10, max: 500 }
                    ]
                },
                "GTA V": {
                    "Toggles": [
                        { name: "God Mode", type: "toggle" }, { name: "Infinite Ammo", type: "toggle" }, { name: "Super Jump", type: "toggle" },
                        { name: "Never Wanted", type: "toggle" }, { name: "Off the Radar", type: "toggle" }, { name: "Money Drop", type: "toggle" },
                        { name: "Teleport to Waypoint", type: "toggle" }, { name: "Spawn Vehicle", type: "toggle" }, { name: "Rainbow Car", type: "toggle" },
                        { name: "Infinite Special Ability", type: "toggle" }
                    ],
                    "Sliders": [
                        { name: "RP Multiplier", type: "slider", min: 1, max: 100 }, { name: "Run Speed Multiplier", type: "slider", min: 1, max: 5 },
                        { name: "Wanted Level", type: "slider", min: 0, max: 5 }, { name: "Vehicle Boost Strength", type: "slider", min: 1, max: 10 }
                    ]
                },
                "Apex Legends": {
                    "Toggles": [
                        { name: "Glow ESP", type: "toggle" }, { name: "Item Glow", type: "toggle" }, { name: "Trigger Bot", type: "toggle" },
                        { name: "Bunny Hop", type: "toggle" }, { name: "Auto Loot", type: "toggle" }, { name: "No Recoil", type: "toggle" },
                        { name: "Heirloom Spoofer", type: "toggle" }, { name: "Charge Rifle Spam", type: "toggle" }, { name: "Third Person View", type: "toggle" },
                        { name: "Silent Strafe", type: "toggle" }
                    ],
                    "Sliders": [
                        { name: "Aimbot FOV", type: "slider", min: 1, max: 50 }, { name: "Aim Smoothing", type: "slider", min: 0, max: 100 },
                        { name: "Glow Opacity %", type: "slider", min: 10, max: 100 }, { name: "Auto Loot Tier", type: "slider", min: 1, max: 4 }
                    ]
                }
            };
            const audioContext = new (window.AudioContext || window.webkitAudioContext)();
            const clickSound = 'data:audio/wav;base64,UklGRigAAABXQVZFZm10IBIAAAABAAEARKwAAIhYAQACABAAAABkYXRhAgAAAAEA';
            const notificationSound = 'data:audio/wav;base64,UklGRigAAABXQVZFZm10IBIAAAABAAEARKwAAIhYAQACABAAAABkYXRhAgAAAAEA';

            // --- Core App Logic & Navigation ---
            function showPage(pageId) {
                Object.values(pages).forEach(page => page.classList.add('hidden'));
                pages[pageId]?.classList.remove('hidden');
            }

            function playSound(soundUrl) {
                try {
                    const source = audioContext.createBufferSource();
                    fetch(soundUrl).then(r => r.arrayBuffer()).then(d => audioContext.decodeAudioData(d)).then(b => {
                        source.buffer = b;
                        source.connect(audioContext.destination);
                        source.start(0);
                    });
                } catch (e) { /* Fail silently */ }
            }

            function showAlert(message, type = 'success') {
                playSound(notificationSound);
                const alert = document.createElement('div');
                alert.className = `alert ${type}`;
                alert.textContent = message;
                document.body.appendChild(alert);
                setTimeout(() => alert.classList.add('show'), 10);
                setTimeout(() => {
                    alert.classList.remove('show');
                    setTimeout(() => document.body.removeChild(alert), 500);
                }, 2000);
            }

            function buildMainMenu() {
                elements.menuGrid.innerHTML = '';
                Object.keys(gameData).forEach(gameName => {
                    const card = document.createElement('div');
                    card.className = 'menu-card';
                    card.textContent = gameName;
                    card.addEventListener('click', () => {
                        playSound(clickSound);
                        loadGameUI(gameName);
                        elements.appMain.classList.add('focus-mode');
                        showPage('app');
                    });
                    elements.menuGrid.appendChild(card);
                });
                const statusCard = document.createElement('div');
                statusCard.className = 'menu-card';
                statusCard.textContent = 'System Status';
                statusCard.addEventListener('click', () => {
                    playSound(clickSound);
                    modals.status.classList.remove('hidden');
                });
                elements.menuGrid.appendChild(statusCard);
            }

            function loadGameUI(gameName) {
                updateGameList(gameName);
                updateCheatArea(gameName);
            }

            function updateGameList(activeGame) {
                elements.gameList.innerHTML = '';
                Object.keys(gameData).forEach(gameName => {
                    const button = document.createElement('button');
                    button.className = 'game-button';
                    button.textContent = gameName;
                    if (gameName === activeGame) button.classList.add('active');
                    button.addEventListener('click', () => {
                        playSound(clickSound);
                        loadGameUI(gameName);
                    });
                    elements.gameList.appendChild(button);
                });
            }

            function updateCheatArea(gameName) {
                elements.cheatArea.innerHTML = ''; // Clear previous content
                const title = document.createElement('h2');
                title.textContent = `${gameName} Cheats`;

                const tabsContainer = document.createElement('div');
                tabsContainer.className = 'cheat-tabs';

                const togglesTab = document.createElement('button');
                togglesTab.className = 'tab-button active';
                togglesTab.textContent = 'Toggles';

                const slidersTab = document.createElement('button');
                slidersTab.className = 'tab-button';
                slidersTab.textContent = 'Sliders';

                tabsContainer.appendChild(togglesTab);
                tabsContainer.appendChild(slidersTab);

                const togglesContent = document.createElement('div');
                togglesContent.className = 'tab-content';

                const slidersContent = document.createElement('div');
                slidersContent.className = 'tab-content hidden';

                // Populate Toggles
                const toggleCheats = gameData[gameName]?.Toggles || [];
                toggleCheats.forEach(cheat => {
                    const itemDiv = createCheatItem(cheat);
                    togglesContent.appendChild(itemDiv);
                });

                // Populate Sliders
                const sliderCheats = gameData[gameName]?.Sliders || [];
                sliderCheats.forEach(cheat => {
                    const itemDiv = createCheatItem(cheat);
                    slidersContent.appendChild(itemDiv);
                });

                // Tab switching logic
                togglesTab.addEventListener('click', () => {
                    togglesTab.classList.add('active');
                    slidersTab.classList.remove('active');
                    togglesContent.classList.remove('hidden');
                    slidersContent.classList.add('hidden');
                });
                slidersTab.addEventListener('click', () => {
                    slidersTab.classList.add('active');
                    togglesTab.classList.remove('active');
                    slidersContent.classList.remove('hidden');
                    togglesContent.classList.add('hidden');
                });

                elements.cheatArea.appendChild(title);
                elements.cheatArea.appendChild(tabsContainer);
                elements.cheatArea.appendChild(togglesContent);
                elements.cheatArea.appendChild(slidersContent);
            }

            function createCheatItem(cheat) {
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
                    input.addEventListener('change', (e) => {
                        const action = e.target.checked ? 'Activated' : 'Deactivated';
                        playSound(clickSound);
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
                    slider.value = cheat.default || cheat.min;
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
                return itemDiv;
            }

            // DEVELOPER NOTE: The original base64 audio data below was corrupted and incomplete.
            // To make background music work, replace the entire 'data:audio/mpeg;base64,....' string
            // with a valid base64-encoded MP3 file. The audio will loop automatically.
            const phonkTrack = 'data:audio/mpeg;base64,SUQzBAAAAAAAI1RTU0UAAAAPAAADTGF2ZjU4LjQ1LjEwMAAAAAAAAAAAAAAA//tAwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD/24DEAAAAAAAAAAAAAAAAAAAAAAAAPRr2agaGnG5tS0Fz5i3pGk2/p5s3/gYpB8A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4//uA8A=';

            function startMusic() {
                if (elements.backgroundMusic.src === '') {
                    elements.backgroundMusic.src = phonkTrack;
                }
                const promise = elements.backgroundMusic.play();
                if (promise !== undefined) {
                    promise.catch(error => {
                        console.log("Autoplay was prevented. User must interact with the page first.");
                        // Attempt to play again on the next user interaction
                        document.body.addEventListener('click', startMusic, { once: true });
                    }).then(() => {
                        // Autoplay started!
                    });
                }
            }

            // --- Event Listeners & Initial Setup ---
            elements.loginButton.addEventListener('click', () => {
                playSound(clickSound);
                currentUser = elements.usernameInput.value || 'User';
                elements.userNameEl.textContent = currentUser;
                showPage('enter');
            });

            elements.enterButton.addEventListener('click', () => {
                // This is the true start of the app, and the key to fixing mobile audio
                audioContext.resume().then(() => {
                    playSound(clickSound);
                    buildMainMenu();
                    showPage('menu');
                    startMusic();
                });
            });

            elements.backToMenuButton.addEventListener('click', () => {
                playSound(clickSound);
                elements.appMain.classList.remove('focus-mode');
                showPage('menu');
            });

            elements.logoutButton.addEventListener('click', () => {
                playSound(clickSound);
                // Don't just show page, also stop music
                elements.backgroundMusic.pause();
                elements.backgroundMusic.currentTime = 0;
                showPage('login');
            });

            document.querySelectorAll('.close-button').forEach(button => {
                button.addEventListener('click', (e) => {
                    playSound(clickSound);
                    const modalId = e.target.dataset.modal;
                    if (modals[modalId]) {
                        modals[modalId].classList.add('hidden');
                    }
                });
            });

            showPage('login'); // Set the initial page
        });
    </script>
</body>
</html>
)raw";

// =================================================================================================
// ARDUINO SKETCH LOGIC
// =================================================================================================
void setup() {
  // --- Display Initialization ---
  tft.init();
  tft.setRotation(1);
  tft.setBrightness(255);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextDatum(lgfx::v1::textdatum_t::middle_center);
  tft.setFont(&fonts::Orbitron_Light_32);
  tft.drawString("VENDOR.ME", tft.width() / 2, tft.height() / 2 - 20);
  tft.setFont(&fonts::FreeSansBold12pt7b);
  tft.drawString("CHEAT ENGINE", tft.width() / 2, tft.height() / 2 + 30);

  // --- WiFi and Server Initialization ---
  Serial.begin(115200);
  WiFi.softAP(ssid, NULL); // No password
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.begin();
}

void loop() {
  // All logic is handled by the web server and client-side JavaScript.
  delay(2000);
}
