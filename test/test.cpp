//
// Created by timo on 22.10.22.
//

#include "gtest/gtest.h"

#include "pretty_diagnostics/file_group.h"

#include <filesystem>
#include <fstream>

auto read_file(const std::string &path) -> Details {
    std::ifstream source_file(path, std::ios::in | std::ios::binary);

    auto size = std::filesystem::file_size(path);
    auto source = std::string(size, '\0');
    source_file.read(source.data(), (long) size);

    return {source, path};
}

TEST (SimpleTest, IsPrintable) {
    auto first_details = read_file("./first.ark");
    auto second_details = read_file("./second.ark");

    auto report = Report::Builder()
            .type(ReportType::ERROR)
            .message("Couldn't implicitly convert the expression to the desired type.")
            .code(003)
            .label(Label::Builder()
                           .message("This is the 5th label")
                           .span({&first_details, 120, 120})
                           .build())
            .label(Label::Builder()
                           .message("This is the 4th label")
                           .span({&first_details, 110, 119})
                           .build())
            .label(Label::Builder()
                           .message("This is the 3rd label")
                           .span({&first_details, 112, 116})
                           .build())
            .label(Label::Builder()
                           .message("This is the 2nd label")
                           .span({&first_details, 113, 117})
                           .build())
            .label(Label::Builder()
                           .message("This is the 1st label")
                           .span({&first_details, 121, 123})
                           .build())
            .note("Try to cast the expression to the type of the variable.")
            .build();

    report.print(std::cout);
}
