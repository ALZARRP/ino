#define LGFX_USE_V1
#include <LovyanGFX.hpp>

// Display Configuration for WAVESHARE ESP32-S3-LCD-1.47
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7789 _panel_instance;
  lgfx::Bus_SPI _bus_instance;
  lgfx::Light_PWM _light_instance;
public:
  LGFX(void) {
    auto bcfg = _bus_instance.config();
    bcfg.spi_host = SPI3_HOST;
    bcfg.spi_mode = 0;
    bcfg.freq_write = 40000000;
    bcfg.pin_sclk = 21;
    bcfg.pin_mosi = 18;
    bcfg.pin_miso = -1;
    bcfg.pin_dc = 20;
    _bus_instance.config(bcfg);
    _panel_instance.setBus(&_bus_instance);
    auto pcfg = _panel_instance.config();
    pcfg.pin_cs = 19;
    pcfg.pin_rst = 38;
    pcfg.pin_busy = -1;
    pcfg.panel_width = 172;
    pcfg.panel_height = 320;
    pcfg.offset_x = 34;
    pcfg.offset_y = 0;
    pcfg.invert = false;
    pcfg.bus_shared = true;
    _panel_instance.config(pcfg);
    auto lcfg = _light_instance.config();
    lcfg.pin_bl = 39;
    lcfg.invert = true;
    lcfg.freq = 44100;
    lcfg.pwm_channel = 7;
    _light_instance.config(lcfg);
    _panel_instance.setLight(&_light_instance);
    setPanel(&_panel_instance);
  }
};

LGFX tft;

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_RED);
  delay(1000);
  tft.fillScreen(TFT_GREEN);
  delay(1000);
  tft.fillScreen(TFT_BLUE);
  delay(1000);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Display Test");
}

void loop() {
  // Nothing needed here
}
