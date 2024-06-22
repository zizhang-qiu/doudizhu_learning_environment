add_library(doudizhu utils.h
        utils.cc
        doudizhu_card.cc
        doudizhu_card.h
        doudizhu_utils.cc
        doudizhu_utils.h
        doudizhu_hand.cc
        doudizhu_hand.h
        doudizhu_move.cc
        doudizhu_move.h
        doudizhu_game.cc
        doudizhu_game.h
        doudizhu_deck.cc
        doudizhu_deck.h
        doudizhu_state.cc
        doudizhu_state.h
        doudizhu_history_item.cc
        doudizhu_history_item.h
        doudizhu_observation.cc
        doudizhu_observation.h
        observation_encoder.h
        douzero_encoder.cc
        douzero_encoder.h
)

target_include_directories(doudizhu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


add_executable(doudizhu_test doudizhu_test.cc)
target_link_libraries(doudizhu_test doudizhu)