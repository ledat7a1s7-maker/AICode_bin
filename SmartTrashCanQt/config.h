/**
 * config.h - Cấu hình cho Smart Trash Can Qt Application
 * 
 * Chứa các hằng số và cấu hình mặc định cho toàn bộ ứng dụng
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

// ==================== CẤU HÌNH ESP32-CAM ====================
namespace ESP32Config {
    const QString DEFAULT_IP = "192.168.1.100";  // IP mặc định của ESP32-CAM
    const int HTTP_PORT = 80;                      // Port HTTP server
    const QString STREAM_ENDPOINT = "/stream";    // URL video stream
    const QString COMMAND_ENDPOINT = "/command";  // URL gửi lệnh
}

// ==================== CẤU HÌNH AI DETECTION ====================
namespace AIConfig {
    const double CONFIDENCE_THRESHOLD = 0.5;      // Ngưỡng confidence tối thiểu
    const int CENTER_ZONE_MIN = 100;               // Vùng giữa - ranh trái
    const int CENTER_ZONE_MAX = 220;               // Vùng giữa - ranh phải
    const int NEAR_THRESHOLD_AREA = 15000;         // Ngưỡng diện tích (pixel) - gần/xa
    const int LOST_TIMEOUT_MS = 3000;              // Thời gian mất mục tiêu (ms)
}

// ==================== CẤU HÌNH GIAO DIỆN ====================
namespace UIConfig {
    const int CAMERA_WIDTH = 640;                  // Chiều rộng hiển thị camera
    const int CAMERA_HEIGHT = 480;                 // Chiều cao hiển thị camera
    const int UPDATE_INTERVAL_MS = 100;            // Tần suất cập nhật UI (ms)
    const int DETECTION_INTERVAL_MS = 200;         // Tần suất detection (ms)
}

// ==================== CÁC LỆNH ĐIỀU KHIỂN ====================
namespace Commands {
    const QString FORWARD = "FORWARD";
    const QString BACKWARD = "BACKWARD";
    const QString LEFT = "LEFT";
    const QString RIGHT = "RIGHT";
    const QString ROTATE_LEFT = "ROTATE_LEFT";
    const QString ROTATE_RIGHT = "ROTATE_RIGHT";
    const QString STOP = "STOP";
}

// ==================== CÁC CHẾ ĐỘ HOẠT ĐỘNG ====================
enum class RobotMode {
    AUTO,      // Tự động bám mục tiêu
    MANUAL     // Điều khiển bằng tay
};

// ==================== TRẠNG THÁI ROBOT ====================
enum class RobotState {
    IDLE,              // Chờ
    MOVING_FORWARD,    // Đang tiến
    MOVING_BACKWARD,   // Đang lùi
    MOVING_LEFT,       // Đang sang trái
    MOVING_RIGHT,      // Đang sang phải
    ROTATING_LEFT,     // Đang xoay trái
    ROTATING_RIGHT,    // Đang xoay phải
    STOPPED,           // Đã dừng
    SEARCHING          // Đang tìm mục tiêu
};

// ==================== LOẠI VẬT THỂ ====================
enum class ObjectType {
    NONE,       // Không có vật thể
    BOTTLE,     // Chai nước
    ORANGE,     // Vỏ cam
    APPLE       // Táo (nếu cần)
};

/**
 * Chuyển đổi ObjectType sang string
 */
inline QString objectTypeToString(ObjectType type) {
    switch (type) {
        case ObjectType::BOTTLE: return "Water Bottle";
        case ObjectType::ORANGE: return "Orange";
        case ObjectType::APPLE: return "Apple";
        default: return "No Object";
    }
}

/**
 * Chuyển đổi RobotState sang string
 */
inline QString robotStateToString(RobotState state) {
    switch (state) {
        case RobotState::IDLE: return "Idle";
        case RobotState::MOVING_FORWARD: return "Moving Forward";
        case RobotState::MOVING_BACKWARD: return "Moving Backward";
        case RobotState::MOVING_LEFT: return "Moving Left";
        case RobotState::MOVING_RIGHT: return "Moving Right";
        case RobotState::ROTATING_LEFT: return "Rotating Left";
        case RobotState::ROTATING_RIGHT: return "Rotating Right";
        case RobotState::STOPPED: return "Stopped";
        case RobotState::SEARCHING: return "Searching";
        default: return "Unknown";
    }
}

#endif // CONFIG_H
