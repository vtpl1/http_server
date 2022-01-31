// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "function_request_data.h"

FunctionRequestData::FunctionRequestData(std::string func_name_, std::vector<uint8_t> args_)
    : func_name(std::move(func_name_)), args(std::move(args_))
{
}
