#pragma once

#include <exception>
#include <string>

class ExceptionWithErrorMsg : public std::exception {
private:
    std::string error_msg;
public:
    ExceptionWithErrorMsg(const std::string &error);

    std::string get_error_msg() const;
};
