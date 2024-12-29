#include <chrono>
#include <iostream>
#include <thread>
#include "server-connection.hpp"

int main (int argc, char *argv[]) {
    ServerConnection conn = ServerConnection("http://127.0.0.1:5000");
    try {
        conn.login();
        Ticket ticket = conn.generate_ticket();

        std::cout << "The ticket barcode string is " << ticket.get_ticket_string() << std::endl;

        while(true) {
            if (conn.check_ticket_validity(ticket)) {
                std::cout << "The ticket is still valid" << std::endl;
            }
            else {
                std::cout << "The ticket is invalid!" << std::endl;
                return 1;
            }
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }
    catch (const CommunicationError &e) {
        std::cout << "Error while communicating with server: " << e.get_error_msg() << std::endl;
    }
    
    return 0;
}
