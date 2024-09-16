#include "gtest/gtest.h"

#include "pretty_diagnostics/report.h"
#include "pretty_diagnostics/label.h"
#include "pretty_diagnostics/file.h"

#include <filesystem>
#include <fstream>

auto read_file(const std::string &path) -> std::shared_ptr<File> {
    std::ifstream source_file(path, std::ios::in | std::ios::binary);

    auto size = std::filesystem::file_size(path);
    auto source = std::string(size, '\0');
    source_file.read(source.data(), (long) size);

    return std::make_shared<File>(source, path);
}

TEST (SimpleTest, IsPrintable) {
    auto second_details = read_file(TEST_DATA_PATH "second.ark");
    auto first_details = read_file(TEST_DATA_PATH "first.ark");

    auto report = Report::Builder()
            .type(Report::Type::ERROR)
            .message("Couldn't implicitly convert the expression to the desired type.")
            .code(003)
            .label(Label::Builder()
                           .message("This is the 1st label")
                           .span({0, 3, 0})
                           .file(first_details)
                           .build())
            .label(Label::Builder()
                           .message("This is the 2nd label")
                           .span({184, 190, 14})
                           .build())
            .note("Try to cast the expression to the type of the variable.")
            .build();

    std::cout << report;
}
