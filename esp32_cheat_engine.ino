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
    <style>
        :root {
            --neon-glow: #00ffde; --background-color: #0d0d0d; --container-bg: #1a1a1a;
            --border-color: #2a2a2a; --text-color: #e0e0e0; --success-color: #00ff8c; --error-color: #ff4d4d;
            --font-family: 'Segoe UI', 'Roboto', 'Helvetica Neue', sans-serif;
        }
        body { background-color: var(--background-color); color: var(--text-color); font-family: var(--font-family); margin: 0; padding: 0; overflow: hidden; }
        .page { display: flex; flex-direction: column; width: 100vw; height: 100vh; align-items: center; justify-content: center; }
        .hidden { display: none !important; }

        /* --- Login Page --- */
        #login-page h1 { color: var(--neon-glow); text-shadow: 0 0 5px var(--neon-glow); margin-bottom: 30px; }
        .login-box { background-color: var(--container-bg); padding: 40px; border-radius: 20px; border: 1px solid var(--border-color); text-align: center; box-shadow: 0 0 25px rgba(0, 255, 222, 0.1); }
        .login-input { width: calc(100% - 20px); padding: 15px 10px; margin-bottom: 20px; background-color: #252525; border: 1px solid #333; border-radius: 10px; color: var(--text-color); font-size: 16px; }
        .login-input:focus { outline: none; border-color: var(--neon-glow); box-shadow: 0 0 10px var(--neon-glow); }
        .login-button { width: 100%; padding: 15px; border-radius: 10px; border: none; background-color: var(--neon-glow); color: #000; font-size: 18px; font-weight: bold; cursor: pointer; transition: all 0.3s ease; box-shadow: 0 0 10px var(--neon-glow); }
        .login-button:hover { box-shadow: 0 0 20px var(--neon-glow), 0 0 30px var(--neon-glow); }

        /* --- Main Menu Page --- */
        #menu-page h2 { font-size: 32px; color: var(--neon-glow); text-shadow: 0 0 10px var(--neon-glow); margin-bottom: 40px; }
        #menu-grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 25px; padding: 20px; }
        .menu-card { background-color: var(--container-bg); border: 1px solid var(--border-color); border-radius: 15px; padding: 40px 20px; cursor: pointer; transition: all 0.3s ease; font-size: 20px; font-weight: bold; }
        .menu-card:hover { transform: translateY(-5px); border-color: var(--neon-glow); color: var(--neon-glow); box-shadow: 0 5px 20px rgba(0, 255, 222, 0.1); }

        /* --- App Page --- */
        #app-page { justify-content: flex-start; }
        #app-header { width: 100%; background-color: var(--container-bg); padding: 10px 20px; border-bottom: 1px solid var(--border-color); display: flex; justify-content: space-between; align-items: center; box-shadow: 0 2px 10px rgba(0,0,0,0.3); box-sizing: border-box; }
        #app-header h1 { color: var(--neon-glow); font-size: 24px; margin: 0; text-shadow: 0 0 5px var(--neon-glow); }
        #back-to-menu { background: #2a2a2a; border: 1px solid #444; color: var(--text-color); padding: 8px 12px; border-radius: 8px; cursor: pointer; transition: all .2s; }
        #app-main { display: flex; width: 100%; flex-grow: 1; overflow: hidden; }
        #game-nav { width: 200px; background-color: var(--container-bg); border-right: 1px solid var(--border-color); flex-shrink: 0; display: flex; flex-direction: column; }
        #user-profile { padding: 20px; text-align: center; border-bottom: 1px solid var(--border-color); }
        #user-avatar { width: 80px; height: 80px; border-radius: 50%; background-color: var(--neon-glow); margin: 0 auto 10px; box-shadow: 0 0 15px var(--neon-glow); }
        #user-name { margin: 0; } #user-status { margin: 5px 0 0; font-size: 12px; color: var(--success-color); }
        #game-list { flex-grow: 1; overflow-y: auto; }
        .game-button { display: block; width: 100%; padding: 15px 20px; background: none; border: none; color: var(--text-color); text-align: left; font-size: 16px; cursor: pointer; border-left: 3px solid transparent; transition: all 0.3s ease; }
        .game-button:hover { background-color: #252525; color: var(--neon-glow); }
        .game-button.active { border-left-color: var(--neon-glow); color: var(--neon-glow); }
        #cheat-area { flex-grow: 1; padding: 30px; overflow-y: auto; }
        #cheat-area h2 { margin-top: 0; border-bottom: 1px solid var(--border-color); padding-bottom: 10px; }
        .cheat-category { background-color: var(--container-bg); border: 1px solid var(--border-color); border-radius: 15px; padding: 20px; margin-bottom: 25px; }
        .cheat-category h3 { margin-top: 0; color: var(--neon-glow); }
        .cheat-item { display: flex; justify-content: space-between; align-items: center; padding: 15px 0; border-bottom: 1px solid #2a2a2a; }
        .cheat-item:last-child { border-bottom: none; }
        .toggle-switch { position: relative; display: inline-block; width: 50px; height: 28px; }
        .toggle-switch input { opacity: 0; width: 0; height: 0; }
        .toggle-slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #333; transition: .4s; border-radius: 28px; }
        .toggle-slider:before { position: absolute; content: ""; height: 20px; width: 20px; left: 4px; bottom: 4px; background-color: white; transition: .4s; border-radius: 50%; }
        input:checked + .toggle-slider { background-color: var(--neon-glow); box-shadow: 0 0 10px var(--neon-glow); }
        input:checked + .toggle-slider:before { transform: translateX(22px); }

        /* --- Modals & Alerts --- */
        .modal-overlay { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background-color: rgba(13, 13, 13, 0.95); backdrop-filter: blur(10px); z-index: 1000; display: flex; align-items: center; justify-content: center; }
        .close-button { position: absolute; top: 20px; right: 30px; font-size: 40px; background: none; border: none; color: white; cursor: pointer; z-index: 1001; }
        #status-window h2 { color: var(--neon-glow); text-shadow: 0 0 10px var(--neon-glow); font-size: 32px; margin-bottom: 40px; }
        .status-grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 30px; width: 80%; }
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

    <div id="menu-page" class="page hidden">
        <h2>Main Menu</h2>
        <div id="menu-grid"></div>
    </div>

    <div id="app-page" class="page hidden">
        <div id="app-header">
            <button id="back-to-menu" class="login-button" style="width:auto;">&larr; Menu</button>
            <h1>VEND.ME</h1>
            <div id="status-indicator" style="color:var(--success-color);">● Connected</div>
        </div>
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

    <!-- Modals -->
    <div id="status-window" class="modal-overlay hidden">
        <button class="close-button" data-modal="status-window">&times;</button>
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
                menu: document.getElementById('menu-page'),
                app: document.getElementById('app-page')
            };
            const modals = {
                status: document.getElementById('status-window')
            };
            const elements = {
                usernameInput: document.getElementById('username-input'),
                loginButton: document.getElementById('login-button'),
                menuGrid: document.getElementById('menu-grid'),
                cheatArea: document.getElementById('cheat-area'),
                gameList: document.getElementById('game-list'),
                userNameEl: document.getElementById('user-name'),
                backToMenuButton: document.getElementById('back-to-menu')
            };
            const gameData = {
                "Warzone": { "Aimbot": [{ name: "Enable Aimbot", type: "toggle" }], "Visuals": [{ name: "Player ESP", type: "toggle" }] },
                "BO6": { "Aimbot": [{ name: "Silent Aim", type: "toggle" }], "ESP": [{ name: "Box ESP", type: "toggle" }] },
                "R6 Siege": { "Player": [{ name: "No Recoil", type: "toggle" }], "Gadget": [{ name: "Drone ESP", type: "toggle" }] },
                "Fortnite": { "Combat": [{ name: "Aimbot", type: "toggle" }], "Visuals": [{ name: "Player ESP", type: "toggle" }] },
                "GTA V": { "Player": [{ name: "God Mode", type: "toggle" }], "Money": [{ name: "Money Drop", type: "toggle" }] },
                "Apex Legends": { "Aimbot": [{ name: "Prediction", type: "toggle" }], "Visuals": [{ name: "Glow ESP", type: "toggle" }] }
            };
            const audioContext = new (window.AudioContext || window.webkitAudioContext)();
            const clickSound = 'data:audio/wav;base64,UklGRigAAABXQVZFZm10IBIAAAABAAEARKwAAIhYAQACABAAAABkYXRhAgAAAAEA';

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
                    button.addEventListener('click', () => loadGameUI(gameName));
                    elements.gameList.appendChild(button);
                });
            }

            function updateCheatArea(gameName) {
                elements.cheatArea.innerHTML = '';
                const title = document.createElement('h2');
                title.textContent = `${gameName} Cheats`;
                elements.cheatArea.appendChild(title);
                const categoryData = gameData[gameName];
                for (const categoryName in categoryData) {
                    const categoryDiv = document.createElement('div');
                    categoryDiv.className = 'cheat-category';
                    const categoryTitle = document.createElement('h3');
                    categoryTitle.textContent = categoryName;
                    categoryDiv.appendChild(categoryTitle);
                    categoryData[categoryName].forEach(cheat => {
                        const itemDiv = document.createElement('div');
                        itemDiv.className = 'cheat-item';
                        const label = document.createElement('label');
                        label.textContent = cheat.name;
                        itemDiv.appendChild(label);
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
                        categoryDiv.appendChild(itemDiv);
                    });
                    elements.cheatArea.appendChild(categoryDiv);
                }
            }

            // --- Event Listeners & Initial Setup ---
            elements.loginButton.addEventListener('click', () => {
                playSound(clickSound);
                currentUser = elements.usernameInput.value || 'User';
                elements.userNameEl.textContent = currentUser;
                buildMainMenu();
                showPage('menu');
            });

            elements.backToMenuButton.addEventListener('click', () => {
                playSound(clickSound);
                showPage('menu');
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
