#ifndef __PROGTEST__
#pragma once

#include <iostream>
#include <stack>
#include <utility>

#include "CSpreadsheet.hpp"
#include "expression.h"
#include "CPos.hpp"
#include "stackoperators_custom.hpp"

using CValue = std::variant<std::monostate, double, std::string>;
class CPos;
class CSpreadsheet;
#endif /* __PROGTEST__ */

/*------------------------------------------------------------------
 *                  PostfixExpressionBuilder
 *
 *derived class used for parsing expression to postfix notation
 *parsing itself is done in compiled class CExprBuilder and called
 *by function ParseExpression
 *
 *returns exception std::invalid_argument on invalid expression
 *creates m_parsed_vector which is a part of CellValue class
 *m_parsed_vector contains unique_ptrs of polymorphic classes
 *classes represent unary/binary operators and values
 *class isn't saved to any other structure of class
 *some operators accept only doubles otherwise NaN -> std::monostate
 ------------------------------------------------------------------*/


class PostfixExpressionBuilder : public CExprBuilder {
public:
    PostfixExpressionBuilder() = default;

    void opAdd() override; //binary operator '+' -> accepts std::string/double/combination
    void opSub() override; //binary operator '-' -> accepts only doubles
    void opMul() override; //binary operator '*' -> accepts only doubles
    void opDiv() override; //binary operator '/' -> accepts only doubles
    void opPow() override; //binary operator '^' -> accepts only doubles
    void opNeg() override; //unary operator '-' -> accepts only doubles
    void opEq() override; //binary operator '==' -> accepts std::string/double
    void opNe() override; //binary operator '!=' -> accepts std::string/double
    void opLt() override; //binary operator '<' -> accepts std::string/double
    void opLe() override; //binary operator '<=' -> accepts std::string/double
    void opGt() override; //binary operator '>' -> accepts std::string/double
    void opGe() override; //binary operator '>=' -> accepts std::string/double

    void valNumber(double val) override; //returns double m_canonical_value
    void valString(std::string val) override; //returns std::string m_canonical_value

    //returns relative or absolute reference to another cell
    void valReference(std::string val) override; //$ = fixed coordinate (A10, $A10, A$10, $A$10)
    void valRange(std::string val) override; //returns range - rectangle of cells references (A5:F$10)

    //returns function name and number of parameters that it uses
    //these parameters were already pushed to m_parsed vector before funcCall
    void funcCall(std::string fnName, int paramCount) override;

    //getter for newly allocated cell -> used in setcell function in class CSpreadsheet
    CellValue getCell() {
        return m_new_cell;
    }

    virtual ~PostfixExpressionBuilder() = default;

private:
    CellValue m_new_cell; //includes m_parsed vector -> saved to cell in CSpreadsheet after successfull build
};

