#pragma once

#include <string>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include "exceptions-base.hpp"
#include "ticket.hpp"

using nlohmann::json;

class CommunicationError : public ExceptionWithErrorMsg {
    using ExceptionWithErrorMsg::ExceptionWithErrorMsg; //inherit constructor
};

class ServerConnection {
private:
    const int retry_counter = 3;

    httplib::Client cli;
    std::string access_token;

    bool is_new_error(std::string &old_error_msg, const std::string &error_msg) const;

    json post(const std::string &route, const httplib::Params &form_params = {}, const httplib::Headers &headers = {});

    json get(const std::string &route, const httplib::Params &query_params = {}, const httplib::Headers &headers = {});

    json post_logged_in(const std::string &route, const httplib::Params &form_params = {});

public:
    ServerConnection(std::string server_url);

    void login();

    Ticket generate_ticket();

    bool check_ticket_validity(const Ticket &ticket);
};
