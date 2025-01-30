#include <QApplication>
#include <iostream>
#include "server-connection.hpp"
#include "QRCodeWindow.h"

int main(int argc, char *argv[]) {
    ServerConnection conn("http://127.0.0.1:5000");

    try {
        conn.login();
        std::vector<Ticket> tickets;
        conn.get_tickets(tickets);

        if (tickets.empty()) {
            std::cout << "This user doesn't own any tickets" << std::endl;
            return 1;
        } else if (tickets.size() > 1) {
            std::cout << "This user owns multiple tickets. Displaying first one only" << std::endl;
        }

        Ticket ticket = *tickets.begin();

        QApplication app(argc, argv);

        // Initialisiere das Fenster mit dem QR-Code des aktuellen Tickets
        std::string ticketToken = ticket.get_ticket_token();  
        std::string totpSecret = ticket.get_totp_secret();

        QRCodeWindow window(QString::fromStdString(ticket.get_ticket_string()), ticketToken, totpSecret, &conn);
        window.show();
        // Timer, um alle 15 Sekunden das Ticket zu aktualisieren
        QTimer timer;
        QObject::connect(&timer, &QTimer::timeout, [&]() {
            std::string new_ticket_string = ticket.get_ticket_string();
            std::cout << "Updated ticket barcode string: " << new_ticket_string << std::endl;
            window.updateQRCode(QString::fromStdString(new_ticket_string));
        });
        timer.start(15000);  // Alle 15 Sekunden

        return app.exec();
    }
    catch (const CommunicationError &e) {
        std::cout << "Error while communicating with server: " << e.get_error_msg() << std::endl;
        return 1;
    }
}
