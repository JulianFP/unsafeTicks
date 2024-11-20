#include <iostream>
#include <nlohmann/json.hpp>
#include <httplib.h>

using json = nlohmann::json;

int main (int argc, char *argv[]) {
    httplib::Client cli("http://example.org");
    auto res = cli.Get("/");

    std::cout << json::parse(res->body) << std::endl;

    return 0;
}
