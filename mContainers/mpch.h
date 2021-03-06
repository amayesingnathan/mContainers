#pragma once

#include <cstddef>
#include <cmath>
#include <cassert>
#include <cfloat>
#include <cstring>
#include <cstdint> 
#include <climits>
#include <initializer_list>
#include <array>
#include <iostream>
#include <sstream>
#include <memory>
#include <filesystem>
#include <chrono>

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning(pop)