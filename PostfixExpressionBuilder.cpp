#ifndef __PROGTEST__
#include "PostfixExpressionBuilder.hpp"
#include "constantexpressions.hpp"
#endif /* __PROGTEST__ */

//-------------------POSTFIXEXPRESSIONBUILDER METHODS USED FOR CREATING M_PARSED_VECTOR OF NEW CELL------------------

//each method allocates it's operator and sets value of value operators -> push unique_ptr to m_parsed_vector

//binary operator '+'
void PostfixExpressionBuilder::opAdd() {
    auto ptr = std::make_unique<operSum>(operSum());
    m_new_cell.m_parsed_vector.push_back(std::move(ptr));
}

//binary operator '-'
void PostfixExpressionBuilder::opSub() {
    auto ptr = std::make_unique<operSub>(operSub());
    m_new_cell.m_parsed_vector.push_back(std::move(ptr));
}

//binary operator '*'
void PostfixExpressionBuilder::opMul() {
    auto ptr = std::make_unique<operMul>(operMul());
    m_new_cell.m_parsed_vector.push_back(std::move(ptr));
}

//binary operator '/'
void PostfixExpressionBuilder::opDiv() {
    auto ptr = std::make_unique<operDiv>(operDiv());
    m_new_cell.m_parsed_vector.push_back(std::move(ptr));
}

//binary operator '^'
void PostfixExpressionBuilder::opPow() {
    auto ptr = std::make_unique<operPow>(operPow());
    m_new_cell.m_parsed_vector.push_back(std::move(ptr));
}

//unary operator '-'
void PostfixExpressionBuilder::opNeg() {
    auto ptr = std::make_unique<operNeg>(operNeg());
    m_new_cell.m_parsed_vector.push_back(std::move(ptr));
}

//binary operator '=='
void PostfixExpressionBuilder::opEq() {
    auto ptr = std::make_unique<operEq>(operEq());
    m_new_cell.m_parsed_vector.push_back(std::move(ptr));
}

//binary operator '!='
void PostfixExpressionBuilder::opNe() {
    auto ptr = std::make_unique<operNe>(operNe());
    m_new_cell.m_parsed_vector.push_back(std::move(ptr));
}

//binary operator '<'
void PostfixExpressionBuilder::opLt() {
    auto ptr = std::make_unique<operLt>(operLt());
    m_new_cell.m_parsed_vector.push_back(std::move(ptr));
}

//binary operator '<='
void PostfixExpressionBuilder::opLe() {
    auto ptr = std::make_unique<operLe>(operLe());
    m_new_cell.m_parsed_vector.push_back(std::move(ptr));
}

//binary operator '>'
void PostfixExpressionBuilder::opGt() {
    auto ptr = std::make_unique<operGt>(operGt());
    m_new_cell.m_parsed_vector.push_back(std::move(ptr));
}

//binary operator '>='
void PostfixExpressionBuilder::opGe() {
    auto ptr = std::make_unique<operGe>(operGe());
    m_new_cell.m_parsed_vector.push_back(std::move(ptr));
}

//operator representing double value
void PostfixExpressionBuilder::valNumber(double val) {
    auto ptr = std::make_unique<operValNumber>(operValNumber(val));
    m_new_cell.m_parsed_vector.push_back(std::move(ptr));
}

//operator representing std::string value
void PostfixExpressionBuilder::valString(std::string val) {
    auto ptr = std::make_unique<operValString>(operValString(val));
    m_new_cell.m_parsed_vector.push_back(std::move(ptr));
}

//operator representing reference address to another cell
void PostfixExpressionBuilder::valReference(std::string val) {
    auto ptr = std::make_unique<operValReference>(operValReference(val));
    m_new_cell.m_parsed_vector.push_back(std::move(ptr));
}

//operator representing range of references addresse to another cells
void PostfixExpressionBuilder::valRange(std::string val) {
    auto ptr = std::make_unique<operValRange>(operValRange(val));
    m_new_cell.m_parsed_vector.push_back(std::move(ptr));
}

//operator representing funtion call
void PostfixExpressionBuilder::funcCall(std::string fnName, int paramCount) {
    int function_type;
    if (fnName == "sum") {
        function_type = SUM;
    } else if (fnName == "count") {
        function_type = COUNT;
    } else if (fnName == "max") {
        function_type = MAX;
    } else if (fnName == "min") {
        function_type = MIN;
    } else if (fnName == "countval") {
        function_type = COUNTVAL;
    } else {
        function_type = IF;
    }

    auto ptr = std::make_unique<operFuncCall>(operFuncCall(function_type, paramCount));
    m_new_cell.m_parsed_vector.push_back(std::move(ptr));
}








