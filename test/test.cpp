//
// Created by timo on 22.10.22.
//

#include "gtest/gtest.h"

#include "pretty_diagnostics/library.h"

#include <filesystem>
#include <fstream>

auto read_file (const std::string &path) -> pretty_diagnostics::Details {
  std::ifstream source_file (path, std::ios::in | std::ios::binary);

  auto size = std::filesystem::file_size (path);
  auto source = std::string (size, '\0');
  source_file.read (source.data (), (long)size);

  return {source, path};
}

TEST (SimpleTest, IsPrintable) {
  auto first_details = read_file ("/media/mass_storage/Linux/Projects/worked_on/pretty_diagnostics/test/first.ark");
  auto second_details = read_file ("/media/mass_storage/Linux/Projects/worked_on/pretty_diagnostics/test/second.ark");

  auto report = pretty_diagnostics::Report (
      pretty_diagnostics::ReportType::ERROR,
      "Couldn't implicitly convert the expression to the desired type.",
      3,
      {
          pretty_diagnostics::Label (
              "Provides an {RED}expression{/} of the type \"i64\".",
              pretty_diagnostics::Span (&first_details, 119, 123),
              pretty_diagnostics::ColorType::RED),
          pretty_diagnostics::Label (
              "But the {GREEN}variable{/} requires the type \"i32\".",
              pretty_diagnostics::Span (&first_details, 110, 110),
              pretty_diagnostics::ColorType::GREEN),
          pretty_diagnostics::Label (
              "But the {GREEN}variable{/} requires the type \"i32\".",
              pretty_diagnostics::Span (&second_details, 110, 110),
              pretty_diagnostics::ColorType::GREEN),
      },
      "Try to cast the {RED}expression{/} to the type of the {GREEN}variable{/}.");

  report.print (std::cout);
}
