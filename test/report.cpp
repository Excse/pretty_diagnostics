#include "gtest/gtest.h"

#include <filesystem>
#include <fstream>

#include "pretty_diagnostics/report.h"
#include "pretty_diagnostics/label.h"
#include "pretty_diagnostics/file.h"

std::shared_ptr<File> read_file(const std::string &path) {
    std::ifstream source_file(path, std::ios::in | std::ios::binary);

    std::ostringstream ss;
    ss << source_file.rdbuf();

    return std::make_shared<File>(ss.str(), path);
}

TEST(Report, Valid) {
    auto source = read_file(TEST_DATA_PATH "example");

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
                   .label(Label::Builder()
                              .message(R"(There is no dot.)")
                              .span({{2, 13},
                                     {2, 14}})
                              .build())
                   .file(source)
                   .build())
        .message("Hello World!")
        .build();

    std::cout << report;
}
