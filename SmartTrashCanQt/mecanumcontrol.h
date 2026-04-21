/**
 * mecanumcontrol.h - Header cho class xử lý logic di chuyển Mecanum
 */

#ifndef MECANUMCONTROL_H
#define MECANUMCONTROL_H

#include <QObject>
#include "config.h"

/**
 * MecanumControl - Class tính toán lệnh di chuyển cho robot Mecanum
 * 
 * Chức năng:
 * - Tính toán lệnh dựa trên vị trí vật thể (Auto mode)
 * - Chuyển đổi button presses thành commands (Manual mode)
 * - Xử lý logic tìm kiếm khi mất mục tiêu
 */
class MecanumControl : public QObject
{
    Q_OBJECT

public:
    explicit MecanumControl(QObject *parent = nullptr);

    /**
     * Tính toán lệnh trong Auto mode dựa trên detection
     */
    QString calculateAutoCommand(int objectX, int objectArea, bool detected);

    /**
     * Lấy lệnh cho Manual mode
     */
    QString getManualCommand(const QString &direction);

    /**
     * Reset trạng thái
     */
    void reset();

private:
    RobotState m_currentState;
    unsigned long m_lastDetectionTime;
};

#endif // MECANUMCONTROL_H
