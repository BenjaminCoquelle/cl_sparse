#ifndef MATRIX_TYPES_H
#define MATRIX_TYPES_H

#include <string>

// Matrix indexing
#define DENSE_IND(ai,aj,nrow,ncol) ai + aj*nrow
//#define DENSE_IND(ai,aj,nrow,ncol) aj + ai*nol

#define ELL_IND(row, el , nrow, max_row) el*nrow + row
//#define ELL_IND(row, el , nrow, max_row) el + max_row*row

#define DIA_IND(row, el, nrow, ndiag) el*nrow + row
//#define DIA_IND(row, el, nrow, ndiag) el + ndiag*row


/// Matrix Names
const std::string _matrix_format_names [4] = {"COO",
                                              "CSR",
                                              "ELL",
                                              "HYB"
};

/// Matrix Enumeration
enum _matrix_format {COO   = 0,
                     CSR   = 1,
                     ELL   = 2,
                     HYB   = 3};


/// Sparse Matrix - Coordinate Format
template <typename ValueType, typename IndexType>
struct s_MatrixCOO {
  /// Row index
  IndexType *row;

  /// Column index
  IndexType *col;

  // Values
  ValueType *val;
};

/// Sparse Matrix - Coordinate Format
template <typename ValueType, typename IndexType>
struct s_gMatrixCOO {
  /// Row index
  IndexType row;

  /// Column index
  IndexType col;

  // Values
  ValueType val;
};

/// Sparse Matrix - Sparse Compressed Row Format
template <typename ValueType, typename IndexType>
struct s_MatrixCSR {
  /// Row offsets (row ptr)
  IndexType *row_offset;

  /// Column index
  IndexType *col;

  /// Values
  ValueType *val;
};

/// Sparse Matrix - Sparse Compressed Row Format
template <typename ValueType, typename IndexType>
struct s_gMatrixCSR {
  /// Row offsets (row ptr)
  IndexType row_offset;

  /// Column index
  IndexType col;

  /// Values
  ValueType val;
};



/// Sparse Matrix - ELL Format (see ELL_IND for indexing)
template <typename ValueType, typename IndexType>
struct s_MatrixELL {
  /// Maximal elements per row
  int max_row;

  /// Column index
  IndexType *col;

  /// Values
  ValueType *val;
};

/// Sparse Matrix - ELL Format (see ELL_IND for indexing)
template <typename ValueType, typename IndexType>
struct s_gMatrixELL {
  /// Maximal elements per row
  int max_row;

  /// Column index
  IndexType col;

  /// Values
  ValueType val;
};

/// Sparse Matrix - Contains ELL and COO Matrices
template <typename ValueType, typename IndexType, typename Index = IndexType>
struct s_MatrixHYB {
  s_MatrixELL<ValueType, IndexType> ELL;
  s_MatrixCOO<ValueType, IndexType> COO;
};

#endif // MATRIX_TYPES_H
