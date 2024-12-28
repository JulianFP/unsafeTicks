#include <exception>
#include <iostream>
#include <nlohmann/json.hpp>
#include <httplib.h>
#include <string>

using json = nlohmann::json;

class Ticket {
private:
    std::string ticket_token;
    std::string totp_secret;
public:
    Ticket(std::string ticket_token, std::string totp_secret): ticket_token(ticket_token), totp_secret(totp_secret){}

    std::string get_ticket_string();
};

class ServerConnection {
private:
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
        int retry_counter = 3;

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
    class CommunicationError : std::exception {
    private:
        std::string error_msg;
    public:
        CommunicationError(std::string msg) : error_msg(msg){}

        std::string get_error_msg() const {
            return error_msg;
        }
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

    Ticket generate_token() {
        json response = post_logged_in("/generate_token");
        return Ticket(response["ticket_token"], response["totp_secret"]);
    }
};

int main (int argc, char *argv[]) {
    ServerConnection conn = ServerConnection("http://127.0.0.1:5000");
    try {
        conn.login();
        Ticket ticket = conn.generate_token();
    }
    catch (const ServerConnection::CommunicationError &e) {
        std::cout << "Error while communicating with server: " << e.get_error_msg() << std::endl;
    }
    
    return 0;
}
