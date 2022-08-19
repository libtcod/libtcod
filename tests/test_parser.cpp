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

TEST_CASE("Parse sample2.cfg") {
  TCOD_clear_error();
  TCODParser parser{};
  TCODParserStruct* config = parser.newStructure("config");
  config->addProperty("debug", TCOD_TYPE_BOOL, false);
  config->addProperty("multithread", TCOD_TYPE_BOOL, false);
  config->addProperty("threadPoolSize", TCOD_TYPE_INT, false);

  TCODParserStruct* display = parser.newStructure("display");
  display->addProperty("wallColor", TCOD_TYPE_COLOR, true);
  display->addProperty("groundColor", TCOD_TYPE_COLOR, true);
  display->addProperty("memoryWallColor", TCOD_TYPE_COLOR, true);
  display->addProperty("playerLightRange", TCOD_TYPE_INT, true);
  display->addProperty("playerLightColor", TCOD_TYPE_COLOR, true);
  display->addProperty("playerLightColorEnd", TCOD_TYPE_COLOR, true);
  display->addProperty("messageLife", TCOD_TYPE_FLOAT, true);
  display->addProperty("debugColor", TCOD_TYPE_COLOR, true);
  display->addProperty("infoColor", TCOD_TYPE_COLOR, true);
  display->addProperty("warnColor", TCOD_TYPE_COLOR, true);
  display->addProperty("criticalColor", TCOD_TYPE_COLOR, true);
  display->addProperty("fadeTime", TCOD_TYPE_FLOAT, true);
  display->addProperty("fireSpeed", TCOD_TYPE_FLOAT, true);
  display->addProperty("hitFlashDelay", TCOD_TYPE_FLOAT, true);
  display->addProperty("flashColor", TCOD_TYPE_COLOR, true);
  display->addProperty("corpseColor", TCOD_TYPE_COLOR, true);
  display->addProperty("treasureLightColor", TCOD_TYPE_COLOR, true);
  display->addProperty("treasureLightRange", TCOD_TYPE_FLOAT, true);
  display->addProperty("treasureIntensityDelay", TCOD_TYPE_FLOAT, true);
  display->addProperty("treasureIntensityPattern", TCOD_TYPE_STRING, true);
  display->addProperty("finalExplosionTime", TCOD_TYPE_FLOAT, true);
  config->addStructure(display);

  TCODParserStruct* fog = parser.newStructure("fog");
  fog->addProperty("maxLevel", TCOD_TYPE_FLOAT, true);
  fog->addProperty("scale", TCOD_TYPE_FLOAT, true);
  fog->addProperty("octaves", TCOD_TYPE_FLOAT, true);
  fog->addProperty("speed", TCOD_TYPE_FLOAT, true);
  fog->addProperty("color", TCOD_TYPE_COLOR, true);
  config->addStructure(fog);

  TCODParserStruct* spells = parser.newStructure("spells");

  TCODParserStruct* fireball = parser.newStructure("fireball");
  fireball->addProperty("lightColor", TCOD_TYPE_COLOR, true);
  fireball->addProperty("trailLength", TCOD_TYPE_INT, true);
  fireball->addProperty("speed", TCOD_TYPE_FLOAT, true);
  fireball->addProperty("sparkLife", TCOD_TYPE_FLOAT, true);
  fireball->addProperty("sparkleLife", TCOD_TYPE_FLOAT, true);
  fireball->addProperty("standardLife", TCOD_TYPE_FLOAT, true);
  fireball->addProperty("sparkleSpeed", TCOD_TYPE_FLOAT, true);
  fireball->addProperty("baseRange", TCOD_TYPE_FLOAT, true);
  fireball->addProperty("baseDamage", TCOD_TYPE_FLOAT, true);
  fireball->addProperty("stunDelay", TCOD_TYPE_FLOAT, true);
  spells->addStructure(fireball);

  config->addStructure(spells);

  TCODParserStruct* gameplay = parser.newStructure("gameplay");
  gameplay->addProperty("timeScale", TCOD_TYPE_FLOAT, true);
  gameplay->addProperty("nbLevels", TCOD_TYPE_INT, true);
  gameplay->addProperty("dungeonMinSize", TCOD_TYPE_INT, true);
  gameplay->addProperty("dungeonMaxSize", TCOD_TYPE_INT, true);
  gameplay->addProperty("penumbraLevel", TCOD_TYPE_INT, true);
  gameplay->addProperty("darknessLevel", TCOD_TYPE_INT, true);
  config->addStructure(gameplay);

  TCODParserStruct* ai_director = parser.newStructure("ai_director");
  ai_director->addProperty("waveLength", TCOD_TYPE_FLOAT, true);
  ai_director->addProperty("lowLevel", TCOD_TYPE_FLOAT, true);
  ai_director->addProperty("medLevel", TCOD_TYPE_FLOAT, true);
  ai_director->addProperty("medRate", TCOD_TYPE_FLOAT, true);
  ai_director->addProperty("highRate", TCOD_TYPE_FLOAT, true);
  ai_director->addProperty("maxCreatures", TCOD_TYPE_INT, true);
  ai_director->addProperty("spawnSourceRange", TCOD_TYPE_INT, true);
  ai_director->addProperty("hordeDelay", TCOD_TYPE_INT, true);
  ai_director->addProperty("distReplace", TCOD_TYPE_INT, true);
  ai_director->addProperty("itemKillCount", TCOD_TYPE_INT, true);
  config->addStructure(ai_director);

  TCODParserStruct* creatures = parser.newStructure("creatures");
  creatures->addProperty("burnDamage", TCOD_TYPE_FLOAT, true);
  creatures->addProperty("pathDelay", TCOD_TYPE_FLOAT, true);

  TCODParserStruct* minion = parser.newStructure("minion");
  minion->addProperty("char", TCOD_TYPE_CHAR, true);
  minion->addProperty("color", TCOD_TYPE_COLOR, true);
  minion->addProperty("life", TCOD_TYPE_INT, true);
  minion->addProperty("speed", TCOD_TYPE_FLOAT, true);
  minion->addProperty("damage", TCOD_TYPE_FLOAT, true);
  creatures->addStructure(minion);

  TCODParserStruct* boss = parser.newStructure("boss");
  boss->addProperty("char", TCOD_TYPE_CHAR, true);
  boss->addProperty("color", TCOD_TYPE_COLOR, true);
  boss->addProperty("life", TCOD_TYPE_INT, true);
  boss->addProperty("speed", TCOD_TYPE_FLOAT, true);
  boss->addProperty("secureDist", TCOD_TYPE_INT, true);
  boss->addProperty("secureCoef", TCOD_TYPE_FLOAT, true);
  boss->addProperty("summonTime", TCOD_TYPE_FLOAT, true);
  boss->addProperty("minionCount", TCOD_TYPE_INT, true);
  creatures->addStructure(boss);

  TCODParserStruct* player = parser.newStructure("player");
  player->addProperty("char", TCOD_TYPE_CHAR, true);
  player->addProperty("color", TCOD_TYPE_COLOR, true);
  player->addProperty("speed", TCOD_TYPE_FLOAT, true);
  player->addProperty("sprintLength", TCOD_TYPE_FLOAT, true);
  player->addProperty("sprintRecovery", TCOD_TYPE_FLOAT, true);
  player->addProperty("rangeAccommodation", TCOD_TYPE_FLOAT, true);
  player->addProperty("maxPathFinding", TCOD_TYPE_INT, true);
  player->addProperty("healRate", TCOD_TYPE_FLOAT, true);
  player->addProperty("healIntensityDelay", TCOD_TYPE_FLOAT, true);
  player->addProperty("healIntensityPattern", TCOD_TYPE_STRING, true);
  player->addProperty("longButtonDelay", TCOD_TYPE_FLOAT, true);
  player->addProperty("longSpellDelay", TCOD_TYPE_FLOAT, true);
  player->addProperty("moveUpKey", TCOD_TYPE_CHAR, true);
  player->addProperty("moveDownKey", TCOD_TYPE_CHAR, true);
  player->addProperty("moveLeftKey", TCOD_TYPE_CHAR, true);
  player->addProperty("moveRightKey", TCOD_TYPE_CHAR, true);
  player->addProperty("quickslot1", TCOD_TYPE_CHAR, true);
  player->addProperty("quickslot2", TCOD_TYPE_CHAR, true);
  player->addProperty("quickslot3", TCOD_TYPE_CHAR, true);
  player->addProperty("quickslot4", TCOD_TYPE_CHAR, true);
  player->addProperty("quickslot5", TCOD_TYPE_CHAR, true);
  player->addProperty("quickslot6", TCOD_TYPE_CHAR, true);
  player->addProperty("quickslot7", TCOD_TYPE_CHAR, true);
  player->addProperty("quickslot8", TCOD_TYPE_CHAR, true);
  player->addProperty("quickslot9", TCOD_TYPE_CHAR, true);
  player->addProperty("quickslot10", TCOD_TYPE_CHAR, true);
  creatures->addStructure(player);

  config->addStructure(creatures);

  parser.run(get_file("cfg/sample2.cfg").c_str(), NULL);
  REQUIRE(TCOD_get_error() == std::string(""));
}
