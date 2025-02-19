#ifndef QRCODEWINDOW_H
#define QRCODEWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QTimer>
#include "server-connection.hpp" 
#include "ticket.hpp"

class QRCodeWindow : public QWidget {
    Q_OBJECT

public:
    explicit QRCodeWindow(const QString &text, const std::string &ticketToken, const std::string &totpSecret, ServerConnection *conn, QWidget *parent = nullptr);

    void updateQRCode(const QString &text);

private:
    QLabel *qrCodeLabel;
    QLabel *statusLabel;
    QPushButton *checkButton;
    ServerConnection *conn;  // Verbindung zum Server
    Ticket ticket;  // Ticket für die Validitätsprüfung

    void generateQRCode(const QString &text);

private slots:
    void onCheckButtonClicked();  // Slot für den Button
    void checkTicketValidity();  // Funktion zur Validitätsprüfung des Tickets
    void updateStatus(const QString &status);  // Status-Update anzeigen
};

#endif // QRCODEWINDOW_H
