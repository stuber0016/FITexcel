#ifndef __PROGTEST__
#pragma once
#include <memory>
#include <variant>
#include <vector>
#include <stack>

using CValue = std::variant<std::monostate, double, std::string>;
#endif /* __PROGTEST__ */

/*----------------------------------------------------------------------------
 *                              CELLVALUE
 *
 *Class representing one cell in CSpreadsheet
 *data types can be double or std::string
 *No data is linked to CSpreadheet -> can be copied to another CSpreadheet
 *Needs custom Copy constructor and operator= because of the
 *m_parsed_vector of unique_ptrs of polymorphic classes
 ---------------------------------------------------------------------------*/


//abstract class representing values and operators
class operAbstract;

struct CellValue {
    CellValue() = default;

    CellValue(const CellValue &other);

    CellValue &operator=(const CellValue &other);

    std::string m_canonical_value; //m_canonical_value of cell
    std::pair<unsigned long long int, unsigned long long> m_shift = {0, 0}; //m_shift used for relative references
    std::vector<std::unique_ptr<operAbstract> > m_parsed_vector; //represents operators and values in postfix
    //used for calculating m_canonical_value in postfix - normally empty
    std::stack<std::pair<int, CValue> > m_calculated_stack;
};


