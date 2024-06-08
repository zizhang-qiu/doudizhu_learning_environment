//
// Created by qzz on 2024/6/6.
//
#include <iostream>
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "doudizhu_state.h"

namespace py = pybind11;

namespace doudizhu_learning_env {

void CheckPyTupleSize(const py::tuple &t, const size_t size) {
  if (t.size() != size) {
    std::cerr << "The tuple needs " << size << " items, but got " << t.size() <<
              " items!" << std::endl;
    std::abort();
  }
}

void Test(std::vector<int> &arr) {
  ++arr[0];
  ++arr[1];
}

PYBIND11_MODULE(pydoudizhu, m) {
  py::enum_<Suit>(m, "Suit")
      .value("INVALID_SUIT", Suit::kInvalidSuit)
      .value("CLUBS_SUIT", Suit::kClubsSuit)
      .value("DIAMOND_SUIT", Suit::kDiamondSuit)
      .value("SPADE_SUIT", Suit::kSpadeSuit)
      .export_values();

  m.attr("BLACK_JOKER") = kBlackJoker;
  m.attr("RED_JOKER") = kRedJoker;
  m.attr("ALL_SUITS") = kAllSuits;
  m.attr("NUM_RANKS") = kNumRanks;

  m.def("test", &Test);

  py::class_<DoudizhuCard>(m, "DoudizhuCard")
      .def(py::init<>())
      .def(py::init<int, Suit>(), py::arg("rank"), py::arg("suit"))
      .def("__eq__", &DoudizhuCard::operator==)
      .def("is_valid", &DoudizhuCard::IsValid)
      .def("is_joker", &DoudizhuCard::IsJoker)
      .def("__repr__", &DoudizhuCard::ToString)
      .def("card_rank", &DoudizhuCard::CardRank)
      .def("card_suit", &DoudizhuCard::CardSuit)
      .def(py::pickle(
          [](const DoudizhuCard &card) {
            // __getstate__
            return py::make_tuple(card.CardRank(), card.CardSuit());
          },
          [](const py::tuple &t) {
            // __setstate__
            const DoudizhuCard card{t[0].cast<int>(), t[1].cast<Suit>()};
            return card;
          }
      ));

  m.def("uid_2_rank", &Uid2Rank);
  m.def("uid_2_suit", &Uid2Suit);
  m.def("card_index", py::overload_cast<int, Suit>(&CardIndex));
  m.def("card_index", py::overload_cast<const DoudizhuCard &>(&CardIndex));

  py::class_<DoudizhuHand>(m, "DoudizhuHand")
      .def(py::init<>())
      .def("add_card", py::overload_cast<const DoudizhuCard &>(&DoudizhuHand::AddCard), py::arg("card"))
      .def("add_card", py::overload_cast<int>(&DoudizhuHand::AddCard), py::arg("rank"))
      .def("remove_from_hand", py::overload_cast<int>(&DoudizhuHand::RemoveFromHand), py::arg("rank"))
      .def("size", &DoudizhuHand::Size)
      .def("cards_per_rank", &DoudizhuHand::CardsPerRank)
      .def("can_add", &DoudizhuHand::CanAdd, py::arg("rank"))
      .def("can_remove", &DoudizhuHand::CanRemove, py::arg("rank"))
      .def("__repr__", &DoudizhuHand::ToString)
      .def("__eq__", &DoudizhuHand::operator==)
      .def(py::pickle(
          [](const DoudizhuHand &hand) {
            // __getstate__
            return py::make_tuple(hand.CardsPerRank());
          },
          [](const py::tuple &t) {
            // __setstate__
            DoudizhuHand hand{};
            const auto cards_per_rank = t[0].cast<std::array<uint8_t, kNumRanks>>();
            for (int i = 0; i < kNumRanks; ++i) {
              const int num_cards_this_rank = cards_per_rank[i];
              for (int j = 0; j < num_cards_this_rank; ++j) {
                hand.AddCard(i);
              }
            }
            return hand;
          }
      ));

}
}