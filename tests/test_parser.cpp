#include <catch2/catch_all.hpp>
#include <libtcod/parser.hpp>

#include "common.hpp"

TEST_CASE("TCODParser") {
  TCOD_clear_error();
  TCODParser parser{};

  TCODParserStruct* myStruct = parser.newStructure("myStruct");
  myStruct->addProperty("bool_field", TCOD_TYPE_BOOL, true);
  myStruct->addProperty("char_field", TCOD_TYPE_CHAR, true);
  myStruct->addProperty("int_field", TCOD_TYPE_INT, true);
  myStruct->addProperty("float_field", TCOD_TYPE_FLOAT, true);
  myStruct->addProperty("string_field", TCOD_TYPE_STRING, true);
  myStruct->addProperty("color_field", TCOD_TYPE_COLOR, true);
  myStruct->addProperty("dice_field", TCOD_TYPE_DICE, true);

  myStruct->addListProperty("bool_list", TCOD_TYPE_BOOL, true);
  myStruct->addListProperty("char_list", TCOD_TYPE_CHAR, true);
  myStruct->addListProperty("integer_list", TCOD_TYPE_INT, true);
  myStruct->addListProperty("float_list", TCOD_TYPE_FLOAT, true);
  myStruct->addListProperty("string_list", TCOD_TYPE_STRING, true);
  myStruct->addListProperty("color_list", TCOD_TYPE_COLOR, true);
  myStruct->addListProperty("dice_list", TCOD_TYPE_DICE, true);

  parser.run(get_file("cfg/sample.cfg").c_str(), NULL);
  REQUIRE(TCOD_get_error() == std::string(""));

  REQUIRE(parser.getBoolProperty("myStruct.bool_field") == true);
  REQUIRE(parser.getCharProperty("myStruct.char_field") == 'Z');
  REQUIRE(parser.getIntProperty("myStruct.int_field") == 24);
  REQUIRE(parser.getFloatProperty("myStruct.float_field") == Catch::Approx(3.14));
  REQUIRE(parser.getStringProperty("myStruct.string_field") == std::string("hello"));
  REQUIRE(parser.getColorProperty("myStruct.color_field") == tcod::ColorRGB{255, 128, 128});
  parser.getDiceProperty("myStruct.dice_field");

  REQUIRE(parser.getBoolProperty("myStruct.bool_field2") == false);
  REQUIRE(parser.getCharProperty("myStruct.char_field2") == '@');
  REQUIRE(parser.getIntProperty("myStruct.int_field2") == 4);
  REQUIRE(parser.getFloatProperty("myStruct.float_field2") == Catch::Approx(4.3));
  REQUIRE(parser.getStringProperty("myStruct.string_field2") == std::string("world"));
  REQUIRE(parser.getColorProperty("myStruct.color_field2") == tcod::ColorRGB{0xFF, 0x22, 0xCC});
  parser.getDiceProperty("myStruct.dice_field2");

  parser.getListProperty("myStruct.bool_list", TCOD_TYPE_BOOL);
  parser.getListProperty("myStruct.char_list", TCOD_TYPE_CHAR);
  parser.getListProperty("myStruct.integer_list", TCOD_TYPE_INT);
  parser.getListProperty("myStruct.float_list", TCOD_TYPE_FLOAT);
  parser.getListProperty("myStruct.string_list", TCOD_TYPE_STRING);
  parser.getListProperty("myStruct.color_list", TCOD_TYPE_COLOR);
  parser.getListProperty("myStruct.dice_list", TCOD_TYPE_DICE);

  parser.getListProperty("myStruct.bool_list2", TCOD_TYPE_BOOL);
  parser.getListProperty("myStruct.char_list2", TCOD_TYPE_CHAR);
  parser.getListProperty("myStruct.integer_list2", TCOD_TYPE_INT);
  parser.getListProperty("myStruct.float_list2", TCOD_TYPE_FLOAT);
  parser.getListProperty("myStruct.string_list2", TCOD_TYPE_STRING);
  parser.getListProperty("myStruct.color_list2", TCOD_TYPE_COLOR);

  REQUIRE(parser.getIntProperty("dynStruct.someVar") == 4);
  REQUIRE(parser.getFloatProperty("dynStruct.subStruct.anotherVar") == Catch::Approx(4.3));

  REQUIRE(parser.hasProperty("dynStruct.someVar"));
  REQUIRE(!parser.hasProperty("nonexistant"));

  REQUIRE(TCOD_get_error() == std::string(""));
}
