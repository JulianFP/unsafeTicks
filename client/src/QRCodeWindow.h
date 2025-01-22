#ifndef QRCODEWINDOW_H
#define QRCODEWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QString>
#include <qrencode.h>  // QR code generation library

class QRCodeWindow : public QWidget {
    Q_OBJECT  // Required for Qt signals and slots

public:
    explicit QRCodeWindow(const QString &data, QWidget *parent = nullptr);
    
    ~QRCodeWindow();


private:
    QString qrData;
    QLabel *qrLabel;

    // Function to generate and display a QR code
    void generateQRCode();

    // Function to create a QR code image from text data
    QImage createQRCodeImage(const QString &data);
};

#endif // QRCODEWINDOW_H