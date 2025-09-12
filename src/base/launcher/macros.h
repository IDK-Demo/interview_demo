#pragma once
#include <iostream>

#include <cpptrace/from_current.hpp>
#include <cpptrace/formatting.hpp>

#define IDK_CATCH_AND_PRINT_STACKTRACE(e) \
    std::cout << "Exception: " << e.what() << std::endl; \
    auto trace = cpptrace::from_current_exception(); \
    auto formatter = cpptrace::formatter{} \
    .paths(cpptrace::formatter::path_mode::basename) \
    .colors(cpptrace::formatter::color_mode::always); \
    for(const auto &frame : trace) { \
      if (frame.symbol.contains("idk")) { \
        std::cout << formatter.format(frame) << std::endl; \
      } \
    }
