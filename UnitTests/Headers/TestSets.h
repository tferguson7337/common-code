#pragma once

// STL
#include <functional>
#include <string>

// SUTL
#include <UnitTestRunner.h>

using TestName = const std::string;
using TestSetFunc = std::function<bool(UnitTestRunner&)>;
using TestPair = std::pair<TestName, TestSetFunc>;
using TestFuncMap = std::vector<TestPair>;

extern const TestFuncMap s_TestFuncMap;

const TestSetFunc GetTestSetFunction(_In_ const std::string& str);

std::list<TestSetFunc> GetTestSetFunctions(_In_ const int& argc, _In_z_count_(argc) const char* argv[]);
