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

  py::enum_<DoudizhuMove::Type>(m, "MoveType")
      .value("INVALID", DoudizhuMove::Type::kInvalid)
      .value("DEAL", DoudizhuMove::Type::kDeal)
      .value("AUCTION", DoudizhuMove::Type::kAuction)
      .value("PLAY", DoudizhuMove::Type::kPlay)
      .export_values();

  py::enum_<DoudizhuMove::AuctionType>(m, "AuctionType")
      .value("INVALID", DoudizhuMove::AuctionType::kInvalid)
      .value("PASS", DoudizhuMove::AuctionType::kPass)
      .value("ONE", DoudizhuMove::AuctionType::kOne)
      .value("TWO", DoudizhuMove::AuctionType::kTwo)
      .value("THREE", DoudizhuMove::AuctionType::kThree)
      .export_values();

  py::enum_<DoudizhuMove::PlayType>(m, "PlayType")
      .value("INVALID", DoudizhuMove::PlayType::kInvalid)
      .value("PASS", DoudizhuMove::PlayType::kPass)
      .value("SOLO", DoudizhuMove::PlayType::kSolo)
      .value("PAIR", DoudizhuMove::PlayType::kPair)
      .value("TRIO", DoudizhuMove::PlayType::kTrio)
      .value("BOMB", DoudizhuMove::PlayType::kBomb)
      .value("TRIO_WITH_SOLO", DoudizhuMove::PlayType::kTrioWithSolo)
      .value("TRIO_WITH_PAIR", DoudizhuMove::PlayType::kTrioWithPair)
      .value("CHAIN_OF_SOLO", DoudizhuMove::PlayType::kChainOfSolo)
      .value("CHAIN_OF_PAIR", DoudizhuMove::PlayType::kChainOfPair)
      .value("CHAIN_OF_TRIO", DoudizhuMove::PlayType::kChainOfTrio)
      .value("PLANE_WITH_SOLO", DoudizhuMove::PlayType::kPlaneWithSolo)
      .value("PLANE_WITH_PAIR", DoudizhuMove::PlayType::kPlaneWithPair)
      .value("QUAD_WITH_SOLO", DoudizhuMove::PlayType::kQuadWithSolo)
      .value("QUAD_WITH_PAIR", DoudizhuMove::PlayType::kQuadWithPair)
      .value("ROCKET", DoudizhuMove::PlayType::kRocket)
      .export_values();

  py::class_<SingleRank>(m, "SingleRank")
      .def(py::init<>())
      .def(py::init<int, int>(), py::arg("r"), py::arg("n"))
      .def("__eq__", &SingleRank::operator==)
      .def_readonly("rank", &SingleRank::rank)
      .def_readonly("num_cards", &SingleRank::num_cards)
      .def(py::pickle(
          [](const SingleRank &single_rank) {
            return py::make_tuple(single_rank.rank, single_rank.num_cards);
          },
          [](const py::tuple &t) {
            CheckPyTupleSize(t, 2);
            const auto rank = t[0].cast<int>();
            const auto num_cards = t[1].cast<int>();
            return SingleRank{/*r=*/rank, /*n=*/num_cards};
          }
      ));

  py::enum_<ChainType>(m, "ChainType")
      .value("NOT_CHAIN", ChainType::kNotChain)
      .value("SOLO", ChainType::kSolo)
      .value("PAIR", ChainType::kPair)
      .value("TRIO", ChainType::kTrio)
      .export_values();

  py::class_<Chain>(m, "Chain")
      .def(py::init<>())
      .def(py::init<ChainType, int, int>(), py::arg("chain_type"), py::arg("length"), py::arg("start_rank"))
      .def("__eq__", &Chain::operator==)
      .def_readonly("length", &Chain::length)
      .def_readonly("start_rank", &Chain::start_rank)
      .def_readonly("chain_type", &Chain::chain_type)
      .def(py::pickle(
          [](const Chain &chain) {
            return py::make_tuple(chain.chain_type, chain.length, chain.start_rank);
          },
          [](const py::tuple &t) {
            CheckPyTupleSize(t, 3);
            const auto chain_type = t[0].cast<ChainType>();
            const auto length = t[1].cast<int>();
            const auto start_rank = t[2].cast<int>();
            return Chain{/*chain_type=*/chain_type, /*length=*/length, /*start_rank=*/start_rank};
          }
      ));

  py::enum_<KickerType>(m, "KickerType")
      .value("UNKNOWN", KickerType::kUnknown)
      .value("SOLO", KickerType::kSolo)
      .value("PAIR", KickerType::kPair)
      .export_values();

  py::class_<TrioComb>(m, "TrioComb")
      .def(py::init<>())
      .def(py::init<KickerType, int>(), py::arg("kt"), py::arg("tr"))
      .def("__eq__", &TrioComb::operator==)
      .def_readonly("kicker_type", &TrioComb::kicker_type)
      .def_readonly("trio_rank", &TrioComb::trio_rank)
      .def(py::pickle(
          [](const TrioComb &trio_comb) {
            return py::make_tuple(trio_comb.kicker_type, trio_comb.trio_rank);
          },
          [](const py::tuple &t) {
            CheckPyTupleSize(t, 2);
            const auto kicker_type = t[0].cast<KickerType>();
            const auto trio_rank = t[1].cast<int>();
            return TrioComb{/*kt*/kicker_type, /*tr=*/trio_rank};
          }
      ));

  py::class_<QuadComb>(m, "QuadComb")
      .def(py::init<>())
      .def(py::init<KickerType, int>(), py::arg("kt"), py::arg("qr"))
      .def("__eq__", &QuadComb::operator==)
      .def_readonly("kicker_type", &QuadComb::kicker_type)
      .def_readonly("quad_rank", &QuadComb::quad_rank)
      .def(py::pickle(
          [](const QuadComb &quad_comb) {
            return py::make_tuple(quad_comb.kicker_type, quad_comb.quad_rank);
          },
          [](const py::tuple &t) {
            CheckPyTupleSize(t, 2);
            const auto kicker_type = t[0].cast<KickerType>();
            const auto quad_rank = t[1].cast<int>();
            return QuadComb{/*kt*/kicker_type, /*qr=*/quad_rank};
          }
      ));;

  m.def("get_possible_kickers",
        py::overload_cast<const TrioComb &>(&GetPossibleKickers), py::arg("trio_comb"));

  m.def("get_possible_kickers",
        py::overload_cast<const QuadComb &>(&GetPossibleKickers), py::arg("quad_comb"));

  py::class_<Plane>(m, "Plane")
      .def(py::init<>())
      .def(py::init<KickerType, int, int>(),
           py::arg("kt"), py::arg("l"), py::arg("sr"))
      .def("__eq__", &Plane::operator==)
      .def_readonly("kicker_type", &Plane::kicker_type)
      .def_readonly("length", &Plane::length)
      .def_readonly("start_rank", &Plane::start_rank)
      .def(py::pickle(
          [](const Plane &plane) {
            return py::make_tuple(plane.kicker_type, plane.length, plane.start_rank);
          },
          [](const py::tuple &t) {
            CheckPyTupleSize(t, 3);
            const auto kicker_type = t[0].cast<KickerType>();
            const auto length = t[1].cast<int>();
            const auto start_rank = t[2].cast<int>();
            return Plane{/*kt=*/kicker_type, /*l=*/length, /*sr=*/start_rank};
          }
      ));

  m.def("get_possible_kickers",
        py::overload_cast<const Plane &>(&GetPossibleKickers), py::arg("plane"));

  py::class_<DoudizhuMove>(m, "DoudizhuMove")
      .def(py::init<>())
      .def(py::init<DoudizhuMove::Type,
                    DoudizhuMove::AuctionType,
                    DoudizhuMove::PlayType,
                    const DoudizhuCard &,
                    SingleRank,
                    Chain,
                    TrioComb,
                    QuadComb,
                    Plane,
                    const std::vector<int> &>(),
           py::arg("move_type"),
           py::arg("auction_type"),
           py::arg("play_type"),
           py::arg("deal_card"),
           py::arg("single_rank"),
           py::arg("chain"),
           py::arg("trio_comb"),
           py::arg("quad_comb"),
           py::arg("plane"),
           py::arg("kickers"))
      .def(py::init<const DoudizhuCard &>(), py::arg("deal_card"))
      .def(py::init<DoudizhuMove::AuctionType>(), py::arg("auction_type"))
      .def(py::init<DoudizhuMove::PlayType,
                    SingleRank, Chain, TrioComb, QuadComb, Plane,
                    const std::vector<int> &>(),
           py::arg("play_type"), py::arg("single_rank"),
           py::arg("chain"), py::arg("trio_comb"),
           py::arg("quad_comb"), py::arg("plane"),
           py::arg("kickers"))
      .def(py::init<SingleRank>(), py::arg("single_rank"))
      .def(py::init<Chain>(), py::arg("chain"))
      .def(py::init<TrioComb, const std::vector<int> &>(), py::arg("trio_comb"), py::arg("kickers"))
      .def(py::init<QuadComb, const std::vector<int> &>(), py::arg("quad_comb"), py::arg("kickers"))
      .def(py::init<Plane, const std::vector<int> &>(), py::arg("plane"), py::arg("kickers"))
      .def(py::init<DoudizhuMove::PlayType>(), py::arg("play_type"))
      .def("__repr__", &DoudizhuMove::ToString)
      .def("move_type", &DoudizhuMove::MoveType)
      .def("deal_card", &DoudizhuMove::DealCard)
      .def("auction", &DoudizhuMove::Auction)
      .def("get_play_type", &DoudizhuMove::GetPlayType)
      .def("get_single_rank", &DoudizhuMove::GetSingleRank)
      .def("get_chain", &DoudizhuMove::GetChain)
      .def("get_trio_comb", &DoudizhuMove::GetTrioComb)
      .def("get_quad_comb", &DoudizhuMove::GetQuadComb)
      .def("get_plane", &DoudizhuMove::GetPlane)
      .def("kickers", &DoudizhuMove::Kickers)
      .def("is_bomb", &DoudizhuMove::IsBomb)
      .def("is_valid", &DoudizhuMove::IsValid, py::arg("check_kickers") = false)
      .def("bomb_rank", &DoudizhuMove::BombRank)
      .def("to_ranks", &DoudizhuMove::ToRanks)
      .def("__eq__", &DoudizhuMove::operator==)
      .def(py::pickle(
          [](const DoudizhuMove &move) {
            //__getstate__
            return py::make_tuple(
                move.MoveType(),
                move.Auction(),
                move.GetPlayType(),
                move.DealCard(),
                move.GetSingleRank(),
                move.GetChain(),
                move.GetTrioComb(),
                move.GetQuadComb(),
                move.GetPlane(),
                move.Kickers()
            );
          },
          [](const py::tuple &t) {
            //__setstate__
            CheckPyTupleSize(t, 10);
            const auto move_type = t[0].cast<DoudizhuMove::Type>();
            const auto auction_type = t[1].cast<DoudizhuMove::AuctionType>();
            const auto play_type = t[2].cast<DoudizhuMove::PlayType>();
            const auto deal_card = t[3].cast<DoudizhuCard>();
            const auto single_rank = t[4].cast<SingleRank>();
            const auto chain = t[5].cast<Chain>();
            const auto trio_comb = t[6].cast<TrioComb>();
            const auto quad_comb = t[7].cast<QuadComb>();
            const auto plane = t[8].cast<Plane>();
            const auto kickers = t[9].cast<std::vector<int>>();
            return DoudizhuMove{
                /*move_type=*/move_type,
                /*auction_type=*/auction_type,
                /*play_type=*/play_type,
                /*deal_card=*/deal_card,
                /*single_rank=*/single_rank,
                /*chain=*/chain,
                /*trio_comb=*/trio_comb,
                /*quad_comb=*/quad_comb,
                /*plane=*/plane,
                /*kickers=*/kickers
            };
          }
      ));

}
}