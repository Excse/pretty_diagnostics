#include "pretty_diagnostics/file_group.h"

FileGroup::Builder &FileGroup::Builder::file(std::shared_ptr<File> file) {
    _file = std::move(file);
    return *this;
}

FileGroup::Builder &FileGroup::Builder::label(Label &&label) {
    auto can_fit = [&label](const LabelGroup &group) { return group.can_fit(label); };
    auto result = std::find_if(_groups.begin(), _groups.end(), can_fit);

    auto &group = (result != _groups.end()) ? *result : _groups.emplace_back();
    group.insert(std::move(label));

    return *this;
}

FileGroup FileGroup::Builder::build() {
    if (!_file) throw InvalidFileGroupState("file");
    if (_groups.empty()) throw NoLabels();
    return {std::move(_file), std::move(_groups)};
}