# FITexcel
Backend of spreadsheet processor. Classes provide options to set, edit, copy, evaluate(count, sum...) cells and to save/load spreadsheets.

CSpreadsheet - spreadsheet processor itself
  - save -> saves spreadsheet to file and creates checksum
  - load -> reads checksum thne loads spreadsheet from file or throws error
  - setCell -> edits content of cell
  - getValue -> reads current cell value (checks cyclic dependencies of cells an respectively throws undefined value)
  - copyRect -> copies range of cells from source to destination

CPos - Cell address
  - used to convert address to more convenient internally used format B2 -> 1,2

CValue - value stored in cells
  - Text
  - Float number
  - Undefined value (reference to non-existent cell, math error...)

Numeric functions - used with setCell
  - sum (range) - returns sum of cells containing double values within given range, no numeric cell -> undefined value
  - count (range) - returns count of non-empty cells within given range
  - max (range) - returns maximum value found within given range, no numeric cell -> undefined value
  - min (range) - returns minimum value found within given range, no numeric cell -> undefined value
  - countval (range) - returns count of cells matching given value within given range
  - if (cond, ifTrue, ifFalse) - evaluates condition and respectively return ifTrue/ifFalse, cond != 0 -> true, otherwise false

CellValue, Postfixexpressionbuilder, stackoperators_custom are classes or family of polymorphic classes internally used to parse input values, calculate them etc. Code is full of comments that should give the reader an idea of the principle.

make_all_in_one.sh - script for merging the code to one file for easier compilation on your machine without Cmake if needed. (expression.h library of expression parser still must be linked)

EXAMPLES OF INPUT SYNTAX AND CODE USAGE CAN BE FOUND IN "test.cpp"
