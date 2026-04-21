/**
 * ESP32-CAM WiFi Bridge Firmware
 * 
 * Chức năng:
 * 1. Stream camera qua WiFi (MJPEG) cho Qt app
 * 2. Nhận lệnh điều khiển từ Qt app qua HTTP
 * 3. Chuyển lệnh đến Arduino Uno qua Serial UART
 * 
 * Kết nối:
 * - WiFi: Stream video và nhận lệnh từ Qt app
 * - Serial (TX/RX): Gửi lệnh đến Arduino Uno
 * 
 * Lưu ý:
 * - Rút chân TX/RX khi nạp code
 * - Cài đặt board: "AI Thinker ESP32-CAM"
 * - Partition Scheme: "Minimal SPIFFS"
 */

#include "esp_camera.h"
#include "WiFi.h"
#include "WebServer.h"

// ==================== CẤU HÌNH WIFI ====================
const char* ssid = "YOUR_WIFI_SSID";        // Thay bằng WiFi của bạn
const char* password = "YOUR_WIFI_PASSWORD"; // Thay bằng mật khẩu WiFi

// ==================== CẤU HÌNH CAMERA ====================
#define CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM       5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// ==================== CẤU HÌNH SERIAL ====================
#define SERIAL_BAUD 115200
#define SERIAL_TX 1  // GPIO1 (TX)
#define SERIAL_RX 3  // GPIO3 (RX)

// ==================== BIẾN TOÀN CỤC ====================
WebServer server(80);
bool cameraInitialized = false;

// ==================== KHỞI TẠO CAMERA ====================
bool initCamera() {
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
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Chất lượng ảnh - thấp hơn = nhanh hơn
  config.frame_size = FRAMESIZE_QVGA;  // 320x240 (nhanh)
  config.jpeg_quality = 15;            // 10-63 (thấp hơn = đẹp hơn)
  config.fb_count = 1;

  // Khởi tạo camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return false;
  }
  
  Serial.println("Camera initialized successfully");
  return true;
}

// ==================== HANDLER: VIDEO STREAM ====================
void handleStream() {
  camera_fb_t *fb = NULL;
  
  // Lấy frame từ camera
  fb = esp_camera_fb_get();
  if (!fb) {
    server.send(500, "text/plain", "Camera capture failed");
    return;
  }
  
  // Gửi MJPEG frame
  server.setContentLength(fb->len);
  server.send(200, "image/jpeg");
  server.sendContent((const char*)fb->buf, fb->len);
  
  // Release framebuffer
  esp_camera_fb_return(fb);
}

// ==================== HANDLER: NHẬN LỆNH ĐIỀU KHIỂN ====================
void handleCommand() {
  // Kiểm tra có tham số 'cmd' không
  if (!server.hasArg("cmd")) {
    server.send(400, "text/plain", "ERROR: Missing 'cmd' parameter");
    return;
  }
  
  // Lấy lệnh từ URL
  String command = server.arg("cmd");
  
  // Gửi lệnh đến Arduino qua Serial
  Serial.println(command);
  
  // Phản hồi cho Qt app
  server.send(200, "text/plain", "OK: " + command);
  
  // Log để debug
  Serial.print("Command received: ");
  Serial.println(command);
}

// ==================== HANDLER: TRANG CHỦ ====================
void handleRoot() {
  String html = "<html><head><title>ESP32-CAM Smart Trash</title></head><body>";
  html += "<h1>Smart Trash Can - ESP32-CAM Bridge</h1>";
  html += "<h2>Endpoints:</h2>";
  html += "<ul>";
  html += "<li><a href='/stream'>/stream</a> - Video stream</li>";
  html += "<li>/command?cmd=FORWARD - Send motor command</li>";
  html += "</ul>";
  html += "<h2>Available Commands:</h2>";
  html += "<ul>";
  html += "<li>FORWARD, BACKWARD, LEFT, RIGHT</li>";
  html += "<li>ROTATE_LEFT, ROTATE_RIGHT, STOP</li>";
  html += "</ul>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// ==================== HANDLER: TRẠNG THÁI ====================
void handleStatus() {
  String status = "{";
  status += "\"camera\":" + String(cameraInitialized ? "true" : "false") + ",";
  status += "\"wifi\":\"connected\",";
  status += "\"ip\":\"" + WiFi.localIP().toString() + "\"";
  status += "}";
  
  server.send(200, "application/json", status);
}

// ==================== SETUP ====================
void setup() {
  // Khởi tạo Serial với Arduino
  Serial.begin(SERIAL_BAUD);
  Serial.println("\n=== ESP32-CAM WiFi Bridge ===");
  Serial.println("Starting system...");
  
  // Khởi tạo camera
  cameraInitialized = initCamera();
  
  // Kết nối WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: http://");
    Serial.println(WiFi.localIP());
    Serial.println("/stream - Video stream");
    Serial.println("/command?cmd=XXX - Motor control");
  } else {
    Serial.println("\nWiFi connection failed!");
  }
  
  // Cấu hình routes cho web server
  server.on("/", handleRoot);
  server.on("/stream", handleStream);
  server.on("/command", handleCommand);
  server.on("/status", handleStatus);
  
  // Khởi động web server
  server.begin();
  Serial.println("HTTP server started on port 80");
  Serial.println("=== System Ready ===\n");
}

// ==================== LOOP ====================
void loop() {
  // Xử lý các request HTTP từ Qt app
  server.handleClient();
  
  // Không cần delay vì server.handleClient() cần chạy liên tục
}
