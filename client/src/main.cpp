#include <QApplication>
#include <iostream>
#include "server-connection.hpp"
#include "QRCodeWindow.h"

int main (int argc, char *argv[]) {
    ServerConnection conn = ServerConnection("http://127.0.0.1:5000");
    try {
        conn.login();
        std::vector<Ticket> tickets;
        conn.get_tickets(tickets);

        if(tickets.size() == 0) {
            std::cout << "This user doesn't own any tickets" << std::endl;
            return 1;
        }
        else if (tickets.size() > 1) {
            std::cout << "This user owns multiple tickets. Displaying first one only" << std::endl;
        }

        Ticket ticket = *tickets.begin();

        //TODO: ticket.begin()->get_ticket_string() needs to run every 15 seconds, and its output should be the new QR code every time
        std::string current_ticket_string = tickets.begin()->get_ticket_string();
        std::cout << "The current ticket barcode string is " << current_ticket_string << std::endl;
        if (conn.check_ticket_validity(ticket)) {
            std::cout << "The ticket is valid" << std::endl;
        }
        else {
            std::cout << "The ticket is invalid!" << std::endl;
            return 1;
        }

        // Initialize the Qt application
        QApplication app(argc, argv);

        // Create and show the QR code window with the server message as the QR code data
        QRCodeWindow window(QString::fromStdString(current_ticket_string));
        window.show();

        // Run the Qt application event loop
        return app.exec();
    }
    catch (const CommunicationError &e) {
        std::cout << "Error while communicating with server: " << e.get_error_msg() << std::endl;
        return 1;
    }
    
    return 0;
}
