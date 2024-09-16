#pragma once

#include <memory>
#include <vector>
#include <string>

#include "label.h"
#include "span.h"

class File {
public:
    File(std::string source, std::string path)
        : _source(std::move(source)), _path(std::move(path)) {}

    [[nodiscard]] const auto &source() const { return _source; };

    [[nodiscard]] const auto &path() const { return _path; };

private:
    std::string _source, _path;
};