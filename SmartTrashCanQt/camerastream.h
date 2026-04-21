/**
 * camerastream.h - Header cho class xử lý video stream từ ESP32-CAM
 */

#ifndef CAMERASTREAM_H
#define CAMERASTREAM_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include <QTimer>
#include <QByteArray>

/**
 * CameraStream - Class quản lý video stream từ ESP32-CAM
 * 
 * Chức năng:
 * - Kết nối đến MJPEG stream của ESP32-CAM
 * - Decode frames và phát signal để update UI
 * - Xử lý lỗi kết nối và tự động reconnect
 */
class CameraStream : public QObject
{
    Q_OBJECT

public:
    explicit CameraStream(QObject *parent = nullptr);
    ~CameraStream();

    /**
     * Bắt đầu stream từ URL
     * @param url URL của MJPEG stream (ví dụ: http://192.168.1.100/stream)
     */
    void startStream(const QString &url);

    /**
     * Dừng stream
     */
    void stopStream();

    /**
     * Kiểm tra đang stream hay không
     */
    bool isStreaming() const { return m_isStreaming; }

signals:
    /**
     * Phát ra khi có frame mới
     * @param pixmap Frame dưới dạng QPixmap để hiển thị lên QLabel
     */
    void frameReceived(const QPixmap &pixmap);

    /**
     * Phát ra khi có lỗi kết nối
     * @param error Mô tả lỗi
     */
    void connectionError(const QString &error);

    /**
     * Phát ra khi kết nối thành công
     */
    void connected();

private slots:
    /**
     * Xử lý khi nhận được data từ network
     */
    void onReadyRead();

    /**
     * Xử lý khi có lỗi
     */
    void onError(QNetworkReply::NetworkError error);

    /**
     * Xử lý khi kết nối thành công
     */
    void onConnected();

private:
    /**
     * Parse MJPEG data để tách frames
     */
    void parseMJPEGData(const QByteArray &data);

    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_currentReply;
    
    QString m_streamUrl;
    bool m_isStreaming;
    
    QByteArray m_buffer;  // Buffer để chứa data nhận được
};

#endif // CAMERASTREAM_H
