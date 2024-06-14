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
std::unordered_map<char, int> GetStringCounter(const std::string &str) {
  std::unordered_map<char, int> counter{};
  for (const char &c : str) {
    ++counter[c];
  }
  return counter;
}
bool IsVectorConsecutive(const std::vector<int> &vec) {
  if (vec.size() <= 1) {
    return true;
  }
  const auto sorted_vec = SortedCopy(vec, /*ascending=*/true);
  for (int i = 1; i < static_cast<int>(sorted_vec.size()); ++i) {
    if (sorted_vec[i] != sorted_vec[0] + i) {
      return false;
    }
  }
  return true;
}
int FindNonContinuousNumber(const std::vector<int> &nums) {
  if (nums.size() < 2) {
    throw std::invalid_argument("The array must have at least two elements.");
  }

  std::vector<int> sortedNums = nums;
  std::sort(sortedNums.begin(), sortedNums.end());

  if (sortedNums[1] != sortedNums[0] + 1) {
    return sortedNums[0];
  }

  if (sortedNums[sortedNums.size() - 1] != sortedNums[sortedNums.size() - 2] + 1) {
    return sortedNums[sortedNums.size() - 1];
  }

  for (size_t i = 1; i < sortedNums.size() - 1; ++i) {
    if (sortedNums[i] != sortedNums[i - 1] + 1 && sortedNums[i] != sortedNums[i + 1] - 1) {
      return sortedNums[i];
    }
  }

  throw std::runtime_error("No non-continuous number found.");
}

}  // namespace doudizhu_learning_env