//
// Created by timo on 22.10.22.
//

#include "gtest/gtest.h"

#include "library.h"

#include <filesystem>
#include <fstream>

auto create_invalid_type_combination (const std::string &provided_type,
                                      const pretty_diagnostics::Span &provided_span,
                                      const std::string &required_type,
                                      const pretty_diagnostics::Span &required_span) -> pretty_diagnostics::Report {
  return pretty_diagnostics::Report (
      pretty_diagnostics::ReportType::ERROR,
      "Couldn't implicitly convert the expression to the desired type.",
      3,
      {
          pretty_diagnostics::Label (
              "Provides an {RED}expression{/} of the type \"" + provided_type + "\".",
              provided_span,
              pretty_diagnostics::ColorType::RED),
          pretty_diagnostics::Label (
              "But the {BLUE}variable{/} requires the type \"" + required_type + "\".",
              required_span,
              pretty_diagnostics::ColorType::BLUE),
      },
      "Try to cast the {RED}expression{/} to the type of the {BLUE}variable{/}.");
}

auto read_file (const std::string &path) -> pretty_diagnostics::Details {
  std::ifstream source_file (path, std::ios::in | std::ios::binary);

  auto size = std::filesystem::file_size (path);
  auto source = std::string (size, '\0');
  source_file.read (source.data (), (long)size);

  return pretty_diagnostics::Details (source, path);
}

TEST (SimpleTest, IsPrintable) {
  auto first_details = read_file ("/media/mass_storage/Linux/Projects/worked_on/pretty_diagnostics/test/first.ark");
  auto second_details = read_file ("/media/mass_storage/Linux/Projects/worked_on/pretty_diagnostics/test/second.ark");

  auto report = create_invalid_type_combination (
      "i64", pretty_diagnostics::Span (&first_details, 120, 124),
      "i32", pretty_diagnostics::Span (&second_details, 111, 111)
  );
  report.print ();
}
