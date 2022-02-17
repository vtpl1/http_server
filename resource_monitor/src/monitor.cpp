#include <Poco/DateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Path.h>
#include <Poco/Timespan.h>
#include <Poco/Timestamp.h>
#include <fmt/chrono.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <spdlog/logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <sstream>
#include <tuple>

#include "monitor.h"
#include "protobuf_helper.h"

constexpr int FUNC_TYPE_DATA{1};
constexpr int FUNC_TYPE_EOF{2};
constexpr int MONITOR_PORT_NUMBER{29000};
constexpr float max_sleep_upto_sec{10.0};
constexpr int max_size = 1048576 * 5;
constexpr int max_files = 3;
constexpr int banner_spaces = 80;

Monitor::Monitor(std::string session_folder, std::string target_host_address, uint16_t target_port)
    : _session_folder(std::move(session_folder)), _target_host_address(std::move(target_host_address)),
      _target_port(target_port)
{
  _thread = std::make_unique<std::thread>(&Monitor::run, this);
}

Monitor& Monitor::getInstance(std::string session_folder, std::string target_host_address, uint16_t target_port)
{
  static Monitor instance(std::move(session_folder), std::move(target_host_address), target_port);
  return instance;
}

Monitor& Monitor::getInstance() { return getInstance("session"); }

Monitor& Monitor::getInstance(std::string session_folder)
{
  return getInstance(std::move(session_folder), "127.0.0.1", MONITOR_PORT_NUMBER);
}

Monitor::~Monitor()
{
  _do_shutdown = true;
  _thread->join();
  for (auto&& it : _resource_map) {
    it.second.reset();
    it.second = nullptr;
  }
}

std::string getCurrentTimeStr()
{
  // std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
  // std::stringstream ss;
  // auto t = std::chrono::system_clock::to_time_t(tp);
  // auto tp2 = std::chrono::system_clock::from_time_t(t);
  // if (tp2 > tp) {
  //   t = std::chrono::system_clock::to_time_t(tp - std::chrono::seconds(1));
  // }
  // ss << std::put_time(std::localtime(&t), "%Y-%m-%d %T") << "." << std::setfill('0') << std::setw(3)
  //    << (std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count() % 1000);
  // return ss.str();
  Poco::Timestamp now;
  return Poco::DateTimeFormatter::format(now, Poco::DateTimeFormat::SORTABLE_FORMAT);
}

int64_t getCurrentTimeInMs()
{
  // std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  // auto duration = now.time_since_epoch();
  // auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
      .count();
  // return millis;
}

void Monitor::setStatusInternal(uint64_t id)
{
  if (_is_already_shutting_down) {
    return;
  }
  auto it = _resource_map.find(id);
  if (it != _resource_map.end()) {
    it->second->value++;
    it->second->last_update_time_in_ms.store(getCurrentTimeInMs());
  } else {
    _resource_map.insert(std::make_pair(id, std::move(std::make_unique<Status>())));
  }
}
uint64_t get_unique_id_from_app_id_channel_id_id(int16_t app_id, int16_t channel_id, uint64_t id)
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  return app_id * 100000 + channel_id * 100 + id;
}

std::tuple<int16_t, int16_t, uint64_t> get_app_id_channel_id_id_from_unique_id(uint64_t unique_id)
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  auto app_id = static_cast<int16_t>(unique_id / 100000);
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  auto channel_id_id = static_cast<int16_t>(unique_id % 100000);
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  auto channel_id = channel_id_id / 100;
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  uint64_t id = channel_id_id % 100;
  return {app_id, channel_id, id};
}
void send_to_server(std::shared_ptr<Poco::Net::StreamSocket>& s, ::resource::MachineStatus& machine_status,
                    int64_t& data_timestamp, const std::string& target_host_address, const uint16_t& target_port)
{
  int func_type = FUNC_TYPE_DATA;
  try {
    if (s == nullptr) {
      s = std::make_shared<Poco::Net::StreamSocket>();
      s->connect(Poco::Net::SocketAddress(target_host_address, target_port), Poco::Timespan(2, 0));
      s->setSendTimeout(Poco::Timespan(2, 0));
    }
    if (s != nullptr) {
      Poco::Net::SocketStream ss(*s);
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      ss.write(reinterpret_cast<char*>(&func_type), sizeof(func_type));
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      ss.write(reinterpret_cast<char*>(&data_timestamp), sizeof(data_timestamp));
      size_t data_len = machine_status.ByteSizeLong();
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      ss.write(reinterpret_cast<char*>(&data_len), sizeof(data_len));
      if (machine_status.SerializeToOstream(&ss)) {
        try {
          s->shutdown();
        } catch (const Poco::Net::NetException& e) {
        }
        try {
          s->close();
        } catch (const Poco::Net::NetException& e) {
        }
        throw std::runtime_error("SerializeToOstream error");
      }
      ss.flush();
    }
  } catch (const Poco::Net::NetException& e) {
  } catch (const Poco::TimeoutException& e) {
  } catch (const std::runtime_error& e) {
  }
}
void write_header(std::shared_ptr<spdlog::logger>& logger, std::map<uint64_t, std::unique_ptr<Status>>& resource_map)
{
  logger->info(fmt::format("┌{0:─^{2}}┐\n"
                           "│{1: ^{2}}│\n"
                           "└{0:─^{2}}┘",
                           "", getCurrentTimeStr(), banner_spaces));

  {
    std::stringstream ss;
    for (auto&& it : resource_map) {
      ss << "app.chn. id   fps|";
    }
    logger->info(ss.str());
  }

  {
    std::stringstream ss;
    for (auto&& it : resource_map) {
      std::tuple<int16_t, int16_t, uint64_t> a_c_i = get_app_id_channel_id_id_from_unique_id(it.first);
      ss << fmt::format("{:03}.{:03}.{:03}   fps|", std::get<0>(a_c_i), std::get<1>(a_c_i), std::get<2>(a_c_i));
    }
    logger->info(ss.str());
  }
}

void Monitor::run()
{
  int sleep_upto_sec = max_sleep_upto_sec;
  int iteration_counter = 0;
  int fps_log_counter = 0;
  Poco::Path base_path(_session_folder);
  base_path.append("fps.txt");
  std::string monitor_path = base_path.absolute().toString();
  std::cout << "Monitor folder is at: " << monitor_path << std::endl;
  auto logger = spdlog::rotating_logger_mt("fps", monitor_path, max_size, max_files);
  size_t last_map_size = 0;
  // change log pattern
  logger->set_pattern("%v");
  std::shared_ptr<Poco::Net::StreamSocket> s;
  while (!_do_shutdown) {
    if (sleep_upto_sec > 0) {
      sleep_upto_sec--;
    } else {
      if (last_map_size != _resource_map.size()) {
        write_header(logger, _resource_map);
        last_map_size = _resource_map.size();
      }

      sleep_upto_sec = max_sleep_upto_sec;
      ::resource::MachineStatus machine_status;
      machine_status.set_id(1);
      machine_status.set_channel_id(iteration_counter++);
      ::resource::ProcessStatus* p_process_status = machine_status.add_process_status();
      p_process_status->set_id(1);
      p_process_status->set_channel_id(1);

      int64_t data_timestamp = getCurrentTimeInMs();
      std::stringstream ss;
      for (auto&& it : _resource_map) {
        float diff =
            ((static_cast<float>(it.second->value) - static_cast<float>(it.second->last_value)) / max_sleep_upto_sec);
        ::resource::ThreadStatus* l_p_th = p_process_status->add_thread_status();
        l_p_th->set_id(it.first);
        l_p_th->set_channel_id(it.first);
        l_p_th->set_value(it.second->value);
        l_p_th->set_last_value(it.second->last_value);
        l_p_th->set_last_updated_in_ms(it.second->last_update_time_in_ms);
        it.second->last_value.store(it.second->value);

        std::tuple<int16_t, int16_t, uint64_t> a_c_i = get_app_id_channel_id_id_from_unique_id(it.first);
        {

          // ss << std::setfill('0') << std::setw(3) << std::get<0>(a_c_i) << "." << std::setfill('0') << std::setw(3)
          //    << std::get<1>(a_c_i) << "." << std::setfill('0') << std::setw(2) << std::get<2>(a_c_i) << " : "
          //    << std::setfill('0') << std::setw(5) << std::fixed << std::setprecision(2) << diff << "   ";
          ss << fmt::format("{:>17.02}|", diff);
        }
      }
      logger->info(ss.str());
      logger->flush();
      send_to_server(s, machine_status, data_timestamp, _target_host_address, _target_port);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  // logger->info(fmt::format("┌{0:─^{2}}┐\n"
  //                          "│{1: ^{2}}│\n"
  //                          "└{0:─^{2}}┘",
  //                          "", "Closed monitor", banner_spaces));
}

void Monitor::setStatus(int16_t app_id, int16_t channel_id, uint64_t id)
{
  Monitor::getInstance().setStatusInternal(get_unique_id_from_app_id_channel_id_id(app_id, channel_id, id));
}
