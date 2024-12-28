#include <chrono>
#include <exception>
#include <iostream>
#include <string>
#include <thread>
#include <nlohmann/json.hpp>
#include <httplib.h>
#include <cotp.h>

using json = nlohmann::json;

class ExceptionWithErrorMsg : public std::exception {
private:
    std::string error_msg;
public:
    ExceptionWithErrorMsg(const std::string &error) : error_msg(error){}

    std::string get_error_msg() const {
        return error_msg;
    }
};

class Ticket {
private:
    std::string ticket_token;
    std::string totp_secret;
public:
    class TicketError : public ExceptionWithErrorMsg {
        using ExceptionWithErrorMsg::ExceptionWithErrorMsg; //inherit constructor
    };

    Ticket(std::string ticket_token, std::string totp_secret): ticket_token(ticket_token), totp_secret(totp_secret){}

    //this function looks like this because libcotp is a C library
    std::string get_ticket_string() const {
        //get current one-time-password from totp secret
        cotp_error_t error;
        char *result = get_totp(totp_secret.c_str(), 10, 15, 0, &error);
        if(result == NULL) {
            throw TicketError("TOTP Error code " + std::to_string(error));
        }
        
        //create json object, write it as string
        json ticket_json = {
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
};

class ServerConnection {
private:
    const int retry_counter = 3;

    httplib::Client cli;
    std::string access_token;

    bool is_new_error(std::string &old_error_msg, const std::string &error_msg) const {
        if (old_error_msg == error_msg) {
            return false;
        }
        else {
            old_error_msg = error_msg;
            return true;
        }
    }

    json post(const std::string &route, const httplib::Params &form_params = {}, const httplib::Headers &headers = {}) {
        json json_object;
        bool error = false;
        std::string error_msg;

        for(int i = 0; i < retry_counter; ++i){
            if (auto res = cli.Post(route, headers, form_params)){
                try{
                    json_object = json::parse(res->body);
                }
                catch (const json::parse_error& e) {
                    error = true;
                    if (is_new_error(error_msg, "Server response is not valid json")){
                        i = 0;
                    }
                }
                json_object["status"] = res->status; //include the status code in the json object too for easy access
                break; //no error
            }
            else {
                error = true;
                if (is_new_error(error_msg, "Communication with server failed")){
                    i = 0;
                }
            }
        }
        if (error) {
            throw CommunicationError(error_msg);
        }
        return json_object;
    }

    json get(const std::string &route, const httplib::Params &query_params = {}, const httplib::Headers &headers = {}) {
        json json_object;
        bool error = false;
        std::string error_msg;

        for(int i = 0; i < retry_counter; ++i){
            if (auto res = cli.Get(route, query_params, headers)){
                try{
                    json_object = json::parse(res->body);
                }
                catch (const json::parse_error& e) {
                    error = true;
                    if (is_new_error(error_msg, "Server response is not valid json")){
                        i = 0;
                    }
                }
                json_object["status"] = res->status; //include the status code in the json object too for easy access
                break; //no error
            }
            else {
                error = true;
                if (is_new_error(error_msg, "Communication with server failed")){
                    i = 0;
                }
            }
        }
        if (error) {
            throw CommunicationError(error_msg);
        }
        return json_object;
    }

    json post_logged_in(const std::string &route, const httplib::Params &form_params = {}) {
        if (access_token.length() == 0) {
            throw CommunicationError("Please login first before calling post_logged_in!");
        }
        else {
            httplib::Headers headers = {
                { "Authorization", "Bearer " + access_token }
            };
            return post(route, form_params, headers);
        }
    }

public:
    class CommunicationError : public ExceptionWithErrorMsg {
        using ExceptionWithErrorMsg::ExceptionWithErrorMsg; //inherit constructor
    };

    ServerConnection(std::string server_url): cli(httplib::Client(server_url)), access_token(""){}

    void login() {
        httplib::Params params{
            { "username", "defaultUser" },
            { "password", "password1234"}
        };
        json response = post("/login", params);

        access_token = response["client_token"];
    }

    Ticket generate_ticket() {
        json response = post_logged_in("/generate_ticket");
        return Ticket(response["ticket_token"], response["totp_secret"]);
    }

    bool check_ticket_validity(const Ticket &ticket) {
        httplib::Params params{
            { "ticket", ticket.get_ticket_string() }
        };
        json response = get("/check_ticket_validity", params);

        if (response["status"] == 200) return true;
        else return false;
    }
};

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
    catch (const ServerConnection::CommunicationError &e) {
        std::cout << "Error while communicating with server: " << e.get_error_msg() << std::endl;
    }
    
    return 0;
}
