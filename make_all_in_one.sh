grep -vh '^#include' CPos.hpp constantexpressions.hpp HashForPair.hpp CellValue.hpp stackoperators_custom.hpp PostfixExpressionBuilder.hpp CSpreadsheet.hpp CPos.cpp CellValue.cpp stackoperators_custom.cpp PostfixExpressionBuilder.cpp CSpreadsheet.cpp | grep -vh '^#pragma' > all_in_one.cpp