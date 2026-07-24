#include "../src/Hungarian.h"

#include <algorithm>
#include <functional>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "gtest/gtest.h"

namespace {
double bruteForceCost(const std::vector<double> &costs, std::size_t rows, std::size_t columns) {
  const std::size_t targetAssignments = std::min(rows, columns);
  std::vector<unsigned char> used(columns, 0);
  double best = std::numeric_limits<double>::infinity();

  std::function<void(std::size_t, std::size_t, double)> search =
      [&](std::size_t row, std::size_t assigned, double cost) {
        if (assigned + rows - row < targetAssignments || cost >= best) {
          return;
        }
        if (row == rows) {
          if (assigned == targetAssignments) {
            best = cost;
          }
          return;
        }

        if (rows > columns) {
          search(row + 1, assigned, cost);
        }
        for (std::size_t column = 0; column < columns; ++column) {
          if (!used[column]) {
            used[column] = 1;
            search(row + 1, assigned + 1, cost + costs[row * columns + column]);
            used[column] = 0;
          }
        }
      };

  search(0, 0, 0.0);
  return best;
}
}  // namespace

TEST(HungarianAlgorithmTest, SolvesSquareMatrix) {
  const std::vector<double> costs{
      82, 83, 69, 92,
      77, 37, 49, 92,
      11, 69, 5, 86,
      8, 9, 98, 23,
  };
  std::vector<int> assignment;

  const double cost = HungarianAlgorithm().Solve(costs, 4, 4, assignment);

  EXPECT_DOUBLE_EQ(cost, 140.0);
  EXPECT_EQ(assignment, (std::vector<int>{2, 1, 0, 3}));
}

TEST(HungarianAlgorithmTest, SolvesWideMatrix) {
  const std::vector<std::vector<double>> costs{
      {4, 1, 3},
      {2, 0, 5},
  };
  std::vector<int> assignment;

  const double cost = HungarianAlgorithm().Solve(costs, assignment);

  EXPECT_DOUBLE_EQ(cost, 3.0);
  ASSERT_EQ(assignment.size(), 2u);
  EXPECT_NE(assignment[0], assignment[1]);
}

TEST(HungarianAlgorithmTest, LeavesRowsUnassignedInTallMatrix) {
  const std::vector<double> costs{
      1, 9,
      2, 3,
      9, 1,
  };
  std::vector<int> assignment;

  const double cost = HungarianAlgorithm().Solve(costs, 3, 2, assignment);

  EXPECT_DOUBLE_EQ(cost, 2.0);
  EXPECT_EQ(assignment, (std::vector<int>{0, -1, 1}));
}

TEST(HungarianAlgorithmTest, HandlesEmptyDimensions) {
  std::vector<int> assignment;

  EXPECT_DOUBLE_EQ(HungarianAlgorithm().Solve({}, 0, 0, assignment), 0.0);
  EXPECT_TRUE(assignment.empty());
  EXPECT_DOUBLE_EQ(HungarianAlgorithm().Solve({}, 3, 0, assignment), 0.0);
  EXPECT_EQ(assignment, (std::vector<int>{-1, -1, -1}));
}

TEST(HungarianAlgorithmTest, RejectsMalformedMatrices) {
  std::vector<int> assignment;

  EXPECT_THROW(HungarianAlgorithm().Solve({1.0}, 2, 2, assignment), std::invalid_argument);
  EXPECT_THROW(HungarianAlgorithm().Solve({{1.0}, {1.0, 2.0}}, assignment), std::invalid_argument);
}

TEST(HungarianAlgorithmTest, MatchesBruteForceForSmallRectangularMatrices) {
  std::mt19937 generator(42);
  std::uniform_int_distribution<int> costDistribution(0, 100);

  for (std::size_t rows = 1; rows <= 5; ++rows) {
    for (std::size_t columns = 1; columns <= 5; ++columns) {
      for (int sample = 0; sample < 20; ++sample) {
        std::vector<double> costs(rows * columns);
        std::generate(costs.begin(), costs.end(), [&] {
          return static_cast<double>(costDistribution(generator));
        });
        std::vector<int> assignment;

        const double actual = HungarianAlgorithm().Solve(costs, rows, columns, assignment);

        EXPECT_DOUBLE_EQ(actual, bruteForceCost(costs, rows, columns))
            << "rows=" << rows << ", columns=" << columns << ", sample=" << sample;
      }
    }
  }
}
