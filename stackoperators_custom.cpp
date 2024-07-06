#ifndef __PROGTEST__
#include "stackoperators_custom.hpp"
#include "constantexpressions.hpp"
#endif /* __PROGTEST__ */

//----------------------------STATIC METHODS OF BASE ABSTRACT CLASS OPER_ABSTRACT------------------------------

//function used to push undefined value to m_calculated stack
//when one operation gets undefined input it returns CValue() = invalid value
//this fault propagates itself to the top of the stack
void operAbstract::notANumber(CSpreadsheet &sheet, const CPos &pos) {
    auto invalid = CValue();
    invalid = std::monostate{};
    std::pair<int, CValue> to_be_pushed;
    to_be_pushed = std::make_pair(UNDEFINED_VAL, invalid);
    sheet.m_sheet[pos.pos()].m_calculated_stack.push(to_be_pushed);
}

//reads top of m_calculated_stack of given cell
std::pair<int, CValue> operAbstract::topCalculated_stack(CSpreadsheet &sheet, const CPos &pos) {
    return sheet.m_sheet[pos.pos()].m_calculated_stack.top();
}

//pushes passed value to the top of m_Calculated_Stack of given cell
void operAbstract::pushCalculated_stack(CSpreadsheet &sheet, const CPos &pos, std::pair<int, CValue> &val) {
    sheet.m_sheet[pos.pos()].m_calculated_stack.push(val);
}

//deletes top of the m_calculated_stack of given cell
void operAbstract::popCalculated_stack(CSpreadsheet &sheet, const CPos &pos) {
    sheet.m_sheet[pos.pos()].m_calculated_stack.pop();
}

//reads m_shift from cell of given address
std::pair<unsigned long long int, unsigned long long> operAbstract::getShift(
    CSpreadsheet &sheet, const CPos &pos) {
    return sheet.m_sheet[pos.pos()].m_shift;
}

//logic behind relative/fixed references, returns shifted address
CPos operAbstract::dollarLogic(CSpreadsheet &sheet, const CPos &pos, std::string val) {
    std::string pos_str;

    //USED FOR DETERMINING WHICH SHIFT IS APPLIED
    size_t dollar_count = 0;
    for (const auto &elem: val) {
        if (elem != '$') {
            pos_str += elem;
        } else {
            dollar_count++;
        }
    }

    //reads shift coordinates from current cell
    auto shift = getShift(sheet, pos);

    //fixed references shift coordinate is set to -> 0
    if (dollar_count == 2) {
        shift = {0, 0};
    } else {
        if (dollar_count == 0) {
        }
        //one dollar found
        else {
            if (val[0] == '$')
                shift.first = 0;
            else {
                shift.second = 0;
            }
        }
    }

    //uses shift constructor of CPos -> adds shift vector to given address and returns shifted address
    return CPos(pos_str, shift);
}

//returns vector of all cells address within given range
std::vector<std::pair<unsigned long long int, unsigned long long int> >
operAbstract::getRangeAddresses(const CPos &start, const CPos &end) {
    size_t width = end.pos().first - start.pos().first + 1;
    size_t height = end.pos().second - start.pos().second + 1;
    std::vector<std::pair<unsigned long long int, unsigned long long int> > result_vec;

    for (size_t x = 0; x < width; x++) {
        for (size_t y = 0; y < height; y++) {
            auto shifted_x = start.pos().first + x;
            auto shifted_y = start.pos().second + y;
            result_vec.push_back({shifted_x, shifted_y});
        }
    }
    return result_vec;
}

//returns sum of cells containing double values within given range, no numeric cell -> undefined value
CValue operAbstract::evalSum(CSpreadsheet &sheet, const CPos &start, const CPos &end) {
    auto address_vec = getRangeAddresses(start, end);
    double sum = 0;
    bool foundOne = false;
    for (const auto &elem: address_vec) {
        auto value = sheet.getValue(CPos(elem));
        if (std::holds_alternative<double>(value)) {
            sum += std::get<double>(value);
            foundOne = true;
        }
    }
    if (foundOne) {
        return CValue(sum);
    }
    return CValue();
}

//returns count of non-empty cells within given range
CValue operAbstract::evalCount(CSpreadsheet &sheet, const CPos &start, const CPos &end) {
    auto address_vec = getRangeAddresses(start, end);
    double count = 0;
    for (const auto &elem: address_vec) {
        if (sheet.m_sheet.contains(CPos(elem).pos())) {
            count++;
        }
    }
    return CValue(count);
}

//returns maximum value found within given range, no numeric cell -> undefined value
CValue operAbstract::evalMax(CSpreadsheet &sheet, const CPos &start, const CPos &end) {
    auto address_vec = getRangeAddresses(start, end);
    double max = 0;
    bool foundOne = false;
    for (const auto &elem: address_vec) {
        auto value = sheet.getValue(CPos(elem));
        if (std::holds_alternative<double>(value)) {
            if (foundOne) {
                if (std::get<double>(value) > max) {
                    max = std::get<double>(value);
                }
            } else {
                max = std::get<double>(value);
                foundOne = true;
            }
        }
    }
    if (foundOne) {
        return CValue(max);
    }
    return CValue();
}

//returns minimum value found within given range, no numeric cell -> undefined value
CValue operAbstract::evalMin(CSpreadsheet &sheet, const CPos &start, const CPos &end) {
    auto address_vec = getRangeAddresses(start, end);
    double min = 0;
    bool foundOne = false;
    for (const auto &elem: address_vec) {
        auto value = sheet.getValue(CPos(elem));
        if (std::holds_alternative<double>(value)) {
            if (foundOne) {
                if (std::get<double>(value) < min) {
                    min = std::get<double>(value);
                }
            } else {
                min = std::get<double>(value);
                foundOne = true;
            }
        }
    }
    if (foundOne) {
        return CValue(min);
    }
    return CValue();
}

//returns count of cells matching given value within given range
CValue operAbstract::evalCountval(CSpreadsheet &sheet, const CPos &start, const CPos &end, CValue &val) {
    auto address_vec = getRangeAddresses(start, end);
    double count = 0;
    for (const auto &elem: address_vec) {
        auto value = sheet.getValue(CPos(elem));
        if (value == val) {
            count++;
        }
    }
    return CValue(count);
}

//--------------------------------METHODS OF EACH OPERATORS OPERATIONS-------------------------------------

//binary operator '+' substitutes two values on m_calculated_stack with result of the operation
//pushes to m_calculated_stack undefined value when one of the operands is undefined value
void operSum::operation(CSpreadsheet &sheet, const CPos &pos) const {
    //get operands
    auto tmp_value1 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    auto tmp_value2 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    //check for undefined value
    if (tmp_value1.first == UNDEFINED_VAL || tmp_value2.first == UNDEFINED_VAL) {
        notANumber(sheet, pos);
        return;
    }

    //combination of string and double
    if (tmp_value1.first != tmp_value2.first) {
        //converts double to string and adds
        if (tmp_value1.first == DOUBLE) {
            std::pair<int, CValue> to_be_pushed = {
                STRING, std::get<std::string>(tmp_value2.second) + std::to_string(std::get<double>(tmp_value1.second))
            };
            pushCalculated_stack(sheet, pos, to_be_pushed);
        } else {
            std::pair<int, CValue> to_be_pushed = {
                STRING, std::to_string(std::get<double>(tmp_value2.second))
                        + std::get<std::string>(tmp_value1.second)
            };
            pushCalculated_stack(sheet, pos, to_be_pushed);
        }
        return;
    }
    //doubles
    if (tmp_value1.first == DOUBLE) {
        std::pair<int, CValue> to_be_pushed = {
            DOUBLE, std::get<double>(tmp_value2.second) + std::get<double>(tmp_value1.second)
        };
        pushCalculated_stack(sheet, pos, to_be_pushed);
        //strings
    } else {
        // tmp_value2 je popnuta niz, takze ma byt prvni ve vysledku
        std::pair<int, CValue> to_be_pushed = {
            STRING, std::get<std::string>(tmp_value2.second) + std::get<std::string>(tmp_value1.second)
        };
        //push result ot m_calculated_stack
        pushCalculated_stack(sheet, pos, to_be_pushed);
    }
}

//binary operator '-' substitutes two values on m_calculated_stack with result of the operation
//pushes to m_calculated_stack undefined value when one of the operands is undefined value or std::string
void operSub::operation(CSpreadsheet &sheet, const CPos &pos) const {
    //get operands
    auto tmp_value1 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    auto tmp_value2 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);


    //check for undefined value
    if (tmp_value1.first == DOUBLE && tmp_value2.first == DOUBLE) {
        std::pair<int, CValue> to_be_pushed = {
            DOUBLE, std::get<double>(tmp_value2.second) - std::get<double>(tmp_value1.second)
        };
        //push result to m_calculated_stack
        pushCalculated_stack(sheet, pos, to_be_pushed);
        return;
    }

    notANumber(sheet, pos);
}

//binary operator '*' substitutes two values on m_calculated_stack with result of the operation
//pushes to m_calculated_stack undefined value when one of the operands is undefined value or std::string
void operMul::operation(CSpreadsheet &sheet, const CPos &pos) const {
    //get operands
    auto tmp_value1 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    auto tmp_value2 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    //check for undefined value
    if (tmp_value1.first == DOUBLE && tmp_value2.first == DOUBLE) {
        std::pair<int, CValue> to_be_pushed = {
            DOUBLE, std::get<double>(tmp_value2.second) * std::get<double>(tmp_value1.second)
        };
        //push result to m_calculated_stack
        pushCalculated_stack(sheet, pos, to_be_pushed);
        return;
    }

    notANumber(sheet, pos);
}

//binary operator '/' substitutes two values on m_calculated_stack with result of the operation
//pushes to m_calculated_stack undefined value when one of the operands is undefined value or std::string
void operDiv::operation(CSpreadsheet &sheet, const CPos &pos) const {
    //get operands
    auto tmp_value1 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    auto tmp_value2 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    //check for undefined value
    if (tmp_value1.first == DOUBLE && tmp_value2.first == DOUBLE) {
        //!CHECK DIVIDING BY ZERO '0'
        if (std::get<double>(tmp_value1.second) == 0) {
            notANumber(sheet, pos);
            return;
        }
        std::pair<int, CValue> to_be_pushed = {
            DOUBLE, std::get<double>(tmp_value2.second) / std::get<double>(tmp_value1.second)
        };
        //push result to m_calculated_stack
        pushCalculated_stack(sheet, pos, to_be_pushed);
        return;
    }

    notANumber(sheet, pos);
}

//binary operator '^' substitutes two values on m_calculated_stack with result of the operation
//pushes to m_calculated_stack undefined value when one of the operands is undefined value or std::string
void operPow::operation(CSpreadsheet &sheet, const CPos &pos) const {
    //get operands
    auto tmp_value1 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    auto tmp_value2 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    //check for undefined value
    if (tmp_value1.first == DOUBLE && tmp_value2.first == DOUBLE) {
        std::pair<int, CValue> to_be_pushed = {
            DOUBLE, pow(std::get<double>(tmp_value2.second), std::get<double>(tmp_value1.second))
        };
        //push result to m_calculated_stack
        pushCalculated_stack(sheet, pos, to_be_pushed);
        return;
    }

    notANumber(sheet, pos);
}

//unary operator '-' substitutes one value on m_calculated_stack with result of the operation
//pushes to m_calculated_stack undefined value when operand is undefined value or std::string
void operNeg::operation(CSpreadsheet &sheet, const CPos &pos) const {
    //get operand
    auto tmp_value1 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    //check for undefined value
    if (tmp_value1.first == DOUBLE) {
        std::pair<int, CValue> to_be_pushed = {DOUBLE, -std::get<double>(tmp_value1.second)};
        //push result to m_calculated_stack
        pushCalculated_stack(sheet, pos, to_be_pushed);
        return;
    }

    notANumber(sheet, pos);
}

//binary operator '==' substitutes two values on m_calculated_stack with result of the operation, match = 1, diff = 0
//pushes to m_calculated_stack undefined value when one of the operands is undefined value or operand types dont match
void operEq::operation(CSpreadsheet &sheet, const CPos &pos) const {
    //get operands
    auto tmp_value1 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    auto tmp_value2 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    //check for undefined value
    if (tmp_value1.first == UNDEFINED_VAL || tmp_value2.first == UNDEFINED_VAL || tmp_value1.first != tmp_value2.
        first) {
        notANumber(sheet, pos);
        return;
    }


    //push result to m_calculated_stack
    double result = tmp_value1 == tmp_value2;
    std::pair<int, CValue> to_be_pushed = {DOUBLE, CValue(result)};
    pushCalculated_stack(sheet, pos, to_be_pushed);
}

//binary operator '!=' substitutes two values on m_calculated_stack with result of the operation, match = 0, diff = 1
//pushes to m_calculated_stack undefined value when one of the operands is undefined value or operand types dont match
void operNe::operation(CSpreadsheet &sheet, const CPos &pos) const {
    //get operands
    auto tmp_value1 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    auto tmp_value2 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    //check for undefined value
    if (tmp_value1.first == UNDEFINED_VAL || tmp_value2.first == UNDEFINED_VAL || tmp_value1.first != tmp_value2.
        first) {
        notANumber(sheet, pos);
        return;
    }

    //push result to m_calculated_stack
    double result = tmp_value1 != tmp_value2;
    std::pair<int, CValue> to_be_pushed = {DOUBLE, CValue(result)};
    pushCalculated_stack(sheet, pos, to_be_pushed);
}

//binary operator '<' substitutes two values on m_calculated_stack with result of the operation, true = 1, false = 0
//pushes to m_calculated_stack undefined value when one of the operands is undefined value or operand types dont match
void operLt::operation(CSpreadsheet &sheet, const CPos &pos) const {
    //get operands
    auto tmp_value1 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    auto tmp_value2 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);


    //check for undefined value
    if (tmp_value1.first == UNDEFINED_VAL || tmp_value2.first == UNDEFINED_VAL || tmp_value1.first != tmp_value2.
        first) {
        notANumber(sheet, pos);
        return;
    }

    //push result to m_calculated_stack
    double result = tmp_value2 < tmp_value1;
    std::pair<int, CValue> to_be_pushed = {DOUBLE, CValue(result)};
    pushCalculated_stack(sheet, pos, to_be_pushed);
}

//binary operator '<=' substitutes two values on m_calculated_stack with result of the operation, true = 1, false = 0
//pushes to m_calculated_stack undefined value when one of the operands is undefined value or operand types dont match
void operLe::operation(CSpreadsheet &sheet, const CPos &pos) const {
    //get operands
    auto tmp_value1 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    auto tmp_value2 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);


    //check for undefined value
    if (tmp_value1.first == UNDEFINED_VAL || tmp_value2.first == UNDEFINED_VAL || tmp_value1.first != tmp_value2.
        first) {
        notANumber(sheet, pos);
        return;
    }

    //push result to m_calculated_stack
    double result = tmp_value2 <= tmp_value1;
    std::pair<int, CValue> to_be_pushed = {DOUBLE, CValue(result)};
    pushCalculated_stack(sheet, pos, to_be_pushed);
}

//binary operator '>' substitutes two values on m_calculated_stack with result of the operation, true = 1, false = 0
//pushes to m_calculated_stack undefined value when one of the operands is undefined value or operand types dont match
void operGt::operation(CSpreadsheet &sheet, const CPos &pos) const {
    //get operands

    auto tmp_value1 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    auto tmp_value2 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    //check for undefined value
    if (tmp_value1.first == UNDEFINED_VAL || tmp_value2.first == UNDEFINED_VAL || tmp_value1.first != tmp_value2.
        first) {
        notANumber(sheet, pos);
        return;
    }

    //push result to m_calculated_stack
    double result = tmp_value2 > tmp_value1;
    std::pair<int, CValue> to_be_pushed = {DOUBLE, CValue(result)};
    pushCalculated_stack(sheet, pos, to_be_pushed);
}

//binary operator '>=' substitutes two values on m_calculated_stack with result of the operation, true = 1, false = 0
//pushes to m_calculated_stack undefined value when one of the operands is undefined value or operand types dont match
void operGe::operation(CSpreadsheet &sheet, const CPos &pos) const {
    //get operands
    auto tmp_value1 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    auto tmp_value2 = topCalculated_stack(sheet, pos);
    popCalculated_stack(sheet, pos);

    //check for undefined value
    if (tmp_value1.first == UNDEFINED_VAL || tmp_value2.first == UNDEFINED_VAL || tmp_value1.first != tmp_value2.
        first) {
        notANumber(sheet, pos);
        return;
    }

    //push result to m_calculated_stack
    double result = tmp_value2 >= tmp_value1;
    std::pair<int, CValue> to_be_pushed = {DOUBLE, CValue(result)};
    pushCalculated_stack(sheet, pos, to_be_pushed);
}

//operator that pushes it's double value to m_calculated_stack
void operValNumber::operation(CSpreadsheet &sheet, const CPos &pos) const {
    //prepare value type and value
    std::pair<int, CValue> tmp_value;
    tmp_value.first = DOUBLE;
    tmp_value.second = val;
    //push result to m_calculated_stack
    pushCalculated_stack(sheet, pos, tmp_value);
}

//operator that pushes it's std::string value to m_calculated_stack
void operValString::operation(CSpreadsheet &sheet, const CPos &pos) const {
    //prepare value type and value
    std::pair<int, CValue> tmp_value;
    tmp_value.first = STRING;
    tmp_value.second = val;
    //push result to m_calculated_stack
    pushCalculated_stack(sheet, pos, tmp_value);
}

//operator that pushes value of referenced cell to m_calculated_stack
void operValReference::operation(CSpreadsheet &sheet, const CPos &pos) const {
    //gets relative reference address
    auto shifted_pos = dollarLogic(sheet, pos, val);

    //calculates value of reference
    CValue tmp_value = sheet.getValue(shifted_pos);

    //check for undefined value
    if (std::holds_alternative<std::monostate>(tmp_value)) {
        notANumber(sheet, pos);
    }

    //double
    else if (std::holds_alternative<double>(tmp_value)) {
        //std::cout << "Z bunky " << pos_str << " vytahnuto " << std::get<double>(tmp_value) << std::endl;
        std::pair<int, CValue> to_be_pushed = {DOUBLE, tmp_value};
        //push result to m_calculated_stack
        pushCalculated_stack(sheet, pos, to_be_pushed);
    }
    //std::string
    else {
        //std::cout << "Z bunky " << pos_str << " vytahnuto " << std::get<std::string>(tmp_value)<< std::endl;
        std::pair<int, CValue> to_be_pushed = {STRING, tmp_value};
        //push result to m_calculated_stack
        pushCalculated_stack(sheet, pos, to_be_pushed);
    }
}

//operator that pushes range of cell references to m_calculated stack (A15:B$20)
void operValRange::operation(CSpreadsheet &sheet, const CPos &pos) const {
    //prepare value type and value
    std::pair<int, CValue> tmp_value = {STRING, val};
    //push result to m_calculated_stack
    pushCalculated_stack(sheet, pos, tmp_value);
}

//operator that reads operands from m_calculated_stack and substitutes them for the result of a called function
void operFuncCall::operation(CSpreadsheet &sheet, const CPos &pos) const {
    CValue result;

    //IF FUNCTION
    if (function == IF) {
        //read of operands
        auto if_false = topCalculated_stack(sheet, pos);
        popCalculated_stack(sheet, pos);
        auto if_true = topCalculated_stack(sheet, pos);
        popCalculated_stack(sheet, pos);
        auto condition = topCalculated_stack(sheet, pos);
        popCalculated_stack(sheet, pos);

        //evaluating condition
        CSpreadsheet tmp_sheet;
        if (condition.first == STRING) {
            tmp_sheet.setCell(CPos("A1"), std::get<std::string>(condition.second));
        } else {
            tmp_sheet.setCell(CPos("A1"), std::to_string(std::get<double>(condition.second)));
        }
        auto tmp_result = tmp_sheet.getValue(CPos("A1"));

        //returning if_true/if_false
        if (std::holds_alternative<double>(tmp_result)) {
            if (std::get<double>(tmp_result) != 0) {
                result = if_true.second;
            } else {
                result = if_false.second;
            }
        } else {
            result = CValue();
        }
        //SUM, COUNT, MAX, MIN, COUNTVAL
    } else {
        //read range
        auto tmp_value1 = topCalculated_stack(sheet, pos);
        popCalculated_stack(sheet, pos);
        std::string range_str = std::get<std::string>(tmp_value1.second);
        char delimeter = ':';
        std::stringstream tmp_stream(range_str);
        std::string start_str;
        std::string end_str;
        std::getline(tmp_stream, start_str, delimeter);
        std::getline(tmp_stream, end_str, delimeter);

        CPos start = dollarLogic(sheet, pos, start_str);
        CPos end = dollarLogic(sheet, pos, end_str);


        //call static methods for each function
        switch (function) {
            case SUM:
                result = evalSum(sheet, start, end);
                break;
            case COUNT:
                result = evalCount(sheet, start, end);
                break;
            case MIN:
                result = evalMin(sheet, start, end);
                break;
            case MAX:
                result = evalMax(sheet, start, end);
                break;
            case COUNTVAL:
                //read value needed for countval function
                auto val = topCalculated_stack(sheet, pos);
                popCalculated_stack(sheet, pos);
                result = evalCountval(sheet, start, end, val.second);
                break;
        }
    }

    //check for invalid value
    if (std::holds_alternative<std::monostate>(result)) {
        notANumber(sheet, pos);
    }
    //push result to m_calculated_stack
    if (std::holds_alternative<double>(result)) {
        std::pair<int, CValue> to_be_pushed = {DOUBLE, result};
        pushCalculated_stack(sheet, pos, to_be_pushed);
    } else {
        std::pair<int, CValue> to_be_pushed = {STRING, result};
        pushCalculated_stack(sheet, pos, to_be_pushed);
    }
}

//--------------------DEBUG PRINT METHODS---------------------
std::string operSum::printOper() const {
    std::cout << "+" << std::endl;
    return "+";
}

std::string operSub::printOper() const {
    std::cout << "-" << std::endl;
    return "-";
}

std::string operMul::printOper() const {
    std::cout << "*" << std::endl;
    return "*";
}

std::string operDiv::printOper() const {
    std::cout << "/" << std::endl;
    return "/";
}

std::string operNeg::printOper() const {
    std::cout << "--" << std::endl;
    return "--";
}

std::string operPow::printOper() const {
    std::cout << "^" << std::endl;
    return "^";
}

std::string operEq::printOper() const {
    std::cout << "==" << std::endl;
    return "==";
}

std::string operNe::printOper() const {
    std::cout << "!=" << std::endl;
    return "!=";
}

std::string operGt::printOper() const {
    std::cout << ">" << std::endl;
    return ">";
}

std::string operGe::printOper() const {
    std::cout << ">=" << std::endl;
    return ">=";
}

std::string operLt::printOper() const {
    std::cout << "<" << std::endl;
    return "<";
}

std::string operLe::printOper() const {
    std::cout << "<=" << std::endl;
    return "<=";
}

std::string operValNumber::printOper() const {
    std::cout << val << std::endl;
    return std::to_string(val);
}

std::string operValString::printOper() const {
    std::cout << val << std::endl;
    return val;
}

std::string operValReference::printOper() const {
    std::cout << val << std::endl;
    return val;
}

std::string operValRange::printOper() const {
    std::cout << val << std::endl;
    return val;
}

std::string operFuncCall::printOper() const {
    std::cout << function << " " << param << std::endl;
    return function + " " + std::to_string(param);
}

//------------------------COPY METHODS-----------------------------
std::unique_ptr<operAbstract> operSum::copy() const {
    return std::make_unique<operSum>(*this);
}

std::unique_ptr<operAbstract> operSub::copy() const {
    return std::make_unique<operSub>(*this);
}

std::unique_ptr<operAbstract> operMul::copy() const {
    return std::make_unique<operMul>(*this);
}

std::unique_ptr<operAbstract> operDiv::copy() const {
    return std::make_unique<operDiv>(*this);
}

std::unique_ptr<operAbstract> operNeg::copy() const {
    return std::make_unique<operNeg>(*this);
}

std::unique_ptr<operAbstract> operPow::copy() const {
    return std::make_unique<operPow>(*this);
}

std::unique_ptr<operAbstract> operEq::copy() const {
    return std::make_unique<operEq>(*this);
}

std::unique_ptr<operAbstract> operNe::copy() const {
    return std::make_unique<operNe>(*this);
}

std::unique_ptr<operAbstract> operGt::copy() const {
    return std::make_unique<operGt>(*this);
}

std::unique_ptr<operAbstract> operGe::copy() const {
    return std::make_unique<operGe>(*this);
}

std::unique_ptr<operAbstract> operLt::copy() const {
    return std::make_unique<operLt>(*this);
}

std::unique_ptr<operAbstract> operLe::copy() const {
    return std::make_unique<operLe>(*this);
}

std::unique_ptr<operAbstract> operValNumber::copy() const {
    return std::make_unique<operValNumber>(*this);
}

std::unique_ptr<operAbstract> operValReference::copy() const {
    return std::make_unique<operValReference>(*this);
}

std::unique_ptr<operAbstract> operValString::copy() const {
    return std::make_unique<operValString>(*this);
}

std::unique_ptr<operAbstract> operValRange::copy() const {
    return std::make_unique<operValRange>(*this);
}

std::unique_ptr<operAbstract> operFuncCall::copy() const {
    return std::make_unique<operFuncCall>(*this);
}

//---------------------------------END-----------------------------------------
