/**
 * camerastream.cpp - Implementation cho MJPEG stream handler
 */

#include "camerastream.h"
#include <QDebug>

CameraStream::CameraStream(QObject *parent)
    : QObject(parent), m_networkManager(nullptr), m_currentReply(nullptr), m_isStreaming(false)
{
    m_networkManager = new QNetworkAccessManager(this);
}

CameraStream::~CameraStream()
{
    stopStream();
}

void CameraStream::startStream(const QString &url)
{
    if (m_isStreaming) {
        stopStream();
    }

    m_streamUrl = url;
    m_isStreaming = true;
    m_buffer.clear();

    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
    
    m_currentReply = m_networkManager->get(request);
    
    connect(m_currentReply, &QNetworkReply::readyRead, this, &CameraStream::onReadyRead);
    connect(m_currentReply, &QNetworkReply::errorOccurred, this, &CameraStream::onError);
    connect(m_currentReply, &QNetworkReply::finished, this, &CameraStream::onConnected);
    
    qDebug() << "Connecting to stream:" << url;
}

void CameraStream::stopStream()
{
    m_isStreaming = false;
    
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }
    
    m_buffer.clear();
    qDebug() << "Stream stopped";
}

void CameraStream::onReadyRead()
{
    if (!m_currentReply) return;
    
    QByteArray data = m_currentReply->readAll();
    m_buffer.append(data);
    
    // Parse MJPEG - tìm frame boundaries
    parseMJPEGData(m_buffer);
}

void CameraStream::parseMJPEGData(const QByteArray &data)
{
    // Tìm JPEG start marker: FF D8
    int startIndex = -1;
    for (int i = 0; i < data.size() - 1; i++) {
        if ((quint8)data[i] == 0xFF && (quint8)data[i+1] == 0xD8) {
            startIndex = i;
            break;
        }
    }
    
    if (startIndex == -1) return;
    
    // Tìm JPEG end marker: FF D9
    int endIndex = -1;
    for (int i = startIndex + 2; i < data.size() - 1; i++) {
        if ((quint8)data[i] == 0xFF && (quint8)data[i+1] == 0xD9) {
            endIndex = i + 1;
            break;
        }
    }
    
    if (endIndex == -1) return;
    
    // Trích xuất JPEG frame
    QByteArray jpgData = data.mid(startIndex, endIndex - startIndex + 1);
    
    // Load thành QPixmap
    QPixmap pixmap;
    if (pixmap.loadFromData(jpgData, "JPEG")) {
        emit frameReceived(pixmap);
    }
    
    // Xóa buffer đã xử lý
    m_buffer = m_buffer.mid(endIndex + 1);
}

void CameraStream::onError(QNetworkReply::NetworkError error)
{
    QString errorMsg = m_currentReply->errorString();
    qWarning() << "Stream error:" << errorMsg;
    emit connectionError(errorMsg);
}

void CameraStream::onConnected()
{
    if (m_currentReply->error() == QNetworkReply::NoError) {
        qDebug() << "Stream connected successfully";
        emit connected();
    }
}
