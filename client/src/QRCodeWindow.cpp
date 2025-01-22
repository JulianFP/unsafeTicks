#include "QRCodeWindow.h"
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QFont>
#include <qrencode.h>  // QR code generation library
#include <iostream>

QRCodeWindow::QRCodeWindow(const QString &data, QWidget *parent)
    : QWidget(parent), qrData(data) {
    setWindowTitle("QR Code Generator");
    setFixedSize(400, 300);

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Create the button
    QPushButton *scanButton = new QPushButton("Generate QR Code", this);
    layout->addWidget(scanButton);

    // Create the label to display the QR code
    qrLabel = new QLabel(this);
    qrLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(qrLabel);

    // Connect the button to the QR code generation
    connect(scanButton, &QPushButton::clicked, this, [this]() {
        generateQRCode();
    });

    setLayout(layout);
}

QRCodeWindow::~QRCodeWindow() {
    // Clean up resources if necessary
}

// Function to generate and display a QR code
void QRCodeWindow::generateQRCode() {
    QImage qrImage = createQRCodeImage(qrData);
    if (!qrImage.isNull()) {
        qrLabel->setPixmap(QPixmap::fromImage(qrImage));
    } else {
        qrLabel->setText("Failed to generate QR code");
    }
}

// Function to create a QR code image from text data
QImage QRCodeWindow::createQRCodeImage(const QString &data) {
    const int MAX_QR_LENGTH = 100;
    QString qrDataLimited = data.left(MAX_QR_LENGTH);
    QRcode *qr = QRcode_encodeString(data.toUtf8().constData(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);
    if (!qr) {
        std::cerr << "Failed to generate QR code" << std::endl;
        return QImage();
    }

    int qrSize = qr->width;
    int scaleFactor = 10;  // Scale each QR pixel by 10x10 for better visibility
    int imageSize = qrSize * scaleFactor;
    QImage image(imageSize, imageSize, QImage::Format_RGB32);
    image.fill(Qt::white);

    QPainter painter(&image);
    painter.setBrush(Qt::black);

    for (int y = 0; y < qrSize; ++y) {
        for (int x = 0; x < qrSize; ++x) {
            if (qr->data[y * qrSize + x] & 0x01) {
                painter.drawRect(x * scaleFactor, y * scaleFactor, scaleFactor, scaleFactor);
            }
        }
    }

    QRcode_free(qr);
    return image;
}