#include "CPos.hpp"
#include <cmath>
#include <sstream>

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





