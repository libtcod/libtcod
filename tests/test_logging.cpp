#include <libtcod/logging.h>

#include <catch2/catch.hpp>
#include <string>

struct CapturedLog {
  friend bool operator==(const CapturedLog& lhs, const CapturedLog& rhs) {
    return lhs.message == rhs.message && lhs.level == rhs.level && lhs.source == rhs.source && lhs.line == rhs.line;
  }
  std::string message;
  int level;
  std::string source;
  int line;
};

using LogStorage = std::vector<CapturedLog>;

static void logger_callback(const TCOD_LogMessage* message, void* userdata) {
  static_cast<LogStorage*>(userdata)->push_back(CapturedLog{
      message->message,
      message->level,
      message->source,
      message->lineno,
  });
}

TEST_CASE("Logging tests") {
  auto log = LogStorage{};
  TCOD_set_log_callback(&logger_callback, static_cast<void*>(&log));
  TCOD_set_log_level(TCOD_LOG_INFO);
  TCOD_log_verbose_("DEBUG", TCOD_LOG_DEBUG, "source", 0);
  TCOD_log_verbose_fmt_(TCOD_LOG_INFO, "source", 1, "%s", "INFO");
  TCOD_log_verbose_fmt_(TCOD_LOG_WARNING, "source", 2, "%s", "WARNING");
  TCOD_log_verbose_("ERROR", TCOD_LOG_ERROR, "source", 3);
  REQUIRE(
      log == LogStorage{
                 CapturedLog{"INFO", TCOD_LOG_INFO, "source", 1},
                 CapturedLog{"WARNING", TCOD_LOG_WARNING, "source", 2},
                 CapturedLog{"ERROR", TCOD_LOG_ERROR, "source", 3},
             });
}
