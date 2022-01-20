// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef function_response_data_h
#define function_response_data_h
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <string>

class FunctionResponseData
{
private:
public:
  std::string func_name{};
  std::vector<uint8_t> ret;
  FunctionResponseData() = default;
  ~FunctionResponseData() = default;
  template <class Archive> void serialize(Archive& archive) { archive(CEREAL_NVP(func_name), CEREAL_NVP(ret)); }
};

#endif // function_response_data_h
