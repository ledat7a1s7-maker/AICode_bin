"""
AI Detection Script - YOLOv8 Object Detection
Chức năng:
1. Nhận diện chai nước (bottle) và vỏ cam (orange) bằng YOLOv8
2. Trả về vị trí, kích thước và loại vật thể dưới dạng JSON
3. Chạy độc lập, nhận ảnh từ stdin hoặc file temp

Sử dụng:
- Qt app gửi ảnh đến script này
- Script trả về kết quả detection qua stdout (JSON format)
- Có thể chạy test độc lập với camera stream

Cài đặt thư viện:
pip install ultralytics opencv-python numpy
"""

import sys
import json
import cv2
import numpy as np
from ultralytics import YOLO

# ==================== CẤU HÌNH ====================

# Tải model YOLOv8 nano (nhẹ nhất, nhanh nhất)
# Model tự động download lần đầu chạy (~6MB)
MODEL_PATH = 'yolov8n.pt'

# Các lớp vật thể cần nhận diện (COCO dataset)
# 39: bottle, 47: apple, 48: orange
TARGET_CLASSES = {
    39: 'bottle',
    47: 'apple',
    48: 'orange'
}

# Ngưỡng confidence tối thiểu
CONFIDENCE_THRESHOLD = 0.5

# Kích thước frame nhận được từ camera
FRAME_WIDTH = 320   # QVGA width
FRAME_HEIGHT = 240  # QVGA height

# ==================== KHỞI TẠO MODEL ====================

print("Loading YOLOv8 model...", file=sys.stderr)
model = YOLO(MODEL_PATH)
print("Model loaded successfully!", file=sys.stderr)

# ==================== HÀM DETECTION ====================

def detect_objects(frame):
    """
    Nhận diện vật thể trong frame
    
    Args:
        frame: Image numpy array (BGR format từ OpenCV)
    
    Returns:
        Dictionary chứa thông tin vật thể phát hiện được
    """
    # Chạy YOLOv8 inference
    results = model(frame, verbose=False)
    
    # Lấy kết quả đầu tiên
    result = results[0]
    
    # Tìm vật thể có confidence cao nhất trong các lớp target
    best_detection = None
    best_confidence = 0
    
    # Duyệt qua tất cả detections
    for box in result.boxes:
        # Lấy confidence score
        confidence = float(box.conf[0])
        
        # Bỏ qua nếu confidence quá thấp
        if confidence < CONFIDENCE_THRESHOLD:
            continue
        
        # Lấy class ID
        class_id = int(box.cls[0])
        
        # Chỉ xử lý các lớp target (bottle, apple, orange)
        if class_id not in TARGET_CLASSES:
            continue
        
        # Lấy bounding box coordinates
        x1, y1, x2, y2 = box.xyxy[0].tolist()
        
        # Tính toán vị trí trung tâm
        center_x = int((x1 + x2) / 2)
        center_y = int((y1 + y2) / 2)
        
        # Tính toán kích thước
        width = int(x2 - x1)
        height = int(y2 - y1)
        area = width * height
        
        # Lấy tên vật thể
        object_name = TARGET_CLASSES[class_id]
        
        # Cập nhật nếu confidence cao hơn
        if confidence > best_confidence:
            best_confidence = confidence
            best_detection = {
                "detected": True,
                "object": object_name,
                "x": center_x,
                "y": center_y,
                "width": width,
                "height": height,
                "area": area,
                "confidence": round(confidence, 3)
            }
    
    # Nếu không phát hiện được vật thể nào
    if best_detection is None:
        return {
            "detected": False
        }
    
    return best_detection

# ==================== TEST MODE ====================

def test_with_camera():
    """
    Test detection với camera stream từ ESP32-CAM
    Chạy độc lập để test AI detection
    """
    import requests
    import io
    
    # Cấu hình URL camera
    esp32_ip = input("Enter ESP32-CAM IP (default: 192.168.1.100): ").strip()
    if not esp32_ip:
        esp32_ip = "192.168.1.100"
    
    stream_url = f"http://{esp32_ip}/stream"
    
    print(f"Connecting to {stream_url}...", file=sys.stderr)
    
    try:
        # Kết nối đến camera stream
        response = requests.get(stream_url, stream=True)
        response.raise_for_status()
        
        print("Connected! Starting detection...", file=sys.stderr)
        print("Press Ctrl+C to stop\n", file=sys.stderr)
        
        # Đọc MJPEG stream
        bytes_data = b""
        frame_count = 0
        
        for chunk in response.iter_content(chunk_size=1024):
            bytes_data += chunk
            
            # Tìm frame boundary
            a = bytes_data.find(b'\xff\xd8')  # JPEG start
            b = bytes_data.find(b'\xff\xd9')  # JPEG end
            
            if a != -1 and b != -1:
                # Trích xuất JPEG frame
                jpg = bytes_data[a:b+2]
                bytes_data = bytes_data[b+2:]
                
                # Decode image
                frame = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
                
                if frame is None:
                    continue
                
                # Resize về QVGA nếu cần
                if frame.shape[1] != FRAME_WIDTH or frame.shape[0] != FRAME_HEIGHT:
                    frame = cv2.resize(frame, (FRAME_WIDTH, FRAME_HEIGHT))
                
                # Detection
                result = detect_objects(frame)
                
                # In kết quả
                frame_count += 1
                if result["detected"]:
                    print(f"Frame {frame_count}: {result['object']} at ({result['x']}, {result['y']}), "
                          f"confidence: {result['confidence']}, area: {result['area']}", file=sys.stderr)
                else:
                    print(f"Frame {frame_count}: No object detected", file=sys.stderr)
                
                # Output JSON cho Qt app
                print(json.dumps(result), flush=True)
                
    except KeyboardInterrupt:
        print("\nStopped by user", file=sys.stderr)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

# ==================== MAIN ====================

if __name__ == "__main__":
    # Kiểm tra chế độ chạy
    if len(sys.argv) > 1 and sys.argv[1] == "--test":
        # Chế độ test: đọc từ camera stream
        test_with_camera()
    else:
        # Chế độ bình thường: đọc ảnh từ stdin
        # Qt app sẽ gửi ảnh qua stdin hoặc file temp
        
        print("AI Detection Service Ready", file=sys.stderr)
        print("Waiting for frames from Qt app...", file=sys.stderr)
        
        # Đọc frames từ stdin (binary mode)
        while True:
            try:
                # Đọc độ dài ảnh (4 bytes)
                length_bytes = sys.stdin.buffer.read(4)
                if len(length_bytes) < 4:
                    break
                
                length = int.from_bytes(length_bytes, 'big')
                
                # Đọc image data
                image_data = sys.stdin.buffer.read(length)
                if len(image_data) < length:
                    break
                
                # Decode image
                frame = cv2.imdecode(np.frombuffer(image_data, dtype=np.uint8), cv2.IMREAD_COLOR)
                
                if frame is None:
                    continue
                
                # Detection
                result = detect_objects(frame)
                
                # Output JSON result
                print(json.dumps(result), flush=True)
                
            except Exception as e:
                print(f"Error processing frame: {e}", file=sys.stderr)
                break
