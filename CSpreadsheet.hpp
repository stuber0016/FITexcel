#ifndef __PROGTEST__
#pragma once
#include <iomanip>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <variant>
#include <utility>
#include "CPos.hpp"
#include "HashForPair.hpp"
#include "PostfixExpressionBuilder.hpp"
#include "CellValue.hpp"

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


