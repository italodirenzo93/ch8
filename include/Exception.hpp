#pragma once

#include <string>

namespace ch8
{

    class Exception : public std::exception
    {
    public:
        Exception() noexcept;
        Exception(const std::string& msg) noexcept;
        Exception(const Exception& other) noexcept;

        virtual const char* what() const noexcept override;
    private:
        std::string message;
    };

}
