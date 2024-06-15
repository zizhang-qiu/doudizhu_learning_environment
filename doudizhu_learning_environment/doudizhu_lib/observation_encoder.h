//
// Created by qzz on 2024/6/15.
//

#ifndef DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_OBSERVATION_ENCODER_H_
#define DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_OBSERVATION_ENCODER_H_
#include "doudizhu_observation.h"
namespace doudizhu_learning_env {
struct Feature {
  std::vector<int> encoding;
  std::vector<int> dims;
  std::string desc{};
};

using Features = std::unordered_map<std::string, Feature>;

class ObservationEncoder {
 public:
  virtual ~ObservationEncoder() = default;

  virtual Features Encode(const DoudizhuObservation &obs) = 0;
};
}
#endif //DOUDIZHU_LEARNING_ENVIRONMENT_DOUDIZHU_LIB_OBSERVATION_ENCODER_H_
