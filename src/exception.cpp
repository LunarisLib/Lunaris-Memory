#include <Lunaris/Memory/exception.h>


namespace Lunaris {
namespace Memory {

    MemoryException::MemoryException(const std::string& msg) noexcept
        : std::runtime_error(msg)
    {
    }

    MemoryException::MemoryException(const char* msg) noexcept
        : std::runtime_error(msg)
    {
    }

    const char* MemoryException::what() const noexcept {
        return std::runtime_error::what();
    }

} // namespace Memory
} // namespace Lunaris