// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#include <Poco/Exception.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPMessage.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/WebSocket.h>
#include <array>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <string>

#include "command_receiver.h"
#include "logging.h"
#include "rpc_handler.h"

CommandReceiver::CommandReceiver(std::string host, int port, JobListManager& jlm)
    : _host(std::move(host)), _port(port), _jlm(jlm)
{
}

CommandReceiver::~CommandReceiver() { stop(); }

void CommandReceiver::start() { _thread = std::make_unique<std::thread>(&CommandReceiver::run, this); }
void CommandReceiver::signal_to_stop() { _do_shutdown = true; }
void CommandReceiver::stop()
{
  if (_is_already_shutting_down) {
    return;
  }
  _is_already_shutting_down = true;
  signal_to_stop();

  if (_thread) {
    if (_thread->joinable()) {
      _thread->join();
    }
  }
}

// void CommandReceiver::run()
// {
//   auto last_op_time = std::chrono::high_resolution_clock::now();
//   while (!_do_shutdown_composite()) {
//     try {
//       Poco::Net::HTTPClientSession cs(_host, _port);
//       Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, "/ws", Poco::Net::HTTPMessage::HTTP_1_1);
//       Poco::Net::HTTPResponse response;
//       Poco::Net::WebSocket ws(cs, request, response);
//       ws.setReceiveTimeout(Poco::Timespan(0, RECEIVE_TIMEOUT_MICOR_SEC)); // Timespan(long seconds, long
//       microseconds) ws.setSendTimeout(Poco::Timespan(0, RECEIVE_TIMEOUT_MICOR_SEC));    // Timespan(long seconds,
//       long microseconds)

//       RAY_LOG_INF << "WebSocket connection established.";
//       std::array<uint8_t, MAX_BUFFER_SIZE> buffer{};
//       int flags = 0;
//       int n = 0;
//       while (!_do_shutdown_composite()) {
//         flags = 0;
//         n = 0;
//         try {
//           n = ws.receiveFrame(buffer.data(), sizeof(buffer), flags);
//           _last_sleep_time_in_sec = 1;
//           // RAY_LOG_INF << Poco::format("Frame received (length=%d, flags=0x%x).", n, unsigned(flags));
//         } catch (Poco::TimeoutException& e) {
//           // FIXME: add consecutive time out count
//         } catch (Poco::Net::NetException& e) {
//           RAY_LOG_INF << e.what();
//           break;
//         }
//         if (n > 0) {
//           JobList job_list;

//           std::stringstream ss;
//           std::copy(buffer.begin(), buffer.begin() + n, std::ostream_iterator<uint8_t>(ss));
//           cereal::BinaryInputArchive iarchive(ss);
//           iarchive >> job_list;
//           std::stringstream ss1;
//           {
//             cereal::JSONOutputArchive oarchive_json(ss1);
//             oarchive_json(CEREAL_NVP(job_list));
//           }
//           _jlm.update_job_list(job_list);
//         }
//         if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() -
//         last_op_time)
//                 .count() >= PING_SEND_INTERVAL_SEC) {
//           try {
//             ws.sendFrame(buffer.data(), 0, Poco::Net::WebSocket::FRAME_OP_PING);
//             last_op_time = std::chrono::high_resolution_clock::now();
//             // RAY_LOG_INF << "PING sent";
//           } catch (Poco::Net::NetException& e) {
//             RAY_LOG_ERR << e.what();
//             break;
//           }
//         }
//         if ((static_cast<unsigned int>(flags) & Poco::Net::WebSocket::FRAME_OP_BITMASK) ==
//         Poco::Net::WebSocket::FRAME_OP_CLOSE) {
//           break;
//         }
//       }
//       try {
//         ws.close();
//       } catch (Poco::Net::NetException& e) {
//         RAY_LOG_INF << "Exception " << e.what();
//       }
//       RAY_LOG_INF << "WebSocket connection closed.";
//     } catch (Poco::Net::WebSocketException& exc) {
//       RAY_LOG_ERR << exc.what();
//     } catch (Poco::Net::NetException& e) {
//       RAY_LOG_ERR << e.what();
//     }
//     _jlm.clear_job_list();
//     if (_last_sleep_time_in_sec < 32) {
//       _last_sleep_time_in_sec *= 2;
//     }
//     std::this_thread::sleep_for(std::chrono::seconds(_last_sleep_time_in_sec));
//   }
// }

void CommandReceiver::run()
{
  auto last_op_time = std::chrono::high_resolution_clock::now();
  while (!_do_shutdown_composite()) {
    try {
      Poco::Net::HTTPClientSession cs(_host, _port);
      Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, "/ws", Poco::Net::HTTPMessage::HTTP_1_1);
      Poco::Net::HTTPResponse response;
      Poco::Net::WebSocket ws(cs, request, response);
      RAY_LOG_INF << "WebSocket connection established.";
      RpcHandler rpc_handler(ws, true);
      while (!_do_shutdown_composite()) {
        if (!rpc_handler.do_next()) {
          break;
        }
      }
      try {
        ws.close();
      } catch (Poco::Net::NetException& e) {
        RAY_LOG_INF << "Exception " << e.what();
      }
      RAY_LOG_INF << "WebSocket connection closed.";
    } catch (Poco::Net::WebSocketException& exc) {
      RAY_LOG_ERR << exc.what();
    } catch (Poco::Net::NetException& e) {
      RAY_LOG_ERR << e.what();
    }
    _jlm.clear_job_list();
    // if (_last_sleep_time_in_sec < 32) {
    //   _last_sleep_time_in_sec *= 2;
    // }
    std::this_thread::sleep_for(std::chrono::seconds(_last_sleep_time_in_sec));
  }
}