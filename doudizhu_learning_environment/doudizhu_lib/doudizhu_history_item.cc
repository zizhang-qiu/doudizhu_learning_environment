//
// Created by qzz on 2024/6/5.
//

#include "doudizhu_history_item.h"
namespace doudizhu_learning_env {

std::string DoudizhuHistoryItem::ToString() const {
  std::string str = "<" + move.ToString();
  if (player >= 0) {
    str += " by player " + std::to_string(player);
  }
  str += ">";
  return str;
}
}