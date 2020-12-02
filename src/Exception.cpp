#include "Exception.hpp"

namespace ch8
{

    Exception::Exception() noexcept
        : message("Unhandled exception")
    {
    }

    Exception::Exception(const std::string& msg) noexcept
        : message(msg)
    {
    }

    Exception::Exception(const Exception& other) noexcept
        : message(other.message)
    {
    }

    const char* Exception::what() const noexcept
    {
        return message.c_str();
    }

}
