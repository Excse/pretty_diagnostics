#include "gtest/gtest.h"

#include "pretty_diagnostics/report.h"
#include "pretty_diagnostics/label.h"
#include "pretty_diagnostics/file.h"

TEST(Report, Valid) {
    auto source = std::make_shared<File>("Thes ist a test!", "some/weird/path");

    auto report = Report::Builder()
        .type(Report::Type::ERROR)
        .code(42)
        .group(FileGroup::Builder()
                   .label(Label::Builder()
                              .message(R"(There shouldn't be a "t".)")
                              .span({{0, 7},
                                     {0, 8}})
                              .build())
                   .label(Label::Builder()
                              .message(R"(There should be an "i" instead of an "e".)")
                              .span({{0, 2},
                                     {0, 3}})
                              .build())
                   .file(source)
                   .build())
        .message("Hello World!")
        .build();

    std::cout << report;
}
