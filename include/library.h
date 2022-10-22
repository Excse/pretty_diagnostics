#ifndef PRETTY_ERRORS_LIBRARY_H
#define PRETTY_ERRORS_LIBRARY_H

#include <cstdint>
#include <cassert>
#include <string>
#include <vector>
#include <memory>

#include <termcolor/termcolor.hpp>

#define COLOR_RED 228, 38, 103
#define COLOR_GREEN 175, 255, 95
#define COLOR_WHITE 220, 238, 235
#define COLOR_GREY 148, 148, 148
#define COLOR_BEACH 125, 199, 164
#define COLOR_LIGHT_GREY 170, 173, 176

#define COLOR_RGB(text, rgb) termcolor::color<rgb> << text << termcolor::reset

#define COLOR_TEXT_RED(text) COLOR_RGB(text, COLOR_RED)
#define COLOR_TEXT_WHITE(text) COLOR_RGB(text, COLOR_WHITE)
#define COLOR_TEXT_GREY(text) COLOR_RGB(text, COLOR_GREY)
#define COLOR_TEXT_BEACH(text) COLOR_RGB(text, COLOR_BEACH)
#define COLOR_TEXT_LIGHT_GREY(text) COLOR_RGB(text, COLOR_LIGHT_GREY)

#define COLOR_BY_TYPE(type, text) color_by_type (std::cout, type) << text << termcolor::reset

#define assertm(exp, msg) assert(((void)msg, exp))

#define LINE_PADDING 1

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
};

auto color_by_type (std::ostream &stream, ColorType type) -> std::ostream &;

struct Label;

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

class Details;

class Label {
 public:
  Label (const std::string &message, const Span &span, ColorType color_type);

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
  Span span_;
  ColorType color_;
  size_t line_;
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
  LabelGroup (Details *general_details, std::vector<Label *> labels);

  void print (const std::string &spaces_prefix) const;

  void print_descenting_labels (const std::string &spaces_prefix,
                                const std::vector<Label *> &labels,
                                const Span &line_span) const;

  [[nodiscard]]
  auto find_labels_in_line (size_t line_index) const -> std::vector<Label *>;

  [[nodiscard]]
  auto get_last_label () const -> Label *;

 private:
  std::vector<Label *> labels_;
  Details *general_details_;
  Label *first_label_;
  Label *last_label_;
};

class FileGroup {
 public:
  FileGroup (Details *details, std::vector<Label *> labels);

  void print (const std::string &spaces_prefix);

  [[nodiscard]]
  auto get_biggest_displayed_number () -> size_t;

 private:
  std::vector<LabelGroup> label_groups_;
  Details *details_;
};

class Report {
 public:
  Report (ReportType type, std::string message, size_t code, std::vector<Label> labels,
          std::string tip);

  void print ();

  [[nodiscard]]
  auto find_file_groups () -> std::vector<FileGroup>;

 private:
  ReportType type_;

  std::string message_;
  size_t code_;

  std::vector<Label> labels_;
  std::string note_;
};

}

#endif //PRETTY_ERRORS_LIBRARY_H
