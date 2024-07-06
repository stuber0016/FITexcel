#ifndef __PROGTEST__
#pragma once
#include <iostream>
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


