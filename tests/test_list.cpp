
#include <catch2/catch_all.hpp>
#include <libtcod/list.hpp>

/// Convert libtcod list to vector.
static std::vector<int> as_vector(TCODList<int> list) {
  auto result = std::vector<int>{};
  result.reserve(list.size());
  for (auto& it : list) result.push_back(it);
  return result;
}
/// Convert libtcod list to vector.
static std::vector<int> as_vector(TCOD_List* list) {
  auto result = std::vector<int>{};
  result.reserve(TCOD_list_size(list));
  for (void** it = TCOD_list_begin(list); it != TCOD_list_end(list); ++it) {
    result.push_back(static_cast<int>(reinterpret_cast<intptr_t>(*it)));
  }
  return result;
}

TEST_CASE("TCOD_List") {
  TCOD_list_t int_list = TCOD_list_new();
  TCOD_list_push(int_list, (void*)3);
  TCOD_list_push(int_list, (void*)5);
  REQUIRE(TCOD_list_contains(int_list, (void*)5));
  REQUIRE_FALSE(TCOD_list_contains(int_list, (void*)0));
  REQUIRE(as_vector(int_list) == std::vector{3, 5});
  {
    TCOD_list_t int_list2 = TCOD_list_duplicate(int_list);
    REQUIRE(as_vector(int_list) == as_vector(int_list2));
    TCOD_list_delete(int_list2);
  }

  TCOD_list_set(int_list, (void*)1, 0);
  TCOD_list_set(int_list, (void*)7, 2);
  REQUIRE(as_vector(int_list) == std::vector{1, 5, 7});
  TCOD_list_reverse(int_list);
  REQUIRE(as_vector(int_list) == std::vector{7, 5, 1});

  TCOD_list_remove(int_list, (void*)5);
  REQUIRE(as_vector(int_list) == std::vector{7, 1});
  TCOD_list_reverse(int_list);
  REQUIRE(as_vector(int_list) == std::vector{1, 7});

  TCOD_list_clear(int_list);
  REQUIRE(as_vector(int_list) == std::vector<int>{});

  REQUIRE(TCOD_list_pop(int_list) == nullptr);
  TCOD_list_delete(int_list);
}

TEST_CASE("TCOD_List iter") {
  TCOD_list_t int_list = TCOD_list_new();
  TCOD_list_push(int_list, (void*)0);
  TCOD_list_push(int_list, (void*)1);
  TCOD_list_push(int_list, (void*)2);
  TCOD_list_push(int_list, (void*)3);
  void** it = TCOD_list_begin(int_list);
  REQUIRE(as_vector(int_list) == std::vector{0, 1, 2, 3});
  it = TCOD_list_remove_iterator(int_list, it);
  ++it;
  REQUIRE((intptr_t)(*it) == 1);
  REQUIRE(as_vector(int_list) == std::vector{1, 2, 3});
  it = TCOD_list_remove_iterator_fast(int_list, it);
  REQUIRE(as_vector(int_list) == std::vector{3, 2});
  TCOD_list_delete(int_list);
}

TEST_CASE("TCODList") {
  auto int_list = TCODList<int>(5);
  REQUIRE(int_list.size() == 0);
  REQUIRE(int_list.isEmpty());
  REQUIRE(as_vector(int_list) == std::vector<int>{});
  int_list.push(3);
  REQUIRE(as_vector(int_list) == std::vector{3});
  int_list.push(5);
  REQUIRE(as_vector(int_list) == std::vector{3, 5});
  REQUIRE(int_list.contains(5));
  REQUIRE_FALSE(int_list.contains(0));

  int_list.set(1, 0);
  REQUIRE(as_vector(int_list) == std::vector{1, 5});
  int_list.set(7, 2);
  REQUIRE(as_vector(int_list) == std::vector{1, 5, 7});
  int_list.reverse();
  REQUIRE(as_vector(int_list) == std::vector{7, 5, 1});

  int_list.remove(5);
  REQUIRE(as_vector(int_list) == std::vector{7, 1});
  int_list.reverse();
  REQUIRE(as_vector(int_list) == std::vector{1, 7});

  int_list.clear();
  REQUIRE(as_vector(int_list) == std::vector<int>{});

  REQUIRE(int_list.pop() == 0);
}

TEST_CASE("TCODList iter") {
  auto int_list = TCODList<int>();
  int_list.push(0);
  int_list.push(1);
  int_list.push(2);
  int_list.push(3);
  auto* it = int_list.begin();
  REQUIRE(as_vector(int_list) == std::vector{0, 1, 2, 3});
  it = int_list.remove(it);
  ++it;
  REQUIRE(*it == 1);
  REQUIRE(as_vector(int_list) == std::vector{1, 2, 3});
  it = int_list.removeFast(it);
  REQUIRE(as_vector(int_list) == std::vector{3, 2});
}
