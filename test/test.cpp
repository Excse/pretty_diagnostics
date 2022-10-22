//
// Created by timo on 22.10.22.
//

#include "gtest/gtest.h"

#include "library.h"

#include <filesystem>
#include <fstream>

auto create_invalid_type_combination (pretty_errors::Details *details,
                                      const std::string &provided_type,
                                      const pretty_errors::Span &provided_span,
                                      const std::string &required_type,
                                      const pretty_errors::Span &required_span) -> pretty_errors::Report {
  return pretty_errors::Report (
      details,
      pretty_errors::ReportType::ERROR,
      "Couldn't implicitly convert the expression to the desired type.",
      3,
      {
          pretty_errors::Label{
              .message_ =
              "Provides an {RED}expression{/} of the type \"" + provided_type + "\".",
              .span_ = provided_span,
              .color_type_ = pretty_errors::ColorType::RED,
          },
          pretty_errors::Label{
              .message_ = "But the {BLUE}variable{/} requires the type \"" + required_type + "\".",
              .span_ = required_span,
              .color_type_ = pretty_errors::ColorType::BLUE,
          }
      },
      "Try to cast the {RED}expression{/} to the type of the {BLUE}variable{/}.");
}

TEST (SimpleTest, IsPrintable) {
  auto input_path = "/media/mass_storage/Linux/Projects/worked_on/pretty_errors/test/source.ark";
  std::ifstream source_file (input_path, std::ios::in | std::ios::binary);

  auto size = std::filesystem::file_size (input_path);
  auto source = std::string (size, '\0');
  source_file.read (source.data (), (long)size);

  auto details = pretty_errors::Details (source, input_path);
  details.find_line_spans ();

  auto report = create_invalid_type_combination (
      &details,
      "i64", pretty_errors::Span (120, 124),
      "i32", pretty_errors::Span (111, 111)
  );
  report.print ();
}
