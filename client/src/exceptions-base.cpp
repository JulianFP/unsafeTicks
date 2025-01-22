#include "exceptions-base.hpp"

ExceptionWithErrorMsg::ExceptionWithErrorMsg(const std::string &error)
    : error_msg(error)
    {}

std::string ExceptionWithErrorMsg::get_error_msg() const {
    return error_msg;
}
