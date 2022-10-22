#ifndef PRETTY_ERRORS_LIBRARY_H
#define PRETTY_ERRORS_LIBRARY_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

namespace pretty_diagnostics {

enum class ReportType {
  ERROR,
  INFO,
  WARNING,
};

auto report_type_to_prefix (ReportType type) -> std::string;

auto report_type_to_string (ReportType type) -> std::string;

enum class ColorType {
  NONE,
  RED,
  BLUE,
  YELLOW,
  GREEN,
};

struct Label;

class Span {
 public:
  Span (size_t start_index, size_t end_index);

  Span ();

  [[nodiscard]]
  auto relative_to (const Span &span) const -> Span;

  [[nodiscard]]
  auto is_label_in_range (const Label &label) const -> bool;

  [[nodiscard]]
  auto get_start_index () const -> size_t;

  [[nodiscard]]
  auto get_end_index () const -> size_t;

  void set_end_index (size_t end_index);

  [[nodiscard]]
  auto get_width () const -> size_t;

 private:
  size_t start_index_;
  size_t end_index_;
};

struct Label {
  std::string message_;
  Span span_;
  ColorType color_type_;
};

struct AscendingLabels {
  bool operator() (const Label *first, const Label *second) const;
};

struct DescendingLabels {
  bool operator() (const Label *first, const Label *second) const;
};

struct LabelExtremsResult {
  size_t first_line_number_;
  size_t last_line_number_;
};

class Details {
 public:
  Details (std::string source, std::string path);

  [[nodiscard]]
  auto get_line_source (const Span &span) -> std::string;

  [[nodiscard]]
  auto get_label_line (const Label &label) -> size_t;

  void find_line_spans ();

  [[nodiscard]]
  auto get_line_spans () const -> const std::vector<std::shared_ptr<Span>> &;

  [[nodiscard]]
  auto get_path () const -> const std::string &;

 private:
  std::vector<std::shared_ptr<Span>> line_spans_;
  std::string source_;
  std::string path_;
};

class Report {
 public:
  Report (Details *details, ReportType type, std::string message, size_t code, std::vector<
      Label> labels, std::string tip);

  void print ();

  [[nodiscard]]
  auto find_label_extrems () -> LabelExtremsResult;

  [[nodiscard]]
  auto find_first_label_in_line (size_t line_index) -> Label *;

  [[nodiscard]]
  auto find_labels_in_line (size_t line_index) -> std::vector<Label *>;

  void print_descenting_labels (const std::string &number_spaces, const std::vector<Label *> &labels,
                                const Span &line_span);

 private:
  Details *details_;
  ReportType type_;

  std::string message_;
  size_t code_;

  std::vector<Label> labels_;
  std::string tip_;
};

}

#endif //PRETTY_ERRORS_LIBRARY_H
