#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <concepts>
#include <functional>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>

#include <dpp/dpp.h>

#include <gtkmm.h>

#include "haumea/env.hpp"

template<typename Signature>
class Delegate {
  static_assert([] { return false; }(), R"X(The signature must be of type "ReturnType(Type1, Type2, Type3)" e.g. std::vector<int>(int, std::string))X");
};

template<typename RetT, typename... ArgsTs>
class Delegate<RetT(ArgsTs...)> {
  using Signature = RetT(ArgsTs...);
  using BoundFunctions = std::vector<std::function<Signature>>;

public:
  using BindId = typename BoundFunctions::size_type;

  Delegate() = default;

  template<typename CallableT, typename... ArgsToBindTs>
  BindId Bind(CallableT&& Callable, ArgsToBindTs&&... ArgsToBind) {
    static_assert(std::invocable<CallableT, ArgsToBindTs..., ArgsTs...>, "The callable is not invocable with the arguments required for the delegate.");

    if constexpr(sizeof...(ArgsToBindTs) == 0) {
      Functions.push_back(std::forward<CallableT>(Callable));
    } else {
      Functions.push_back(std::bind_front(std::forward<CallableT>(Callable), std::forward<ArgsToBindTs>(ArgsToBind)...));
    }

    return Functions.size() - 1;
  }

  template<typename... LocalArgsTs>
    requires(std::convertible_to<LocalArgsTs, ArgsTs> && ...)
  void Call(LocalArgsTs&&... Args) {
    for(auto const Func : Functions) {
      Func(std::forward<LocalArgsTs>(Args)...);
    }
  }

  bool IsBound() {
    return ! Functions.empty();
  }

  void Unbind(BindId Id) {
    assert(Id < Functions.size() && "Bind id is equal or above the amount of bound functions which is impossible.");

    Functions.erase(Functions.begin() + Id);
  }

  void Reset() {
    Functions.clear();
  }

private:
  BoundFunctions Functions;
};

Delegate<void()> OnClick;

struct MessageInfo {
  std::string message;
  std::string author;
  std::string color;
};

Delegate<void(MessageInfo const&)> OnNewMessage;

class DiscordClient {
public:
  DiscordClient()
    : bot(prepare_bot()) {
    // init_logging();

    bot.on_ready([](dpp::ready_t event) { spdlog::info("Logged in!"); });

    bot.start(true);

    OnClick.Bind(&DiscordClient::send_message, this);
  }

private:

  auto init_logging() -> void {
    const std::string log_name = "mybot.log";

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
  }

  auto prepare_bot() -> dpp::cluster {
    auto const env = hm::Env(".env");

    auto const discord_token = env["DISCORD_TOKEN"];

    if(not discord_token) {
      spdlog::error("DISCORD_TOKEN not present in .env.");
      std::exit(1);
    }

    return dpp::cluster(std::string(discord_token.value()));
  }

  auto send_message() -> void {
    spdlog::info("CALLED");

    // bot.message_create(dpp::message(1117372769244549211, "Booga"));
    // bot.message_create(dpp::message(1117372769244549211, "Booga"));
  }

  dpp::cluster bot;
};

class MyWindow : public Gtk::Window {
public:

  MyWindow()
    : tag_table(Gtk::TextBuffer::TagTable::create()),
      text_buffer(Gtk::TextBuffer::create(tag_table)),
      box(Gtk::Orientation::VERTICAL),
      button("Chuj kurwa") {
    connect_gtk_signals();

    connect_delegates();

    create_persistent_tags();

    build_ui();
  }

  ~MyWindow() override = default;

protected:
  void create_persistent_tags() {
    // auto tag = Gtk::TextBuffer::Tag::create("bold");
    // tag->property_weight() = 900;
    // tag_table->add(tag);
  }

  auto create_color_tag(std::string hex_color) {
    auto tag = Gtk::TextBuffer::Tag::create();

    tag->property_foreground_rgba() = Gdk::RGBA(hex_color);
    tag->property_weight() = 1200;
    tag_table->add(tag);

    return tag;
  }

  void add_message(MessageInfo const& message_info) {
    auto it_before_author = text_buffer->end();
    text_buffer->insert_with_tag(it_before_author, message_info.author + "  ", create_color_tag(message_info.color));
    text_buffer->insert(text_buffer->end(), message_info.message + "\n");
    // Glib::signal_idle().connect([this]() {
    //   auto end = text_buffer->end();
    //   text_view.scroll_to(end);

    //   return false; // disconnect the signal
    // });
  }

  void dummy_add_new_message() {
    static auto i = 0;
    auto messages = std::vector<MessageInfo> {
      { .message = "Hello, there!",
        .author = "b4mbus",
        .color = "green" },
      { .message = "Kurwa chlopaki wiecie gdzie znalezc kamieniowa roxe?",
        .author = "Furno",
        .color = "#ff0000" },
      { .message = "Whatssup",
        .author = "yourold",
        .color = "#ff00f0" },
      { .message = "Wpierdalam kamienie",
        .author = "Furno",
        .color = "#ff0000" },
      { .message = "Wyruchalem dzisiaj kamien",
        .author = "Furno",
        .color = "#ff0000" },

      { .message = "Hej", .author = "Furno", .color = "#ff0000" },
      { .message = " ", .author = "Furno", .color = "#ff0000" },
      { .message = "Jestem", .author = "Furno", .color = "#ff0000" },
      { .message = " ", .author = "Furno", .color = "#ff0000" },
      { .message = "Furno", .author = "Furno", .color = "#ff0000" },
    };

    add_message(messages[i++ % messages.size()]);
  }

  void connect_delegates() {
    OnNewMessage.Bind(&MyWindow::add_message, this);
    OnClick.Bind(&MyWindow::dummy_add_new_message, this);
  }

  void connect_gtk_signals() {
    button.signal_clicked().connect([] {
      spdlog::info("Button clicked btw.");

      OnClick.Call();
    });

    text_buffer->signal_changed().connect([this]() {
      auto vadjustment = scrolled.get_vadjustment();
      vadjustment->set_value(vadjustment->get_upper());
    });
  }

  void build_ui() {
    set_title("Haumea");
    set_default_size(720, 480);

    box.set_margin(5);
    set_child(box);

    scrolled.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    scrolled.set_expand();
    scrolled.set_child(text_view);

    text_view.set_editable(false);
    text_view.set_cursor_visible(false);
    text_view.set_buffer(text_buffer);

    button.set_margin(10);

    box.append(scrolled);
    box.append(button);
  }

  //Member widgets:
  std::vector<Glib::RefPtr<Gtk::TextBuffer>> texts;

  Glib::RefPtr<Gtk::TextBuffer::TagTable> tag_table;
  Glib::RefPtr<Gtk::TextBuffer> text_buffer;
  Gtk::ScrolledWindow scrolled;
  Gtk::TextView text_view;
  Gtk::Box box;
  Gtk::Button button;
};

auto main(int argc, char** argv) -> int {
  auto client = DiscordClient();

  auto app = Gtk::Application::create("org.b4mbus.haumea");

  return app->make_window_and_run<MyWindow>(argc, argv);
}
