//
// Created by timo on 22.10.22.
//

#include "gtest/gtest.h"

#include "library.h"

#include <filesystem>
#include <fstream>

auto create_invalid_type_combination (pretty_diagnostics::Details *details,
                                      const std::string &provided_type,
                                      const pretty_diagnostics::Span &provided_span,
                                      const std::string &required_type,
                                      const pretty_diagnostics::Span &required_span) -> pretty_diagnostics::Report {
  return pretty_diagnostics::Report (
      details,
      pretty_diagnostics::ReportType::ERROR,
      "Couldn't implicitly convert the expression to the desired type.",
      3,
      {
          pretty_diagnostics::Label{
              .message_ =
              "Provides an {RED}expression{/} of the type \"" + provided_type + "\".",
              .span_ = provided_span,
              .color_type_ = pretty_diagnostics::ColorType::RED,
          },
          pretty_diagnostics::Label{
              .message_ = "But the {BLUE}variable{/} requires the type \"" + required_type + "\".",
              .span_ = required_span,
              .color_type_ = pretty_diagnostics::ColorType::BLUE,
          }
      },
      "Try to cast the {RED}expression{/} to the type of the {BLUE}variable{/}.");
}

TEST (SimpleTest, IsPrintable) {
  auto input_path = "/media/mass_storage/Linux/Projects/worked_on/pretty_diagnostics/test/source.ark";
  std::ifstream source_file (input_path, std::ios::in | std::ios::binary);

  auto size = std::filesystem::file_size (input_path);
  auto source = std::string (size, '\0');
  source_file.read (source.data (), (long)size);

  auto details = pretty_diagnostics::Details (source, input_path);
  details.find_line_spans ();

  auto report = create_invalid_type_combination (
      &details,
      "i64", pretty_diagnostics::Span (120, 124),
      "i32", pretty_diagnostics::Span (192, 192)
  );
  report.print ();
}
