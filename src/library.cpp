#include "library.h"

#include <iostream>
#include <iomanip>
#include <utility>
#include <ranges>
#include <regex>
#include <map>

using namespace pretty_diagnostics;

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

Span::Span () : end_index_ (), start_index_ (), details_ () {}

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
    : message_ (std::move (message)), span_ (span), color_ (color_type) {
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
  auto result = this->source_.substr (span.get_start_index (),
                                      span.get_end_index () - span.get_start_index ());
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

auto Details::get_path () const -> const std::string & {
  return this->path_;
}

LabelGroup::LabelGroup (Details *general_details_, std::vector<Label *> labels)
    : labels_ (std::move (labels)), general_details_ (general_details_), first_label_ (),
      last_label_ () {
  assertm(!this->labels_.empty (), "Couldn't find the last labels as there are no labels.");

  auto ascending_labels (this->labels_);
  std::sort (ascending_labels.begin (), ascending_labels.end (), AscendingLabels ());

  this->first_label_ = ascending_labels.front ();
  this->last_label_ = ascending_labels.back ();
}

void LabelGroup::print (const std::string &spaces_prefix) const {
  auto first_line = this->first_label_->get_line ();
  auto last_line = this->last_label_->get_line ();

  auto beginning_line = first_line - LINE_PADDING;
  auto ending_line = last_line + LINE_PADDING;

  for (auto line_index = beginning_line; line_index <= ending_line; line_index++) {
    auto line_span = this->general_details_->get_line_spans ()[line_index];
    auto line_number = line_index + 1;
    std::cout << "  " << COLOR_TEXT_GREY(std::setw (spaces_prefix.length () - 3)
                                             << std::setfill (' ')
                                             << line_number << " │  ");

    auto line_source = this->general_details_->get_line_source (*line_span);
    auto descending_line_labels = this->find_labels_in_line (line_index);

    auto color_line = this->color_source_line_using_labels (line_source, descending_line_labels);
    std::cout << color_line << std::endl;

    if (descending_line_labels.empty ()) {
      continue;
    }

    std::sort (descending_line_labels.begin (), descending_line_labels.end (), DescendingLabels ());
    this->print_descenting_labels (spaces_prefix, descending_line_labels, *line_span);
  }
}

void LabelGroup::print_descenting_labels (const std::string &spaces_prefix,
                                          const std::vector<Label *> &labels,
                                          const Span &line_span) const {
  std::map<size_t, Label *> mapped_labels;
  for (auto &label : labels) {
    auto relative_span = label->get_span ().relative_to (line_span);
    mapped_labels[relative_span.get_start_index ()] = label;
  }

  std::cout << spaces_prefix << COLOR_TEXT_GREY("·  ");
  size_t last_label_ending = 0u;
  Label *last_label = nullptr;
  for (auto index = 1u; index < line_span.get_width () + 1; index++) {
    auto result = mapped_labels.find (index);
    if (result == mapped_labels.end ()) {
      if (index == last_label_ending) {
        COLOR_BY_TYPE(last_label->get_color (), "╯");
      } else if (index < last_label_ending) {
        COLOR_BY_TYPE(last_label->get_color (), "─");
      } else {
        std::cout << " ";
      }

      continue;
    }

    auto label = result->second;
    auto relative_span = label->get_span ().relative_to (line_span);

    if (last_label_ending >= index) {
      COLOR_BY_TYPE(label->get_color (), "┤");
    } else if (relative_span.get_end_index () > index) {
      COLOR_BY_TYPE(label->get_color (), "├");
    } else {
      COLOR_BY_TYPE(label->get_color (), "│");
    }

    last_label_ending = relative_span.get_end_index ();
    last_label = label;
  }

  std::cout << std::endl;

  for (auto label : labels) {
    std::cout << spaces_prefix << COLOR_TEXT_GREY("·  ");

    for (auto index = 1u; index < line_span.get_width () + 1; index++) {
      auto result = mapped_labels.find (index);
      if (result == mapped_labels.end ()) {
        std::cout << " ";
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

    while (dash_amount--) {
      COLOR_BY_TYPE(label->get_color (), "─");
    }

    COLOR_BY_TYPE(label->get_color (), "▶ ") << COLOR_TEXT_WHITE(label->get_message ())
                                             << std::endl;
  }
}

auto LabelGroup::find_labels_in_line (size_t line_index) const -> std::vector<Label *> {
  std::vector<Label *> result;

  auto line_span = this->general_details_->get_line_spans ().at (line_index);
  for (auto &item : this->labels_) {
    if (line_span->is_label_in_range (*item)) {
      result.push_back (item);
    }
  }

  return result;
}

auto LabelGroup::get_last_label () const -> Label * {
  return this->last_label_;
}
auto LabelGroup::color_source_line_using_labels (
    const std::string &source, const std::vector<Label *> &labels) const -> std::string {
  std::map<size_t, Label *> mapped_labels;
  for (const auto &label : labels) {
    auto &line_span = this->general_details_->get_line_spans ()[label->get_line ()];
    auto relative_span = label->get_span ().relative_to (*line_span);
    mapped_labels[relative_span.get_start_index () - 1] = label;
  }

  std::ostringstream output;

  // Needs to be done as termcolor checks the stream if the colorized flag is set.
  auto is_cout_tty = termcolor::_internal::is_atty (std::cout);
  output.iword (termcolor::_internal::colorize_index ()) = is_cout_tty;

  output << termcolor::color<COLOR_LIGHT_GREY>;
  for (auto char_index = 0u; char_index < source.length (); char_index++) {
    auto current_char = source.at (char_index);
    if (!mapped_labels.contains (char_index)) {
      output << current_char;
      continue;
    }

    auto label = mapped_labels.at (char_index);

    color_by_type (output, label->get_color ());
    output << current_char;

    auto starting_index = char_index;
    for (char_index++; char_index < starting_index + label->get_span ().get_width () + 1;
         char_index++) {
      if (mapped_labels.contains (char_index)) {
        break;
      }

      auto label_char = source.at (char_index);
      output << label_char;
    }
    char_index--;

    output << termcolor::color<COLOR_LIGHT_GREY>;
  }
  output << termcolor::reset;

  return output.str ();
}

FileGroup::FileGroup (Details *details, std::vector<Label *> labels)
    : details_ (details) {
  assertm(!labels.empty (), "Cannot find label current_labels if there are no labels_collection.");

  std::vector<std::vector<Label *>> labels_collection;
  auto current_labels = &labels_collection.emplace_back ();

  size_t last_label_line = labels.at (0)->get_line ();
  for (auto &label : labels) {
    auto label_line = label->get_line ();
    auto line_difference = label_line - last_label_line;
    if (line_difference > LINE_PADDING) {
      current_labels = &labels_collection.emplace_back ();
    }

    current_labels->push_back (label);
    last_label_line = label_line;
  }

  for (const auto &collected_labels : labels_collection) {
    this->label_groups_.emplace_back (details, collected_labels);
  }
}

void FileGroup::print (const std::string &spaces_prefix) {
  std::cout << COLOR_TEXT_GREY("─[")
            << COLOR_TEXT_WHITE(this->details_->get_path ())
            << COLOR_TEXT_GREY("]") << std::endl;
  std::cout << spaces_prefix << COLOR_TEXT_GREY("·") << std::endl;

  for (auto index = 0u; index < this->label_groups_.size (); index++) {
    auto &labels_group = this->label_groups_.at (index);
    labels_group.print (spaces_prefix);

    if (index != this->label_groups_.size () - 1) {
      std::cout << spaces_prefix << COLOR_TEXT_GREY("⋮") << std::endl;
    }
  }
}

auto FileGroup::get_biggest_displayed_number () -> size_t {
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
  biggest_number += 1 + LINE_PADDING;

  return biggest_number;
}

Report::Report (ReportType type, std::string message, size_t code, std::vector<Label> labels,
                std::string tip)
    : type_ (type), message_ (std::move (message)), code_ (code), labels_ (std::move (labels)),
      note_ (std::move (tip)) {}

void Report::print () {
  std::cout << COLOR_TEXT_RED("[" << report_type_to_prefix (this->type_)
                                  << std::setw (3) << std::setfill ('0')
                                  << this->code_ << "] "
                                  << report_type_to_string (this->type_) << ":")
            << " " << COLOR_TEXT_WHITE(this->message_)
            << std::endl;

  auto file_groups = this->find_file_groups ();

  auto biggest_number = 0u;
  for (auto &file_group : file_groups) {
    auto file_biggest_number = file_group.get_biggest_displayed_number ();
    if (file_biggest_number > biggest_number) {
      biggest_number = file_biggest_number;
    }
  }

  auto biggest_number_width = std::to_string (biggest_number).length ();
  auto spaces_prefix = std::string (biggest_number_width + 3, ' ');

  std::cout << spaces_prefix << COLOR_TEXT_GREY("╭");
  for (auto index = 0u; index < file_groups.size (); index++) {
    auto &file_group = file_groups.at (index);
    file_group.print (spaces_prefix);

    if (index != file_groups.size () - 1) {
      std::cout << spaces_prefix << COLOR_TEXT_GREY("·") << std::endl;
      std::cout << spaces_prefix << COLOR_TEXT_GREY("├");
    }
  }

  std::cout << spaces_prefix << COLOR_TEXT_GREY("·") << std::endl;
  std::cout << spaces_prefix << COLOR_TEXT_GREY("│")
            << COLOR_TEXT_BEACH("  Note: ")
            << COLOR_TEXT_WHITE(this->note_)
            << std::endl;

  auto dashes_prefix = biggest_number_width + 3;
  while (dashes_prefix--) {
    std::cout << COLOR_TEXT_GREY("─");
  }
  std::cout << COLOR_TEXT_GREY("╯") << std::endl;
}

auto Report::find_file_groups () -> std::vector<FileGroup> {
  std::map<Details *, std::vector<Label *>> group_mappings;
  for (auto &label : this->labels_) {
    auto details = label.get_span ().get_details ();
    auto result = group_mappings.find (details);
    if (result == group_mappings.end ()) {
      group_mappings[details] = {&label};
    } else {
      result->second.push_back (&label);
    }
  }

  std::vector<FileGroup> file_groups;
  for (auto &[details, labels] : group_mappings) {
    file_groups.emplace_back (details, labels);
  }

  return file_groups;
}
