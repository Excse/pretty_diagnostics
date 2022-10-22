#include "library.h"

#include <iostream>
#include <cassert>
#include <iomanip>
#include <utility>
#include <map>

#define assertm(exp, msg) assert(((void)msg, exp))

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
  case ReportType::ERROR: return "ERROR";
  case ReportType::INFO: return "INFO";
  case ReportType::WARNING: return "WARNING";
  default: assertm(false, "This report type is not implemented yet.");
  }
}

bool AscendingLabels::operator() (const Label *first, const Label *second) const {
  auto difference = (int)first->span_.get_start_index () - (int)second->span_.get_start_index ();
  if (difference == 0) {
    return first->span_.get_end_index () < second->span_.get_end_index ();
  }

  return difference < 0;
}

bool DescendingLabels::operator() (const Label *first, const Label *second) const {
  auto difference = (int)first->span_.get_end_index () - (int)second->span_.get_end_index ();
  if (difference == 0) {
    return first->span_.get_start_index () > second->span_.get_start_index ();
  }

  return difference > 0;
}

Details::Details (std::string source, std::string path)
    : line_spans_ (), source_ (std::move (source)), path_ (std::move (path)) {}

auto Details::get_line_source (const Span &span) -> std::string {
  return this->source_.substr (span.get_start_index (),
                               span.get_end_index () - span.get_start_index ());
}

void Details::find_line_spans () {
  Span *current_span = nullptr;
  for (auto index = 0u; index < this->source_.size (); index++) {
    if (current_span == nullptr) {
      auto new_span = std::make_shared<Span> (index, index);
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

auto Details::get_label_line (const Label &label) -> size_t {
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

Report::Report (Details *details, ReportType type, std::string message, size_t code,
                std::vector<Label> labels, std::string tip)
    : details_ (details), type_ (type), message_ (std::move (message)), code_ (code),
      labels_ (std::move (labels)), tip_ (std::move (tip)) {}

#define LINE_PADDING 1

void Report::print () {
  std::cout << "[" << report_type_to_prefix (this->type_)
            << std::setw (3) << std::setfill ('0') << this->code_ << "] ";
  std::cout << report_type_to_string (this->type_) << ": ";
  std::cout << this->message_ << std::endl;

  auto [first_line_number, last_line_number] = this->find_label_extrems ();

  auto beginning_line = first_line_number - LINE_PADDING;
  auto first_label = this->find_first_label_in_line (first_line_number);
  auto first_line_span = this->details_->get_line_spans ().at (first_line_number);
  auto relative_first_span = first_label->span_.relative_to (*first_line_span);
  auto first_line = first_line_number + 1;

  auto last_line = last_line_number + LINE_PADDING;
  auto number_space_width = std::to_string (last_line + 1).length ();
  auto number_spaces = std::string (number_space_width + 3, ' ');

  std::cout << number_spaces << "╭─ [" << this->details_->get_path () << ":" << first_line << ":"
            << relative_first_span.get_start_index () << "]" << std::endl;
  std::cout << number_spaces << "·" << std::endl;

  for (auto line_index = beginning_line; line_index <= last_line; line_index++) {
    auto line_span = this->details_->get_line_spans ()[line_index];
    auto line_number = line_index + 1;
    std::cout << "  " << std::setw ((int)number_space_width)
              << std::setfill (' ') << line_number << " │  ";

    auto line_source = this->details_->get_line_source (*line_span);
    std::cout << line_source << std::endl;

    auto descending_line_labels = this->find_labels_in_line (line_index);
    if (descending_line_labels.empty ()) {
      continue;
    }

    std::sort (descending_line_labels.begin (), descending_line_labels.end (), DescendingLabels ());
    this->print_descenting_labels (number_spaces, descending_line_labels, *line_span);
  }

  std::cout << number_spaces << "·" << std::endl;
  std::cout << number_spaces << "│  Tip: " << this->tip_ << std::endl;
  std::cout << "─────╯" << std::endl;
}

void Report::print_descenting_labels (const std::string &number_spaces,
                                      const std::vector<Label *> &labels,
                                      const Span &line_span) {
  std::map<size_t, Label *> relative_start_indicies;
  for (auto &label : labels) {
    auto relative_span = label->span_.relative_to (line_span);
    relative_start_indicies[relative_span.get_start_index ()] = label;
  }

  std::cout << number_spaces << "·  ";
  size_t last_label_ending = 0u;
  for (auto index = 1u; index < line_span.get_width () + 1; index++) {
    auto result = relative_start_indicies.find (index);
    if (result == relative_start_indicies.end ()) {
      if (index == last_label_ending) {
        std::cout << "╯";
      } else if (index < last_label_ending) {
        std::cout << "─";
      } else {
        std::cout << " ";
      }

      continue;
    }

    auto relative_span = result->second->span_.relative_to (line_span);
    if (last_label_ending >= index) {
      std::cout << "┤";
    } else if (relative_span.get_end_index () > index) {
      std::cout << "├";
    } else {
      std::cout << "│";
    }

    last_label_ending = relative_span.get_end_index ();
  }

  std::cout << std::endl;

  for (auto label_index = 0u; label_index < labels.size (); label_index++) {
    std::cout << number_spaces << "·  ";

    auto label = labels.at (label_index);
    for (auto index = 1u; index < line_span.get_width () + 1; index++) {
      auto result = relative_start_indicies.find (index);
      if (result == relative_start_indicies.end ()) {
        std::cout << " ";
      } else if (result->second == label) {
        break;
      } else {
        std::cout << "│";
      }
    }

    std::cout << "╰";

    auto dash_amount = label->span_.get_width ();
    if (dash_amount == 0) {
      dash_amount = 1;
    }

    while (dash_amount--) {
      std::cout << "─";
    }

    std::cout << "▶ " << label->message_ << std::endl;
  }
}

auto Report::find_label_extrems () -> LabelExtremsResult {
  assertm(!this->labels_.empty (), "Couldn't find the last labels as there are no labels.");

  std::vector<Label *> sorted_labels;
  for (auto &label : this->labels_) {
    sorted_labels.push_back (&label);
  }

  std::sort (sorted_labels.begin (), sorted_labels.end (), AscendingLabels ());

  auto first_label = sorted_labels.front ();
  auto first_line = this->details_->get_label_line (*first_label);

  auto last_label = sorted_labels.back ();
  auto last_line = this->details_->get_label_line (*last_label);

  return {
      .first_line_number_ = first_line,
      .last_line_number_ = last_line,
  };
}

auto Report::find_first_label_in_line (size_t line_index) -> Label * {
  auto labels = this->find_labels_in_line (line_index);
  if (labels.empty ()) {
    assertm(false, "Couldn't find the first label with this line index.");
  }

  std::sort (labels.begin (), labels.end (), AscendingLabels ());
  return labels.front ();
}

auto Report::find_labels_in_line (size_t line_index) -> std::vector<Label *> {
  std::vector<Label *> result;

  auto line_span = this->details_->get_line_spans ().at (line_index);
  for (auto &item : this->labels_) {
    if (line_span->is_label_in_range (item)) {
      result.push_back (&item);
    }
  }

  return result;
}

Span::Span (size_t start_index, size_t end_index)
    : start_index_ (start_index), end_index_ (end_index) {}

Span::Span () : end_index_ (), start_index_ () {}

auto Span::relative_to (const Span &span) const -> Span {
  return Span (this->start_index_ - span.start_index_,
               this->end_index_ - span.start_index_);
}

auto Span::is_label_in_range (const Label &label) const -> bool {
  return (this->start_index_ <= label.span_.start_index_)
         && (this->end_index_ >= label.span_.end_index_);
}

auto Span::get_start_index () const -> size_t {
  return this->start_index_;
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
