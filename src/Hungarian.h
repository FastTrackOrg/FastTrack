///////////////////////////////////////////////////////////////////////////////
// Hungarian.h: Header file for Class HungarianAlgorithm.
//
// This is a C++ wrapper with slight modification of a hungarian algorithm
// implementation by Markus Buehren.
//
// Both this code and the original code are published under the BSD license.
///////////////////////////////////////////////////////////////////////////////

#ifndef HUNGARIAN_H
#define HUNGARIAN_H

#include <cstddef>
#include <vector>

class HungarianAlgorithm {
 public:
  // Costs are stored in row-major order. Unassigned rows are reported as -1.
  double Solve(const std::vector<double> &costs, std::size_t rows, std::size_t columns, std::vector<int> &assignment) const;

  // Compatibility overload for callers using a vector of rows.
  double Solve(const std::vector<std::vector<double>> &costs, std::vector<int> &assignment) const;

 private:
  double solveRectangular(const std::vector<double> &costs, std::size_t rows, std::size_t columns, bool transposed, std::vector<int> &assignment) const;
};

#endif
