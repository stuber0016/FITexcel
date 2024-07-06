#ifndef __PROGTEST__
#pragma once
#include <stack>
#include "CellValue.hpp"
#include "CSpreadsheet.hpp"
#include <utility>
#include <cmath>
#include <iostream>
#include <variant>
using CValue = std::variant<std::monostate, double, std::string>;
#endif /* __PROGTEST__ */


class CSpreadsheet; //class representing excel spreadsheet
class CPos; //class representing cell address

/*-------------------------------------------------------------------
 *           OPER_ABSTRACT and derived classes operators
 *
 *abstract class giving a base to all the operators and values
 *stored in class CellValue in m_parsed_vector
 *result of these operations is stored as the last and only
 *value on top of m_calculated_stack in class CellValue
 *
 *operAbstract is a friend class of CSpreadsheet so it's able
 *to edit cells (CellValue), all derived classes use static methods of
 *operAbstract so they can edit cells indirectly
 *
 *virtual method operation does the operation of each operator
 *value operators push their value to m_calculated_stack
 *m_calculated_stack has stores std::pair<int,CValue>
 *where int is constexpr used for determine which data type is stored
 *calculations operators (+,*,/...) substitute these values
 *with result of the operation
 *
 *copy method is fundamental for copying m_parsed_vector to
 *different instances of class CSpreadsheet because all
 *operators are dynamically allocated by std::unique_ptr
 *
-------------------------------------------------------------------*/


class operAbstract {
public:
    //------------------------VIRTUAL METHODS-------------------------
    virtual void operation(CSpreadsheet &sheet, const CPos &pos) const = 0; //does each operator operation
    virtual std::string printOper() const = 0; //prints the type of operator or its value and returns the same
    virtual std::unique_ptr<operAbstract> copy() const = 0; //returns newly allocated instance of operator
    virtual ~operAbstract() = default;

    //------------------------STATIC METHODS--------------------------

    //edit m_calculated_stack
    static void notANumber(CSpreadsheet &sheet, const CPos &pos);

    static void pushCalculated_stack(CSpreadsheet &sheet, const CPos &pos, std::pair<int, CValue> &val);

    static void popCalculated_stack(CSpreadsheet &sheet, const CPos &pos);

    //get values from m_calculated_stack
    static std::pair<int, CValue> topCalculated_stack(CSpreadsheet &sheet, const CPos &pos);

    //cell addresses logic
    static std::pair<unsigned long long int, unsigned long long int> getShift(CSpreadsheet &sheet, const CPos &pos);

    static CPos dollarLogic(CSpreadsheet &sheet, const CPos &pos, std::string val);

    static std::vector<std::pair<unsigned long long int, unsigned long long int> >
    getRangeAddresses(const CPos &start, const CPos &end);

    //spreadsheet functions
    static CValue evalSum(CSpreadsheet &sheet, const CPos &start, const CPos &end);

    static CValue evalCount(CSpreadsheet &sheet, const CPos &start, const CPos &end);

    static CValue evalMin(CSpreadsheet &sheet, const CPos &start, const CPos &end);

    static CValue evalMax(CSpreadsheet &sheet, const CPos &start, const CPos &end);

    static CValue evalCountval(CSpreadsheet &sheet, const CPos &start, const CPos &end, CValue &val);
};

//binary operator '+' -> accepts std::string/double/combination
class operSum : public operAbstract {
public:
    operSum() = default;

    void operation(CSpreadsheet &sheet
                   , const CPos &pos) const override;

    std::string printOper() const override;

    std::unique_ptr<operAbstract> copy() const override;
};

//binary operator '-' -> accepts only doubles
class operSub : public operAbstract {
public:
    operSub() = default;

    void operation(CSpreadsheet &sheet
                   , const CPos &pos) const override;

    std::string printOper() const override;

    std::unique_ptr<operAbstract> copy() const override;
};

//binary operator '*' -> accepts only doubles
class operMul : public operAbstract {
public:
    operMul() = default;

    void operation(CSpreadsheet &sheet
                   , const CPos &pos) const override;

    std::string printOper() const override;

    std::unique_ptr<operAbstract> copy() const override;
};

//binary operator '/' -> accepts only doubles
class operDiv : public operAbstract {
public:
    operDiv() = default;

    void operation(CSpreadsheet &sheet
                   , const CPos &pos) const override;

    std::string printOper() const override;

    std::unique_ptr<operAbstract> copy() const override;
};

//binary operator '^' -> accepts only doubles
class operPow : public operAbstract {
public:
    operPow() = default;

    void operation(CSpreadsheet &sheet
                   , const CPos &pos) const override;

    std::string printOper() const override;

    std::unique_ptr<operAbstract> copy() const override;
};

//unary operator '-' -> accepts only doubles
class operNeg : public operAbstract {
public:
    operNeg() = default;

    void operation(CSpreadsheet &sheet
                   , const CPos &pos) const override;

    std::string printOper() const override;

    std::unique_ptr<operAbstract> copy() const override;
};

//binary operator '==' -> accepts std::string/double
class operEq : public operAbstract {
public:
    operEq() = default;

    void operation(CSpreadsheet &sheet, const CPos &pos) const override;

    std::string printOper() const override;

    std::unique_ptr<operAbstract> copy() const override;
};

//binary operator '!=' -> accepts std::string/double
class operNe : public operAbstract {
public:
    operNe() = default;

    void operation(CSpreadsheet &sheet
                   , const CPos &pos) const override;

    std::string printOper() const override;

    std::unique_ptr<operAbstract> copy() const override;
};

//binary operator '<' -> accepts std::string/double
class operLt : public operAbstract {
public:
    operLt() = default;

    void operation(CSpreadsheet &sheet
                   , const CPos &pos) const override;

    std::string printOper() const override;

    std::unique_ptr<operAbstract> copy() const override;
};

//binary operator '<=' -> accepts std::string/double
class operLe : public operAbstract {
public:
    operLe() = default;

    void operation(CSpreadsheet &sheet
                   , const CPos &pos) const override;

    std::string printOper() const override;

    std::unique_ptr<operAbstract> copy() const override;
};

//binary operator '>' -> accepts std::string/double
class operGt : public operAbstract {
public:
    operGt() = default;

    void operation(CSpreadsheet &sheet
                   , const CPos &pos) const override;

    std::string printOper() const override;

    std::unique_ptr<operAbstract> copy() const override;
};

//binary operator '>=' -> accepts std::string/double
class operGe : public operAbstract {
public:
    operGe() = default;

    void operation(CSpreadsheet &sheet
                   , const CPos &pos) const override;

    std::string printOper() const override;

    std::unique_ptr<operAbstract> copy() const override;
};

//returns double m_canonical_value
class operValNumber : public operAbstract {
public:
    operValNumber(double value) : val(value) {
    }

    void operation(CSpreadsheet &sheet, const CPos &pos) const override;

    std::string printOper() const override;

    std::unique_ptr<operAbstract> copy() const override;

private:
    double val;
};

//returns std::string m_canonical_value
class operValString : public operAbstract {
public:
    operValString(std::string &value) : val(value) {
    }

    void operation(CSpreadsheet &sheet, const CPos &pos) const override;

    std::string printOper() const override;

    std::unique_ptr<operAbstract> copy() const override;

private:
    std::string val;
};

//returns relative or absolute reference to another cell, $ = fixed coordinate (A10, $A10, A$10, $A$10)
class operValReference : public operAbstract {
public:
    operValReference(std::string &value) : val(value) {
    }

    void operation(CSpreadsheet &sheet, const CPos &pos) const override;

    std::string printOper() const override;

    std::unique_ptr<operAbstract> copy() const override;

private:
    std::string val;
};

//returns range - rectangle of cells references (A5:F$10) - similar to operValReference
class operValRange : public operAbstract {
public:
    operValRange(std::string &value) : val(value) {
    }

    void operation(CSpreadsheet &sheet, const CPos &pos) const override;

    std::string printOper() const override;

    std::unique_ptr<operAbstract> copy() const override;

private:
    std::string val;
};

//returns function name and number of parameters that it uses
//these parameters were already pushed to m_parsed_vector before funcCall
class operFuncCall : public operAbstract {
public:
    operFuncCall(int func, int parameter) : function(func), param(parameter) {
    }

    void operation(CSpreadsheet &sheet, const CPos &pos) const override;

    std::string printOper() const override;

    std::unique_ptr<operAbstract> copy() const override;

private:
    int function;
    int param;
};
