//
// Created by qzz on 2024/5/30.
//

#ifndef DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_UTILS_H_
#define DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_UTILS_H_
#include <algorithm>
#include <memory>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace doudizhu_learning_env {

constexpr int Combination(int n, int r) {
  if (r == 0)
    return 1;

  /*
   Extra computation saving for large R,
   using property:
   N choose R = N choose (N-R)
  */
  if (r > n / 2)
    return Combination(n, n - r);

  long res = 1;

  for (int k = 1; k <= r; ++k) {
    res *= n - k + 1;
    res /= k;
  }

  return res;
}

std::unordered_map<char, int> GetStringCounter(const std::string &str);

std::vector<int> Range(int start, int stop, int step = 1);

void CombineHelper(const std::vector<int> &nums, int k, int start,
                   std::vector<int> &current,
                   std::vector<std::vector<int>> &result);

std::vector<std::vector<int>> Combine(const std::vector<int> &nums, int k);

template <typename T>
std::vector<T> SortedCopy(const std::vector<T> &vec, bool ascending = true) {
  std::vector<T> sorted = vec;

  if (ascending) {
    std::sort(sorted.begin(), sorted.end());
  } else {
    std::sort(sorted.begin(), sorted.end(), std::greater<T>());
  }

  return sorted;
}

bool IsVectorConsecutive(const std::vector<int> &vec);

int FindNonContinuousNumber(const std::vector<int> &nums);

inline int CountUniqueElements(const std::vector<int> &vec) {
  std::unordered_set<int> unique_elements(vec.begin(), vec.end());
  return static_cast<int>(unique_elements.size());
}

template <typename Container>
std::unordered_map<int, std::vector<int>> FindIndices(const Container &arr,
                                         const std::vector<int> &values) {
  std::unordered_map<int, std::vector<int>> value_to_indices;
  for (const int value : values) {
    value_to_indices[value] = {};
  }

  // Populate the map with the indices of each value in the array.
  for (int i = 0; i < arr.size(); ++i) {
    int v = arr[i];
    if (std::find(values.begin(), values.end(), v) != values.end()) {
      value_to_indices[v].push_back(i);
    }
  }
  return value_to_indices;
}

bool HasKElemEqualWithTarget(const std::vector<int> &nums, int k, int target);

// Make sure that arbitrary structures can be printed out.
template <typename T>
std::ostream &operator<<(std::ostream &stream, const std::unique_ptr<T> &v);
template <typename T, typename U>
std::ostream &operator<<(std::ostream &stream, const std::pair<T, U> &v);
template <typename T>
std::ostream &operator<<(std::ostream &stream, const std::vector<T> &v);
template <typename T, std::size_t N>
std::ostream &operator<<(std::ostream &stream, const std::array<T, N> &v);
template <typename T>
std::ostream &operator<<(std::ostream &stream, const std::optional<T> &v);

template <typename T>
std::ostream &operator<<(std::ostream &stream, const std::vector<T> &v) {
  stream << "[";
  for (const auto &element : v) {
    stream << element << " ";
  }
  stream << "]";
  return stream;
}

template <typename T, std::size_t N>
std::ostream &operator<<(std::ostream &stream, const std::array<T, N> &v) {
  stream << "[";
  for (const auto &element : v) {
    stream << element << " ";
  }
  stream << "]";
  return stream;
}

template <typename T>
std::ostream &operator<<(std::ostream &stream, const std::unique_ptr<T> &v) {
  return stream << *v;
}

template <typename T, typename U>
std::ostream &operator<<(std::ostream &stream, const std::pair<T, U> &v) {
  stream << "(" << v.first << "," << v.second << ")";
  return stream;
}

template <typename T>
std::ostream &operator<<(std::ostream &stream, const std::optional<T> &v) {
  return stream << *v;
}

template <typename T>
T UniformSample(const std::vector<T> &vec, std::mt19937 &rng) {
  std::uniform_int_distribution<int> dis(0, vec.size() - 1);
  const int index = dis(rng);
  return vec[index];
}

using GameParameters = std::unordered_map<std::string, std::string>;

// Returns string associated with key in params, parsed as template type.
// If key is not in params, returns the provided default value.
template <class T>
T ParameterValue(const std::unordered_map<std::string, std::string> &params,
                 const std::string &key, T default_value);

template <>
int ParameterValue(const std::unordered_map<std::string, std::string> &params,
                   const std::string &key, int default_value);
template <>
double
ParameterValue(const std::unordered_map<std::string, std::string> &params,
               const std::string &key, double default_value);
template <>
std::string
ParameterValue(const std::unordered_map<std::string, std::string> &params,
               const std::string &key, std::string default_value);
template <>
bool ParameterValue(const std::unordered_map<std::string, std::string> &params,
                    const std::string &key, bool default_value);

// Report a runtime error.
[[noreturn]] void FatalError(const std::string &error_msg);

namespace internal {

template <typename Out, typename T> void StrOut(Out &out, const T &arg) {
  out << arg;
}

template <typename Out, typename T, typename... Args>
void StrOut(Out &out, const T &arg1, Args &&...args) {
  out << arg1;
  StrOut(out, std::forward<Args>(args)...);
}

template <typename... Args> std::string StrCat(Args &&...args) {
  std::ostringstream out;
  StrOut(out, std::forward<Args>(args)...);
  return out.str();
}

} // namespace internal

#define CHECK_OP(x_exp, op, y_exp)                                             \
  do {                                                                         \
    auto x = x_exp;                                                            \
    auto y = y_exp;                                                            \
    if (!((x)op(y)))                                                           \
      doudizhu_learning_env::FatalError(                                       \
          doudizhu_learning_env::internal::StrCat(                             \
              __FILE__, ":", __LINE__, " ", #x_exp " " #op " " #y_exp,         \
              "\n" #x_exp, " = ", x, ", " #y_exp " = ", y));                   \
  } while (false)

#define CHECK_EQ(x, y) CHECK_OP(x, ==, y)
#define CHECK_NE(x, y) CHECK_OP(x, !=, y)
#define CHECK_LE(x, y) CHECK_OP(x, <=, y)
#define CHECK_LT(x, y) CHECK_OP(x, <, y)
#define CHECK_LT(x, y) CHECK_OP(x, <, y)
#define CHECK_GE(x, y) CHECK_OP(x, >=, y)
#define CHECK_GT(x, y) CHECK_OP(x, >, y)

#define CHECK_TRUE(x)                                                          \
  while (!(x))                                                                 \
  doudizhu_learning_env::FatalError(doudizhu_learning_env::internal::StrCat(   \
      __FILE__, ":", __LINE__, " CHECK_TRUE(", #x, ")"))

#define CHECK_PROB(x)                                                          \
  CHECK_GE(x, 0);                                                              \
  CHECK_LE(x, 1);                                                              \
  CHECK_FALSE(std::isnan(x) || std::isinf(x))

#define CHECK_FALSE(x)                                                         \
  while (x)                                                                    \
  doudizhu_learning_env::FatalError(doudizhu_learning_env::internal::StrCat(   \
      __FILE__, ":", __LINE__, " CHECK_FALSE(", #x, ")"))

} // namespace doudizhu_learning_env
#endif // DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_UTILS_H_
