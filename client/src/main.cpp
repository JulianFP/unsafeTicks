#include <chrono>
#include <iostream>
#include <thread>
#include "server-connection.hpp"

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

        while(true) {
            std::cout << "The current ticket barcode string is " << tickets.begin()->get_ticket_string() << std::endl;
            if (conn.check_ticket_validity(ticket)) {
                std::cout << "The ticket is still valid" << std::endl;
            }
            else {
                std::cout << "The ticket is invalid!" << std::endl;
                return 1;
            }
            std::this_thread::sleep_for(std::chrono::seconds(15));
        }
    }
    catch (const CommunicationError &e) {
        std::cout << "Error while communicating with server: " << e.get_error_msg() << std::endl;
    }
    
    return 0;
}
