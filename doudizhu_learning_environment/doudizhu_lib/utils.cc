//
// Created by qzz on 2024/5/30.
//
#include <iostream>
#include <algorithm>
#include "utils.h"

namespace doudizhu_learning_env {
template<>
int ParameterValue<int>(
    const std::unordered_map<std::string, std::string> &params,
    const std::string &key, int default_value) {
  auto iter = params.find(key);
  if (iter == params.end()) {
    return default_value;
  }

  return std::stoi(iter->second);
}

template<>
std::string ParameterValue<std::string>(
    const std::unordered_map<std::string, std::string> &params,
    const std::string &key, std::string default_value) {
  auto iter = params.find(key);
  if (iter == params.end()) {
    return default_value;
  }

  return iter->second;
}

template<>
double ParameterValue<double>(
    const std::unordered_map<std::string, std::string> &params,
    const std::string &key, double default_value) {
  auto iter = params.find(key);
  if (iter == params.end()) {
    return default_value;
  }

  return std::stod(iter->second);
}

template<>
bool ParameterValue<bool>(
    const std::unordered_map<std::string, std::string> &params,
    const std::string &key, bool default_value) {
  auto iter = params.find(key);
  if (iter == params.end()) {
    return default_value;
  }

  return iter->second == "1" || iter->second == "true" ||
      iter->second == "True";
}
void FatalError(const std::string &error_msg) {
  std::cerr << "Fatal Error: " << error_msg << std::endl
            << std::endl
            << std::flush;
  std::exit(1);
}
std::vector<std::vector<int>> Combine(const std::vector<int> &nums, int k) {
  std::vector<std::vector<int>> result;
  std::vector<int> current;
  std::vector<int> sorted_nums = nums;
  std::sort(sorted_nums.begin(), sorted_nums.end());
  CombineHelper(sorted_nums, k, 0, current, result);
  return result;
}
void CombineHelper(const std::vector<int> &nums,
                   int k,
                   int start,
                   std::vector<int> &current,
                   std::vector<std::vector<int>> &result) {
  if (current.size() == k) {
    result.push_back(current);
    return;
  }

  for (int i = start; i < nums.size(); ++i) {
    if (i > start && nums[i] == nums[i - 1]) continue;
    current.push_back(nums[i]);
    CombineHelper(nums, k, i + 1, current, result);
    current.pop_back();
  }
}
std::vector<int> Range(int start, int stop, int step) {
  std::vector<int> result{};

  if (step == 0) {
    FatalError("The step is zero!");
  }

  if (step > 0) {
    for (int i = start; i < stop; i += step) {
      result.push_back(i);
    }
  } else {
    for (int i = start; i > stop; i += step) {
      result.push_back(i);
    }
  }

  return result;
}

}  // namespace doudizhu_learning_env