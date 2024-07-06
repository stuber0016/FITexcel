#ifndef __PROGTEST__
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <climits>
#include <cfloat>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <variant>
#include <optional>
#include <compare>
#include <charconv>
#include <span>
#include <utility>
#include "expression.h"
#include "CPos.hpp"
#include "CSpreadsheet.hpp"
#include "PostfixExpressionBuilder.hpp"
using namespace std::literals;
using CValue = std::variant<std::monostate, double, std::string>;


#endif /* __PROGTEST__ */


#ifndef __PROGTEST__

void printus(const CValue &value) {
  if (std::holds_alternative<double>(value)) {
    double tmp = std::get<double>(value);
    std::cout << tmp << std::endl;
    return;
  }
  if (std::holds_alternative<std::monostate>(value)) {
    std::cout << "std::monostate" << std::endl;
    return;
  }
  std::string tmp = std::get<std::string>(value);
  std::cout << tmp << std::endl;
  //ADD EXPRESSION
}

bool valueMatch(const CValue &r, const CValue &s) {
  if (r.index() != s.index())
    return false;
  if (r.index() == 0)
    return true;
  if (r.index() == 2)
    return std::get<std::string>(r) == std::get<std::string>(s);
  if (std::isnan(std::get<double>(r)) && std::isnan(std::get<double>(s)))
    return true;
  if (std::isinf(std::get<double>(r)) && std::isinf(std::get<double>(s)))
    return (std::get<double>(r) < 0 && std::get<double>(s) < 0)
           || (std::get<double>(r) > 0 && std::get<double>(s) > 0);
  return fabs(std::get<double>(r) - std::get<double>(s)) <= 1e8 * DBL_EPSILON * fabs(std::get<double>(r));
}

void test1() {
  CSpreadsheet x0, x1;
  std::ostringstream oss;
  std::istringstream iss;
  std::string data;
  //assert(!x0.setCell(CPos("A 5 5"), "=a33"));
  assert(x0.setCell(CPos("A55"), "=a33"));
  assert(valueMatch ( x0 . getValue ( CPos ( "a55" ) ), CValue () ));
  assert(x0.setCell(CPos("A55"), "=a33"));
  assert(valueMatch ( x0 . getValue ( CPos ( "a55" ) ), CValue () ));
  //assert(!x0.setCell(CPos("A 55"), "=a33"));
  //assert(!x0.setCell(CPos(" A55"), "=a33"));
  //assert(!x0.setCell(CPos("A55 "), "=a33"));
  //assert(!x0.setCell(CPos("A55 "), "=a33"));
  //assert(!x0.setCell(CPos("A55 "), "=a33"));
  x0.setCell(CPos("a32"), "=a33");
  x0.setCell(CPos("a33"), "=a32");
  x0.setCell(CPos("a35"), "=a33");
  x0.setCell(CPos("a35"), "=a33");
  x0.setCell(CPos("a35"), "=a33");
  x0.getValue(CPos("a35"));
  x0.getValue(CPos("a35"));
  x0.getValue(CPos("a35"));
  assert(x0 . setCell ( CPos ( "A1" ), "10" ));
  assert(x0 . setCell ( CPos ( "A2" ), "20.5" ));
  assert(x0 . setCell ( CPos ( "A3" ), "3e1" )); //30
  assert(x0 . setCell ( CPos ( "A4" ), "=40" )); //40
  assert(x0 . setCell ( CPos ( "A5" ), "=5e+1" )); //50
  assert(x0 . setCell ( CPos ( "A6" ), "raw text with any characters, including a quote \" or a newline\n" ));
  assert(
    x0 . setCell ( CPos ( "A7" ),
      "=\"quoted string, quotes must be doubled: \"\". Moreover, backslashes are needed for C++.\"" ));
  assert(valueMatch ( x0 . getValue ( CPos ( "A1" ) ), CValue ( 10.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "A2" ) ), CValue ( 20.5 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "A3" ) ), CValue ( 30.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "A4" ) ), CValue ( 40.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "A5" ) ), CValue ( 50.0 ) ));
  assert(
    valueMatch ( x0 . getValue ( CPos ( "A6" ) ), CValue (
      "raw text with any characters, including a quote \" or a newline\n" ) ));
  assert(
    valueMatch ( x0 . getValue ( CPos ( "A7" ) ), CValue (
      "quoted string, quotes must be doubled: \". Moreover, backslashes are needed for C++." ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "A8" ) ), CValue() ));
  assert(valueMatch ( x0 . getValue ( CPos ( "AAAA9999" ) ), CValue() ));
  assert(x0 . setCell ( CPos ( "B1" ), "=A1+A2*A3" ));
  assert(x0 . setCell ( CPos ( "B2" ), "= -A1 ^ 2 - A2 / 2   " ));
  assert(x0 . setCell ( CPos ( "B3" ), "= 2 ^ $A$1" ));
  assert(x0 . setCell ( CPos ( "B4" ), "=($A1+A$2)^2" ));
  assert(x0 . setCell ( CPos ( "B5" ), "=B1+B2+B3+B4" ));
  assert(x0 . setCell ( CPos ( "B6" ), "=B1+B2+B3+B4+B5" ));
  assert(x0 . setCell ( CPos ( "zsahaugofuasf32154897498494894894984" ), "=B1+B2+B3+B4*B5" ));
  assert(
    x0 . setCell ( CPos ( "zsahaugofuasf32154897498494894894985" ),
      "=B1+B2+B3+B4*B5+zsahaugofuasf32154897498494894894984" ));
  assert(
    x0 . setCell ( CPos ( "zsahaugofuasf32154897498494894894986" ),
      "=B1+B2+B3+B4+B5+zsahaugofuasf32154897498494894894985" ));
  assert(
    x0 . setCell ( CPos ( "zsahaugofuasf32154897498494894894987" ),
      "=B1+B2+B3+B4+B5+zsahaugofuasf32154897498494894894986" ));
  x0.getValue(CPos("zsahaugofuasf32154897498494894894984"));
  x0.getValue(CPos("zsahaugofuasf32154897498494894894985"));
  x0.getValue(CPos("zsahaugofuasf32154897498494894894986"));
  x0.getValue(CPos("zsahaugofuasf32154897498494894894987"));


  assert(valueMatch ( x0 . getValue ( CPos ( "B1" ) ), CValue ( 625.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "B2" ) ), CValue ( -110.25 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "B3" ) ), CValue ( 1024.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "B4" ) ), CValue ( 930.25 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "B5" ) ), CValue ( 2469.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "B6" ) ), CValue ( 4938.0 ) ));
  assert(x0 . setCell ( CPos ( "A1" ), "12" ));
  assert(valueMatch ( x0 . getValue ( CPos ( "B1" ) ), CValue ( 627.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "B2" ) ), CValue ( -154.25 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "B4" ) ), CValue ( 1056.25 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "B5" ) ), CValue ( 5625.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "B6" ) ), CValue ( 11250.0 ) ));
  x1 = x0;
  assert(x0 . setCell ( CPos ( "A2" ), "100" ));
  assert(x1 . setCell ( CPos ( "A2" ), "=A3+A5+A4" ));
  assert(valueMatch ( x0 . getValue ( CPos ( "B1" ) ), CValue ( 3012.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "B2" ) ), CValue ( -194.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "B4" ) ), CValue ( 12544.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "B5" ) ), CValue ( 19458.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "B6" ) ), CValue ( 38916.0 ) ));
  assert(valueMatch ( x1 . getValue ( CPos ( "B1" ) ), CValue ( 3612.0 ) ));
  assert(valueMatch ( x1 . getValue ( CPos ( "B2" ) ), CValue ( -204.0 ) ));
  assert(valueMatch ( x1 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ));
  assert(valueMatch ( x1 . getValue ( CPos ( "B4" ) ), CValue ( 17424.0 ) ));
  assert(valueMatch ( x1 . getValue ( CPos ( "B5" ) ), CValue ( 24928.0 ) ));
  assert(valueMatch ( x1 . getValue ( CPos ( "B6" ) ), CValue ( 49856.0 ) ));
  oss.clear();
  oss.str("");
  assert(x0 . save ( oss ));
  data = oss.str();

  iss.clear();
  iss.str(data);
  assert(x1 . load ( iss ));

  assert(valueMatch ( x1 . getValue ( CPos ( "B1" ) ), CValue ( 3012.0 ) ));
  assert(valueMatch ( x1 . getValue ( CPos ( "B2" ) ), CValue ( -194.0 ) ));
  assert(valueMatch ( x1 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ));
  assert(valueMatch ( x1 . getValue ( CPos ( "B4" ) ), CValue ( 12544.0 ) ));
  assert(valueMatch ( x1 . getValue ( CPos ( "B5" ) ), CValue ( 19458.0 ) ));
  assert(valueMatch ( x1 . getValue ( CPos ( "B6" ) ), CValue ( 38916.0 ) ));
  assert(x0 . setCell ( CPos ( "A3" ), "4e1" ));
  assert(valueMatch ( x1 . getValue ( CPos ( "B1" ) ), CValue ( 3012.0 ) ));
  assert(valueMatch ( x1 . getValue ( CPos ( "B2" ) ), CValue ( -194.0 ) ));
  assert(valueMatch ( x1 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ));
  assert(valueMatch ( x1 . getValue ( CPos ( "B4" ) ), CValue ( 12544.0 ) ));
  assert(valueMatch ( x1 . getValue ( CPos ( "B5" ) ), CValue ( 19458.0 ) ));
  assert(valueMatch ( x1 . getValue ( CPos ( "B6" ) ), CValue ( 38916.0 ) ));
  oss.clear();
  oss.str("");
  assert(x0 . save ( oss ));
  data = oss.str();
  for (size_t i = 0; i < std::min<size_t>(data.length(), 10); i++)
    data[i] ^= 0x5a;

  iss.clear();
  iss.str(data);
  assert(! x1 . load ( iss ));

  assert(x0 . setCell ( CPos ( "D0" ), "10" ));
  assert(x0 . setCell ( CPos ( "D1" ), "20" ));
  assert(x0 . setCell ( CPos ( "D2" ), "30" ));
  assert(x0 . setCell ( CPos ( "D3" ), "40" ));
  assert(x0 . setCell ( CPos ( "D4" ), "50" ));
  assert(x0 . setCell ( CPos ( "E0" ), "60" ));
  assert(x0 . setCell ( CPos ( "E1" ), "70" ));
  assert(x0 . setCell ( CPos ( "E2" ), "80" ));
  assert(x0 . setCell ( CPos ( "E3" ), "90" ));
  assert(x0 . setCell ( CPos ( "E4" ), "100" ));
  assert(x0 . setCell ( CPos ( "F10" ), "=D0+5" ));
  assert(x0 . setCell ( CPos ( "F11" ), "=$D0+5" ));
  assert(x0 . setCell ( CPos ( "F12" ), "=D$0+5" ));
  assert(x0 . setCell ( CPos ( "F13" ), "=$D$0+5" ));
  x0.copyRect(CPos("G11"), CPos("F10"), 1, 4);
  assert(valueMatch ( x0 . getValue ( CPos ( "F10" ) ), CValue ( 15.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "F11" ) ), CValue ( 15.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "F12" ) ), CValue ( 15.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "F13" ) ), CValue ( 15.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "F14" ) ), CValue() ));
  assert(valueMatch ( x0 . getValue ( CPos ( "G10" ) ), CValue() ));
  assert(valueMatch ( x0 . getValue ( CPos ( "G11" ) ), CValue ( 75.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "G12" ) ), CValue ( 25.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "G13" ) ), CValue ( 65.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "G14" ) ), CValue ( 15.0 ) ));
  x0.copyRect(CPos("G11"), CPos("F10"), 2, 4);
  assert(valueMatch ( x0 . getValue ( CPos ( "F10" ) ), CValue ( 15.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "F11" ) ), CValue ( 15.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "F12" ) ), CValue ( 15.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "F13" ) ), CValue ( 15.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "F14" ) ), CValue() ));
  assert(valueMatch ( x0 . getValue ( CPos ( "G10" ) ), CValue() ));
  assert(valueMatch ( x0 . getValue ( CPos ( "G11" ) ), CValue ( 75.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "G12" ) ), CValue ( 25.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "G13" ) ), CValue ( 65.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "G14" ) ), CValue ( 15.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "H10" ) ), CValue() ));
  assert(valueMatch ( x0 . getValue ( CPos ( "H11" ) ), CValue() ));
  assert(valueMatch ( x0 . getValue ( CPos ( "H12" ) ), CValue() ));
  assert(valueMatch ( x0 . getValue ( CPos ( "H13" ) ), CValue ( 35.0 ) ));
  printus(x0.getValue(CPos("H14")));
  assert(valueMatch ( x0 . getValue ( CPos ( "H14" ) ), CValue() ));
  assert(x0 . setCell ( CPos ( "F0" ), "-27" ));
  assert(valueMatch ( x0 . getValue ( CPos ( "H14" ) ), CValue ( -22.0 ) ));
  x0.copyRect(CPos("H12"), CPos("H13"), 1, 2);
  assert(valueMatch ( x0 . getValue ( CPos ( "H12" ) ), CValue ( 25.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "H13" ) ), CValue ( -22.0 ) ));
  assert(valueMatch ( x0 . getValue ( CPos ( "H14" ) ), CValue ( -22.0 ) ));
}

void test2() {
  CSpreadsheet x0;
  CSpreadsheet x1;
  std::istringstream iss;
  std::ostringstream oss;
  std::string data;
  assert(x0 . setCell ( CPos ( "A2" ), "dsds sa HU \" SDS \n" ));
  assert(x0 . setCell ( CPos ( "A3" ), "grew" )); //30
  assert(x0 . setCell ( CPos ( "A4" ), "sda \n assasa \n warasdf \n dsadas" )); //40
  assert(x0 . setCell ( CPos ( "A5" ), "uih0\ni0sd\"  " ));
  oss.clear();
  oss.str("");
  assert(x0 . save ( oss ));
  data = oss.str();
  //std::cout << data;
  iss.str(data);
  x1.load(iss);
  printus(x1.getValue(CPos("A2")));
  printus(x1.getValue(CPos("A3")));
  printus(x1.getValue(CPos("A4")));
  printus(x1.getValue(CPos("A5")));
  assert(valueMatch ( x1 . getValue ( CPos ( "A2" ) ), CValue ( "dsds sa HU \" SDS \n" ) ));
  assert(valueMatch ( x1 . getValue ( CPos ( "A3" ) ), CValue ( "grew" ) ));
  assert(valueMatch ( x1 . getValue ( CPos ( "A4" ) ), CValue ( "sda \n assasa \n warasdf \n dsadas" ) ));
  assert(valueMatch ( x1 . getValue ( CPos ( "A5" ) ), CValue ( "uih0\ni0sd\"  " ) ));
}


void test4() {
  CSpreadsheet x0, x1;
  std::ostringstream oss;
  std::istringstream iss;
  std::string data;
  for (int index = 0; index < 1000; index++) {
    assert(x0.setCell(CPos("A0"), "2"));
    assert(x0.setCell(CPos("B0"), "3"));
    assert(x0.setCell(CPos("C0"), "4"));
    assert(x0.setCell(CPos("D0"), "5"));
    assert(x0.setCell(CPos("E0"), "6"));
    assert(x0.setCell(CPos("F0"), "7"));
    assert(x0.setCell(CPos("G0"), "8"));

    assert(x0.setCell(CPos("A1"), "=A0+(B0*C0)"));

    x0.copyRect(CPos("B1"), CPos("A1"), 1, 1);
    x0.copyRect(CPos("C1"), CPos("A1"), 1, 1);
    x0.copyRect(CPos("D1"), CPos("A1"), 1, 1);
    x0.copyRect(CPos("E1"), CPos("A1"), 1, 1);
    x0.copyRect(CPos("F1"), CPos("A1"), 1, 1);
    x0.copyRect(CPos("G1"), CPos("A1"), 1, 1);

    assert(valueMatch(x0.getValue(CPos("A1")), CValue(14.0)));
    assert(valueMatch(x0.getValue(CPos("B1")), CValue(23.0)));
    assert(valueMatch(x0.getValue(CPos("C1")), CValue(34.0)));
    assert(valueMatch(x0.getValue(CPos("D1")), CValue(47.0)));
    assert(valueMatch(x0.getValue(CPos("E1")), CValue(62.0)));
    assert(valueMatch(x0.getValue(CPos("F1")), CValue()));
    assert(valueMatch(x0.getValue(CPos("G1")), CValue()));
    std::cout << index << std::endl;
  }
}


int main() {
  test1();
  test2();
  test4();

  PostfixExpressionBuilder builder;
  std::string content = "=A8 + 5 * 3 + (8 + 6)";
  parseExpression(content, builder);
  auto value = builder.getCell();
  for (const auto &elem: value.m_parsed_vector) {
    elem->printOper();
  }
  CPos pos("ABC125");
  std::cout << pos.pos().first << " " << pos.pos().second << std::endl;


  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */


