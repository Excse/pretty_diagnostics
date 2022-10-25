#include "pretty_diagnostics/library.h"

#include <iostream>
#include <iomanip>
#include <utility>
#include <ranges>
#include <regex>
#include <map>

#include "pretty_diagnostics/utils.h"

using namespace pretty_diagnostics;

#define DISPLAYED_LINE_PADDING 1

#define COLOR_RED 228, 38, 103
#define COLOR_GREEN 175, 255, 95
#define COLOR_WHITE 220, 238, 235
#define COLOR_GREY 148, 148, 148
#define COLOR_BEACH 125, 199, 164
#define COLOR_LIGHT_GREY 170, 173, 176
#define COLOR_BLUE 0, 116, 217
#define COLOR_ORANGE 255, 133, 27
#define COLOR_YELLOW 255, 220, 0
#define COLOR_AQUA 127, 219, 255

auto pretty_diagnostics::report_type_to_prefix (ReportType type) -> std::string {
  switch (type) {
  case ReportType::ERROR: return "E";
  case ReportType::INFO: return "I";
  case ReportType::WARNING: return "W";
  default: assertm(false, "This report type is not implemented yet.");
  }
}

auto pretty_diagnostics::report_type_to_string (ReportType type) -> std::string {
  switch (type) {
  case ReportType::ERROR: return "Error";
  case ReportType::INFO: return "Info";
  case ReportType::WARNING: return "Warning";
  default: assertm(false, "This report type is not implemented yet.");
  }
}

auto pretty_diagnostics::color_by_type (std::ostream &stream, ColorType type) -> std::ostream & {
  switch (type) {
  case ColorType::DEFAULT: {
    stream << termcolor::color<COLOR_LIGHT_GREY>;
    break;
  }
  case ColorType::RED: {
    stream << termcolor::color<COLOR_RED>;
    break;
  }
  case ColorType::GREEN: {
    stream << termcolor::color<COLOR_GREEN>;
    break;
  }
  case ColorType::BLUE: {
    stream << termcolor::color<COLOR_BLUE>;
    break;
  }
  case ColorType::ORANGE: {
    stream << termcolor::color<COLOR_ORANGE>;
    break;
  }
  case ColorType::YELLOW: {
    stream << termcolor::color<COLOR_YELLOW>;
    break;
  }
  case ColorType::AQUA: {
    stream << termcolor::color<COLOR_AQUA>;
    break;
  }
  default: assertm(false, "This color is not implemented yet.");
  }

  return stream;
}

auto pretty_diagnostics::get_color_by_name (const std::string &name) -> ColorType {
  if (name == "RED") {
    return ColorType::RED;
  } else if (name == "GREEN") {
    return ColorType::GREEN;
  } else if (name == "BLUE") {
    return ColorType::BLUE;
  } else if (name == "ORANGE") {
    return ColorType::ORANGE;
  } else if (name == "YELLOW") {
    return ColorType::YELLOW;
  } else if (name == "AQUA") {
    return ColorType::AQUA;
  }

  assertm (false, "This color is not implemented yet.");
}

void pretty_diagnostics::print_formatted_text (std::ostream &output, const std::string &text) {
  output << termcolor::color<COLOR_WHITE>;

  for (auto index = 0u; index < text.length (); index++) {
    const auto &current_char = text.at (index);
    if (current_char == '{') {
      std::string mode;
      for (index++; index < text.length (); index++) {
        const auto &mode_char = text.at (index);
        if (mode_char == '}') {
          break;
        }

        mode += mode_char;
      }

      if (mode == "/") {
        output << termcolor::color<COLOR_WHITE>;
      } else {
        color_by_type (output, get_color_by_name (mode));
      }
    } else {
      output << current_char;
    }
  }
}

bool AscendingLabels::operator() (const Label *first, const Label *second) const {
  auto difference = (int)first->get_span ().get_start_index () -
                    (int)second->get_span ().get_start_index ();
  if (difference == 0) {
    return first->get_span ().get_end_index () < second->get_span ().get_end_index ();
  }

  return difference < 0;
}

bool DescendingLabels::operator() (const Label *first, const Label *second) const {
  auto difference = (int)first->get_span ().get_end_index () -
                    (int)second->get_span ().get_end_index ();
  if (difference == 0) {
    return first->get_span ().get_start_index () > second->get_span ().get_start_index ();
  }

  return difference > 0;
}

Span::Span (Details *details, size_t start_index, size_t end_index)
    : start_index_ (start_index), details_ (details), end_index_ (end_index) {}

Span::Span () : start_index_ (), details_ (), end_index_ () {}

auto Span::relative_to (const Span &span) const -> Span {
  return {span.get_details (), this->start_index_ - span.start_index_,
          this->end_index_ - span.start_index_};
}

auto Span::is_label_in_range (const Label &label) const -> bool {
  return (this->start_index_ <= label.get_span ().start_index_)
         && (this->end_index_ >= label.get_span ().end_index_);
}

auto Span::get_start_index () const -> size_t {
  return this->start_index_;
}

auto Span::get_details () const -> Details * {
  return this->details_;
}

auto Span::get_end_index () const -> size_t {
  return this->end_index_;
}

void Span::set_end_index (size_t end_index) {
  this->end_index_ = end_index;
}

auto Span::get_width () const -> size_t {
  return this->end_index_ - this->start_index_;
}

Label::Label (std::string message, const Span &span, ColorType color_type)
    : message_ (std::move (message)), color_ (color_type), span_ (span) {
  this->line_ = this->span_.get_details ()->get_label_line (*this);
}

auto Label::get_message () const -> const std::string & {
  return this->message_;
}

auto Label::get_span () const -> const Span & {
  return this->span_;
}

auto Label::get_color () const -> ColorType {
  return this->color_;
}

auto Label::get_line () const -> size_t {
  return this->line_;
}

Details::Details (std::string source, std::string path)
    : line_spans_ (), source_ (std::move (source)), path_ (std::move (path)) {
  Span *current_span = nullptr;
  for (auto index = 0u; index < this->source_.size (); index++) {
    if (current_span == nullptr) {
      auto new_span = std::make_shared<Span> (this, index, index);
      current_span = new_span.get ();
      this->line_spans_.push_back (new_span);
    }

    const auto &source_char = this->source_.at (index);
    if (source_char == '\n') {
      current_span->set_end_index (index);
      current_span = nullptr;
      continue;
    }
  }
}

auto Details::get_line_source (const Span &span) const -> std::string {
  auto result = this->source_.substr (span.get_start_index (), span.get_width ());
  result = std::regex_replace (result, std::regex ("\t"), " ");
  return result;
}

auto Details::get_label_line (const Label &label) const -> size_t {
  for (auto index = 0u; index < this->line_spans_.size (); index++) {
    const auto &span = this->line_spans_.at (index);
    if (span->is_label_in_range (label)) {
      return index;
    }
  }

  assertm(false, "Couldn't find the associated line for this span.");
}

auto Details::get_line_spans () const -> const std::vector<std::shared_ptr<Span>> & {
  return this->line_spans_;
}

auto Details::get_source () const -> const std::string & {
  return this->source_;
}

auto Details::get_path () const -> const std::string & {
  return this->path_;
}

LabelGroup::LabelGroup (Details *general_details_, std::vector<const Label *> labels)
    : labels_ (std::move (labels)), first_label_ (), last_label_ (),
      details_ (general_details_) {
  assertm(!this->labels_.empty (), "Couldn't find the last labels as there are no labels.");

  auto ascending_labels (this->labels_);
  std::sort (ascending_labels.begin (), ascending_labels.end (), AscendingLabels ());

  this->first_label_ = ascending_labels.front ();
  this->last_label_ = ascending_labels.back ();
}

void LabelGroup::print (std::ostream &output, const std::string &spaces_prefix) const {
  auto first_line = this->first_label_->get_line ();
  auto last_line = this->last_label_->get_line ();

  auto beginning_line = first_line - DISPLAYED_LINE_PADDING;
  auto ending_line = last_line + DISPLAYED_LINE_PADDING;

  for (auto line_index = beginning_line; line_index <= ending_line; line_index++) {
    const auto &line_span = this->details_->get_line_spans ()[line_index];

    auto line_number = line_index + 1;
    output << "  " << COLOR_RGB(std::setw (spaces_prefix.length () - 3)
                                    << std::setfill (' ')
                                    << line_number << " │  ", COLOR_GREY);

    auto descending_labels = this->find_labels_in_line (line_index);
    this->print_colored_source_line (output, *line_span, descending_labels);

    if (descending_labels.empty ()) {
      continue;
    }

    std::sort (descending_labels.begin (), descending_labels.end (), DescendingLabels ());
    this->print_descenting_labels (output, spaces_prefix, descending_labels, *line_span);
  }
}

void LabelGroup::print_descenting_labels (std::ostream &output,
                                          const std::string &spaces_prefix,
                                          const std::vector<const Label *> &labels,
                                          const Span &line_span) const {
  std::map<size_t, const Label *> mapped_labels;
  for (auto &label : labels) {
    const auto relative_span = label->get_span ().relative_to (line_span);
    mapped_labels[relative_span.get_start_index ()] = label;
  }

  output << spaces_prefix << COLOR_RGB("·  ", COLOR_GREY);

  const Label *last_label = nullptr;
  size_t last_end_index = 0u;
  for (auto index = 1u; index < line_span.get_width () + 1; index++) {
    const auto &result = mapped_labels.find (index);
    if (result == mapped_labels.end ()) {
      if (index == last_end_index) {
        COLOR_BY_TYPE(last_label->get_color (), "╯");
      } else if (index < last_end_index) {
        COLOR_BY_TYPE(last_label->get_color (), "─");
      } else {
        output << " ";
      }

      continue;
    }

    auto label = result->second;
    auto relative_span = label->get_span ().relative_to (line_span);

    if (last_end_index >= index) {
      COLOR_BY_TYPE(label->get_color (), "┤");
    } else if (relative_span.get_end_index () > index) {
      COLOR_BY_TYPE(label->get_color (), "├");
    } else {
      COLOR_BY_TYPE(label->get_color (), "│");
    }

    last_end_index = relative_span.get_end_index ();
    last_label = label;
  }

  output << "\n";

  for (const auto &label : labels) {
    output << spaces_prefix << COLOR_RGB("·  ", COLOR_GREY);

    for (auto index = 1u; index < line_span.get_width () + 1; index++) {
      const auto &result = mapped_labels.find (index);
      if (result == mapped_labels.end ()) {
        output << " ";
      } else if (result->second == label) {
        break;
      } else {
        auto color_label = result->second;
        COLOR_BY_TYPE(color_label->get_color (), "│");
      }
    }

    COLOR_BY_TYPE(label->get_color (), "╰");

    auto dash_amount = label->get_span ().get_width ();
    if (dash_amount == 0) {
      dash_amount = 1;
    }

    COLOR_BY_TYPE(label->get_color (), repeat_string ("─", dash_amount));
    COLOR_BY_TYPE(label->get_color (), "▶ ");

    print_formatted_text (output, label->get_message ());

    output << "\n";
  }
}

void LabelGroup::print_colored_source_line (std::ostream &output, const Span &label_span,
                                            const std::vector<const Label *> &labels) const {
  const auto source = this->details_->get_line_source (label_span);

  std::map<size_t, const Label *> mapped_labels;
  for (const auto &label : labels) {
    const auto &line_span = this->details_->get_line_spans ()[label->get_line ()];
    auto relative_span = label->get_span ().relative_to (*line_span);
    mapped_labels[relative_span.get_start_index () - 1] = label;
  }

  output << termcolor::color<COLOR_LIGHT_GREY>;
  for (auto char_index = 0u; char_index < source.length (); char_index++) {
    const auto &current_char = source.at (char_index);
    if (!mapped_labels.contains (char_index)) {
      output << current_char;
      continue;
    }

    const auto &label = mapped_labels.at (char_index);

    color_by_type (output, label->get_color ());
    output << current_char;

    auto starting_index = char_index;
    for (char_index++; char_index < starting_index + label->get_span ().get_width () + 1;
         char_index++) {
      if (mapped_labels.contains (char_index)) {
        break;
      }

      const auto &label_char = source.at (char_index);
      output << label_char;
    }
    char_index--;

    output << termcolor::color<COLOR_LIGHT_GREY>;
  }

  output << termcolor::reset << "\n";
}

auto LabelGroup::find_labels_in_line (size_t line_index) const -> std::vector<const Label *> {
  std::vector<const Label *> result;

  const auto &line_span = this->details_->get_line_spans ().at (line_index);
  for (const auto &item : this->labels_) {
    if (line_span->is_label_in_range (*item)) {
      result.push_back (item);
    }
  }

  return result;
}

auto LabelGroup::get_labels () const -> const std::vector<const Label *> & {
  return this->labels_;
}

auto LabelGroup::get_first_label () const -> const Label * {
  return this->first_label_;
}

auto LabelGroup::get_last_label () const -> const Label * {
  return this->last_label_;
}

auto LabelGroup::get_details () const -> Details * {
  return this->details_;
}

FileGroup::FileGroup (Details *details, std::vector<const Label *> labels)
    : details_ (details) {
  assertm(!labels.empty (), "Cannot find label current_labels if there are no labels_collection.");

  std::vector<std::vector<const Label *>> labels_collection;
  auto current_labels = &labels_collection.emplace_back ();

  auto last_line = labels.front ()->get_line ();
  for (const auto &label : labels) {
    auto label_line = label->get_line ();
    auto line_difference = label_line - last_line;
    if (line_difference > DISPLAYED_LINE_PADDING) {
      current_labels = &labels_collection.emplace_back ();
    }

    current_labels->push_back (label);
    last_line = label_line;
  }

  for (const auto &collected_labels : labels_collection) {
    this->label_groups_.emplace_back (details, collected_labels);
  }
}

void FileGroup::print (std::ostream &output, const std::string &spaces_prefix) const {
  output << COLOR_RGB("─[", COLOR_GREY)
         << COLOR_RGB(this->details_->get_path (), COLOR_WHITE)
         << COLOR_RGB("]", COLOR_GREY);
  output << "\n";

  output << spaces_prefix << COLOR_RGB("·", COLOR_GREY);
  output << "\n";

  for (auto index = 0u; index < this->label_groups_.size (); index++) {
    const auto &labels_group = this->label_groups_.at (index);
    labels_group.print (output, spaces_prefix);

    if (index != this->label_groups_.size () - 1) {
      output << spaces_prefix << COLOR_RGB("⋮", COLOR_GREY);
      output << "\n";
    }
  }
}

auto FileGroup::get_biggest_displayed_number () const -> size_t {
  auto biggest_number = 0u;
  for (const auto &labels_group : this->label_groups_) {
    auto last_label = labels_group.get_last_label ();
    auto line_number = last_label->get_line ();
    if (biggest_number < line_number) {
      biggest_number = line_number;
    }
  }

  // Add 1 to get a number starting at 1. Add the line padding to get the last line displayed by
  // the report.
  biggest_number += 1 + DISPLAYED_LINE_PADDING;

  return biggest_number;
}

auto FileGroup::get_label_groups () const -> const std::vector<LabelGroup> & {
  return this->label_groups_;
}

auto FileGroup::get_details () const -> Details * {
  return this->details_;
}

Report::Report (ReportType type, std::string message, size_t code, std::vector<Label> labels,
                std::string tip)
    : labels_ (std::move (labels)), message_ (std::move (message)), note_ (std::move (tip)),
      type_ (type), code_ (code) {}

void Report::print (std::ostream &output) const {
  output << COLOR_RGB("[" << report_type_to_prefix (this->type_)
                          << std::setw (3) << std::setfill ('0')
                          << this->code_ << "] "
                          << report_type_to_string (this->type_) << ":", COLOR_RED)
         << " " << COLOR_RGB(this->message_, COLOR_WHITE);
  output << "\n";

  auto file_groups = this->find_file_groups ();

  auto biggest_number = 0u;
  for (const auto &file_group : file_groups) {
    auto file_biggest_number = file_group.get_biggest_displayed_number ();
    if (file_biggest_number > biggest_number) {
      biggest_number = file_biggest_number;
    }
  }

  auto biggest_number_width = std::to_string (biggest_number).length ();
  auto spaces_prefix = std::string (biggest_number_width + 3, ' ');

  output << spaces_prefix << COLOR_RGB("╭", COLOR_GREY);
  for (auto index = 0u; index < file_groups.size (); index++) {
    const auto &file_group = file_groups.at (index);
    file_group.print (output, spaces_prefix);

    if (index != file_groups.size () - 1) {
      output << spaces_prefix << COLOR_RGB("·", COLOR_GREY);
      output << "\n";

      output << spaces_prefix << COLOR_RGB("├", COLOR_GREY);
    }
  }

  output << spaces_prefix << COLOR_RGB("·", COLOR_GREY);
  output << "\n";

  output << spaces_prefix << COLOR_RGB("│", COLOR_GREY)
         << COLOR_RGB("  Note: ", COLOR_BEACH);
  print_formatted_text (output, this->note_);
  output << "\n";

  auto dashes_prefix = biggest_number_width + 3;
  output << COLOR_RGB(repeat_string ("─", dashes_prefix), COLOR_GREY)
         << COLOR_RGB("╯", COLOR_GREY);
  output << "\n";
}

auto Report::find_file_groups () const -> std::vector<FileGroup> {
  std::map<Details *, std::vector<const Label *>> group_mappings;
  for (const auto &label : this->labels_) {
    auto details = label.get_span ().get_details ();
    auto result = group_mappings.find (details);
    if (result == group_mappings.end ()) {
      group_mappings[details] = {&label};
    } else {
      result->second.push_back (&label);
    }
  }

  std::vector<FileGroup> file_groups;
  for (const auto &[details, labels] : group_mappings) {
    file_groups.emplace_back (details, labels);
  }

  return file_groups;
}

auto Report::get_labels () const -> const std::vector<Label> & {
  return this->labels_;
}

auto Report::get_message () const -> const std::string & {
  return this->message_;
}

auto Report::get_note () const -> const std::string & {
  return this->note_;
}

auto Report::get_type () const -> ReportType {
  return this->type_;
}

auto Report::get_code () const -> size_t {
  return this->code_;
}
