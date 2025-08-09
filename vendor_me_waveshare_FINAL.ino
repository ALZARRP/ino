// VENDOR.ME Cheat Engine for ESP32 - ENHANCED WITH SOUNDS & VISUALS
// Enhanced with Button Sounds and Improved Visual Effects
// Created by Jules, AI Software Engineer
// Copy and paste this entire file into Arduino IDE

// =================================================================================================
// NFC TAG INSTRUCTIONS
// =================================================================================================
// To see a special welcome screen when you open the app, you can use an NFC tag.
// 1. On your phone, use an NFC writing app (e.g., "NFC Tools").
// 2. Write a new "URL / URI" record to your tag.
// 3. The URL to write is: http://192.168.4.1/#welcome
//
// When you tap the tag, it will open the website and show the welcome animation
// before proceeding to the normal login screen.
// =================================================================================================

// =================================================================================================
// LIBRARIES - Ensure these are installed in your Arduino IDE
// =================================================================================================
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <map>

// =================================================================================================
// DISPLAY CONFIGURATION (FOR WAVESHARE ESP32-S3-LCD-1.47) - OFFICIAL
// =================================================================================================
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7789 _panel_instance;
  lgfx::Bus_SPI _bus_instance;
  lgfx::Light_PWM _light_instance;
public:
  LGFX(void) {
    auto bcfg = _bus_instance.config();
    bcfg.spi_host = SPI2_HOST; // Most ESP32-S3 boards use SPI2_HOST for general SPI
    bcfg.spi_mode = 0;
    bcfg.freq_write = 80000000;
    bcfg.pin_sclk = 40;
    bcfg.pin_mosi = 45;
    bcfg.pin_miso = -1;
    bcfg.pin_dc = 41;
    _bus_instance.config(bcfg);
    _panel_instance.setBus(&_bus_instance);
    auto pcfg = _panel_instance.config();
    pcfg.pin_cs = 42;
    pcfg.pin_rst = 39;
    pcfg.pin_busy = -1;
    pcfg.panel_width = 172;
    pcfg.panel_height = 320;
    pcfg.offset_x = 34;
    pcfg.offset_y = 0;
    pcfg.invert = true; // Inverting colors is common for ST7789
    pcfg.bus_shared = true;
    _panel_instance.config(pcfg);
    auto lcfg = _light_instance.config();
    lcfg.pin_bl = 48;
    lcfg.invert = false;
    lcfg.freq = 44100;
    lcfg.pwm_channel = 0;
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
const char* ssid = "VENDOR-ME-CheatEngine";
const char* password = ""; // Open network

// User authentication
bool userLoggedIn = false;
String currentUser = "";
const String validUsername = "admin";
const String validPassword = "cheat123";

// Game states and configurations
String currentGame = "";
String activeConfig = "default";
bool focusMode = false;

// Game database
struct GameCheats {
  std::map<String, bool> basicCheats;
  std::map<String, bool> advancedCheats;
  std::map<String, bool> weaponCheats;
  std::map<String, bool> playerCheats;
  std::map<String, bool> worldCheats;
};

std::map<String, GameCheats> gameDatabase;

// =================================================================================================
// WEB PAGE (HTML, CSS, JS) - ENHANCED WITH SOUND AND IMPROVED VISUALS
// =================================================================================================
const char index_html[] PROGMEM = R"raw(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no, viewport-fit=cover, minimum-scale=1.0, maximum-scale=1.0">
    <title>VENDOR.ME</title>
    <meta name="apple-mobile-web-app-capable" content="yes">
    <meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
    <meta name="apple-mobile-web-app-title" content="VENDOR.ME">
    <meta name="mobile-web-app-capable" content="yes">
    <meta name="theme-color" content="#0c0c1e">
    <link rel="apple-touch-icon" href="data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTgwIiBoZWlnaHQ9IjE4MCIgdmlld0JveD0iMCAwIDE4MCAxODAiIGZpbGw9Im5vbmUiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyI+CjxyZWN0IHdpZHRoPSIxODAiIGhlaWdodD0iMTgwIiByeD0iNDAiIGZpbGw9IiMwYzBjMWUiLz4KPHRleHQgeD0iOTAiIHk9IjEwMCIgZm9udC1mYW1pbHk9Im1vbm9zcGFjZSIgZm9udC1zaXplPSIyNCIgZmlsbD0iI2YwZiIgdGV4dC1hbmNob3I9Im1pZGRsZSI+VkVORE9SLk1FPC90ZXh0Pgo8L3N2Zz4=">
    <link rel="icon" href="data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTgwIiBoZWlnaHQ9IjE4MCIgdmlld0JveD0iMCAwIDE4MCAxODAiIGZpbGw9Im5vbmUiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyI+CjxyZWN0IHdpZHRoPSIxODAiIGhlaWdodD0iMTgwIiByeD0iNDAiIGZpbGw9IiMwYzBjMWUiLz4KPHRleHQgeD0iOTAiIHk9IjEwMCIgZm9udC1mYW1pbHk9Im1vbm9zcGFjZSIgZm9udC1zaXplPSIyNCIgZmlsbD0iI2YwZiIgdGV4dC1hbmNob3I9Im1pZGRsZSI+VkVORE9SLk1FPC90ZXh0Pgo8L3N2Zz4=">
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=VT323&display=swap" rel="stylesheet">
    <style>
        :root {
            --neon-primary: #ff0080;
            --neon-secondary: #00ffff;
            --neon-accent: #ffff00;
            --neon-success: #00ff41;
            --neon-warning: #ff8000;
            --neon-error: #ff0040;
            --background-color: #0a0a1a;
            --container-bg: rgba(20, 20, 40, 0.95);
            --border-color: #8e2de2;
            --text-color: #e0e0e0;
            --success-color: #0f0;
            --error-color: #f00;
            --font-family: 'VT323', monospace;
            --safe-area-inset-top: env(safe-area-inset-top);
            --safe-area-inset-bottom: env(safe-area-inset-bottom);
            --safe-area-inset-left: env(safe-area-inset-left);
            --safe-area-inset-right: env(safe-area-inset-right);
        }

        @keyframes move-boxes-1 {
            0% { transform: translateY(0) rotate(0deg); }
            100% { transform: translateY(-100vh) rotate(360deg); }
        }
        @keyframes move-boxes-2 {
            0% { transform: translateY(0) rotate(0deg); }
            100% { transform: translateY(100vh) rotate(-360deg); }
        }
        @keyframes pulse {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.5; }
        }
        @keyframes glow {
            0%, 100% {
                box-shadow:
                    0 0 10px var(--neon-primary),
                    0 0 20px var(--neon-primary),
                    0 0 30px var(--neon-primary);
            }
            50% {
                box-shadow:
                    0 0 20px var(--neon-primary),
                    0 0 40px var(--neon-primary),
                    0 0 60px var(--neon-primary),
                    0 0 80px var(--neon-primary);
            }
        }
        @keyframes breathe {
            0%, 100% { transform: scale(1); }
            50% { transform: scale(1.05); }
        }
        @keyframes fadeIn {
            0% { opacity: 0; transform: translateY(-10px); }
            100% { opacity: 1; transform: translateY(0); }
        }
        @keyframes fadeOut {
            0% { opacity: 1; transform: translateY(0); }
            100% { opacity: 0; transform: translateY(-10px); }
        }
        @keyframes ripple {
            0% {
                transform: scale(0);
                opacity: 1;
            }
            100% {
                transform: scale(4);
                opacity: 0;
            }
        }
        @keyframes rainbow {
            0% { filter: hue-rotate(0deg); }
            100% { filter: hue-rotate(360deg); }
        }

        * {
            -webkit-touch-callout: none;
            -webkit-user-select: none;
            -webkit-tap-highlight-color: transparent;
            box-sizing: border-box;
        }

        html, body {
            height: 100%;
            width: 100%;
            margin: 0;
            padding: 0;
            overflow: hidden;
            position: fixed;
            -webkit-overflow-scrolling: touch;
        }

        body::before, body::after {
            content: '';
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            z-index: -1;
            pointer-events: none;
        }
        body::before {
            background:
                radial-gradient(circle 150px at 20% 30%, rgba(100, 50, 255, 0.3) 0%, transparent 70%),
                radial-gradient(circle 120px at 80% 20%, rgba(255, 50, 150, 0.25) 0%, transparent 70%),
                radial-gradient(circle 180px at 60% 80%, rgba(50, 200, 255, 0.2) 0%, transparent 70%);
            filter: blur(40px);
            animation: move-boxes-1 25s linear infinite;
        }
        body::after {
            background:
                radial-gradient(circle 130px at 90% 70%, rgba(255, 100, 50, 0.25) 0%, transparent 70%),
                radial-gradient(circle 160px at 10% 90%, rgba(150, 255, 50, 0.2) 0%, transparent 70%),
                radial-gradient(circle 110px at 50% 10%, rgba(200, 50, 200, 0.3) 0%, transparent 70%);
            filter: blur(50px);
            animation: move-boxes-2 30s linear infinite;
        }

        body {
            background: #0a0a0a;
            color: var(--text-color);
            font-family: var(--font-family);
            font-size: 16px;
            padding-top: var(--safe-area-inset-top);
            padding-bottom: var(--safe-area-inset-bottom);
            padding-left: var(--safe-area-inset-left);
            padding-right: var(--safe-area-inset-right);
        }

        .page {
            display: flex;
            flex-direction: column;
            width: 100vw;
            height: 100vh;
            height: calc(100vh - var(--safe-area-inset-top) - var(--safe-area-inset-bottom));
            align-items: center;
            justify-content: center;
            padding: 10px;
            box-sizing: border-box;
        }

        .hidden {
            display: none !important;
        }

        /* Welcome Screen Overlay */
        #welcome-overlay {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background-color: var(--background-color);
            z-index: 10001;
            display: flex;
            align-items: center;
            justify-content: center;
            flex-direction: column;
            opacity: 1;
            transition: opacity 1s ease-in-out;
            pointer-events: none;
        }

        #welcome-overlay.hidden {
            opacity: 0;
        }

        #welcome-overlay h1 {
            font-size: clamp(2.5rem, 10vw, 4rem);
            color: var(--neon-primary);
            text-shadow:
                0 0 10px var(--neon-primary),
                0 0 20px var(--neon-primary),
                0 0 40px var(--neon-primary),
                0 0 80px var(--neon-primary);
            animation: glow 3s ease-in-out infinite alternate, fadeIn 2s ease;
        }

        #welcome-overlay p {
            font-size: clamp(1rem, 4vw, 1.5rem);
            color: var(--neon-secondary);
            text-shadow: 0 0 10px var(--neon-secondary);
            margin-top: 10px;
            animation: fadeIn 2s ease 0.5s;
            animation-fill-mode: backwards;
        }

        #welcome-overlay .instructions {
            font-size: clamp(0.8rem, 2.5vw, 1rem);
            color: var(--text-color);
            margin-top: 40px;
            max-width: 80%;
            text-align: center;
            line-height: 1.4;
            animation: fadeIn 2s ease 1s;
            animation-fill-mode: backwards;
        }

        /* Enhanced Notification System */
        .notification {
            position: fixed;
            top: calc(60px + var(--safe-area-inset-top));
            left: 50%;
            transform: translateX(-50%);
            background: rgba(0, 0, 0, 0.9);
            border: 2px solid;
            padding: 12px 20px;
            border-radius: 8px;
            font-size: 14px;
            font-weight: bold;
            z-index: 9999;
            animation: fadeIn 0.3s ease;
            backdrop-filter: blur(10px);
            min-width: 200px;
            text-align: center;
        }

        .notification.success {
            border-color: #00ff00;
            color: #00ff00;
            background: rgba(0, 40, 0, 0.9);
        }

        .notification.error {
            border-color: #ff0000;
            color: #ff0000;
            background: rgba(40, 0, 0, 0.9);
        }

        .notification.info {
            border-color: #00ffff;
            color: #00ffff;
            background: rgba(0, 20, 40, 0.9);
        }

        .notification.fade-out {
            animation: fadeOut 0.5s ease forwards;
        }

        /* Enhanced Login Page Styles */
        #login-page {
            padding: 20px;
        }

        #login-page h1 {
            color: var(--neon-primary);
            text-shadow:
                0 0 5px var(--neon-primary),
                0 0 10px var(--neon-primary),
                0 0 15px var(--neon-primary),
                0 0 20px var(--neon-primary);
            margin-bottom: 30px;
            font-size: clamp(2rem, 8vw, 3rem);
            text-align: center;
            background: transparent;
            animation: glow 3s ease-in-out infinite alternate;
        }

        .login-box {
            background:
                linear-gradient(135deg, rgba(20, 20, 40, 0.9) 0%, rgba(40, 20, 60, 0.8) 100%);
            padding: 30px 20px;
            border-radius: 20px;
            border: 2px solid var(--border-color);
            text-align: center;
            box-shadow:
                0 0 30px rgba(142, 45, 226, 0.3),
                inset 0 0 30px rgba(142, 45, 226, 0.1);
            width: 100%;
            max-width: 350px;
            margin: 0 auto;
            position: relative;
            overflow: hidden;
        }



        .login-input {
            width: 100%;
            padding: 18px 15px;
            margin-bottom: 20px;
            background: rgba(30, 30, 30, 0.8);
            border: 2px solid #444;
            border-radius: 12px;
            color: var(--text-color);
            font-size: 18px;
            font-family: var(--font-family);
            transition: all 0.3s ease;
            backdrop-filter: blur(10px);
        }

        .login-input:focus {
            outline: none;
            border-color: var(--neon-primary);
            box-shadow:
                0 0 15px var(--neon-primary),
                inset 0 0 15px rgba(255, 0, 128, 0.1);
            transform: scale(1.02);
        }

        .login-button {
            width: 100%;
            padding: 18px;
            border-radius: 12px;
            border: none;
            background: linear-gradient(135deg, var(--neon-primary) 0%, var(--neon-secondary) 100%);
            color: #000;
            font-size: 20px;
            font-weight: bold;
            cursor: pointer;
            transition: all 0.3s ease;
            box-shadow:
                0 0 20px var(--neon-primary),
                0 4px 15px rgba(255, 0, 128, 0.3),
                inset 0 0 15px rgba(255, 255, 255, 0.1);
            font-family: var(--font-family);
            touch-action: manipulation;
            min-height: 50px;
            position: relative;
            overflow: hidden;
            text-shadow: 0 0 5px rgba(0, 0, 0, 0.5);
            text-transform: uppercase;
            letter-spacing: 1px;
        }

        .login-button:hover, .login-button:active {
            box-shadow:
                0 0 30px var(--neon-primary),
                0 0 40px var(--neon-primary),
                0 6px 20px rgba(255, 0, 128, 0.4);
            transform: translateY(-2px) scale(1.02);
            animation: breathe 1s ease-in-out infinite;
        }

        .login-button::before {
            content: '';
            position: absolute;
            top: 50%;
            left: 50%;
            width: 0;
            height: 0;
            border-radius: 50%;
            background: rgba(255, 255, 255, 0.3);
            transform: translate(-50%, -50%);
            transition: width 0.3s, height 0.3s;
        }

        .login-button:active::before {
            width: 300px;
            height: 300px;
            animation: ripple 0.6s ease-out;
        }

        .error-message {
            color: var(--neon-error);
            margin-top: 15px;
            font-size: 14px;
            text-shadow: 0 0 5px var(--neon-error);
        }

        .success-message {
            color: var(--neon-success);
            margin-top: 15px;
            font-size: 14px;
            text-shadow: 0 0 5px var(--neon-success);
        }

        /* Profile Section */
        #profile-section {
            position: fixed;
            top: calc(20px + var(--safe-area-inset-top));
            left: calc(15px + var(--safe-area-inset-left));
            background: rgba(0, 0, 0, 0.8);
            border: 1px solid #333;
            padding: 12px;
            border-radius: 10px;
            cursor: pointer;
            transition: all 0.3s ease;
            font-size: 12px;
            font-family: var(--font-family);
            z-index: 500;
            backdrop-filter: blur(10px);
            min-width: 120px;
        }

        #profile-section:hover {
            border-color: var(--neon-primary);
            box-shadow: 0 0 10px rgba(255, 0, 128, 0.3);
        }

        .profile-pic {
            width: 40px;
            height: 40px;
            border-radius: 50%;
            background: #333;
            margin: 0 auto 8px;
            display: flex;
            align-items: center;
            justify-content: center;
            color: #fff;
            font-size: 18px;
            border: 2px solid #555;
        }

        .profile-name {
            color: var(--text-color);
            font-weight: bold;
            text-align: center;
            margin-bottom: 4px;
        }

        .profile-plan {
            color: #00ff00;
            text-align: center;
            font-size: 10px;
        }

        /* Navigation Buttons */
        #back-to-menu-btn, #logout-btn {
            position: fixed;
            top: calc(20px + var(--safe-area-inset-top));
            background: rgba(0, 0, 0, 0.8);
            border: 1px solid #333;
            color: var(--text-color);
            padding: 12px 18px;
            border-radius: 10px;
            cursor: pointer;
            transition: all 0.3s ease;
            font-size: 14px;
            font-weight: bold;
            font-family: var(--font-family);
            z-index: 500;
            touch-action: manipulation;
            min-height: 44px;
            backdrop-filter: blur(10px);
        }

        #back-to-menu-btn {
            left: calc(15px + var(--safe-area-inset-left));
        }

        #logout-btn {
            right: calc(15px + var(--safe-area-inset-right));
        }

        #back-to-menu-btn:hover, #logout-btn:hover,
        #back-to-menu-btn:active, #logout-btn:active {
            border-color: var(--neon-primary);
            color: var(--neon-primary);
            transform: scale(1.05);
        }

        /* Quick Features Panel */
        #quick-panel {
            position: fixed;
            top: calc(80px + var(--safe-area-inset-top));
            right: calc(15px + var(--safe-area-inset-right));
            background: rgba(0, 0, 0, 0.9);
            border: 1px solid #333;
            border-radius: 10px;
            padding: 10px;
            z-index: 450;
            backdrop-filter: blur(10px);
            width: 180px;
            max-height: 300px;
            overflow-y: auto;
            transform: translateX(100%);
            transition: transform 0.3s ease;
        }

        #quick-panel.open {
            transform: translateX(0);
        }

        #quick-toggle-btn {
            position: fixed;
            top: calc(80px + var(--safe-area-inset-top));
            right: calc(15px + var(--safe-area-inset-right));
            background: rgba(0, 0, 0, 0.8);
            border: 1px solid #333;
            color: var(--text-color);
            padding: 8px 12px;
            border-radius: 8px;
            cursor: pointer;
            font-size: 12px;
            z-index: 500;
            transition: all 0.3s ease;
        }

        #quick-toggle-btn:hover {
            border-color: var(--neon-primary);
        }

        .quick-feature {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 8px 0;
            border-bottom: 1px solid #333;
            font-size: 12px;
        }

        .quick-toggle {
            width: 20px;
            height: 12px;
            background: #333;
            border-radius: 6px;
            position: relative;
            cursor: pointer;
        }

        .quick-toggle.active {
            background: #00ff00;
        }

        .quick-toggle::after {
            content: '';
            width: 8px;
            height: 8px;
            background: #fff;
            border-radius: 50%;
            position: absolute;
            top: 2px;
            left: 2px;
            transition: left 0.2s ease;
        }

        .quick-toggle.active::after {
            left: 10px;
        }

        /* Enhanced Main Menu Styles */
        #menu-page {
            padding: calc(80px + var(--safe-area-inset-top)) 20px calc(20px + var(--safe-area-inset-bottom)) 20px;
            overflow-y: auto;
            -webkit-overflow-scrolling: touch;
        }

        #menu-page h2 {
            font-size: clamp(24px, 6vw, 32px);
            color: var(--neon-primary);
            text-shadow:
                0 0 5px var(--neon-primary),
                0 0 10px var(--neon-primary);
            margin-bottom: 30px;
            text-align: center;
            animation: glow 3s ease-in-out infinite alternate;
            background: none !important;
            border: none !important;
            outline: none !important;
            box-shadow: none !important;
        }

        #menu-grid {
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(160px, 1fr));
            gap: 15px;
            width: 100%;
            max-width: 100vw;
            margin: 0 auto;
            padding: 0 10px;
            max-height: calc(100vh - 180px);
            overflow-y: auto;
            -webkit-overflow-scrolling: touch;
        }

        @media screen and (max-width: 480px) {
            #menu-grid {
                grid-template-columns: repeat(auto-fill, minmax(140px, 1fr));
                gap: 10px;
                padding: 0 5px;
            }
        }

        .menu-card {
            background:
                linear-gradient(135deg, rgba(20, 20, 40, 0.95) 0%, rgba(40, 20, 60, 0.9) 100%);
            border: 2px solid var(--border-color);
            border-radius: 18px;
            padding: 20px 15px;
            text-align: center;
            cursor: pointer;
            transition: all 0.4s ease;
            min-height: 120px;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            position: relative;
            overflow: hidden;
            backdrop-filter: blur(15px);
            box-shadow:
                0 4px 15px rgba(142, 45, 226, 0.3),
                inset 0 0 20px rgba(142, 45, 226, 0.1),
                0 0 1px rgba(255, 255, 255, 0.2);
        }

        .menu-card::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: linear-gradient(45deg, transparent, rgba(255, 0, 128, 0.1), transparent);
            opacity: 0;
            transition: opacity 0.3s ease;
        }

        .menu-card:hover::before {
            opacity: 1;
        }

        .menu-card h3 {
            margin: 0 0 8px 0;
            color: var(--text-color);
            font-size: 16px;
            font-weight: bold;
            line-height: 1.2;
            text-shadow:
                0 0 5px rgba(255, 255, 255, 0.4),
                0 0 10px rgba(255, 255, 255, 0.2);
            background: transparent;
        }

        .menu-card p {
            margin: 0;
            font-size: 12px;
            color: #bbb;
            line-height: 1.3;
            overflow: hidden;
            display: -webkit-box;
            -webkit-line-clamp: 2;
            -webkit-box-orient: vertical;
        }

        @media screen and (max-width: 480px) {
            .menu-card {
                padding: 15px 10px;
                min-height: 100px;
            }

            .menu-card h3 {
                font-size: 14px;
            }

            .menu-card p {
                font-size: 10px;
            }
        }

        .menu-card:hover, .menu-card:active {
            transform: translateY(-5px) scale(1.02);
            border-color: var(--neon-primary);
            box-shadow:
                0 0 25px var(--neon-primary),
                0 0 35px rgba(255, 0, 128, 0.4),
                0 8px 25px rgba(0, 0, 0, 0.3);
            animation: breathe 2s ease-in-out infinite;
        }

        .menu-card:hover h3 {
            color: var(--neon-primary);
            text-shadow:
                0 0 5px var(--neon-primary),
                0 0 10px var(--neon-primary),
                0 0 15px var(--neon-primary);
        }

        /* Enhanced App Page Styles */
        #app-page {
            justify-content: flex-start;
            padding: 0;
        }

        #app-header {
            width: 100%;
            background: linear-gradient(135deg, rgba(20, 20, 40, 0.95) 0%, rgba(40, 20, 60, 0.9) 100%);
            padding: 15px 20px;
            border-bottom: 2px solid var(--border-color);
            display: flex;
            justify-content: space-between;
            align-items: center;
            box-shadow: 0 4px 15px rgba(0,0,0,0.4);
            box-sizing: border-box;
            position: fixed;
            top: var(--safe-area-inset-top);
            left: 0;
            right: 0;
            z-index: 400;
            backdrop-filter: blur(15px);
        }

        #app-header h1 {
            color: var(--neon-primary);
            font-size: clamp(18px, 5vw, 24px);
            margin: 0;
            text-shadow:
                0 0 5px var(--neon-primary),
                0 0 10px var(--neon-primary);
            animation: glow 4s ease-in-out infinite alternate;
            background: none !important;
            border: none !important;
            outline: none !important;
            box-shadow: none !important;
        }

        #app-main {
            display: flex;
            flex-direction: column;
            width: 100%;
            height: calc(100vh - 70px - var(--safe-area-inset-top) - var(--safe-area-inset-bottom));
            margin-top: calc(70px + var(--safe-area-inset-top));
            overflow: hidden;
        }

        #game-nav {
            width: 100%;
            background: linear-gradient(135deg, rgba(20, 20, 40, 0.95) 0%, rgba(40, 20, 60, 0.9) 100%);
            border-bottom: 1px solid var(--border-color);
            flex-shrink: 0;
            display: flex;
            flex-direction: row;
            overflow-x: auto;
            padding: 12px;
            gap: 12px;
            -webkit-overflow-scrolling: touch;
            backdrop-filter: blur(10px);
        }

        #game-list {
            display: flex;
            flex-direction: row;
            gap: 12px;
            min-width: max-content;
        }

        .game-button {
            display: block;
            padding: 12px 16px;
            background: linear-gradient(135deg, rgba(30, 30, 50, 0.9) 0%, rgba(50, 30, 70, 0.8) 100%);
            border: 2px solid var(--border-color);
            color: var(--text-color);
            text-align: center;
            font-size: 14px;
            cursor: pointer;
            border-radius: 10px;
            transition: all 0.3s ease;
            font-family: var(--font-family);
            white-space: nowrap;
            touch-action: manipulation;
            min-width: 90px;
            flex-shrink: 0;
            backdrop-filter: blur(5px);
            box-shadow:
                0 2px 8px rgba(0, 0, 0, 0.3),
                inset 0 0 10px rgba(142, 45, 226, 0.1);
        }

        .game-button:hover, .game-button:active {
            background: linear-gradient(135deg, rgba(40, 40, 60, 0.9) 0%, rgba(60, 40, 80, 0.8) 100%);
            color: var(--neon-secondary);
            transform: scale(1.05);
            border-color: var(--neon-secondary);
            box-shadow:
                0 0 15px var(--neon-secondary),
                0 4px 12px rgba(0, 0, 0, 0.4);
        }

        .game-button.active {
            background: linear-gradient(135deg, var(--neon-primary) 0%, var(--neon-secondary) 100%);
            color: #000;
            border-color: var(--neon-primary);
            box-shadow:
                0 0 20px var(--neon-primary),
                0 4px 15px rgba(255, 0, 128, 0.4);
            animation: glow 2s ease-in-out infinite alternate;
        }

        #cheat-area {
            flex-grow: 1;
            padding: 15px;
            display: flex;
            flex-direction: column;
            overflow: hidden;
        }

        #cheat-area h2 {
            margin: 0 0 15px 0;
            border-bottom: 2px solid var(--border-color);
            padding-bottom: 10px;
            flex-shrink: 0;
            font-size: clamp(18px, 4vw, 22px);
            color: var(--neon-secondary);
            text-shadow:
                0 0 5px var(--neon-secondary),
                0 0 10px var(--neon-secondary);
            background: none !important;
            border-radius: 0;
            box-shadow: none !important;
        }

        .cheat-tabs {
            display: flex;
            border-bottom: 2px solid var(--border-color);
            margin-bottom: 15px;
            flex-shrink: 0;
            overflow-x: auto;
            -webkit-overflow-scrolling: touch;
        }

        .tab-button {
            background: none;
            border: none;
            color: #888;
            padding: 12px 16px;
            cursor: pointer;
            font-size: 16px;
            border-bottom: 3px solid transparent;
            font-family: var(--font-family);
            white-space: nowrap;
            touch-action: manipulation;
            transition: all 0.3s ease;
            min-height: 44px;
        }

        .tab-button.active {
            color: var(--neon-accent);
            border-bottom-color: var(--neon-accent);
            text-shadow: 0 0 10px var(--neon-accent);
        }

        .tab-button:hover, .tab-button:active {
            color: var(--neon-secondary);
            border-bottom-color: var(--neon-secondary);
        }

        .tab-content {
            flex-grow: 1;
            overflow-y: auto;
            -webkit-overflow-scrolling: touch;
            padding-bottom: calc(20px + var(--safe-area-inset-bottom));
        }

        .cheat-controls {
            display: flex;
            flex-wrap: wrap;
            gap: 10px;
            margin-bottom: 15px;
        }

        .search-bar {
            flex: 1 1 100%;
            background: rgba(30, 30, 30, 0.8);
            border: 2px solid #444;
            color: var(--text-color);
            padding: 12px;
            border-radius: 10px;
            font-family: var(--font-family);
            font-size: 16px;
            min-height: 44px;
            box-sizing: border-box;
            backdrop-filter: blur(10px);
            transition: all 0.3s ease;
        }

        .search-bar:focus {
            outline: none;
            border-color: var(--neon-accent);
            box-shadow: 0 0 15px var(--neon-accent);
        }

        .control-btn {
            background: linear-gradient(135deg, rgba(20, 20, 40, 0.9) 0%, rgba(40, 20, 60, 0.8) 100%);
            border: 2px solid var(--border-color);
            color: var(--text-color);
            padding: 12px 16px;
            border-radius: 10px;
            cursor: pointer;
            font-family: var(--font-family);
            font-size: 14px;
            transition: all 0.3s ease;
            touch-action: manipulation;
            min-height: 44px;
            backdrop-filter: blur(10px);
        }

        .control-btn:hover, .control-btn:active {
            background: linear-gradient(135deg, rgba(30, 30, 50, 0.9) 0%, rgba(50, 30, 70, 0.8) 100%);
            border-color: var(--neon-warning);
            color: var(--neon-warning);
            transform: scale(1.05);
            box-shadow: 0 0 15px var(--neon-warning);
        }

        .cheat-item {
            background: linear-gradient(135deg, rgba(25, 25, 45, 0.9) 0%, rgba(45, 25, 65, 0.8) 100%);
            border: 2px solid #333;
            border-radius: 12px;
            padding: 15px;
            margin-bottom: 10px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            transition: all 0.3s ease;
            backdrop-filter: blur(10px);
            box-shadow:
                0 2px 8px rgba(0, 0, 0, 0.3),
                inset 0 0 15px rgba(25, 25, 45, 0.1);
        }

        .cheat-info {
            flex-grow: 1;
            margin-right: 15px;
        }

        .cheat-name {
            font-weight: bold;
            font-size: 16px;
            margin-bottom: 5px;
            color: var(--text-color);
        }

        .cheat-desc {
            font-size: 12px;
            color: #999;
            line-height: 1.3;
        }

        .cheat-toggle {
            width: 50px;
            height: 26px;
            background: #333;
            border-radius: 13px;
            position: relative;
            cursor: pointer;
            transition: all 0.3s ease;
            border: 1px solid #555;
            display: flex;
            align-items: center;
            justify-content: center;
        }

        .cheat-toggle.active {
            background: #1a5a1a;
            border-color: #00ff00;
        }

        .cheat-toggle::after {
            content: '';
            width: 12px;
            height: 12px;
            border-radius: 50%;
            position: absolute;
            transition: all 0.3s ease;
        }

        .cheat-toggle::after {
            background: #ff0000;
            box-shadow: 0 0 8px #ff0000;
        }

        .cheat-toggle.active::after {
            background: #00ff00;
            box-shadow: 0 0 8px #00ff00;
        }

        /* Game Timer & Stats */
        #game-stats {
            position: fixed;
            bottom: calc(20px + var(--safe-area-inset-bottom));
            left: calc(15px + var(--safe-area-inset-left));
            background: rgba(0, 0, 0, 0.9);
            border: 1px solid #333;
            border-radius: 10px;
            padding: 10px;
            z-index: 450;
            backdrop-filter: blur(10px);
            font-size: 12px;
            min-width: 150px;
        }

        .stat-row {
            display: flex;
            justify-content: space-between;
            margin-bottom: 5px;
            color: var(--text-color);
        }

        .stat-value {
            color: var(--neon-accent);
            font-weight: bold;
        }



        /* Favorites System */
        .favorite-star {
            cursor: pointer;
            color: #666;
            margin-left: 8px;
            transition: color 0.2s ease;
        }

        .favorite-star.active {
            color: #ffd700;
        }

        .favorite-star:hover {
            color: #ffd700;
        }

        /* Fix all headings to prevent rectangular glows */
        h1, h2, h3, h4, h5, h6 {
            background: none !important;
            border: none !important;
            box-shadow: none !important;
            outline: none !important;
        }

        .cheat-item:hover {
            border-color: var(--neon-primary);
            box-shadow:
                0 0 15px rgba(255, 0, 128, 0.3),
                0 4px 15px rgba(0, 0, 0, 0.4);
            transform: translateY(-2px);
        }

        .cheat-item.active {
            border-color: var(--neon-success);
            background: linear-gradient(135deg, rgba(25, 45, 25, 0.9) 0%, rgba(45, 65, 25, 0.8) 100%);
            box-shadow:
                0 0 20px rgba(0, 255, 65, 0.4),
                inset 0 0 20px rgba(0, 255, 65, 0.1);
        }

        .cheat-item.active .cheat-name {
            color: var(--neon-success);
            text-shadow: 0 0 10px var(--neon-success);
        }

        .value-input {
            width: 80px;
            background: rgba(30, 30, 30, 0.9);
            border: 2px solid #555;
            color: var(--text-color);
            padding: 8px 10px;
            border-radius: 8px;
            font-family: var(--font-family);
            font-size: 14px;
            margin-left: 10px;
        }

        .value-input:focus {
            outline: none;
            border-color: var(--neon-accent);
            box-shadow: 0 0 10px var(--neon-accent);
        }

        /* Notification Container */
        #notification-container {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 0;
            z-index: 10000;
            pointer-events: none;
        }
    </style>
</head>
<body>
    <!-- Welcome Screen Overlay -->
    <div id="welcome-overlay" class="hidden">
        <h1>VENDOR.ME</h1>
        <p>Welcome</p>
        <p class="instructions">
            For the best experience, add this to your Home Screen:
            <br>
            Tap the Share Icon and select 'Add to Home Screen'.
        </p>
    </div>

    <!-- Notification Container -->
    <div id="notification-container"></div>

    <!-- Login Page -->
    <div id="login-page" class="page">
        <h1>VENDOR.ME</h1>
        <div class="login-box">
            <input type="text" id="username" class="login-input" placeholder="Username" value="admin">
            <input type="password" id="password" class="login-input" placeholder="Password" value="cheat123">
            <button class="login-button" onclick="login()" id="login-btn">LOGIN</button>
            <div id="login-message"></div>
        </div>
    </div>

    <!-- Menu Page -->
    <div id="menu-page" class="page hidden">
        <div id="profile-section" onclick="openProfileUpload()">
            <div class="profile-pic" id="profile-pic">👤</div>
            <div class="profile-name">Admin User</div>
            <div class="profile-plan">Basic Lifetime</div>
        </div>
        <button id="logout-btn" onclick="logout()">LOGOUT</button>
        <h2>Select Game</h2>
        <div id="menu-grid">
            <div class="menu-card" onclick="selectGame('Call of Duty: B06')">
                <h3>Call of Duty: B06</h3>
                <p>Latest Black Ops combat cheats and enhancements</p>
            </div>
            <div class="menu-card" onclick="selectGame('Counter-Strike 2')">
                <h3>Counter-Strike 2</h3>
                <p>Competitive shooter enhancements and tactical advantages</p>
            </div>
            <div class="menu-card" onclick="selectGame('Valorant')">
                <h3>Valorant</h3>
                <p>Agent-based tactical shooter with ability modifications</p>
            </div>
            <div class="menu-card" onclick="selectGame('Apex Legends')">
                <h3>Apex Legends</h3>
                <p>Battle royale legends with enhanced abilities</p>
            </div>
            <div class="menu-card" onclick="selectGame('Fortnite')">
                <h3>Fortnite</h3>
                <p>Building-based battle royale with creative cheats</p>
            </div>
            <div class="menu-card" onclick="selectGame('GTA V Online')">
                <h3>GTA V Online</h3>
                <p>Open-world criminal sandbox with unlimited freedom</p>
            </div>
        </div>
    </div>

    <!-- App Page -->
    <div id="app-page" class="page hidden">
        <button id="back-to-menu-btn" onclick="showMenu()">← MENU</button>
        <button id="quick-toggle-btn" onclick="toggleQuickPanel()">⚡</button>

        <div id="quick-panel">
            <div style="font-weight: bold; margin-bottom: 10px; color: var(--neon-primary);">Quick Features</div>
            <div class="quick-feature">
                <span>Auto Save</span>
                <div class="quick-toggle" onclick="toggleQuickFeature('autosave')"></div>
            </div>
            <div class="quick-feature">
                <span>Fast Mode</span>
                <div class="quick-toggle" onclick="toggleQuickFeature('fastmode')"></div>
            </div>
            <div class="quick-feature">
                <span>Silent Mode</span>
                <div class="quick-toggle" onclick="toggleQuickFeature('silent')"></div>
            </div>
            <div class="quick-feature">
                <span>Performance+</span>
                <div class="quick-toggle" onclick="toggleQuickFeature('performance')"></div>
            </div>
            <div class="quick-feature">
                <span>Background Music</span>
                <div class="quick-toggle active" onclick="toggleQuickFeature('bgmusic')"></div>
            </div>
            <div class="quick-feature">
                <span>Button Sounds</span>
                <div class="quick-toggle active" onclick="toggleQuickFeature('sounds')"></div>
            </div>
        </div>

        <div id="game-stats">
            <div style="font-weight: bold; margin-bottom: 8px; color: var(--neon-primary);">Game Stats</div>
            <div class="stat-row">
                <span>Session Time:</span>
                <span class="stat-value" id="session-time">00:00</span>
            </div>
            <div class="stat-row">
                <span>Cheats Active:</span>
                <span class="stat-value" id="active-cheats">0</span>
            </div>
            <div class="stat-row">
                <span>Performance:</span>
                <span class="stat-value" id="performance">100%</span>
            </div>
        </div>

        <div id="app-header">
            <h1 id="game-title">VENDOR.ME</h1>
        </div>
        <div id="app-main">
            <div id="game-nav">
                <div id="game-list">
                    <div class="game-button" data-game="Call of Duty: B06" onclick="selectGame('Call of Duty: B06')">COD:B06</div>
                    <div class="game-button" data-game="Counter-Strike 2" onclick="selectGame('Counter-Strike 2')">CS2</div>
                    <div class="game-button" data-game="Valorant" onclick="selectGame('Valorant')">Valorant</div>
                    <div class="game-button" data-game="Apex Legends" onclick="selectGame('Apex Legends')">Apex</div>
                    <div class="game-button" data-game="Fortnite" onclick="selectGame('Fortnite')">Fortnite</div>
                    <div class="game-button" data-game="GTA V Online" onclick="selectGame('GTA V Online')">GTA V</div>
                </div>
            </div>
            <div id="cheat-area">
                <h2 id="current-game">Select a Game</h2>
                <div class="cheat-controls">
                    <input type="text" class="search-bar" placeholder="Search cheats..." id="search-input" oninput="filterCheats()">
                    <button class="control-btn" onclick="toggleAllCheats(true)">Enable All</button>
                    <button class="control-btn" onclick="toggleAllCheats(false)">Disable All</button>
                </div>
                <div class="cheat-tabs">
                    <button class="tab-button active" onclick="showTab('basic')">Basic</button>
                    <button class="tab-button" onclick="showTab('advanced')">Advanced</button>
                    <button class="tab-button" onclick="showTab('weapons')">Weapons</button>
                    <button class="tab-button" onclick="showTab('player')">Player</button>
                    <button class="tab-button" onclick="showTab('world')">World</button>
                </div>
                <div class="tab-content" id="cheat-list">
                    <div style="text-align: center; color: #666; margin-top: 50px;">
                        <p>Select a game to view available cheats</p>
                    </div>
                </div>
            </div>
        </div>
    </div>

    <script>
        // Sound System - Generate button click sounds using Web Audio API
        let audioContext;

        function initAudio() {
            if (!audioContext) {
                audioContext = new (window.AudioContext || window.webkitAudioContext)();
                initBackgroundMusic(); // Initialize background music when audio is ready
            }
        }

        function playButtonSound(type = 'click') {
            if (!quickFeatures.sounds) return;

            try {
                initAudio();

                const oscillator = audioContext.createOscillator();
                const gainNode = audioContext.createGain();

                oscillator.connect(gainNode);
                gainNode.connect(audioContext.destination);

                switch(type) {
                    case 'click':
                        oscillator.frequency.setValueAtTime(800, audioContext.currentTime);
                        oscillator.frequency.exponentialRampToValueAtTime(400, audioContext.currentTime + 0.1);
                        gainNode.gain.setValueAtTime(0.1, audioContext.currentTime);
                        gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.1);
                        break;
                    case 'success':
                        oscillator.frequency.setValueAtTime(523, audioContext.currentTime);
                        oscillator.frequency.setValueAtTime(659, audioContext.currentTime + 0.1);
                        oscillator.frequency.setValueAtTime(784, audioContext.currentTime + 0.2);
                        gainNode.gain.setValueAtTime(0.1, audioContext.currentTime);
                        gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.3);
                        break;
                    case 'error':
                        oscillator.frequency.setValueAtTime(200, audioContext.currentTime);
                        oscillator.frequency.exponentialRampToValueAtTime(100, audioContext.currentTime + 0.2);
                        gainNode.gain.setValueAtTime(0.1, audioContext.currentTime);
                        gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.2);
                        break;
                    case 'toggle':
                        oscillator.frequency.setValueAtTime(600, audioContext.currentTime);
                        oscillator.frequency.setValueAtTime(900, audioContext.currentTime + 0.05);
                        gainNode.gain.setValueAtTime(0.08, audioContext.currentTime);
                        gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.1);
                        break;
                    case 'hover':
                        oscillator.frequency.setValueAtTime(300, audioContext.currentTime);
                        oscillator.frequency.exponentialRampToValueAtTime(400, audioContext.currentTime + 0.05);
                        gainNode.gain.setValueAtTime(0.03, audioContext.currentTime);
                        gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.05);
                        break;
                }

                oscillator.start(audioContext.currentTime);
                oscillator.stop(audioContext.currentTime + 0.3);
            } catch (error) {
                console.log('Audio not supported');
            }
        }

        // Add haptic feedback for mobile devices
        function vibrate(pattern = [50]) {
            if (navigator.vibrate) {
                navigator.vibrate(pattern);
            }
        }

        // Enhanced button event listeners
        document.addEventListener('DOMContentLoaded', function() {
            // Add sound and haptic feedback to all clickable elements
            document.addEventListener('click', function(e) {
                if (e.target.matches('button, .menu-card, .game-button, .cheat-toggle, .control-btn, .tab-button')) {
                    playButtonSound('click');
                    vibrate([30]);
                }

                if (e.target.matches('.cheat-toggle')) {
                    playButtonSound('toggle');
                    vibrate([50, 50, 50]);
                }
            });

            // Add hover sounds for desktop
            document.addEventListener('mouseenter', function(e) {
                if (e.target.matches('button, .menu-card, .game-button, .cheat-toggle, .control-btn, .tab-button')) {
                    playButtonSound('hover');
                }
            }, true);

            // Initialize audio context on first user interaction
            document.addEventListener('touchstart', initAudio, { once: true });
            document.addEventListener('click', initAudio, { once: true });

            // Initialize quick panel toggle states
            updateQuickPanelStates();

            // Handle Welcome Screen
            handleWelcomeScreen();
        });

        // NFC-triggered Welcome Screen
        function handleWelcomeScreen() {
            const welcomeOverlay = document.getElementById('welcome-overlay');
            if (window.location.hash === '#welcome') {
                welcomeOverlay.classList.remove('hidden');

                setTimeout(() => {
                    welcomeOverlay.classList.add('hidden');
                }, 300000); // Show for 5 minutes

                // Clean the URL
                history.pushState("", document.title, window.location.pathname + window.location.search);
            }
        }

        // Profile photo upload functionality
        function openProfileUpload() {
            const input = document.createElement('input');
            input.type = 'file';
            input.accept = 'image/*';
            input.onchange = function(e) {
                const file = e.target.files[0];
                if (file) {
                    const reader = new FileReader();
                    reader.onload = function(e) {
                        const profilePic = document.getElementById('profile-pic');
                        profilePic.innerHTML = `<img src="${e.target.result}" style="width: 100%; height: 100%; border-radius: 50%; object-fit: cover;">`;
                        showNotification('Profile photo updated!', 'success');
                    };
                    reader.readAsDataURL(file);
                }
            };
            input.click();
        }

        // NEW FEATURE 1: Quick Features Panel
        let quickFeatures = {
            autosave: false,
            fastmode: false,
            silent: false,
            performance: false,
            bgmusic: true,
            sounds: true
        };

        // Background music setup
        let backgroundMusic = null;
        let musicInitialized = false;

        function initBackgroundMusic() {
            if (musicInitialized) return;

            // Create phonk-style background music using Web Audio API
            if (audioContext) {
                backgroundMusic = {
                    oscillator1: null,
                    oscillator2: null,
                    gainNode: null,
                    filterNode: null,
                    playing: false
                };

                musicInitialized = true;
                if (quickFeatures.bgmusic) {
                    startBackgroundMusic();
                }
            }
        }

        function startBackgroundMusic() {
            if (!musicInitialized || !audioContext || backgroundMusic.playing) return;

            try {
                // Create multiple oscillators for phonk-style beats
                backgroundMusic.oscillator1 = audioContext.createOscillator();
                backgroundMusic.oscillator2 = audioContext.createOscillator();
                backgroundMusic.gainNode = audioContext.createGain();
                backgroundMusic.filterNode = audioContext.createBiquadFilter();

                // Set up phonk-style frequencies and effects
                backgroundMusic.oscillator1.frequency.setValueAtTime(55, audioContext.currentTime); // Low bass
                backgroundMusic.oscillator2.frequency.setValueAtTime(110, audioContext.currentTime); // Sub bass

                backgroundMusic.oscillator1.type = 'sawtooth';
                backgroundMusic.oscillator2.type = 'square';

                // Low-pass filter for that muffled phonk sound
                backgroundMusic.filterNode.type = 'lowpass';
                backgroundMusic.filterNode.frequency.setValueAtTime(200, audioContext.currentTime);
                backgroundMusic.filterNode.Q.setValueAtTime(8, audioContext.currentTime);

                // Very low volume for background
                backgroundMusic.gainNode.gain.setValueAtTime(0.03, audioContext.currentTime);

                // Connect the audio nodes
                backgroundMusic.oscillator1.connect(backgroundMusic.filterNode);
                backgroundMusic.oscillator2.connect(backgroundMusic.filterNode);
                backgroundMusic.filterNode.connect(backgroundMusic.gainNode);
                backgroundMusic.gainNode.connect(audioContext.destination);

                // Start the music
                backgroundMusic.oscillator1.start(audioContext.currentTime);
                backgroundMusic.oscillator2.start(audioContext.currentTime);

                // Create phonk rhythm pattern
                createPhonkPattern();

                backgroundMusic.playing = true;
            } catch (error) {
                console.log('Background music failed to start');
            }
        }

        function createPhonkPattern() {
            if (!backgroundMusic.playing) return;

            // Create a repeating phonk pattern
            setTimeout(() => {
                if (backgroundMusic.playing && backgroundMusic.filterNode) {
                    // Bass drop effect
                    backgroundMusic.filterNode.frequency.exponentialRampToValueAtTime(80, audioContext.currentTime + 0.1);
                    backgroundMusic.gainNode.gain.exponentialRampToValueAtTime(0.05, audioContext.currentTime + 0.1);

                    setTimeout(() => {
                        if (backgroundMusic.playing && backgroundMusic.filterNode) {
                            backgroundMusic.filterNode.frequency.exponentialRampToValueAtTime(200, audioContext.currentTime + 0.2);
                            backgroundMusic.gainNode.gain.exponentialRampToValueAtTime(0.03, audioContext.currentTime + 0.2);
                        }
                    }, 200);
                }

                if (backgroundMusic.playing) {
                    createPhonkPattern(); // Loop the pattern
                }
            }, 800);
        }

        function stopBackgroundMusic() {
            if (!backgroundMusic || !backgroundMusic.playing) return;

            try {
                if (backgroundMusic.oscillator1) {
                    backgroundMusic.oscillator1.stop();
                    backgroundMusic.oscillator1 = null;
                }
                if (backgroundMusic.oscillator2) {
                    backgroundMusic.oscillator2.stop();
                    backgroundMusic.oscillator2 = null;
                }
                backgroundMusic.playing = false;
            } catch (error) {
                console.log('Error stopping background music');
            }
        }

        function toggleQuickPanel() {
            const panel = document.getElementById('quick-panel');
            panel.classList.toggle('open');
            playButtonSound('click');
        }

        function toggleQuickFeature(feature) {
            quickFeatures[feature] = !quickFeatures[feature];
            const toggle = event.target;
            toggle.classList.toggle('active');

            let message = '';
            switch(feature) {
                case 'autosave':
                    message = quickFeatures[feature] ? 'Auto Save enabled' : 'Auto Save disabled';
                    break;
                case 'fastmode':
                    message = quickFeatures[feature] ? 'Fast Mode activated' : 'Fast Mode deactivated';
                    break;
                case 'silent':
                    message = quickFeatures[feature] ? 'Silent Mode on' : 'Silent Mode off';
                    break;
                case 'performance':
                    message = quickFeatures[feature] ? 'Performance+ enabled' : 'Performance+ disabled';
                    break;
                case 'bgmusic':
                    if (quickFeatures[feature]) {
                        initBackgroundMusic();
                        startBackgroundMusic();
                        message = 'Background Music on';
                    } else {
                        stopBackgroundMusic();
                        message = 'Background Music off';
                    }
                    break;
                case 'sounds':
                    message = quickFeatures[feature] ? 'Button Sounds enabled' : 'Button Sounds disabled';
                    break;
            }

            showNotification(message, quickFeatures[feature] ? 'success' : 'info');
            updatePerformanceStats();
        }

        function updateQuickPanelStates() {
            // Set initial states for quick panel toggles
            const toggles = document.querySelectorAll('.quick-toggle');
            toggles.forEach((toggle, index) => {
                const features = ['autosave', 'fastmode', 'silent', 'performance', 'bgmusic', 'sounds'];
                if (features[index] && quickFeatures[features[index]]) {
                    toggle.classList.add('active');
                }
            });
        }

        // NEW FEATURE 2: Game Timer and Performance Stats
        let sessionStartTime = Date.now();
        let gameStatsInterval;

        function startGameStats() {
            if (gameStatsInterval) clearInterval(gameStatsInterval);

            gameStatsInterval = setInterval(() => {
                const elapsed = Date.now() - sessionStartTime;
                const minutes = Math.floor(elapsed / 60000);
                const seconds = Math.floor((elapsed % 60000) / 1000);

                document.getElementById('session-time').textContent =
                    `${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}`;

                updatePerformanceStats();
            }, 1000);
        }

        function updatePerformanceStats() {
            const activeCheats = countActiveCheats();
            document.getElementById('active-cheats').textContent = activeCheats;

            let performance = 100;
            performance -= activeCheats * 2; // Each cheat reduces performance
            performance += quickFeatures.performance ? 15 : 0; // Performance+ boost
            performance = Math.max(60, Math.min(100, performance));

            document.getElementById('performance').textContent = performance + '%';
        }

        function countActiveCheats() {
            let count = 0;
            if (currentGameData) {
                Object.values(currentGameData).forEach(category => {
                    category.forEach(cheat => {
                        if (cheat.active) count++;
                    });
                });
            }
            return count;
        }

        // NEW FEATURE 3: Favorites System
        let favoritesCheats = JSON.parse(localStorage.getItem('favoritesCheats') || '[]');

        function toggleFavorite(cheatName) {
            const index = favoritesCheats.findIndex(fav => fav.name === cheatName);
            if (index === -1) {
                favoritesCheats.push({ name: cheatName, game: currentGame });
                showNotification(`${cheatName} added to favorites`, 'success');
            } else {
                favoritesCheats.splice(index, 1);
                showNotification(`${cheatName} removed from favorites`, 'info');
            }

            localStorage.setItem('favoritesCheats', JSON.stringify(favoritesCheats));

            const star = event.target;
            star.classList.toggle('active');
        }

        function isFavorite(cheatName) {
            return favoritesCheats.some(fav => fav.name === cheatName && fav.game === currentGame);
        }



        // NEW FEATURE 4: Enhanced Cheat Controls with Auto-save
        function autoSaveSettings() {
            if (quickFeatures.autosave && currentGameData) {
                const settings = {
                    game: currentGame,
                    cheats: currentGameData,
                    quickFeatures: quickFeatures,
                    timestamp: Date.now()
                };

                localStorage.setItem('autoSavedSettings', JSON.stringify(settings));
            }
        }

        function loadAutoSavedSettings() {
            const saved = localStorage.getItem('autoSavedSettings');
            if (saved) {
                const settings = JSON.parse(saved);
                if (settings.game === currentGame) {
                    showNotification('Auto-saved settings loaded', 'success');
                }
            }
        }

        // Game data with enhanced cheat descriptions
        const gameData = {
            'Call of Duty: B06': {
                basic: [
                    { name: 'Unlimited Health', desc: 'Never die from enemy damage', active: false },
                    { name: 'Unlimited Ammo', desc: 'Infinite ammunition for all weapons', active: false },
                    { name: 'No Reload', desc: 'Skip all weapon reload animations', active: false },
                    { name: 'Speed Hack', desc: 'Move faster than other players', active: false },
                    { name: 'Jump Hack', desc: 'Jump higher and farther distances', active: false }
                ],
                advanced: [
                    { name: 'Enemy ESP', desc: 'See enemies through walls and obstacles', active: false },
                    { name: 'Radar Hack', desc: 'Show all enemies on minimap', active: false },
                    { name: 'Auto Aim', desc: 'Automatic target acquisition and tracking', active: false },
                    { name: 'Kill Cam Skip', desc: 'Skip death replay sequences', active: false },
                    { name: 'Thermal Vision', desc: 'See heat signatures of all players', active: false }
                ],
                weapons: [
                    { name: 'Damage Multiplier', desc: 'Increase weapon damage output', active: false, hasValue: true, value: '2.0' },
                    { name: 'No Recoil', desc: 'Remove weapon recoil patterns', active: false },
                    { name: 'Rapid Fire', desc: 'Increase weapon fire rate', active: false },
                    { name: 'Instant Kill', desc: 'One-shot eliminate any enemy', active: false },
                    { name: 'Weapon Unlock', desc: 'Access all weapons and attachments', active: false }
                ],
                player: [
                    { name: 'Invisibility', desc: 'Become invisible to enemy players', active: false },
                    { name: 'Super Soldier', desc: 'Enhanced physical abilities', active: false },
                    { name: 'Tactical Insert Spam', desc: 'Unlimited tactical insertions', active: false },
                    { name: 'Field Upgrade Boost', desc: 'Faster field upgrade recharge', active: false },
                    { name: 'Scorestreak Hack', desc: 'Instant scorestreak rewards', active: false }
                ],
                world: [
                    { name: 'Time Manipulation', desc: 'Control game time speed', active: false },
                    { name: 'Weather Control', desc: 'Change weather conditions', active: false },
                    { name: 'Spawn Items', desc: 'Spawn any item/weapon', active: false },
                    { name: 'Remove Fog', desc: 'Clear all fog and smoke', active: false },
                    { name: 'Night Vision', desc: 'See clearly in dark areas', active: false }
                ]
            },
            'Call of Duty: Modern Warfare': {
                basic: [
                    { name: 'Unlimited Health', desc: 'Never die from enemy damage', active: false },
                    { name: 'Unlimited Ammo', desc: 'Infinite ammunition for all weapons', active: false },
                    { name: 'No Reload', desc: 'Skip all weapon reload animations', active: false },
                    { name: 'Speed Hack', desc: 'Move faster than other players', active: false },
                    { name: 'Jump Hack', desc: 'Jump higher and farther distances', active: false }
                ],
                advanced: [
                    { name: 'Enemy ESP', desc: 'See enemies through walls and obstacles', active: false },
                    { name: 'Radar Hack', desc: 'Show all enemies on minimap', active: false },
                    { name: 'Auto Aim', desc: 'Automatic target acquisition and tracking', active: false },
                    { name: 'Kill Cam Skip', desc: 'Skip death replay sequences', active: false },
                    { name: 'Thermal Vision', desc: 'See heat signatures of all players', active: false }
                ],
                weapons: [
                    { name: 'Damage Multiplier', desc: 'Increase weapon damage output', active: false, hasValue: true, value: '2.0' },
                    { name: 'No Recoil', desc: 'Remove weapon recoil patterns', active: false },
                    { name: 'Rapid Fire', desc: 'Increase weapon fire rate', active: false },
                    { name: 'Instant Kill', desc: 'One-shot eliminate any enemy', active: false },
                    { name: 'Weapon Unlock', desc: 'Access all weapons and attachments', active: false }
                ],
                player: [
                    { name: 'Invisibility', desc: 'Become invisible to enemy players', active: false },
                    { name: 'Super Soldier', desc: 'Enhanced physical abilities', active: false },
                    { name: 'Tactical Insert Spam', desc: 'Unlimited tactical insertions', active: false },
                    { name: 'Field Upgrade Boost', desc: 'Faster field upgrade recharge', active: false },
                    { name: 'Scorestreak Hack', desc: 'Instant scorestreak rewards', active: false }
                ],
                world: [
                    { name: 'Time Manipulation', desc: 'Control game time speed', active: false },
                    { name: 'Weather Control', desc: 'Change weather conditions', active: false },
                    { name: 'Spawn Items', desc: 'Spawn any item/weapon', active: false },
                    { name: 'Remove Fog', desc: 'Clear all fog and smoke', active: false },
                    { name: 'Night Vision', desc: 'See clearly in dark areas', active: false }
                ]
            },
            'Counter-Strike 2': {
                basic: [
                    { name: 'Unlimited Health', desc: 'Never die from damage', active: false },
                    { name: 'Unlimited Armor', desc: 'Permanent armor protection', active: false },
                    { name: 'Unlimited Money', desc: 'Buy anything you want', active: false, hasValue: true, value: '16000' },
                    { name: 'No Recoil', desc: 'Weapons have no recoil', active: false },
                    { name: 'Bunny Hop', desc: 'Perfect bunny hopping', active: false }
                ],
                advanced: [
                    { name: 'Triggerbot', desc: 'Auto-fire when crosshair on enemy', active: false },
                    { name: 'Glow ESP', desc: 'Make enemies glow through walls', active: false },
                    { name: 'Bomb Timer', desc: 'Show precise bomb timer', active: false },
                    { name: 'Smoke ESP', desc: 'See through smoke grenades', active: false },
                    { name: 'Sound ESP', desc: 'Visual indicators for sounds', active: false }
                ],
                weapons: [
                    { name: 'Perfect Accuracy', desc: 'All shots hit exactly where aimed', active: false },
                    { name: 'Rapid Fire', desc: 'Increase weapon fire rate', active: false },
                    { name: 'Instant Reload', desc: 'Reload weapons instantly', active: false },
                    { name: 'AWP No Scope', desc: 'AWP accurate without scoping', active: false },
                    { name: 'Knife Range', desc: 'Extend knife attack range', active: false }
                ],
                player: [
                    { name: 'Speed Boost', desc: 'Move faster than opponents', active: false },
                    { name: 'Silent Walk', desc: 'Make no footstep sounds', active: false },
                    { name: 'Anti-Flash', desc: 'Immune to flashbang effects', active: false },
                    { name: 'Third Person', desc: 'View character from behind', active: false },
                    { name: 'No Spread', desc: 'Eliminate weapon spread pattern', active: false }
                ],
                world: [
                    { name: 'X-Ray Walls', desc: 'See enemies through any surface', active: false },
                    { name: 'Grenade Trajectory', desc: 'Show grenade throw paths', active: false },
                    { name: 'Spawn Protection', desc: 'Brief invulnerability after spawn', active: false },
                    { name: 'FPS Boost', desc: 'Increase game performance', active: false },
                    { name: 'Spectator Mode', desc: 'Observe match as spectator', active: false }
                ]
            },
            'Valorant': {
                basic: [
                    { name: 'Unlimited Health', desc: 'Never lose health points', active: false },
                    { name: 'Unlimited Armor', desc: 'Permanent shield protection', active: false },
                    { name: 'No Ability Cooldown', desc: 'Use abilities instantly', active: false },
                    { name: 'Unlimited Credits', desc: 'Buy anything in shop', active: false },
                    { name: 'Movement Speed', desc: 'Increase movement speed', active: false }
                ],
                advanced: [
                    { name: 'Agent ESP', desc: 'See enemy agents through walls', active: false },
                    { name: 'Ability ESP', desc: 'Show enemy ability usage', active: false },
                    { name: 'Spike ESP', desc: 'Always see spike location', active: false },
                    { name: 'Orb ESP', desc: 'Show ultimate orb locations', active: false },
                    { name: 'Utility ESP', desc: 'See all placed utilities', active: false }
                ],
                weapons: [
                    { name: 'Headshot Only', desc: 'All shots register as headshots', active: false },
                    { name: 'Auto Aim', desc: 'Automatic target acquisition', active: false },
                    { name: 'Weapon Swap Speed', desc: 'Instant weapon switching', active: false },
                    { name: 'Vandal/Phantom Mod', desc: 'Enhanced rifle performance', active: false },
                    { name: 'Operator Mods', desc: 'Improved sniper rifle', active: false }
                ],
                player: [
                    { name: 'Jett Dash Spam', desc: 'Unlimited Jett dashes', active: false },
                    { name: 'Sage Heal Spam', desc: 'Unlimited healing orbs', active: false },
                    { name: 'Phoenix Molly Spam', desc: 'Unlimited hot hands', active: false },
                    { name: 'Sova Drone God', desc: 'Enhanced drone abilities', active: false },
                    { name: 'Raze Rocket Spam', desc: 'Unlimited showstoppers', active: false }
                ],
                world: [
                    { name: 'Map Hack', desc: 'See entire map layout', active: false },
                    { name: 'Round Timer', desc: 'Extended round duration', active: false },
                    { name: 'Rank Boost', desc: 'Artificial rank enhancement', active: false },
                    { name: 'Anti-Cheat Bypass', desc: 'Evade detection systems', active: false },
                    { name: 'Performance Mode', desc: 'Optimize game performance', active: false }
                ]
            },
            'Apex Legends': {
                basic: [
                    { name: 'Unlimited Health', desc: 'Never lose health or shields', active: false },
                    { name: 'Unlimited Ammo', desc: 'Never run out of ammunition', active: false },
                    { name: 'No Reload Required', desc: 'Skip all reload animations', active: false },
                    { name: 'Speed Boost', desc: 'Move faster than other players', active: false },
                    { name: 'Jump Boost', desc: 'Jump higher and farther', active: false }
                ],
                advanced: [
                    { name: 'Enemy ESP', desc: 'See enemies through walls', active: false },
                    { name: 'Loot ESP', desc: 'See all loot through walls', active: false },
                    { name: 'Third Party Alert', desc: 'Warning for incoming teams', active: false },
                    { name: 'Ring Predictor', desc: 'Predict next ring location', active: false },
                    { name: 'Team Tracker', desc: 'Track all team movements', active: false }
                ],
                weapons: [
                    { name: 'Aimbot', desc: 'Automatic enemy targeting', active: false },
                    { name: 'No Recoil', desc: 'Remove weapon recoil patterns', active: false },
                    { name: 'Damage Multiplier', desc: 'Increase weapon damage', active: false, hasValue: true, value: '2.5' },
                    { name: 'Kraber Mods', desc: 'Enhanced sniper performance', active: false },
                    { name: 'Mastiff Spam', desc: 'Rapid fire shotgun mode', active: false }
                ],
                player: [
                    { name: 'Wraith Portal Spam', desc: 'Unlimited portal usage', active: false },
                    { name: 'Pathfinder Grapple', desc: 'Extended grapple range', active: false },
                    { name: 'Octane Stim God', desc: 'Permanent stim effects', active: false },
                    { name: 'Bloodhound Scan', desc: 'Permanent enemy scanning', active: false },
                    { name: 'Lifeline Revive', desc: 'Instant revive abilities', active: false }
                ],
                world: [
                    { name: 'Flying Mode', desc: 'Fly around the map freely', active: false },
                    { name: 'Teleport Hack', desc: 'Instant teleportation anywhere', active: false },
                    { name: 'Loot Magnet', desc: 'Auto-collect nearby loot', active: false },
                    { name: 'Zone Immunity', desc: 'Take no damage from ring', active: false },
                    { name: 'Supply Drop Control', desc: 'Control care package drops', active: false }
                ]
            },
            'Fortnite': {
                basic: [
                    { name: 'Unlimited Health', desc: 'Never lose health or shield', active: false },
                    { name: 'Unlimited Materials', desc: 'Infinite building materials', active: false },
                    { name: 'Auto Pickup', desc: 'Automatically collect items', active: false },
                    { name: 'Speed Hack', desc: 'Move faster than opponents', active: false },
                    { name: 'Jump Hack', desc: 'Super jump abilities', active: false }
                ],
                advanced: [
                    { name: 'Player ESP', desc: 'See players through walls', active: false },
                    { name: 'Chest ESP', desc: 'See all chests and loot', active: false },
                    { name: 'Vehicle ESP', desc: 'Locate all vehicles on map', active: false },
                    { name: 'Storm Predictor', desc: 'Predict storm movements', active: false },
                    { name: 'Build ESP', desc: 'See enemy building structures', active: false }
                ],
                weapons: [
                    { name: 'Aimbot', desc: 'Automatic target locking', active: false },
                    { name: 'Silent Aim', desc: 'Hit targets without aiming', active: false },
                    { name: 'Magic Bullet', desc: 'Bullets curve to hit targets', active: false },
                    { name: 'Rapid Fire', desc: 'Increase weapon fire rate', active: false },
                    { name: 'No Bloom', desc: 'Perfect weapon accuracy', active: false }
                ],
                player: [
                    { name: 'Build Anywhere', desc: 'Build in any location', active: false },
                    { name: 'Instant Build', desc: 'Build structures instantly', active: false },
                    { name: 'Anti-Build', desc: 'Prevent enemies from building', active: false },
                    { name: 'Edit Speed', desc: 'Lightning fast structure edits', active: false },
                    { name: 'Glider Redeploy', desc: 'Use glider anytime', active: false }
                ],
                world: [
                    { name: 'V-Bucks Generator', desc: 'Generate free V-Bucks', active: false, hasValue: true, value: '1000' },
                    { name: 'Skin Unlocker', desc: 'Unlock all cosmetic skins', active: false },
                    { name: 'XP Multiplier', desc: 'Boost experience gain', active: false, hasValue: true, value: '5.0' },
                    { name: 'Battle Pass Tier', desc: 'Auto-complete battle pass', active: false },
                    { name: 'Creative Mode', desc: 'Full creative powers in game', active: false }
                ]
            },
            'GTA V Online': {
                basic: [
                    { name: 'Unlimited Health', desc: 'Never die from damage', active: false },
                    { name: 'Unlimited Money', desc: 'Infinite in-game currency', active: false, hasValue: true, value: '999999999' },
                    { name: 'Wanted Level Lock', desc: 'Set and lock wanted level', active: false },
                    { name: 'Super Speed', desc: 'Move extremely fast', active: false },
                    { name: 'Super Jump', desc: 'Jump incredibly high', active: false }
                ],
                advanced: [
                    { name: 'Player ESP', desc: 'See all players on map', active: false },
                    { name: 'Vehicle ESP', desc: 'See all vehicles and info', active: false },
                    { name: 'Pickup ESP', desc: 'See all pickups and collectibles', active: false },
                    { name: 'Mission Helper', desc: 'Auto-complete mission objectives', active: false },
                    { name: 'Radar Mod', desc: 'Enhanced radar capabilities', active: false }
                ],
                weapons: [
                    { name: 'Unlimited Ammo', desc: 'Never run out of ammunition', active: false },
                    { name: 'One Shot Kill', desc: 'Kill any enemy with one shot', active: false },
                    { name: 'Explosive Bullets', desc: 'All bullets explode on impact', active: false },
                    { name: 'Weapon Spawner', desc: 'Spawn any weapon instantly', active: false },
                    { name: 'No Weapon Degradation', desc: 'Weapons never break', active: false }
                ],
                player: [
                    { name: 'Level Modifier', desc: 'Set character level', active: false, hasValue: true, value: '8000' },
                    { name: 'Skill Maxer', desc: 'Max out all character skills', active: false },
                    { name: 'Reputation Boost', desc: 'Increase RP gain rate', active: false },
                    { name: 'Property Unlocker', desc: 'Access any property', active: false },
                    { name: 'VIP Status', desc: 'Get VIP lobby benefits', active: false }
                ],
                world: [
                    { name: 'Vehicle Spawner', desc: 'Spawn any vehicle anywhere', active: false },
                    { name: 'Teleporter', desc: 'Teleport to any location', active: false },
                    { name: 'Weather Control', desc: 'Change weather conditions', active: false },
                    { name: 'Time Control', desc: 'Control day/night cycle', active: false },
                    { name: 'Physics Mod', desc: 'Modify game physics', active: false }
                ]
            }
        };

        let currentGameName = '';
        let currentTab = 'basic';
        let currentGame = '';
        let currentGameData = null;

        // Enhanced Notification System
        function showNotification(message, type = 'info', duration = 2000) {
            const notification = document.createElement('div');
            notification.className = `notification ${type}`;
            notification.textContent = message;

            document.getElementById('notification-container').appendChild(notification);

            // Play appropriate sound
            if (type === 'success') {
                playButtonSound('success');
                vibrate([100, 50, 100]);
            } else if (type === 'error') {
                playButtonSound('error');
                vibrate([200, 100, 200]);
            }

            setTimeout(() => {
                notification.classList.add('fade-out');
                setTimeout(() => {
                    if (notification.parentNode) {
                        notification.parentNode.removeChild(notification);
                    }
                }, 500);
            }, duration);
        }

        // Page Navigation
        function showPage(pageId) {
            document.querySelectorAll('.page').forEach(page => page.classList.add('hidden'));
            document.getElementById(pageId).classList.remove('hidden');
        }

        function showMenu() {
            showPage('menu-page');
            showNotification('Returned to game selection', 'info');
        }

        // Authentication
        async function login() {
            const username = document.getElementById('username').value;
            const password = document.getElementById('password').value;
            const messageDiv = document.getElementById('login-message');

            if (!username || !password) {
                messageDiv.innerHTML = '<div class="error-message">Please enter both username and password</div>';
                showNotification('Please fill all fields', 'error');
                return;
            }

            try {
                const response = await fetch('/login', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json',
                    },
                    body: JSON.stringify({ username, password })
                });

                const result = await response.json();

                if (result.success) {
                    messageDiv.innerHTML = '<div class="success-message">Login successful! Redirecting...</div>';
                    showNotification('Welcome to VENDOR.ME!', 'success');
                    setTimeout(() => {
                        showPage('menu-page');
                    }, 1000);
                } else {
                    messageDiv.innerHTML = '<div class="error-message">' + result.message + '</div>';
                    showNotification('Login failed', 'error');
                }
            } catch (error) {
                messageDiv.innerHTML = '<div class="error-message">Connection error. Please try again.</div>';
                showNotification('Connection error', 'error');
            }
        }

        // Logout
        async function logout() {
            try {
                await fetch('/logout', { method: 'POST' });
                showNotification('Logged out successfully', 'info');
                setTimeout(() => {
                    showPage('login-page');
                    document.getElementById('username').value = 'admin';
                    document.getElementById('password').value = 'cheat123';
                    document.getElementById('login-message').innerHTML = '';
                }, 1000);
            } catch (error) {
                showNotification('Logout error', 'error');
            }
        }

        // Game Selection with Enhanced Features
        function selectGame(gameName) {
            currentGameName = gameName;
            currentGame = gameName; // For favorites system
            currentGameData = gameData[gameName]; // For stats tracking

            document.getElementById('game-title').textContent = gameName;
            showPage('app-page');

            // Update active game button
            document.querySelectorAll('.game-button').forEach(btn => {
                btn.classList.remove('active');
                if (btn.dataset.game === gameName) {
                    btn.classList.add('active');
                }
            });

            loadGameCheats(gameName);
            startGameStats(); // Start session timer and stats
            loadAutoSavedSettings(); // Load auto-saved settings if available
            showNotification(`Selected ${gameName}`, 'success');
        }

        // Tab Management
        function showTab(tabName) {
            currentTab = tabName;
            document.querySelectorAll('.tab-button').forEach(btn => btn.classList.remove('active'));
            event.target.classList.add('active');
            renderCheats();
        }

        // Load Game Cheats
        function loadGameCheats(gameName) {
            document.getElementById('current-game').textContent = gameName;
            renderCheats();
        }

        // Render Cheats
        function renderCheats() {
            const cheatList = document.getElementById('cheat-list');

            if (!currentGameName || !gameData[currentGameName]) {
                cheatList.innerHTML = '<div style="text-align: center; color: #666; margin-top: 50px;"><p>Select a game to view available cheats</p></div>';
                return;
            }

            const cheats = gameData[currentGameName][currentTab] || [];
            const searchTerm = document.getElementById('search-input').value.toLowerCase();

            const filteredCheats = cheats.filter(cheat =>
                cheat.name.toLowerCase().includes(searchTerm) ||
                cheat.desc.toLowerCase().includes(searchTerm)
            );

            if (filteredCheats.length === 0) {
                cheatList.innerHTML = '<div style="text-align: center; color: #666; margin-top: 50px;"><p>No cheats found matching your search</p></div>';
                return;
            }

            cheatList.innerHTML = filteredCheats.map(cheat => `
                <div class="cheat-item ${cheat.active ? 'active' : ''}" data-cheat="${cheat.name}">
                    <div class="cheat-info">
                        <div class="cheat-name">
                            ${cheat.name}
                            <span class="favorite-star ${isFavorite(cheat.name) ? 'active' : ''}" onclick="toggleFavorite('${cheat.name}')">★</span>
                        </div>
                        <div class="cheat-desc">${cheat.desc}</div>
                    </div>
                    <div style="display: flex; align-items: center; gap: 8px;">
                        ${cheat.hasValue ? `<input type="text" class="value-input" value="${cheat.value}" onchange="updateCheatValue('${cheat.name}', this.value)">` : ''}
                        <div class="cheat-toggle ${cheat.active ? 'active' : ''}" onclick="toggleCheat('${cheat.name}')"></div>
                    </div>
                </div>
            `).join('');
        }

        // Enhanced Toggle Individual Cheat
        async function toggleCheat(cheatName) {
            if (!currentGameName) return;

            const cheat = gameData[currentGameName][currentTab].find(c => c.name === cheatName);
            if (!cheat) return;

            cheat.active = !cheat.active;

            try {
                await fetch('/toggle-cheat', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json',
                    },
                    body: JSON.stringify({
                        game: currentGameName,
                        cheat: cheatName,
                        active: cheat.active
                    })
                });

                const statusText = cheat.active ? 'ENABLED' : 'DISABLED';
                const statusType = cheat.active ? 'success' : 'info';
                showNotification(`${cheatName} ${statusText}`, statusType);

                renderCheats();
                updatePerformanceStats(); // Update live stats
                autoSaveSettings(); // Auto-save if enabled
            } catch (error) {
                showNotification('Failed to toggle cheat', 'error');
                cheat.active = !cheat.active; // Revert on error
            }
        }



        // Toggle All Cheats
        function toggleAllCheats(enable) {
            if (!currentGameName) return;

            const cheats = gameData[currentGameName][currentTab];
            cheats.forEach(cheat => cheat.active = enable);

            const statusText = enable ? 'ENABLED' : 'DISABLED';
            const statusType = enable ? 'success' : 'info';
            showNotification(`All ${currentTab} cheats ${statusText}`, statusType);

            renderCheats();
        }

        // Update Cheat Value
        function updateCheatValue(cheatName, value) {
            if (!currentGameName) return;

            const cheat = gameData[currentGameName][currentTab].find(c => c.name === cheatName);
            if (cheat && cheat.hasValue) {
                cheat.value = value;
                showNotification(`${cheatName} value updated to ${value}`, 'info');
            }
        }

        // Filter Cheats
        function filterCheats() {
            renderCheats();
        }

        // Enter key support for login
        document.addEventListener('keypress', function(e) {
            if (e.key === 'Enter') {
                const currentPage = document.querySelector('.page:not(.hidden)');
                if (currentPage && currentPage.id === 'login-page') {
                    login();
                }
            }
        });

        // Initialize the application
        document.addEventListener('DOMContentLoaded', function() {
            showPage('login-page');
        });
    </script>
</body>
</html>
)raw";

// =================================================================================================
// SERVER ENDPOINTS AND ESP32 FUNCTIONALITY
// =================================================================================================

// Initialize game database
void initializeGameDatabase() {
  // Call of Duty: B06
  gameDatabase["Call of Duty: B06"].basicCheats["Unlimited Health"] = false;
  gameDatabase["Call of Duty: B06"].basicCheats["Unlimited Ammo"] = false;
  gameDatabase["Call of Duty: B06"].basicCheats["No Reload"] = false;
  gameDatabase["Call of Duty: B06"].basicCheats["Speed Hack"] = false;
  gameDatabase["Call of Duty: B06"].basicCheats["Jump Hack"] = false;

  // Counter-Strike 2
  gameDatabase["Counter-Strike 2"].basicCheats["Unlimited Health"] = false;
  gameDatabase["Counter-Strike 2"].basicCheats["Unlimited Armor"] = false;
  gameDatabase["Counter-Strike 2"].basicCheats["Unlimited Money"] = false;
  gameDatabase["Counter-Strike 2"].basicCheats["No Recoil"] = false;
  gameDatabase["Counter-Strike 2"].basicCheats["Bunny Hop"] = false;

  // Valorant
  gameDatabase["Valorant"].basicCheats["Unlimited Health"] = false;
  gameDatabase["Valorant"].basicCheats["Unlimited Armor"] = false;
  gameDatabase["Valorant"].basicCheats["No Ability Cooldown"] = false;
  gameDatabase["Valorant"].basicCheats["Unlimited Credits"] = false;
  gameDatabase["Valorant"].basicCheats["Movement Speed"] = false;

  // Apex Legends
  gameDatabase["Apex Legends"].basicCheats["Unlimited Health"] = false;
  gameDatabase["Apex Legends"].basicCheats["Unlimited Ammo"] = false;
  gameDatabase["Apex Legends"].basicCheats["No Reload Required"] = false;
  gameDatabase["Apex Legends"].basicCheats["Speed Boost"] = false;
  gameDatabase["Apex Legends"].basicCheats["Jump Boost"] = false;

  // Fortnite
  gameDatabase["Fortnite"].basicCheats["Unlimited Health"] = false;
  gameDatabase["Fortnite"].basicCheats["Unlimited Materials"] = false;
  gameDatabase["Fortnite"].basicCheats["Auto Pickup"] = false;
  gameDatabase["Fortnite"].basicCheats["Speed Hack"] = false;
  gameDatabase["Fortnite"].basicCheats["Jump Hack"] = false;

  // GTA V Online
  gameDatabase["GTA V Online"].basicCheats["Unlimited Health"] = false;
  gameDatabase["GTA V Online"].basicCheats["Unlimited Money"] = false;
  gameDatabase["GTA V Online"].basicCheats["Wanted Level Lock"] = false;
  gameDatabase["GTA V Online"].basicCheats["Super Speed"] = false;
  gameDatabase["GTA V Online"].basicCheats["Super Jump"] = false;
}

// Handle login requests
void handleLogin(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
  String jsonString = "";
  for (size_t i = 0; i < len; i++) {
    jsonString += (char)data[i];
  }

  // Parse JSON manually (simple approach)
  int usernameStart = jsonString.indexOf("\"username\":\"") + 12;
  int usernameEnd = jsonString.indexOf("\"", usernameStart);
  String username = jsonString.substring(usernameStart, usernameEnd);

  int passwordStart = jsonString.indexOf("\"password\":\"") + 12;
  int passwordEnd = jsonString.indexOf("\"", passwordStart);
  String password = jsonString.substring(passwordStart, passwordEnd);

  String response = "";
  if (username == validUsername && password == validPassword) {
    userLoggedIn = true;
    currentUser = username;
    response = "{\"success\":true,\"message\":\"Login successful\"}";

    // Display enhanced login success on ESP32 screen with neon colors
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_MAGENTA);
    tft.setTextSize(3);
    tft.drawString("LOGIN", 70, 60);
    tft.setTextColor(TFT_GREEN);
    tft.drawString("SUCCESS", 50, 90);
    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(2);
    tft.drawString("VENDOR.ME", 50, 130);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.drawString("User: " + username, 60, 160);
    tft.drawString("Time: " + String(millis()/1000) + "s", 60, 180);

    // Add some visual flair
    for(int i = 0; i < 5; i++) {
      tft.drawRect(i*2, i*2, 240-(i*4), 320-(i*4), TFT_MAGENTA);
    }
  } else {
    response = "{\"success\":false,\"message\":\"Invalid credentials\"}";

    // Display enhanced login failure on ESP32 screen
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED);
    tft.setTextSize(3);
    tft.drawString("LOGIN", 70, 60);
    tft.drawString("FAILED", 60, 90);
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(2);
    tft.drawString("ACCESS DENIED", 20, 130);
    tft.setTextColor(0x8410); // Gray color
    tft.setTextSize(1);
    tft.drawString("Invalid credentials", 60, 160);

    // Add warning borders
    for(int i = 0; i < 3; i++) {
      tft.drawRect(i*3, i*3, 240-(i*6), 320-(i*6), TFT_RED);
    }
  }

  request->send(200, "application/json", response);
}

// Handle logout requests
void handleLogout(AsyncWebServerRequest *request) {
  userLoggedIn = false;
  currentUser = "";
  currentGame = "";

  // Display enhanced logout on ESP32 screen
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(3);
  tft.drawString("LOGGED", 60, 60);
  tft.drawString("OUT", 90, 90);
  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(2);
  tft.drawString("VENDOR.ME", 50, 130);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.drawString("Session ended", 60, 160);
  tft.drawString("Thank you!", 70, 180);

  // Add decorative elements
  tft.drawCircle(120, 200, 30, TFT_YELLOW);
  tft.drawCircle(120, 200, 25, TFT_YELLOW);

  request->send(200, "application/json", "{\"success\":true}");
}

// Handle cheat toggle requests
void handleToggleCheat(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
  if (!userLoggedIn) {
    request->send(401, "application/json", "{\"success\":false,\"message\":\"Not logged in\"}");
    return;
  }

  String jsonString = "";
  for (size_t i = 0; i < len; i++) {
    jsonString += (char)data[i];
  }

  // Parse the cheat toggle request
  int gameStart = jsonString.indexOf("\"game\":\"") + 8;
  int gameEnd = jsonString.indexOf("\"", gameStart);
  String game = jsonString.substring(gameStart, gameEnd);

  int cheatStart = jsonString.indexOf("\"cheat\":\"") + 9;
  int cheatEnd = jsonString.indexOf("\"", cheatStart);
  String cheat = jsonString.substring(cheatStart, cheatEnd);

  bool active = jsonString.indexOf("\"active\":true") > 0;

  // Update the cheat state (simplified - in real implementation, this would interact with the actual game)
  currentGame = game;

  // Display enhanced cheat activation on ESP32 screen
  tft.fillScreen(TFT_BLACK);

  // Neon-style header
  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(2);
  tft.drawString("CHEAT ENGINE", 30, 20);
  tft.setTextColor(TFT_MAGENTA);
  tft.setTextSize(3);
  tft.drawString("VENDOR.ME", 20, 45);

  // Game info
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.drawString("Game: " + game, 10, 80);

  // Status with enhanced visuals
  if (active) {
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);
    tft.drawString("ENABLED", 10, 100);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.drawString(cheat, 10, 125);

    // Add success indicators
    tft.fillCircle(200, 110, 8, TFT_GREEN);
    tft.drawString("ON", 190, 125);
  } else {
    tft.setTextColor(TFT_RED);
    tft.setTextSize(2);
    tft.drawString("DISABLED", 10, 100);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.drawString(cheat, 10, 125);

    // Add disabled indicators
    tft.fillCircle(200, 110, 8, TFT_RED);
    tft.drawString("OFF", 188, 125);
  }

  // Enhanced info display
  tft.setTextColor(0x8410); // Gray color
  tft.drawString("Profile: " + activeConfig, 10, 160);
  tft.drawString("User: " + currentUser, 10, 180);
  tft.drawString("Time: " + String(millis()/1000) + "s", 10, 200);

  // Add decorative border
  tft.drawRect(5, 5, 230, 310, TFT_CYAN);
  tft.drawRect(6, 6, 228, 308, TFT_MAGENTA);

  request->send(200, "application/json", "{\"success\":true}");
}

// =================================================================================================
// ESP32 SETUP AND LOOP
// =================================================================================================

void setup() {
  Serial.begin(115200);

  // Initialize display
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setBrightness(128);

  // Initialize game database
  initializeGameDatabase();

  // Display enhanced startup message
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_MAGENTA);
  tft.setTextSize(3);
  tft.drawString("VENDOR.ME", 40, 60);
  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(2);
  tft.drawString("CHEAT ENGINE", 30, 100);
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(1);
  tft.drawString("Enhanced Edition", 70, 130);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("Loading systems...", 70, 150);

  // Add loading animation
  for(int i = 0; i < 10; i++) {
    tft.fillRect(20 + (i * 20), 170, 15, 8, TFT_GREEN);
    delay(200);
  }

  // Setup WiFi Access Point
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();

  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Clear screen and show enhanced WiFi info
  delay(1000);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(3);
  tft.drawString("WIFI", 80, 40);
  tft.drawString("READY", 70, 70);
  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(2);
  tft.drawString("VENDOR.ME", 50, 110);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.drawString("Network: " + String(ssid), 10, 140);
  tft.drawString("http://" + WiFi.softAPIP().toString(), 10, 160);
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(2);
  tft.drawString("ONLINE", 70, 180);

  // Add network indicator
  tft.fillCircle(200, 50, 15, TFT_GREEN);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.drawString("NET", 188, 45);

  // Setup web server routes
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleLogin);
  server.on("/logout", HTTP_POST, [](AsyncWebServerRequest *request) {
    handleLogout(request);
  });
  server.on("/toggle-cheat", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleToggleCheat);

  // CORS headers for all requests
  server.onNotFound([](AsyncWebServerRequest *request) {
    if (request->method() == HTTP_OPTIONS) {
      request->send(200);
    } else {
      request->send(404, "text/plain", "Not found");
    }
  });

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Handle WiFi and display updates with enhanced visuals
  static unsigned long lastUpdate = 0;
  static int animFrame = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - lastUpdate >= 3000) { // Update every 3 seconds
    lastUpdate = currentMillis;
    animFrame = (animFrame + 1) % 4;

    if (!userLoggedIn) {
      // Show enhanced WiFi status when no user logged in
      tft.fillScreen(TFT_BLACK);

      // Animated title
      tft.setTextColor(TFT_CYAN + (animFrame * 0x100));
      tft.setTextSize(3);
      tft.drawString("VENDOR.ME", 40, 30);

      tft.setTextColor(TFT_MAGENTA);
      tft.setTextSize(2);
      tft.drawString("CHEAT ENGINE", 20, 65);

      tft.setTextColor(TFT_WHITE);
      tft.setTextSize(1);
      tft.drawString("Enhanced with Sounds & Visuals", 20, 90);
      tft.drawString("http://" + WiFi.softAPIP().toString(), 10, 110);

      // Status indicators
      tft.setTextColor(TFT_GREEN);
      tft.setTextSize(2);
      tft.drawString("ONLINE", 70, 130);

      // Animated connection indicator
      int clientCount = WiFi.softAPgetStationNum();
      tft.setTextColor(clientCount > 0 ? TFT_GREEN : TFT_YELLOW);
      tft.setTextSize(1);
      tft.drawString("Clients: " + String(clientCount), 10, 160);
      tft.drawString("Uptime: " + String(millis()/1000) + "s", 10, 180);

      // Add pulsing border
      uint16_t borderColor = (animFrame % 2) ? TFT_CYAN : TFT_MAGENTA;
      tft.drawRect(5, 5, 230, 190, borderColor);

      // Connection status LED
      tft.fillCircle(200, 40, 10, clientCount > 0 ? TFT_GREEN : TFT_YELLOW);
    }
  }

  // Keep the web server running
  delay(10);
}
