#ifndef __PROGTEST__
#pragma once
#include "CSpreadsheet.hpp"


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

