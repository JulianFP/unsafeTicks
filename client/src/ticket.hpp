#pragma once

#include <string>
#include "exceptions-base.hpp"

class TicketError : public ExceptionWithErrorMsg {
    using ExceptionWithErrorMsg::ExceptionWithErrorMsg; //inherit constructor
};

class Ticket {
private:
    std::string ticket_token;
    std::string totp_secret;
public:
    Ticket(const std::string &ticket_token, const std::string &totp_secret);

    std::string get_ticket_string() const;
    std::string get_ticket_token() const;  // Getter for ticket_token
    std::string get_totp_secret() const;  // Getter for totp_secret
};
