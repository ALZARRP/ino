// VENDOR.ME Cheat Engine for ESP32 - Final Polished Version
// Created by Jules, AI Software Engineer
// Final version incorporating all bug fixes, layout adjustments, and aesthetic requests.

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7789 _panel_instance;
  lgfx::Bus_SPI _bus_instance;
  lgfx::Light_PWM _light_instance;
public:
  LGFX(void) {
    auto bcfg = _bus_instance.config();
    bcfg.spi_host = SPI2_HOST; bcfg.spi_mode = 0; bcfg.freq_write = 40000000;
    bcfg.pin_sclk = 14; bcfg.pin_mosi = 13; bcfg.pin_miso = 12; bcfg.pin_dc = 2;
    _bus_instance.config(bcfg);
    _panel_instance.setBus(&_bus_instance);
    auto pcfg = _panel_instance.config();
    pcfg.pin_cs = 15; pcfg.pin_rst = -1; pcfg.pin_busy = -1;
    pcfg.panel_width = 240; pcfg.panel_height = 320; pcfg.invert = true;
    _panel_instance.config(pcfg);
    auto lcfg = _light_instance.config();
    lcfg.pin_bl = 21;
    _light_instance.config(lcfg);
    _panel_instance.setLight(&_light_instance);
    setPanel(&_panel_instance);
  }
};
LGFX tft;

AsyncWebServer server(80);
const char* ssid = "VENDOR.ME Cheat-Engine";

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
            --neon-glow: #f0f; --neon-secondary: #0ff; --background-color: #0c0c1e;
            --container-bg: rgba(26, 26, 52, 0.8); --border-color: #8e2de2; --text-color: #e0e0e0;
            --success-color: #0f0; --error-color: #f00; --font-family: 'VT323', monospace;
        }
        @keyframes move-boxes-1 { 0% { transform: translateY(0); } 100% { transform: translateY(-100vh); } }
        @keyframes move-boxes-2 { 0% { transform: translateY(0); } 100% { transform: translateY(100vh); } }
        body::before, body::after { content: ''; position: fixed; top: 0; left: 0; width: 100%; height: 100%; z-index: -1; }
        body::before { background-image: radial-gradient(ellipse 50px 50px at 20% 20%, rgba(255,0,255,0.1) 0%, transparent 100%), radial-gradient(ellipse 50px 50px at 10% 80%, rgba(255,0,255,0.1) 0%, transparent 100%); animation: move-boxes-1 30s linear infinite; }
        body::after { background-image: radial-gradient(ellipse 50px 50px at 80% 80%, rgba(255, 165, 0, 0.1) 0%, transparent 100%), radial-gradient(ellipse 50px 50px at 90% 10%, rgba(255, 165, 0, 0.1) 0%, transparent 100%); animation: move-boxes-2 30s linear infinite; }
        body { background-color: var(--background-color); color: var(--text-color); font-family: var(--font-family); margin: 0; padding: 0; overflow: hidden; font-size: 20px; }
        .page { display: flex; flex-direction: column; width: 100vw; height: 100vh; align-items: center; justify-content: center; box-sizing: border-box; }
        .hidden { display: none !important; }
        .login-box { background-color: var(--container-bg); padding: 40px; border-radius: 20px; border: 1px solid var(--border-color); text-align: center; }
        .login-input { width: calc(100% - 20px); padding: 15px 10px; margin-bottom: 20px; background-color: #252525; border: 1px solid #333; border-radius: 10px; color: var(--text-color); font-size: 18px; font-family: var(--font-family); }
        .login-button { padding: 15px; border-radius: 10px; border: none; background-color: var(--neon-glow); color: #000; font-size: 22px; font-weight: bold; cursor: pointer; transition: all 0.3s ease; box-shadow: 0 0 10px var(--neon-glow); font-family: var(--font-family); }
        #menu-page, #app-page { padding-top: 80px; }
        #menu-page h2, #app-page #cheat-area h2 { font-size: 28px; color: var(--neon-glow); text-shadow: 0 0 10px var(--neon-glow); margin-bottom: 30px; }
        #menu-grid { display: flex; flex-direction: column; gap: 20px; width: 85vw; max-width: 600px; }
        .menu-card { background-color: var(--container-bg); border: 1px solid var(--border-color); border-radius: 15px; padding: 25px; cursor: pointer; transition: all 0.3s ease; font-size: 22px; text-align: center; }
        .menu-card:hover { transform: translateY(-5px); color: var(--neon-secondary); border-color: var(--neon-secondary); box-shadow: 0 0 15px var(--neon-glow), 0 0 25px var(--neon-glow); }
        #app-header { width: 100%; height: 60px; background-color: var(--container-bg); padding: 10px 20px; border-bottom: 1px solid var(--border-color); display: flex; justify-content: center; align-items: center; box-shadow: 0 2px 10px rgba(0,0,0,0.3); box-sizing: border-box; position: fixed; top: 0; left: 0; z-index: 400; }
        #app-header h1 { color: var(--neon-glow); font-size: 28px; margin: 0; }
        .nav-button { position: fixed; top: 15px; background: var(--container-bg); border: 1px solid var(--border-color); color: var(--text-color); padding: 8px 15px; border-radius: 8px; cursor: pointer; transition: all .2s; font-size: 18px; font-family: var(--font-family); z-index: 500; }
        .nav-button:hover { box-shadow: 0 0 15px var(--neon-glow); color: var(--neon-glow); }
        #back-to-menu-btn { left: 15px; } #logout-btn { right: 15px; } #mute-btn { right: 110px; }
        #app-main { display: flex; width: 100%; height: 100%; overflow: hidden; }
        #cheat-area { flex-grow: 1; padding: 20px; display: flex; flex-direction: column; }
        #cheat-area h2 { flex-shrink: 0; text-align: center; padding-bottom: 15px; margin-bottom: 15px; border-bottom: 1px solid var(--border-color); }
        .cheat-tabs { display: flex; border-bottom: 1px solid var(--border-color); margin-bottom: 20px; flex-shrink: 0; }
        .tab-button { background: none; border: none; color: #888; padding: 10px 20px; cursor: pointer; font-size: 20px; font-family: var(--font-family); border-bottom: 3px solid transparent; flex-grow: 1; }
        .tab-button.active { color: var(--neon-glow); border-bottom-color: var(--neon-glow); }
        .tab-content { flex-grow: 1; overflow-y: auto; padding: 5px; }
        .cheat-item { display: flex; justify-content: space-between; align-items: center; padding: 20px; background-color: var(--container-bg); border: 1px solid var(--border-color); border-radius: 10px; margin-bottom: 15px; transition: all .2s ease; }
        .cheat-item:hover { box-shadow: 0 0 15px var(--neon-glow); border-color: var(--neon-glow); }
        .toggle-switch { position: relative; display: inline-block; width: 50px; height: 28px; }
        .toggle-switch input { opacity: 0; width: 0; height: 0; }
        .toggle-slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #333; transition: .4s; border-radius: 28px; }
        .toggle-slider:before { position: absolute; content: ""; height: 20px; width: 20px; left: 4px; bottom: 4px; background-color: white; transition: .4s; border-radius: 50%; }
        input:checked + .toggle-slider { background-color: var(--neon-glow); box-shadow: 0 0 10px var(--neon-glow); }
        input:checked + .toggle-slider:before { transform: translateX(22px); }
        .modal-overlay { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background-color: rgba(13, 13, 13, 0.95); backdrop-filter: blur(10px); z-index: 1000; display: flex; align-items: center; justify-content: center; flex-direction: column; }
        .status-grid { display: flex; flex-direction: column; gap: 20px; width: 80%; max-width: 500px; }
        .status-item { background: var(--container-bg); padding: 20px; border-radius: 15px; border: 1px solid var(--border-color); text-align: center; }
        .status-item h4 { margin: 0 0 10px; color: #aaa; font-size: 20px; } .status-item p { margin: 0; font-size: 22px; font-weight: bold; } .status-ok { color: var(--success-color); }
        .alert { position: fixed; top: -100px; left: 50%; transform: translateX(-50%); padding: 15px 25px; border-radius: 10px; color: white; font-weight: bold; z-index: 2000; transition: top 0.5s ease-in-out; font-size: 20px; }
        .alert.show { top: 30px; } .alert.success { background-color: var(--success-color); } .alert.error { background-color: var(--error-color); }
    </style>
</head>
<body>
    <div id="login-page" class="page"><div class="login-box"><h1>VEND.ME</h1><input type="text" id="username-input" class="login-input" placeholder="Username"><input type="password" class="login-input" placeholder="Password"><button id="login-button" class="login-button">Authenticate</button></div></div>
    <div id="enter-page" class="page hidden"><button id="enter-button" class="login-button">Click to Enter</button></div>
    <div id="menu-page" class="page hidden"><h2>Main Menu</h2><div id="menu-grid"></div></div>
    <div id="app-page" class="page hidden"><div id="app-header"><h1>VEND.ME</h1></div><button id="back-to-menu-btn" class="nav-button">&larr; Menu</button><button id="logout-btn" class="nav-button">Logout</button><button id="mute-btn" class="nav-button">🔊</button><div id="app-main"><div id="cheat-area"></div></div></div>
    <div id="status-window" class="modal-overlay hidden"><h2>System Status</h2><div class="status-grid"><div class="status-item"><h4>Injection Status</h4><p class="status-ok">Kernel-Level</p></div><div class="status-item"><h4>VAC/BattlEye</h4><p class="status-ok">Undetected</p></div><div class="status-item"><h4>Connection</h4><p class="status-ok">Encrypted</p></div></div><button class="nav-button" style="position:relative; top: 40px;" data-modal-close="status-window">&larr; Back to Menu</button></div>
    <audio id="bg-music" loop></audio>
    <script>
        document.addEventListener('DOMContentLoaded', () => {
          try {
            const pages={login:document.getElementById('login-page'),enter:document.getElementById('enter-page'),menu:document.getElementById('menu-page'),app:document.getElementById('app-page')};
            const modals={status:document.getElementById('status-window')};
            const elements={usernameInput:document.getElementById('username-input'),loginButton:document.getElementById('login-button'),enterButton:document.getElementById('enter-button'),menuGrid:document.getElementById('menu-grid'),cheatArea:document.getElementById('cheat-area'),backToMenuButton:document.getElementById('back-to-menu-btn'),logoutButton:document.getElementById('logout-btn'),muteButton:document.getElementById('mute-btn'),backgroundMusic:document.getElementById('bg-music')};
            const gameData = {"Warzone":{"Toggles":[{name:"Player ESP"},{name:"Item ESP"},{name:"Radar Hack"},{name:"No Recoil"},{name:"No Spread"},{name:"Heartbeat Sensor"},{name:"Unlock All"},{name:"UAV Spam"},{name:"Wallhack"},{name:"Rapid Fire"}],"Sliders":[{name:"Aimbot FOV",min:1,max:100},{name:"Aim Smoothing",min:0,max:100},{name:"ESP Distance",min:50,max:1000},{name:"Recoil Control %",min:0,max:100}]},"BO6":{"Toggles":[{name:"Silent Aim"},{name:"Trigger Bot"},{name:"Box ESP"},{name:"Skeleton ESP"},{name:"Visibility Check"},{name:"Auto-Ping"},{name:"Fast Reload"},{name:"Infinite Sprint"},{name:"Chams"},{name:"Spoof Name"}],"Sliders":[{name:"Aim Assist Strength",min:0,max:100},{name:"Field of View",min:80,max:150},{name:"Spread Control %",min:0,max:100},{name:"Radar Zoom",min:1,max:5}]},"R6 Siege":{"Toggles":[{name:"Caveira ESP"},{name:"Gadget ESP"},{name:"No Flash"},{name:"No Smoke"},{name:"Unlock All Ops"},{name:"Instant Lean"},{name:"Silent Plant"},{name:"Drone Vision"},{name:"Player Chams"},{name:"Weapon Chams"}],"Sliders":[{name:"Speed Hack %",min:100,max:130},{name:"Recoil Reduction %",min:0,max:100},{name:"Spread Reduction %",min:0,max:100},{name:"Glow Intensity",min:0,max:10}]},"Fortnite":{"Toggles":[{name:"Player ESP"},{name:"Loot ESP"},{name:"Vehicle ESP"},{name:"Building Helper"},{name:"Instant Revive"},{name:"First Shot Accuracy"},{name:"No Bloom"},{name:"Air Walk"},{name:"Infinite Build"},{name:"Aim While Jumping"}],"Sliders":[{name:"Aimbot Strength",min:0,max:100},{name:"Trigger Bot Delay (ms)",min:0,max:200},{name:"Building Edit Speed",min:100,max:300},{name:"Loot Distance",min:10,max:500}]},"GTA V":{"Toggles":[{name:"God Mode"},{name:"Infinite Ammo"},{name:"Super Jump"},{name:"Never Wanted"},{name:"Off the Radar"},{name:"Money Drop"},{name:"Teleport to Waypoint"},{name:"Spawn Vehicle"},{name:"Rainbow Car"},{name:"Infinite Special Ability"}],"Sliders":[{name:"RP Multiplier",min:1,max:100},{name:"Run Speed Multiplier",min:1,max:5},{name:"Wanted Level",min:0,max:5},{name:"Vehicle Boost Strength",min:1,max:10}]},"Apex Legends":{"Toggles":[{name:"Glow ESP"},{name:"Item Glow"},{name:"Trigger Bot"},{name:"Bunny Hop"},{name:"Auto Loot"},{name:"No Recoil"},{name:"Heirloom Spoofer"},{name:"Charge Rifle Spam"},{name:"Third Person View"},{name:"Silent Strafe"}],"Sliders":[{name:"Aimbot FOV",min:1,max:50},{name:"Aim Smoothing",min:0,max:100},{name:"Glow Opacity %",min:10,max:100},{name:"Auto Loot Tier",min:1,max:4}]}};
            const audioContext=new(window.AudioContext||window.webkitAudioContext)();
            const clickSound='data:audio/wav;base64,UklGRigAAABXQVZFZm10IBIAAAABAAEARKwAAIhYAQACABAAAABkYXRhAgAAAAEA';
            const phonkTrack='data:audio/mpeg;base64,SUQzBAAAAAAAI1RTU0UAAAAPAAADTGF2ZjU4LjQ1LjEwMAAAAAAAAAAAAAAA//tAwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD/24DEAAAAAAAAAAAAAAAAAAAAAAAAPRr2agaGnG5tS0Fz5i3pGk2/p5s3/gYpB8A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4D+A/gP4//uA8A=';
            function showPage(pageId){Object.values(pages).forEach(p=>p.classList.add('hidden'));pages[pageId]?.classList.remove('hidden');}
            function playSound(url){try{const s=audioContext.createBufferSource();fetch(url).then(r=>r.arrayBuffer()).then(d=>audioContext.decodeAudioData(d)).then(b=>{s.buffer=b;s.connect(audioContext.destination);s.start(0);});}catch(e){}}
            function showAlert(msg,type){const el=document.createElement('div');el.className=`alert ${type}`;el.textContent=msg;document.body.appendChild(el);setTimeout(()=>el.classList.add('show'),10);setTimeout(()=>{el.classList.remove('show');setTimeout(()=>document.body.removeChild(el),500);},2000);}
            function buildMainMenu(){elements.menuGrid.innerHTML='';Object.keys(gameData).forEach(name=>{const c=document.createElement('div');c.className='menu-card';c.textContent=name;c.addEventListener('click',()=>{playSound(clickSound);loadGameUI(name);showPage('app');});elements.menuGrid.appendChild(c);});const s=document.createElement('div');s.className='menu-card';s.textContent='System Status';s.addEventListener('click',()=>{playSound(clickSound);modals.status.classList.remove('hidden');});elements.menuGrid.appendChild(s);}
            function loadGameUI(name){elements.cheatArea.innerHTML='';const h=document.createElement('h2');h.textContent=`${name} Cheats`;elements.cheatArea.appendChild(h);const t=document.createElement('div');t.className='cheat-tabs';const b1=document.createElement('button');b1.className='tab-button active';b1.textContent='Toggles';const b2=document.createElement('button');b2.className='tab-button';b2.textContent='Sliders';t.appendChild(b1);t.appendChild(b2);const c1=document.createElement('div');c1.className='tab-content';const c2=document.createElement('div');c2.className='tab-content hidden';(gameData[name]?.Toggles||[]).forEach(ch=>c1.appendChild(createCheatItem(ch)));(gameData[name]?.Sliders||[]).forEach(ch=>c2.appendChild(createCheatItem(ch)));b1.addEventListener('click',()=>{b1.classList.add('active');b2.classList.remove('active');c1.classList.remove('hidden');c2.classList.add('hidden');});b2.addEventListener('click',()=>{b2.classList.add('active');b1.classList.remove('active');c2.classList.remove('hidden');c1.classList.add('hidden');});elements.cheatArea.appendChild(t);elements.cheatArea.appendChild(c1);elements.cheatArea.appendChild(c2);}
            function createCheatItem(c){const d=document.createElement('div');d.className='cheat-item';const l=document.createElement('label');l.textContent=c.name;d.appendChild(l);if(c.type==='toggle'){const sL=document.createElement('label');sL.className='toggle-switch';const i=document.createElement('input');i.type='checkbox';i.addEventListener('change',e=>{const a=e.target.checked?'Activated':'Deactivated';playSound(clickSound);showAlert(`${c.name} ${a}`,e.target.checked?'success':'error');});const s=document.createElement('span');s.className='toggle-slider';sL.appendChild(i);sL.appendChild(s);d.appendChild(sL);}else if(c.type==='slider'){const sC=document.createElement('div');const s=document.createElement('input');s.type='range';s.min=c.min;s.max=c.max;s.value=c.min;const vS=document.createElement('span');vS.textContent=s.value;s.addEventListener('input',()=>{vS.textContent=s.value;});s.addEventListener('change',()=>{playSound(clickSound);showAlert(`${c.name} set to ${s.value}`);});sC.appendChild(s);sC.appendChild(vS);d.appendChild(sC);}return d;}
            function startMusic(){if(elements.backgroundMusic.src===''){elements.backgroundMusic.src=phonkTrack;}const p=elements.backgroundMusic.play();if(p!==undefined){p.catch(e=>{document.body.addEventListener('click',startMusic,{once:true});});}}
            elements.loginButton.addEventListener('click',()=>{playSound(clickSound);showPage('enter');});
            elements.enterButton.addEventListener('click',()=>{audioContext.resume().then(()=>{playSound(clickSound);buildMainMenu();showPage('menu');startMusic();});});
            elements.backToMenuButton.addEventListener('click',()=>{playSound(clickSound);showPage('menu');});
            elements.logoutButton.addEventListener('click',()=>{playSound(clickSound);elements.backgroundMusic.pause();elements.backgroundMusic.currentTime=0;showPage('login');});
            elements.muteButton.addEventListener('click',()=>{playSound(clickSound);elements.backgroundMusic.muted=!elements.backgroundMusic.muted;elements.muteButton.textContent=elements.backgroundMusic.muted?'🔇':'🔊';});
            document.querySelectorAll('[data-modal-close]').forEach(b=>{b.addEventListener('click',()=>{playSound(clickSound);modals[b.dataset.modalClose].classList.add('hidden');});});
            showPage('login');
          } catch (e) { document.body.innerHTML = `<h1>Critical Error</h1><p>${e.message}</p>`; }
        });
    </script>
</body>
</html>
)raw";

void setup() {
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
  Serial.begin(115200);
  WiFi.softAP(ssid, NULL);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  server.begin();
}

void loop() {
  delay(2000);
}
