//
// Created by timo on 24.10.22.
//

#include "pretty_diagnostics/utils.h"

auto repeat_string (const std::string &input, size_t amount) -> std::string {
  std::string output;
  while(amount--) {
    output += input;
  }

  return output;
}
