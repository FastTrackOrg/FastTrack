///////////////////////////////////////////////////////////////////////////////
// Hungarian.cpp: Implementation file for Class HungarianAlgorithm.
//
// Uses the shortest augmenting path formulation of the Hungarian algorithm.
// Both this code and the original implementation are published under the BSD
// license.
///////////////////////////////////////////////////////////////////////////////

#include "Hungarian.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

double HungarianAlgorithm::Solve(const std::vector<double> &costs,
                                 std::size_t rows,
                                 std::size_t columns,
                                 std::vector<int> &assignment) const {
  if (rows != 0 && columns > std::numeric_limits<std::size_t>::max() / rows) {
    throw std::invalid_argument("Cost matrix dimensions overflow.");
  }
  if (costs.size() != rows * columns) {
    throw std::invalid_argument("Cost matrix dimensions do not match its data.");
  }

  assignment.assign(rows, -1);
  if (rows == 0 || columns == 0) {
    return 0.0;
  }

  for (double cost : costs) {
    if (!std::isfinite(cost)) {
      throw std::invalid_argument("Cost matrix elements must be finite.");
    }
  }

  return solveRectangular(costs, rows, columns, rows > columns, assignment);
}

double HungarianAlgorithm::Solve(const std::vector<std::vector<double>> &costs,
                                 std::vector<int> &assignment) const {
  const std::size_t rows = costs.size();
  const std::size_t columns = rows == 0 ? 0 : costs.front().size();
  std::vector<double> flatCosts;
  flatCosts.reserve(rows * columns);

  for (const auto &row : costs) {
    if (row.size() != columns) {
      throw std::invalid_argument("Cost matrix must be rectangular.");
    }
    flatCosts.insert(flatCosts.end(), row.begin(), row.end());
  }

  return Solve(flatCosts, rows, columns, assignment);
}

double HungarianAlgorithm::solveRectangular(const std::vector<double> &costs,
                                            std::size_t rows,
                                            std::size_t columns,
                                            bool transposed,
                                            std::vector<int> &assignment) const {
  // The shortest augmenting path formulation requires no more rows than
  // columns. For a tall input matrix, access its transpose without copying it.
  const std::size_t solverRows = transposed ? columns : rows;
  const std::size_t solverColumns = transposed ? rows : columns;
  const auto costAt = [&](std::size_t row, std::size_t column) {
    return transposed ? costs[column * columns + row] : costs[row * columns + column];
  };

  std::vector<double> rowPotential(solverRows + 1, 0.0);
  std::vector<double> columnPotential(solverColumns + 1, 0.0);
  std::vector<std::size_t> matchedRow(solverColumns + 1, 0);
  std::vector<std::size_t> previousColumn(solverColumns + 1, 0);
  std::vector<double> minimum(solverColumns + 1);
  std::vector<unsigned char> used(solverColumns + 1);

  for (std::size_t row = 1; row <= solverRows; ++row) {
    matchedRow[0] = row;
    std::fill(minimum.begin(), minimum.end(), std::numeric_limits<double>::infinity());
    std::fill(used.begin(), used.end(), 0);

    std::size_t currentColumn = 0;
    do {
      used[currentColumn] = 1;
      const std::size_t currentRow = matchedRow[currentColumn];
      double delta = std::numeric_limits<double>::infinity();
      std::size_t nextColumn = 0;

      for (std::size_t column = 1; column <= solverColumns; ++column) {
        if (used[column]) {
          continue;
        }

        const double reducedCost =
            costAt(currentRow - 1, column - 1) - rowPotential[currentRow] - columnPotential[column];
        if (reducedCost < minimum[column]) {
          minimum[column] = reducedCost;
          previousColumn[column] = currentColumn;
        }
        if (minimum[column] < delta) {
          delta = minimum[column];
          nextColumn = column;
        }
      }

      for (std::size_t column = 0; column <= solverColumns; ++column) {
        if (used[column]) {
          rowPotential[matchedRow[column]] += delta;
          columnPotential[column] -= delta;
        }
        else {
          minimum[column] -= delta;
        }
      }
      currentColumn = nextColumn;
    } while (matchedRow[currentColumn] != 0);

    do {
      const std::size_t precedingColumn = previousColumn[currentColumn];
      matchedRow[currentColumn] = matchedRow[precedingColumn];
      currentColumn = precedingColumn;
    } while (currentColumn != 0);
  }

  double totalCost = 0.0;
  for (std::size_t column = 1; column <= solverColumns; ++column) {
    if (matchedRow[column] == 0) {
      continue;
    }

    if (transposed) {
      const std::size_t originalRow = column - 1;
      const std::size_t originalColumn = matchedRow[column] - 1;
      assignment[originalRow] = static_cast<int>(originalColumn);
      totalCost += costs[originalRow * columns + originalColumn];
    }
    else {
      const std::size_t originalRow = matchedRow[column] - 1;
      const std::size_t originalColumn = column - 1;
      assignment[originalRow] = static_cast<int>(originalColumn);
      totalCost += costs[originalRow * columns + originalColumn];
    }
  }

  return totalCost;
}
