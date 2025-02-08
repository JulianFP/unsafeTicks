#include "QRCodeWindow.h"
#include <QVBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QFont>
#include <qrencode.h>
#include <iostream>

QRCodeWindow::QRCodeWindow(const QString &text, const std::string &ticketToken, const std::string &totpSecret, ServerConnection *conn, QWidget *parent)
    : QWidget(parent), 
    qrCodeLabel(new QLabel(this)), 
    statusLabel(new QLabel(this)),
    checkButton(new QPushButton("Check Ticket Validity", this)), 
    conn(conn),
    ticket(ticketToken, totpSecret) {

    // Layout und Ausrichtung
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(qrCodeLabel, 0, Qt::AlignCenter);  // QR-Code in der Mitte
    layout->addWidget(statusLabel, 0, Qt::AlignCenter);  // Statusnachricht unter dem QR-Code
    layout->addWidget(checkButton, 0, Qt::AlignCenter);  // Button in der Mitte

    generateQRCode(text);

    connect(checkButton, &QPushButton::clicked, this, &QRCodeWindow::onCheckButtonClicked);

    setLayout(layout);
    setWindowTitle("QR Code Viewer");

    resize(700, 700);
}

void QRCodeWindow::generateQRCode(const QString &text) {
    QRcode *qr = QRcode_encodeString(text.toUtf8().constData(), 10, QR_ECLEVEL_Q, QR_MODE_8, 1);

    if (qr) {
        // Berechne die Größe des QR-Codes und passe sie an
        int size = qr->width > 0 ? qr->width : 1;
        
        // Erhöhe die Auflösung des QR-Codes, um mehr Details zu erfassen
        int scaleFactor = 8;  // Skalierungsfaktor, je größer, desto detaillierter das Bild
        QImage image(size * scaleFactor, size * scaleFactor, QImage::Format_RGB888);
        image.fill(Qt::white);  // Hintergrund auf Weiß setzen

        QPainter painter(&image);
        painter.setPen(QPen(Qt::black));  // Setze den Stift auf Schwarz
        painter.setBrush(Qt::black);      // Setze den Pinsel auf Schwarz

        // QR-Code Daten zeichnen
        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                if (qr->data[y * size + x] & 0x01) {
                    // Zeichne die Zellen des QR-Codes in der neuen Auflösung
                    painter.drawRect(x * scaleFactor, y * scaleFactor, scaleFactor, scaleFactor);
                }
            }
        }

        // QR-Code in das Label setzen und das Bild skalieren
        qrCodeLabel->setPixmap(QPixmap::fromImage(image).scaled(350, 350, Qt::KeepAspectRatio));

        QRcode_free(qr);  // Speicher freigeben
    } else {
        qrCodeLabel->setText("Failed to generate QR code");
    }
}

void QRCodeWindow::onCheckButtonClicked() {
    checkTicketValidity();  // Direkt im Fenster die Ticketprüfung durchführen
}

// Check ticket validity
void QRCodeWindow::checkTicketValidity() {
    try {
        bool isValid = conn->check_ticket_validity(ticket);

        if (isValid) {
            updateStatus("Ticket is valid.");
        } else {
            updateStatus("Ticket is invalid.");
        }
    } catch (const TicketError &e) {
        updateStatus(QString("Error: %1").arg(e.what()));
    }
}

void QRCodeWindow::updateStatus(const QString &status) {
    statusLabel->setText(status);  // Zeigt den Status im Fenster an
}

void QRCodeWindow::updateQRCode(const QString &newQRCodeData) {
    generateQRCode(newQRCodeData); // Re-generate QR code with new data
}
