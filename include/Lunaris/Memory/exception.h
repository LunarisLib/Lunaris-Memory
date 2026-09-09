#pragma once

#include <stdexcept>

namespace Lunaris {
namespace Memory {

    class MemoryException : public std::runtime_error {
    public:
        explicit MemoryException(const std::string&) noexcept;
        explicit MemoryException(const char*) noexcept;

        const char* what() const noexcept;
    };

} // namespace Memory
} // namespace Lunaris