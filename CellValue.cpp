#include "CellValue.hpp"
#include "stackoperators_custom.hpp"

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
