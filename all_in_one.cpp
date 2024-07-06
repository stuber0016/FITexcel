#ifndef __PROGTEST__


class CSpreadsheet; //class representing excell spreadsheet - uses CPos as address for cells
#endif /* __PROGTEST__ */

/*------------------------------------------------------------
 *                          CPos
 *
 *class representing address of one cell in excel spreadsheet
 *able to convert address to coordinates or add m_shift vector
------------------------------------------------------------*/


class CPos {
public:
    CPos() { m_pos = {0, 0}; } //default constructor - sets address to invalid coordinates x = 0, y = 0

    CPos(std::string_view str); //constructor converting address to coordinates B6 -> x = 2, y = 6

    //constructor for addres + m_shift vector - used for relative references
    CPos(const std::string_view &str, std::pair<unsigned long long int, unsigned long long int> shift);

    //constuctor for coordinates
    CPos(std::pair<unsigned long long int, unsigned long long int> input) : m_pos(input) {
    }

    //common part of constructors in one function
    friend CPos constructorLogic(const std::string_view &str,
                                 const std::pair<unsigned long long int, unsigned long long int> &shift);

    //getter for coordinates
    std::pair<unsigned long long int, unsigned long long int> pos() const { return m_pos; }

    //operator ==
    bool operator==(const CPos &other) const {
        return (this->m_pos.first == other.m_pos.first && this->m_pos.second == other.m_pos.second);
    }

private:
    std::pair<unsigned long long int, unsigned long long int> m_pos; //coordinates first = x, second = y
};

constexpr size_t DOUBLE = 0;
constexpr size_t UNDEFINED_VAL = 3;
constexpr size_t STRING = 1;
constexpr size_t SUM = 1;
constexpr size_t COUNT = 2;
constexpr size_t MAX = 3;
constexpr size_t MIN = 4;
constexpr size_t COUNTVAL = 5;
constexpr size_t IF = 6;
#ifndef __PROGTEST__
#endif /* __PROGTEST__ */

/*------------------------------------------------------------------
 *                          HASHFORPAIR
 *
 *Hash function for std::unordered_map -> allows you to use pair of
 *any fundamental data types
 *
 *used for m_sheet map in CSpreadsheet class
------------------------------------------------------------------*/


struct HashForPair {
    template<typename T1, typename T2>
    auto operator()(const std::pair<T1, T2> &value) const {
        return std::hash<T1>{}(value.first) ^ (~std::hash<T2>{}(value.second)) << 1;
    }
};


#ifndef __PROGTEST__

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


#ifndef __PROGTEST__
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
#ifndef __PROGTEST__



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

#ifndef __PROGTEST__

constexpr unsigned SPREADSHEET_CYCLIC_DEPS = 0x01;
constexpr unsigned SPREADSHEET_FUNCTIONS = 0x02;
constexpr unsigned SPREADSHEET_FILE_IO = 0x04;
constexpr unsigned SPREADSHEET_SPEED = 0x08;
constexpr unsigned SPREADSHEET_PARSER = 0x10;

using namespace std::literals;
using CValue = std::variant<std::monostate, double, std::string>;
#endif /* __PROGTEST__ */


class CPos; //class representing adress of one cell
class PostfixExpressionBuilder; //class of expression parser
class operAbstract; //abstract class representing values and operators

/*----------------------------------------------------------
 *                      CSpreadsheet
 *
 *class representing excel sheet -> edit, read, save, load
 *data types can be double or std::string
 *cells are saved to std::unordered_map m_sheet
 *key from m_sheet is CPos
 *every cell has it's address represented by class CPos
 *m_canonical_value of cell is represented by class CellValue
----------------------------------------------------------*/


class CSpreadsheet {
public:
    //flags for ProgTest to determine which capabilities to test
    static unsigned capabilities() {
        return SPREADSHEET_CYCLIC_DEPS | SPREADSHEET_FUNCTIONS | SPREADSHEET_FILE_IO | SPREADSHEET_SPEED;
    }

    CSpreadsheet() = default;

    friend PostfixExpressionBuilder; //class of expression parser
    friend operAbstract; //abstract class representing values and operators

    //loads spreadsheet from stream
    bool load(std::istream &is); //true = spreadsheet was loaded, false = failed stream or corrupted data
    //saves spreadsheet to stream
    bool save(std::ostream &os) const; //true = spreadsheet was saved, false = failed stream
    //sets m_canonical_value of cell
    bool setCell(CPos pos, std::string contents); // true = cell was set, false = not set(invalid address or content)
    //returns std::variant of cell m_canonical_value
    CValue getValue(CPos pos); //std::string/double/std::monostate = empty cell or undefined m_canonical_value
    //copies a rectangle of cells from source to destination, allows overlap
    void copyRect(CPos dst, CPos src, int w = 1, int h = 1);

private:
    //stores cells as class CellValue with key of address converted to coordinates
    std::unordered_map<std::pair<long long int, long long int>, CellValue, HashForPair> m_sheet;
    //internally used for getValue - cleared on successful setCell call
    std::unordered_map<std::pair<long long int, long long int>, CValue, HashForPair> m_cache_map;
};



//------------------------------CPOS CONSTRUCTORS AND CONSTRUCTOR LOGIC FUNCTION

//returns calculated coordinates of cell address - used by constructors
CPos constructorLogic(const std::string_view &str,
                      const std::pair<unsigned long long int, unsigned long long int> &shift) {
    CPos result;
    result.m_pos.first = 0;
    result.m_pos.second = 0;

    //FIND INDEX OF FIRST DIGIT
    int first_digit_index = 0;
    for (size_t i = 0; i < str.size(); i++) {
        if (isdigit(str[i])) {
            first_digit_index = i;
            break;
        }
        if (!isalpha(str[i])) {
            result.m_pos.first = 0;
            result.m_pos.second = 0;
            return result;
        }
    }
    if (first_digit_index == 0) {
        std::cout << "Invalid position - No Numbers or number on first index" << std::endl;
        result.m_pos.first = 0;
        result.m_pos.second = 0;
        return result;
    }

    //CONVERT LETTERS
    size_t letter_index = 0;
    for (int i = first_digit_index - 1; i >= 0; i--) {
        unsigned long long int tmp_val = tolower(str[i]) - ('a' - 1);
        tmp_val *= pow(26, letter_index);
        result.m_pos.first += tmp_val;
        letter_index++;
    }

    if (result.m_pos.first == 0) {
        std::cout << "Invalid position - No Letters" << std::endl;
        result.m_pos.first = 0;
        result.m_pos.second = 0;
        return result;
    }

    //ADDING POSSIBLE X SHIFT FROM REFERENCES
    result.m_pos.first += shift.first;

    //CONVERT NUMBERS
    std::string tmp_val;
    for (size_t i = first_digit_index; i < str.size(); i++) {
        if (!isdigit(str[i])) {
            std::cout << "Invalid position - letters in numbers" << std::endl;
            result.m_pos.first = 0;
            result.m_pos.second = 0;
            return result;
        }
        tmp_val += str[i];
    }


    std::stringstream sender(tmp_val);
    sender >> result.m_pos.second;
    //ADDING POSSIBLE Y SHIFT FROM REFERENCES
    result.m_pos.second += shift.second;
    return result;
}

//default constructor with no shift vector - checks for invalid addresses
CPos::CPos(std::string_view str) {
    auto result = constructorLogic(str, {0, 0}).m_pos;
    if (result.first == 0 && result.second == 0) {
        throw std::invalid_argument("error CPos constructor");
    }
    m_pos = result;
}

//constructor with shift vector - no need for check of invalid addresses
//because it's used internally from already existing addresses
CPos::CPos(const std::string_view &str, std::pair<unsigned long long int, unsigned long long int> shift) {
    m_pos = constructorLogic(str, shift).m_pos;
}






//------------------COPY CONSTRUCTOR AND OPERATOR= OF CLASS CELLVALUE------------------------------

CellValue::CellValue(const CellValue &other) {
    this->m_canonical_value = other.m_canonical_value;
    this->m_shift = other.m_shift;
    this->m_parsed_vector.clear();
    this->m_parsed_vector.reserve(other.m_parsed_vector.size());
    for (const auto &elem: other.m_parsed_vector) {
        this->m_parsed_vector.push_back(elem->copy());//copy method neeeded because of the unique_ptr dynamic alloc
    }
    while (!this->m_calculated_stack.empty()) {
        m_calculated_stack.pop();
    }
}

CellValue &CellValue::operator=(const CellValue &other) {
    if (this == &other) {
        return *this;
    }
    this->m_canonical_value = other.m_canonical_value;
    this->m_shift = other.m_shift;
    this->m_parsed_vector.clear();
    this->m_parsed_vector.reserve(other.m_parsed_vector.size());
    for (const auto &elem: other.m_parsed_vector) {
        this->m_parsed_vector.push_back(elem->copy());//copy method neeeded because of the unique_ptr dynamic alloc
    }
    while (!this->m_calculated_stack.empty()) {
        m_calculated_stack.pop();
    }
    return *this;
}
#ifndef __PROGTEST__
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
#ifndef __PROGTEST__
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









//----------------METHODS OF CLASS CSPREADHSEET-----------------

bool CSpreadsheet::setCell(CPos pos, std::string contents) {
    //check for empty string
    if (contents.empty()) {
        //empty string -> setcell failed
        return false;
    }

    //allocate CellValue
    CellValue result;

    //parse expression by PostfixExpressionBuilder -> create m_parsed_vector saved in CellValue
    try {
        PostfixExpressionBuilder builder;
        parseExpression(contents, builder);
        result = builder.getCell();
    } catch (...) {
        //invalid expression -> setcell failed
        return false;
    }


    //clear cache because of the reference dependencies
    m_cache_map.clear();

    //canonical value save
    result.m_canonical_value = contents;

    //save or overwrite new cell to spreadsheet
    m_sheet[pos.pos()] = result;
    return true;
}


CValue CSpreadsheet::getValue(CPos pos) {
    //this function can be called recursively so it uses m_cache_map located in CSpreadsheet to determine if it's in
    //infinite loop and also to optimize reading values of already calculated cells

    //check for not existing cell
    if (!m_sheet.contains(pos.pos())) {
        //non-existent cell -> invalid value
        return CValue();
    }

    //check if result is already stored in cache
    if (m_cache_map.contains(pos.pos())) {
        //found in cache -> instant return
        return m_cache_map[pos.pos()];
    }


    //marked as invalid value -> used for DFS
    m_cache_map[pos.pos()] = CValue();


    //calculate value with operators and values in m_parsed_vector -> result saved to top of m_calculated_stack
    for (auto &elem: m_sheet[pos.pos()].m_parsed_vector) {
        elem->operation(*this, pos);
    }


    //read result
    try {
        //save result to cache
        CValue x = m_sheet[pos.pos()].m_calculated_stack.top().second;
        m_cache_map[pos.pos()] = x;
    } catch (...) {
        //invalid value
        return CValue();
    }

    //delete result from m_calculated stack
    m_sheet[pos.pos()].m_calculated_stack.pop();

    //return value from cache
    return m_cache_map[pos.pos()];
}

void CSpreadsheet::copyRect(CPos dst, CPos src, int w, int h) {
    //clear cache because of the reference dependencies
    m_cache_map.clear();

    CPos source = src;
    CPos destination = dst;

    std::vector<std::pair<CellValue, std::pair<unsigned long long int, unsigned long long int> > > copy_vec;

    //saves all CellValue from source rectangle with its destination address to copy_vec
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            std::pair shifted_source = {source.pos().first + x, source.pos().second + y};
            std::pair shifted_destination = {destination.pos().first + x, destination.pos().second + y};

            //saves non-empty cells from source and its destination
            if (m_sheet.contains(shifted_source) && shifted_source != shifted_destination) {
                CellValue to_be_pushed;
                to_be_pushed = m_sheet[shifted_source];
                copy_vec.push_back({to_be_pushed, shifted_destination});

                //adding shift for relative references
                (copy_vec.end() - 1)->first.m_shift.first += shifted_destination.first - shifted_source.first;
                (copy_vec.end() - 1)->first.m_shift.second += shifted_destination.second - shifted_source.second;
            }
            //empty cells in source are dealt directly by erasing their destination content
            if (!m_sheet.contains(shifted_source) && shifted_source != shifted_destination) {
                m_sheet.erase(shifted_destination);
            }
        }
    }

    //copy whole source rectangle to destination rectangle
    for (auto &elem: copy_vec) {
        m_sheet[elem.second] = elem.first;
    }
}

//function for checking fail and eof flags of stream, true = no flags, false = eof or fail flag
bool check_stream(const std::istream &is) {
    if (is.fail()) {
        return false;
    }
    if (is.eof()) {
        return false;
    }
    return true;
}

//function used for saving whole CSpreadsheet instance to std::ostream, true = successfuly saved, false = failed stream
bool CSpreadsheet::save(std::ostream &os) const {
    //check of stream fail flag
    if (os.fail()) {
        return false;
    }

    uint64_t len_of_mapa = m_sheet.size();
    std::stringstream tmp_stream;

    //writing content (address coordinates, shift vector, canonical value) of all cells to tmp_stream in binary
    tmp_stream.write(reinterpret_cast<char *>(&len_of_mapa), sizeof(len_of_mapa));
    for (const auto &elem: m_sheet) {
        uint64_t len_of_value = elem.second.m_canonical_value.size();
        tmp_stream.write(reinterpret_cast<const char *>(&elem.first.first), sizeof(elem.first.first));
        tmp_stream.write(reinterpret_cast<const char *>(&elem.first.second), sizeof(elem.first.second));
        tmp_stream.write(reinterpret_cast<const char *>(&elem.second.m_shift.first), sizeof(elem.second.m_shift.first));
        tmp_stream.write(reinterpret_cast<const char *>(&elem.second.m_shift.second),
                         sizeof(elem.second.m_shift.second));
        tmp_stream.write(reinterpret_cast<const char *>(&len_of_value), sizeof(len_of_value));
        tmp_stream.write(elem.second.m_canonical_value.c_str(), elem.second.m_canonical_value.size());
    }

    //calculating checksum of saved CSpreadsheet
    size_t hash = std::hash<std::string>{}(tmp_stream.str());

    //saving checksum with content to ostream
    os.write(reinterpret_cast<const char *>(&hash), sizeof(hash));
    os << tmp_stream.str();

    return true;
}

bool CSpreadsheet::load(std::istream &is) {
    //check for stream flags
    if (!check_stream(is)) {
        return false;
    }

    uint64_t len_of_mapa;
    uint64_t len_of_value;
    std::pair<unsigned long long int, unsigned long long int> key;
    std::pair<unsigned long long int, unsigned long long int> shift;
    std::string value;
    size_t hash;

    //read previously saved checksum
    is.read(reinterpret_cast<char *>(&hash), sizeof(hash));
    //check for stream flags
    if (!check_stream(is)) { return false; }

    //save rest of the content to tmp_stream
    std::stringstream tmp_stream;
    tmp_stream << is.rdbuf();
    //check for stream flags
    if (!check_stream(is)) { return false; }

    //calculate actual checksum
    size_t actual_hash = std::hash<std::string>{}(tmp_stream.str());

    //if checksums don't match -> corrupted content
    if (actual_hash != hash) {
        return false;
    }

    //load rest of the content to current instance of CSpreadsheet
    tmp_stream.read(reinterpret_cast<char *>(&len_of_mapa), sizeof(len_of_mapa));
    for (uint64_t i = 0; i < len_of_mapa; i++) {
        tmp_stream.read(reinterpret_cast<char *>(&key.first), sizeof(key.first));
        tmp_stream.read(reinterpret_cast<char *>(&key.second), sizeof(key.second));
        tmp_stream.read(reinterpret_cast<char *>(&shift.first), sizeof(shift.first));
        tmp_stream.read(reinterpret_cast<char *>(&shift.second), sizeof(shift.second));
        tmp_stream.read(reinterpret_cast<char *>(&len_of_value), sizeof(len_of_value));
        value.resize(len_of_value);
        tmp_stream.read(value.data(), len_of_value);
        std::cout << value << std::endl;

        //check for some setcell fails -> shouldn't happen but even checksum can be corrupted by someone...
        //set cell value
        if (!this->setCell(CPos(key), value)) {
            return false;
        }
        //save cell shift_vector
        this->m_sheet[key].m_shift = shift;
    }

    return true;
}

