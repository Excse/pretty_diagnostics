#ifndef PRETTY_ERRORS_LIBRARY_H
#define PRETTY_ERRORS_LIBRARY_H

#include <cstdint>
#include <cassert>
#include <string>
#include <vector>
#include <memory>

#include <termcolor/termcolor.hpp>

#include "config.h"

#define COLOR_BY_TYPE(type, text) color_by_type (std::cout, type) << text << termcolor::reset
#define COLOR_RGB(text, rgb) termcolor::color<rgb> << text << termcolor::reset

#define assertm(exp, msg) assert(((void)msg, exp))

namespace pretty_diagnostics {

enum class ReportType {
  ERROR,
  INFO,
  WARNING,
};

auto report_type_to_prefix (ReportType type) -> std::string;

auto report_type_to_string (ReportType type) -> std::string;

enum class ColorType {
  DEFAULT,
  RED,
  GREEN,
  BLUE,
  ORANGE,
  YELLOW,
  AQUA
};

auto color_by_type (std::ostream &stream, ColorType type) -> std::ostream &;

class Label;

class Details;

class Span {
 public:
  Span (Details *details, size_t start_index, size_t end_index);

  Span ();

  [[nodiscard]]
  auto relative_to (const Span &span) const -> Span;

  [[nodiscard]]
  auto is_label_in_range (const Label &label) const -> bool;

  [[nodiscard]]
  auto get_start_index () const -> size_t;

  [[nodiscard]]
  auto get_details () const -> Details *;

  [[nodiscard]]
  auto get_end_index () const -> size_t;

  void set_end_index (size_t end_index);

  [[nodiscard]]
  auto get_width () const -> size_t;

 private:
  size_t start_index_;
  Details *details_;
  size_t end_index_;
};

class Label {
 public:
  Label (std::string message, const Span &span, ColorType color_type);

  [[nodiscard]]
  auto get_message () const -> const std::string &;

  [[nodiscard]]
  auto get_span () const -> const Span &;

  [[nodiscard]]
  auto get_color () const -> ColorType;

  [[nodiscard]]
  auto get_line () const -> size_t;

 private:
  std::string message_;
  ColorType color_;
  size_t line_;
  Span span_;
};

struct AscendingLabels {
  bool operator() (const Label *first, const Label *second) const;
};

struct DescendingLabels {
  bool operator() (const Label *first, const Label *second) const;
};

class Details {
 public:
  Details (std::string source, std::string path);

  [[nodiscard]]
  auto get_line_source (const Span &span) const -> std::string;

  [[nodiscard]]
  auto get_label_line (const Label &label) const -> size_t;

  [[nodiscard]]
  auto get_line_spans () const -> const std::vector<std::shared_ptr<Span>> &;

  [[nodiscard]]
  auto get_path () const -> const std::string &;

 private:
  std::vector<std::shared_ptr<Span>> line_spans_;
  std::string source_;
  std::string path_;
};

class LabelGroup {
 public:
  LabelGroup (Details *general_details, std::vector<const Label *> labels);

  void print (std::ostream &output, const std::string &spaces_prefix) const;

  void print_descenting_labels (std::ostream &output,
                                const std::string &spaces_prefix,
                                const std::vector<const Label *> &labels,
                                const Span &line_span) const;

  void print_colored_source_line (std::ostream &output, const Span &label_span,
                                  const std::vector<const Label *> &labels) const;

  [[nodiscard]]
  auto find_labels_in_line (size_t line_index) const -> std::vector<const Label *>;

  [[nodiscard]]
  auto get_last_label () const -> const Label *;

 private:
  std::vector<const Label *> labels_;
  const Label *first_label_;
  const Label *last_label_;
  Details *details_;
};

class FileGroup {
 public:
  FileGroup (Details *details, std::vector<const Label *> labels);

  void print (std::ostream &output, const std::string &spaces_prefix) const;

  [[nodiscard]]
  auto get_biggest_displayed_number () const -> size_t;

 private:
  std::vector<LabelGroup> label_groups_;
  Details *details_;
};

class Report {
 public:
  Report (ReportType type, std::string message, size_t code, std::vector<Label> labels,
          std::string tip);

  void print (std::ostream &output) const;

  [[nodiscard]]
  auto find_file_groups () const -> std::vector<FileGroup>;

 private:
  std::vector<Label> labels_;
  std::string message_;
  std::string note_;
  ReportType type_;
  size_t code_;
};

}

#endif //PRETTY_ERRORS_LIBRARY_H
