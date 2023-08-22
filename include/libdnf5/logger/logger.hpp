/*
Copyright Contributors to the libdnf project.

This file is part of libdnf: https://github.com/rpm-software-management/libdnf/

Libdnf is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

Libdnf is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with libdnf.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef LIBDNF5_LOGGER_LOGGER_HPP
#define LIBDNF5_LOGGER_LOGGER_HPP

#include "libdnf5/utils/format.hpp"

#include <unistd.h>

#include <array>
#include <chrono>
#include <optional>
#include <string>


namespace libdnf5 {

/// Logger is an abstract interface used for logging.
/// An implementation (inherited class) can call callbacks, log the messages to memory, file, or somewhere else.
class Logger {
public:
    Logger() = default;
    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;
    Logger & operator=(const Logger &) = delete;
    Logger & operator=(Logger &&) = delete;
    virtual ~Logger() = default;

    enum class Level : unsigned int { CRITICAL, ERROR, WARNING, NOTICE, INFO, DEBUG, TRACE };

    static constexpr const char * level_to_cstr(Level level) noexcept {
        auto ilevel = static_cast<unsigned int>(level);
        return ilevel >= LEVEL_C_STR.size() ? "UNDEFINED" : LEVEL_C_STR[ilevel];
    }

    /// Set logging level for the logger. If set, all messages with the level higher then this value are skipped.
    /// @param level    Maximal level of log messages the logger will write.
    void set_level(Level level) { this->level = level; }

    /// Get logging level of the logger. LoggerErrorLevelNotSet is thrown in case the level is not set.
    Level get_level();

    /// Check whether logging level is set for the level.
    bool is_level_set() { return level.has_value(); }

    /// Check whether the message of given level will be logged.
    bool is_enabled_for(Level msg_level);

    template <typename... Ss>
    void critical(std::string_view format, Ss &&... args) {
        log(Level::CRITICAL, format, std::forward<Ss>(args)...);
    }

    template <typename... Ss>
    void error(std::string_view format, Ss &&... args) {
        log(Level::ERROR, format, std::forward<Ss>(args)...);
    }

    template <typename... Ss>
    void warning(std::string_view format, Ss &&... args) {
        log(Level::WARNING, format, std::forward<Ss>(args)...);
    }

    template <typename... Ss>
    void notice(std::string_view format, Ss &&... args) {
        log(Level::NOTICE, format, std::forward<Ss>(args)...);
    }

    template <typename... Ss>
    void info(std::string_view format, Ss &&... args) {
        log(Level::INFO, format, std::forward<Ss>(args)...);
    }

    template <typename... Ss>
    void debug(std::string_view format, Ss &&... args) {
        log(Level::DEBUG, format, std::forward<Ss>(args)...);
    }

    template <typename... Ss>
    void trace(std::string_view format, Ss &&... args) {
        log(Level::TRACE, format, std::forward<Ss>(args)...);
    }

    template <typename... Ss>
    void log(Level level, std::string_view format, Ss &&... args) {
        log_line(level, libdnf5::utils::sformat(format, std::forward<Ss>(args)...));
    }

    virtual void log_line(Level level, const std::string & message) noexcept;

    virtual void write(
        const std::chrono::time_point<std::chrono::system_clock> & time,
        pid_t pid,
        Level level,
        const std::string & message) noexcept = 0;

private:
#ifndef SWIG
    static constexpr std::array<const char *, static_cast<unsigned int>(Level::TRACE) + 1> LEVEL_C_STR = {
        "CRITICAL", "ERROR", "WARNING", "NOTICE", "INFO", "DEBUG", "TRACE"};
#endif

    std::optional<Level> level;
};

class StringLogger : public Logger {
public:
    void write(
        const std::chrono::time_point<std::chrono::system_clock> & time,
        pid_t pid,
        Level level,
        const std::string & message) noexcept override;

    virtual void write(const char * line) noexcept = 0;
};

}  // namespace libdnf5

#endif
