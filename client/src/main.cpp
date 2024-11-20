#include <iostream>
#include <nlohmann/json.hpp>
#include <httplib.h>

using json = nlohmann::json;

int main (int argc, char *argv[]) {
    httplib::Client cli("http://127.0.0.1:5000");
    auto res = cli.Get("/hello_world");
    
    auto resp = json::parse(res->body);
    std::cout << resp["msg"].get<std::string>() << std::endl;

    return 0;
}
