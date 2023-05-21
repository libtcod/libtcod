
#include <catch2/catch_all.hpp>
#include <libtcod/bsp.hpp>

TEST_CASE("BSP traversal") {
  auto root = TCODBsp{};
  root.splitOnce(0, 0);
  root.getLeft()->splitOnce(0, 0);

  auto pre_order = std::vector<TCODBsp*>{};
  REQUIRE(root.traversePreOrder([&](TCODBsp& node) { pre_order.push_back(&node); }));
  REQUIRE(pre_order.at(0) == &root);
  REQUIRE(pre_order.at(1) == root.getLeft());
  REQUIRE(pre_order.at(2) == root.getLeft()->getLeft());
  REQUIRE(pre_order.at(3) == root.getLeft()->getRight());
  REQUIRE(pre_order.at(4) == root.getRight());
  REQUIRE(root.traverseInOrder([&](TCODBsp&) { return true; }));
  REQUIRE(root.traversePostOrder([&](TCODBsp&) { return true; }));
  REQUIRE(root.traverseLevelOrder([&](TCODBsp&) { return true; }));
  REQUIRE(root.traverseInvertedLevelOrder([&](TCODBsp&) { return true; }));
};
