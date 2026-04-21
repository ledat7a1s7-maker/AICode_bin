/**
 * Arduino Uno - Mecanum Wheel Control Firmware
 * 
 * Chức năng:
 * 1. Nhận lệnh từ ESP32-CAM qua Serial UART
 * 2. Điều khiển 4 động cơ bánh Mecanum qua 2 mạch L298N
 * 3. Hỗ trợ 7 chuyển động: Tiến, Lùi, Trái, Phải, Xoay trái, Xoay phải, Dừng
 * 
 * Sơ đồ kết nối:
 * - L298N #1 (Trái): IN1, IN2, IN3, IN4 → Arduino pins 5, 6, 9, 10
 * - L298N #2 (Phải): IN1, IN2, IN3, IN4 → Arduino pins 11, 12, 3, 2
 * 
 * Lệnh nhận được từ Serial:
 * - FORWARD, BACKWARD, LEFT, RIGHT
 * - ROTATE_LEFT, ROTATE_RIGHT, STOP
 * 
 * Bánh xe Mecanum:
 * - Có con lăn 45 độ cho phép di chuyển đa hướng
 * - Strafe (di chuyển ngang) khác với Rotate (xoay tại chỗ)
 */

// ==================== ĐỊNH NGHĨA CHÂN L298N #1 (BÊN TRÁI) ====================
const int LF_IN1 = 5;   // Left Front motor +
const int LF_IN2 = 6;   // Left Front motor -
const int LR_IN3 = 9;   // Left Rear motor +
const int LR_IN4 = 10;  // Left Rear motor -

// ==================== ĐỊNH NGHĨA CHÂN L298N #2 (BÊN PHẢI) ====================
const int RF_IN1 = 11;  // Right Front motor +
const int RF_IN2 = 12;  // Right Front motor -
const int RR_IN3 = 3;   // Right Rear motor +
const int RR_IN4 = 2;   // Right Rear motor -

// ==================== BIẾN TOÀN CỤC ====================
String command = "";  // Lưu lệnh nhận được từ Serial

// ==================== SETUP ====================
void setup() {
  // Khởi tạo Serial với ESP32-CAM
  Serial.begin(115200);
  
  // Cấu hình tất cả chân là OUTPUT
  pinMode(LF_IN1, OUTPUT);
  pinMode(LF_IN2, OUTPUT);
  pinMode(LR_IN3, OUTPUT);
  pinMode(LR_IN4, OUTPUT);
  pinMode(RF_IN1, OUTPUT);
  pinMode(RF_IN2, OUTPUT);
  pinMode(RR_IN3, OUTPUT);
  pinMode(RR_IN4, OUTPUT);
  
  // Dừng tất cả động cơ khi khởi động
  stopAllMotors();
  
  Serial.println("=== Arduino Mecanum Control Ready ===");
  Serial.println("Waiting for commands from ESP32-CAM...");
}

// ==================== CÁC HÀM ĐIỀU KHIỂN ĐỘNG CƠ ====================

/**
 * Dừng tất cả 4 động cơ
 * Cả 2 chân của mỗi động cơ đều LOW
 */
void stopAllMotors() {
  digitalWrite(LF_IN1, LOW); digitalWrite(LF_IN2, LOW);
  digitalWrite(LR_IN3, LOW); digitalWrite(LR_IN4, LOW);
  digitalWrite(RF_IN1, LOW); digitalWrite(RF_IN2, LOW);
  digitalWrite(RR_IN3, LOW); digitalWrite(RR_IN4, LOW);
}

/**
 * Tiến lên - Tất cả bánh xe quay tới
 */
void moveForward() {
  digitalWrite(LF_IN1, HIGH); digitalWrite(LF_IN2, LOW);
  digitalWrite(LR_IN3, HIGH); digitalWrite(LR_IN4, LOW);
  digitalWrite(RF_IN1, HIGH); digitalWrite(RF_IN2, LOW);
  digitalWrite(RR_IN3, HIGH); digitalWrite(RR_IN4, LOW);
}

/**
 * Lùi lại - Tất cả bánh xe quay lùi
 */
void moveBackward() {
  digitalWrite(LF_IN1, LOW); digitalWrite(LF_IN2, HIGH);
  digitalWrite(LR_IN3, LOW); digitalWrite(LR_IN4, HIGH);
  digitalWrite(RF_IN1, LOW); digitalWrite(RF_IN2, HIGH);
  digitalWrite(RR_IN3, LOW); digitalWrite(RR_IN4, HIGH);
}

/**
 * Di chuyển sang trái (Mecanum strafe left)
 * LF: lùi, LR: tiến, RF: tiến, RR: lùi
 */
void moveLeft() {
  digitalWrite(LF_IN1, LOW);  digitalWrite(LF_IN2, HIGH);  // Left Front: REV
  digitalWrite(LR_IN3, HIGH); digitalWrite(LR_IN4, LOW);   // Left Rear: FWD
  digitalWrite(RF_IN1, HIGH); digitalWrite(RF_IN2, LOW);   // Right Front: FWD
  digitalWrite(RR_IN3, LOW);  digitalWrite(RR_IN4, HIGH);  // Right Rear: REV
}

/**
 * Di chuyển sang phải (Mecanum strafe right)
 * LF: tiến, LR: lùi, RF: lùi, RR: tiến
 */
void moveRight() {
  digitalWrite(LF_IN1, HIGH); digitalWrite(LF_IN2, LOW);   // Left Front: FWD
  digitalWrite(LR_IN3, LOW);  digitalWrite(LR_IN4, HIGH);  // Left Rear: REV
  digitalWrite(RF_IN1, LOW);  digitalWrite(RF_IN2, HIGH);  // Right Front: REV
  digitalWrite(RR_IN3, HIGH); digitalWrite(RR_IN4, LOW);   // Right Rear: FWD
}

/**
 * Xoay trái tại chỗ (Rotate counter-clockwise)
 * Bánh trái lùi, bánh phải tiến
 */
void rotateLeft() {
  digitalWrite(LF_IN1, LOW);  digitalWrite(LF_IN2, HIGH);  // Left Front: REV
  digitalWrite(LR_IN3, LOW);  digitalWrite(LR_IN4, HIGH);  // Left Rear: REV
  digitalWrite(RF_IN1, HIGH); digitalWrite(RF_IN2, LOW);   // Right Front: FWD
  digitalWrite(RR_IN3, HIGH); digitalWrite(RR_IN4, LOW);   // Right Rear: FWD
}

/**
 * Xoay phải tại chỗ (Rotate clockwise)
 * Bánh trái tiến, bánh phải lùi
 */
void rotateRight() {
  digitalWrite(LF_IN1, HIGH); digitalWrite(LF_IN2, LOW);   // Left Front: FWD
  digitalWrite(LR_IN3, HIGH); digitalWrite(LR_IN4, LOW);   // Left Rear: FWD
  digitalWrite(RF_IN1, LOW);  digitalWrite(RF_IN2, HIGH);  // Right Front: REV
  digitalWrite(RR_IN3, LOW);  digitalWrite(RR_IN4, HIGH);  // Right Rear: REV
}

// ==================== XỬ LÝ LỆNH ====================

/**
 * Xử lý lệnh nhận được từ Serial
 * Chuyển đổi string command thành hàm điều khiển tương ứng
 */
void executeCommand(String cmd) {
  cmd.trim();  // Xóa khoảng trắng thừa
  
  Serial.print("Executing: ");
  Serial.println(cmd);
  
  if (cmd == "FORWARD") {
    moveForward();
  }
  else if (cmd == "BACKWARD") {
    moveBackward();
  }
  else if (cmd == "LEFT") {
    moveLeft();
  }
  else if (cmd == "RIGHT") {
    moveRight();
  }
  else if (cmd == "ROTATE_LEFT") {
    rotateLeft();
  }
  else if (cmd == "ROTATE_RIGHT") {
    rotateRight();
  }
  else if (cmd == "STOP") {
    stopAllMotors();
  }
  else {
    Serial.print("Unknown command: ");
    Serial.println(cmd);
    stopAllMotors();  // An toàn: dừng nếu lệnh không hợp lệ
  }
}

// ==================== LOOP ====================
void loop() {
  // Kiểm tra có dữ liệu từ Serial (ESP32-CAM gửi xuống)
  if (Serial.available() > 0) {
    // Đọc lệnh cho đến khi gặp newline
    command = Serial.readStringUntil('\n');
    
    // Xử lý lệnh
    if (command.length() > 0) {
      executeCommand(command);
    }
  }
}
