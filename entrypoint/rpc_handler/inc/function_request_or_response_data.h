// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef function_request_or_response_data_h
#define function_request_or_response_data_h

#include <stdint.h>

#include "function_request_data.h"
#include "function_response_data.h"

class FunctionRequestOrResponseData
{
public:
  enum RequestOrResponse {
    UNKNOWN,
    REQUEST,
    RESPONSE

  };
  RequestOrResponse request_or_response{RequestOrResponse::UNKNOWN};
  std::vector<uint8_t> data;
  FunctionRequestOrResponseData() = default;
  ~FunctionRequestOrResponseData() = default;
  template <class Archive> void serialize(Archive& archive) { archive(CEREAL_NVP(request_or_response), CEREAL_NVP(data)); }
};
#endif // function_request_or_response_data_h
