#pragma once

#include "label_group.h"

class File;

class FileGroup {
private:
    FileGroup(std::shared_ptr<File> &&file, std::vector<LabelGroup> &&groups)
        : _file(std::move(file)), _groups(std::move(groups)) {}

public:
    [[nodiscard]] const auto &groups() const { return _groups; };

    [[nodiscard]] const auto &file() const { return _file; };

private:
    std::vector<LabelGroup> _groups{};
    std::shared_ptr<File> _file;

public:
    class Builder {
    public:
        Builder &file(std::shared_ptr<File> file);

        Builder &label(Label &&label);

        FileGroup build();

    private:
        std::vector<LabelGroup> _groups{};
        std::shared_ptr<File> _file;
    };
};

class InvalidFileGroupState : public std::runtime_error {
public:
    explicit InvalidFileGroupState(const std::string &field)
        : std::runtime_error("The field \"" + field + "\" is required to build a file group.") {}
};

class NoLabels : public std::runtime_error {
public:
    explicit NoLabels() : std::runtime_error("Can't create a file group without labels.") {}
};