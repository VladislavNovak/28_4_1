#ifndef INC_28_4_1_UTILITIES_H
#define INC_28_4_1_UTILITIES_H

#include <iostream>
#include <algorithm>
#include <limits>
#include <vector>
#include <sstream>
#include <iterator>

using std::string;
using std::vector;

template<typename T, typename N>
bool isIncludes(const T &range, const N &item);

double putInput();

template<typename T>
std::stringstream joinListToStream(const std::vector<T> &list, const char* delim = ", ");

double putNumeric(const std::vector<int> &list = {}, const std::vector<int> &excludedList = {}, const std::string &msg = "");

#endif //INC_28_4_1_UTILITIES_H
