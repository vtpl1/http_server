// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef function_response_data_h
#define function_response_data_h

#include <cereal/types/string.hpp>

#include <string>

class FunctionResponseData
{
private:
  std::string _func_name{};
public:
  FunctionResponseData() = default;
  ~FunctionResponseData() = default;
  template <class Archive> void serialize(Archive& archive) { archive(CEREAL_NVP(_func_name)); }
};

#endif // function_response_data_h
