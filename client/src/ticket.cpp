#include "ticket.hpp"
#include <cotp.h>
#include <nlohmann/json.hpp>

Ticket::Ticket(const std::string &ticket_token, const std::string &totp_secret)
    : ticket_token(ticket_token)
    , totp_secret(totp_secret)
    {}

//this function looks like this because libcotp is a C library
std::string Ticket::get_ticket_string() const {
    //get current one-time-password from totp secret
    cotp_error_t error;
    char *result = get_totp(totp_secret.c_str(), 10, 15, 0, &error);
    if(result == NULL) {
        throw TicketError("TOTP Error code " + std::to_string(error));
    }
    
    //create json object, write it as string
    nlohmann::json ticket_json = {
        {"token", ticket_token},
        {"one_time_password", std::string(result)}
    };
    delete[] result; //free memory
    std::string ticket_json_str = ticket_json.dump();

    //convert to binary
    uchar* ticket_json_bin = reinterpret_cast<uchar*>(ticket_json_str.data());

    //encode this json string as base32
    result = base32_encode(ticket_json_bin, ticket_json_str.size()+1, &error);
    if(result == NULL) {
        throw TicketError("Base32 Error code " + std::to_string(error));
    }
    std::string returnVal = std::string(result);
    delete[] result;

    return returnVal;
}

std::string Ticket::get_ticket_token() const {
    return ticket_token;
}

std::string Ticket::get_totp_secret() const {
    return totp_secret;
}