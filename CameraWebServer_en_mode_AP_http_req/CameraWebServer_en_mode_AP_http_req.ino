#include "esp_camera.h"
#include <WiFi.h>

#define LED_PIN 4  // GPIO 4 pour AI-Thinker

//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//
//            You must select partition scheme from the board menu that has at least 3MB APP space.
//            Face Recognition is DISABLED for ESP32 and ESP32-S2, because it takes up from 15
//            seconds to process single frame. Face Detection is ENABLED if PSRAM is enabled as well

// ===================
// Select camera model
// ===================
//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE  // Has PSRAM
//#define CAMERA_MODEL_ESP32S3_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
//#define CAMERA_MODEL_M5STACK_UNITCAM // No PSRAM
//#define CAMERA_MODEL_M5STACK_CAMS3_UNIT  // Has PSRAM
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM
//#define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM
// ** Espressif Internal Boards **
//#define CAMERA_MODEL_ESP32_CAM_BOARD
//#define CAMERA_MODEL_ESP32S2_CAM_BOARD
//#define CAMERA_MODEL_ESP32S3_CAM_LCD
//#define CAMERA_MODEL_DFRobot_FireBeetle2_ESP32S3 // Has PSRAM
//#define CAMERA_MODEL_DFRobot_Romeo_ESP32S3 // Has PSRAM
#include "camera_pins.h"
//#include "esp_camera.h"
//#include <WiFi.h>

// ===========================
// Configuration du point d'accès
// ===========================
const char *ssid = "ESP32-CAM_AP";         // Nom du réseau Wi-Fi (SSID)
const char *password = "12345678";         // Mot de passe du réseau (minimum 8 caractères)
const int led1 = 12;
const int led2 = 13;
const int led3 = 14;
const int led4 = 15;
// Déclaration des fonctions
void startCameraServer();
void setupLedFlash(int pin);

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();


  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);


  // ===========================
  // Configuration de la caméra
  // ===========================
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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;       // Résolution UXGA
  config.pixel_format = PIXFORMAT_JPEG;    // Format JPEG (pour le streaming)
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM; // Tampon dans la PSRAM
  config.jpeg_quality = 12;                // Qualité JPEG (1 = meilleure, 63 = pire)
  config.fb_count = 1;                     // Nombre de tampons (framebuffers)

  if (psramFound()) {
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    config.frame_size = FRAMESIZE_SVGA;    // Résolution réduite si pas de PSRAM
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  // Initialisation de la caméra
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // ===========================
  // Configuration en mode AP
  // ===========================
  WiFi.softAP(ssid, password);  // Démarrage du point d'accès

  // Affichage de l'adresse IP
  Serial.println("");
  Serial.print("AP SSID: ");
  Serial.println(ssid);
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP()); // Affiche l'adresse IP par défaut : 192.168.4.1

  // ===========================
  // Démarrage du serveur caméra
  // ===========================
  startCameraServer();

  Serial.println("Camera Ready! Connect to the AP and use the IP above to access the web interface.");
}


void loop() {
  // Boucle vide : tout est géré par le serveur web
  /*
  // Allumer la LED
  Serial.println("Allumer la LED");
  digitalWrite(LED_PIN, HIGH);
  delay(2000);  // Garder la LED allumée pendant 2 secondes

  // Éteindre la LED
  Serial.println("Éteindre la LED");
  digitalWrite(LED_PIN, LOW);
  delay(2000);  // Garder la LED éteinte pendant 2 secondes
  //delay(10000);
  */
}
