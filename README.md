# Smart Trash Can - Auto-aiming Robot với AI Detection

Dự án thùng rác thông minh có khả năng tự động di chuyển đến rác sử dụng AI detection và bánh xe Mecanum.

## 📋 Tổng quan

Hệ thống bao gồm:
- **ESP32-CAM**: Stream video qua WiFi và nhận lệnh điều khiển
- **Arduino Uno**: Điều khiển 4 bánh xe Mecanum qua 2 mạch L298N
- **Qt Desktop App**: Giao diện điều khiển, hiển thị camera, AI detection
- **Python YOLOv8**: Nhận diện chai nước và vỏ cam

## 🎯 Tính năng

### Chế độ Auto
- Tự động phát hiện rác (chai nước, vỏ cam)
- Tự động di chuyển đến gần rác
- Dừng lại khi rác ở vị trí thích hợp
- Tự động tìm lại khi mất mục tiêu

### Chế độ Manual
- Điều khiển qua giao diện Qt
- 7 hướng di chuyển: Tiến, Lùi, Trái, Phải, Xoay trái, Xoay phải, Dừng
- Press-and-hold control (giữ nút để di chuyển, thả ra để dừng)

## 📁 Cấu trúc thư mục

```
SmartTrashCan/
├── esp32cam_wifi_bridge/          # Firmware ESP32-CAM
│   └── esp32cam_wifi_bridge.ino
├── arduino_mecanum_control/       # Firmware Arduino Uno
│   └── arduino_mecanum_control.ino
├── ai_detection.py                # Python YOLOv8 detection script
├── SmartTrashCanQt/               # Qt Desktop Application
│   ├── SmartTrashCanQt.pro
│   ├── main.cpp
│   ├── mainwindow.h
│   ├── mainwindow.cpp
│   ├── mainwindow.ui
│   ├── camerastream.h
│   ├── camerastream.cpp
│   ├── aidetection.h
│   ├── aidetection.cpp
│   ├── robotcontroller.h
│   ├── robotcontroller.cpp
│   ├── mecanumcontrol.h
│   ├── mecanumcontrol.cpp
│   ├── config.h
│   └── resources.qrc
└── README.md
```

## 🔧 Yêu cầu phần cứng

1. Arduino Uno R3 x1
2. ESP32-CAM x1
3. L298N Motor Driver x2
4. Mecanum wheels x4 với DC motors (6V-12V)
5. LM2596 DC-DC buck converter x2
6. Battery: 2S LiPo (7.4V) hoặc 3S LiPo (11.1V)
7. USB FTDI programmer (cho ESP32-CAM)
8. PC/Laptop (chạy Qt app và AI detection)
9. WiFi router hoặc hotspot

## 💻 Yêu cầu phần mềm

### Cho Arduino IDE
- Arduino IDE 1.8+ hoặc 2.0+
- ESP32 board support (qua Board Manager)
- Board: "AI Thinker ESP32-CAM"

### Cho Qt Application
- Qt 6.x (Qt Creator IDE)
- C++17 compiler
- CMake hoặc qmake

### Cho Python AI Detection
- Python 3.8+
- Thư viện:
  ```bash
  pip install ultralytics opencv-python numpy requests
  ```

## 🔌 Sơ đồ kết nối

### ESP32-CAM
```
VCC → LM2596 #1 (5V)
GND → Common GND
TX (GPIO1) → Arduino RX (Pin 0)
RX (GPIO3) → Arduino TX (Pin 1)
```

### Arduino Uno → L298N #1 (Trái)
```
Pin 5  → IN1 (Left Front +)
Pin 6  → IN2 (Left Front -)
Pin 9  → IN3 (Left Rear +)
Pin 10 → IN4 (Left Rear -)
```

### Arduino Uno → L298N #2 (Phải)
```
Pin 11 → IN1 (Right Front +)
Pin 12 → IN2 (Right Front -)
Pin 3  → IN3 (Right Rear +)
Pin 2  → IN4 (Right Rear -)
```

### Nguồn
```
Battery → LM2596 #1 → 5V → ESP32-CAM
Battery → LM2596 #2 → 5V → Arduino Uno
Battery → L298N #1 & #2 VCC (trực tiếp)
```

**LƯU Ý QUAN TRỌNG**: 
- Tất cả GND phải nối chung (common ground)
- Cài đặt LM2596 ra chính xác 5V trước khi kết nối
- Rút chân TX/RX khi nạp code cho ESP32-CAM

## 🚀 Hướng dẫn cài đặt

### Bước 1: Nạp firmware ESP32-CAM

1. Mở `esp32cam_wifi_bridge/esp32cam_wifi_bridge.ino` trong Arduino IDE
2. Sửa WiFi credentials:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```
3. Chọn board: **AI Thinker ESP32-CAM**
4. Partition Scheme: **Minimal SPIFFS**
5. Rút chân TX/RX
6. Nhấn và giữ nút **FLASH** trên ESP32-CAM
7. Nhấn **Upload**
8. Sau khi upload xong, nối lại TX/RX

### Bước 2: Nạp firmware Arduino Uno

1. Mở `arduino_mecanum_control/arduino_mecanum_control.ino`
2. Chọn board: **Arduino Uno**
3. Nhấn **Upload**

### Bước 3: Cài đặt Python dependencies

```bash
pip install ultralytics opencv-python numpy requests
```

### Bước 4: Build Qt Application

1. Mở Qt Creator
2. Mở file `SmartTrashCanQt/SmartTrashCanQt.pro`
3. Configure project
4. Build (Ctrl+B)
5. Run (Ctrl+R)

## 🎮 Hướng dẫn sử dụng

### 1. Khởi động hệ thống

1. Cấp nguồn cho robot (ESP32-CAM và Arduino)
2. Mở Serial Monitor để xem IP của ESP32-CAM
3. Ghi lại IP (ví dụ: 192.168.1.100)

### 2. Cấu hình Qt App

1. Mở Qt application
2. Nhập ESP32-CAM IP address
3. Chọn chế độ: Auto hoặc Manual
4. Nhấn **Connect**

### 3. Chế độ Auto

1. Chọn **Auto Mode**
2. Robot sẽ tự động:
   - Quét và tìm rác
   - Di chuyển đến gần rác
   - Dừng lại khi rác ở vị trí thích hợp
3. Theo dõi trạng thái trên giao diện

### 4. Chế độ Manual

1. Chọn **Manual Mode**
2. Sử dụng các nút điều hướng:
   - Giữ nút để di chuyển
   - Thả nút để dừng
3. 7 hướng: ↑ ↓ ← → ↰ ↱ ■ (Stop)

## 🔍 Test từng phần

### Test ESP32-CAM Camera

1. Mở browser
2. Truy cập: `http://[ESP32-IP]/stream`
3. Kiểm tra video stream

### Test ESP32-CAM Commands

1. Mở browser
2. Truy cập: `http://[ESP32-IP]/command?cmd=FORWARD`
3. Kiểm tra robot di chuyển
4. Test các lệnh khác: BACKWARD, LEFT, RIGHT, ROTATE_LEFT, ROTATE_RIGHT, STOP

### Test AI Detection

```bash
python ai_detection.py --test
```

Nhập IP của ESP32-CAM và kiểm tra detection.

### Test Motor Direction

1. Đặt robot lên cao (bánh không chạm đất)
2. Gửi lệnh FORWARD
3. Kiểm tra tất cả bánh xe quay đúng hướng
4. Nếu sai, đổi chỗ dây động cơ trên L298N

## ⚙️ Điều chỉnh thông số

### Trong `config.h` (Qt App)

```cpp
// Vùng giữa để robot tiến lên
const int CENTER_ZONE_MIN = 100;   // Tăng = vùng giữa hẹp hơn
const int CENTER_ZONE_MAX = 220;   // Giảm = vùng giữa hẹp hơn

// Ngưỡng khoảng cách
const int NEAR_THRESHOLD_AREA = 15000;  // Tăng = dừng xa hơn

// Thời gian mất mục tiêu
const int LOST_TIMEOUT_MS = 3000;  // Tăng = tìm lâu hơn
```

### Trong `ai_detection.py`

```python
# Ngưỡng confidence
CONFIDENCE_THRESHOLD = 0.5  # Giảm nếu bỏ sót, tăng nếu false positive
```

## 🐛 Xử lý sự cố

### ESP32-CAM không kết nối WiFi
- Kiểm tra SSID và password
- Đảm bảo ESP32-CAM và PC cùng mạng
- Kiểm tra router

### Camera không stream
- Kiểm tra nguồn (ESP32-CAM cần 5V, 500mA+)
- Kiểm tra cable camera
- Giảm resolution trong code

### Robot di chuyển sai hướng
- Đổi chỗ dây động cơ trên L298N
- Hoặc sửa code Arduino (swap IN1/IN2)

### AI không nhận diện được
- Giảm CONFIDENCE_THRESHOLD
- Kiểm tra ánh sáng
- Thử với chai nước rõ ràng
- Đảm bảo Python packages đã cài đúng

### Qt app không kết nối
- Kiểm tra firewall
- Ping ESP32-CAM IP
- Kiểm tra ESP32-CAM có running không

## 📊 Hiệu suất

- Detection FPS: 5-10 FPS
- Latency: ~300-500ms
- Operating range: 0.5m - 3m
- Battery life: ~30-60 phút

## 🎓 Lưu ý cho đồ án

### Điểm nhấn cho bảo vệ
1. **AI Detection**: Sử dụng YOLOv8 - công nghệ hiện đại
2. **Mecanum Wheels**: Di chuyển đa hướng - ấn tượng
3. **Qt Interface**: Giao diện chuyên nghiệp
4. **Wireless Control**: Không dây hoàn toàn

### Demo checklist
- [ ] ESP32-CAM stream hoạt động
- [ ] AI detection nhận diện đúng chai nước
- [ ] Robot di chuyển đúng 7 hướng
- [ ] Auto mode bám mục tiêu ổn định
- [ ] Manual mode điều khiển mượt
- [ ] Giao diện Qt hiển thị đầy đủ thông tin

## 📝 License

Student Project - Educational Purpose

## 👥 Tác giả

Your Name - Final Year Project

## 🆘 Hỗ trợ

Nếu có vấn đề, kiểm tra:
1. Serial Monitor của ESP32-CAM
2. Serial Monitor của Arduino
3. Console output của Qt app
4. Python script output

---

**Chúc bạn thành công với đồ án! 🎉**
#   T e s t A I C o d e _ b i n 
 
 
