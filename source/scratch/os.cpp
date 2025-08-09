#include "os.hpp"

size_t MemoryTracker::totalAllocated = 0;
size_t MemoryTracker::peakUsage = 0;
size_t MemoryTracker::allocationCount = 0;

void Log::log(std::string message, bool printToScreen) {
    if (printToScreen)
        std::cout << message << std::endl;
}
void Log::logWarning(std::string message, bool printToScreen) {
    if (printToScreen)
        std::cout << "\x1b[1;33m" << "Warning: " << message << "\x1b[0m" << std::endl;
}
void Log::logError(std::string message, bool printToScreen) {
    if (printToScreen)
        std::cerr << "\x1b[1;31m" << "Error: " << message << "\x1b[0m" << std::endl;
}
void Log::writeToFile(std::string message, std::string filePath) {
}
