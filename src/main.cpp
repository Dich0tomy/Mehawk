#include <memory>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>

#include <dpp/dpp.h>

auto main() -> int {
  auto bot = dpp::cluster("NjkyMjkyNzQ2MTk0NTgzNTcy.GvDpKW.YyD8iPoPXlbumXNJsxEAM3gEorneb8nhLW5aOM");

  const std::string log_name
    = "mybot.log";

  spdlog::init_thread_pool(8192, 2);
  std::vector<spdlog::sink_ptr> sinks {
    std::make_shared<spdlog::sinks::stdout_color_sink_mt >(),
    std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_name, 1024 * 1024 * 5, 10)
  };

  auto logger = std::make_shared<spdlog::async_logger>("logs", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
  spdlog::register_logger(logger);

  logger->set_pattern("%^%Y-%m-%d %H:%M:%S.%e [%L] [th#%t]%$ : %v");
  logger->set_level(spdlog::level::level_enum::debug);

  bot.on_log([&logger](dpp::log_t const& event) {
    switch(event.severity) {
      case dpp::ll_trace:
        logger->trace("{}", event.message);
        break;
      case dpp::ll_debug:
        logger->debug("{}", event.message);
        break;
      case dpp::ll_info:
        logger->info("{}", event.message);
        break;
      case dpp::ll_warning:
        logger->warn("{}", event.message);
        break;
      case dpp::ll_error:
        logger->error("{}", event.message);
        break;
      case dpp::ll_critical:
      default:
        logger->critical("{}", event.message);
        break;
    }
  });

  bot.on_ready([&bot](dpp::ready_t event) {
    spdlog::info("Logged in!");
  });

  bot.start(dpp::st_wait);
}
