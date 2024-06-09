#include "esp_camera.h"
#include "esp_timer.h"
#include "Arduino.h"
#include "img_converters.h"
#include "wifiNetwork.h" //config file for WiFi parameters
#include "esp_http_client.h"
#include <WiFi.h>
#include <HTTPClient.h>

// Config for POST 
esp_http_client_config_t config = {
    .url = "http://192.168.100.2:5000/",
    .method = HTTP_METHOD_POST,
};
// ---------------------------------------

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
// ---------------------------------------


// Pin defenition for button and LED
#define BUTTON_PIN 12
#define LED_PIN 33
// ---------------------------------------

void setup() {

  // Set up for debugging
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  // ---------------------------------------

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  // ---------------------------------------

  // Set up Camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  // ---------------------------------------


  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  // ---------------------------------------

  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  // ---------------------------------------

  // Take Picture with Camera just to check if it is working
  camera_fb_t * fb = NULL; // pointer
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  // ---------------------------------------


  // Set up Button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  // ---------------------------------------

}
void loop() {
  if (digitalRead(BUTTON_PIN) == LOW && WiFi.status()== WL_CONNECTED) {
    digitalWrite(LED_PIN, LOW);
    esp_http_client_handle_t client = esp_http_client_init(&config);

    camera_fb_t * fb = NULL; 
    fb = esp_camera_fb_get(); 
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    }
    char* input = (char*)fb->buf;

    esp_http_client_set_url(client, "http://192.168.100.2:5000/picture2");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "image");
    esp_http_client_set_post_field(client, input, fb->len);

    esp_err_t err;

    err = esp_http_client_perform(client);


    esp_camera_fb_return(fb);
    esp_http_client_cleanup(client);

    digitalWrite(LED_PIN, HIGH);
    delay(2000);
  }
}