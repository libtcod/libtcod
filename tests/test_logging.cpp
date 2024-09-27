#include <libtcod/logging.h>

#include <catch2/catch_all.hpp>
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

static void logger_callback(const TCODFOV_LogMessage* message, void* userdata) {
  static_cast<LogStorage*>(userdata)->push_back(CapturedLog{
      message->message,
      message->level,
      message->source,
      message->lineno,
  });
}

TEST_CASE("Logging tests") {
  auto log = LogStorage{};
  TCODFOV_set_log_callback(&logger_callback, static_cast<void*>(&log));
  TCODFOV_set_log_level(TCODFOV_log_INFO);
  TCODFOV_log_verbose_("DEBUG", TCODFOV_log_DEBUG, "source", 0);
  TCODFOV_log_verbose_fmt_(TCODFOV_log_INFO, "source", 1, "%s", "INFO");
  TCODFOV_log_verbose_fmt_(TCODFOV_log_WARNING, "source", 2, "%s", "WARNING");
  TCODFOV_log_verbose_("ERROR", TCODFOV_log_ERROR, "source", 3);
  REQUIRE(
      log == LogStorage{
                 CapturedLog{"INFO", TCODFOV_log_INFO, "source", 1},
                 CapturedLog{"WARNING", TCODFOV_log_WARNING, "source", 2},
                 CapturedLog{"ERROR", TCODFOV_log_ERROR, "source", 3},
             });
}
