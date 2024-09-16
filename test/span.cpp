#include "gtest/gtest.h"

#include "pretty_diagnostics/span.h"

TEST(Span, Valid) {
    EXPECT_NO_THROW(Span({0, 0}, {0, 42}));
}

TEST(Span, Invalid) {
    EXPECT_THROW(Span({0, 0}, {1, 0}), std::invalid_argument);

    EXPECT_THROW(Span({1, 0}, {0, 0}), std::invalid_argument);
    EXPECT_THROW(Span({0, 1}, {0, 0}), std::invalid_argument);
    EXPECT_THROW(Span({1, 1}, {0, 0}), std::invalid_argument);

    EXPECT_THROW(Span({0, 0}, {0, 0}), std::invalid_argument);
    EXPECT_THROW(Span({0, 1}, {0, 1}), std::invalid_argument);
    EXPECT_THROW(Span({1, 0}, {1, 0}), std::invalid_argument);
    EXPECT_THROW(Span({1, 1}, {1, 1}), std::invalid_argument);
}

TEST(Span, Width) {
    EXPECT_EQ(Span({0, 0}, {0, 1}).column_width(), 1);
    EXPECT_EQ(Span({0, 0}, {0, 42}).column_width(), 42);
}