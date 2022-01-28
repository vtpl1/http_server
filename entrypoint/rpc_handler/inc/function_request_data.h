// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef function_request_data_h
#define function_request_data_h

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <string>
#include <vector>

class FunctionRequestData
{
private:
public:
  std::string func_name{};
  std::vector<uint8_t> args;
  FunctionRequestData() = default;
  FunctionRequestData(std::string func_name_, std::vector<uint8_t> args_);
  ~FunctionRequestData() = default;
  template <class Archive> void serialize(Archive& archive) { archive(CEREAL_NVP(func_name), CEREAL_NVP(args)); }
};

#endif // function_request_data_h
