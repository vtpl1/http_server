// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef http_server_data_models_h
#define http_server_data_models_h
#include <functional>
#include <string>
#include <vector>
#include <map>
using DocRoots = std::map<std::string const, std::string const>;
using URLCallBackHandler = std::function<void(const std::string&)>;
using StatusCallBackHandler = std::function<void(const std::vector<uint8_t>&)>;
using CommandCallBackHandler = std::function<std::vector<uint8_t>(const std::string&)>;

#endif // http_server_data_models_h
