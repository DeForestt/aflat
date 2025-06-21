#include "Configs.hpp"
#include "catch.hpp"

TEST_CASE("Config Comments", "[Configs]") {
  std::string content =
      "; This is a comment\n"
      "";
  cfg::Config config = cfg::getConfig(content);
  REQUIRE(config.aflatVersion == "");
  REQUIRE(config.modules.size() == 0);
  REQUIRE(config.cFiles.size() == 0);
  REQUIRE(config.testFile == "");
};

TEST_CASE("Config Version", "[Configs]") {
  std::string content =
      "v 0.0.1\n"
      "";
  cfg::Config config = cfg::getConfig(content);
  REQUIRE(config.aflatVersion == "0.0.1");
  REQUIRE(config.modules.size() == 0);
  REQUIRE(config.cFiles.size() == 0);
  REQUIRE(config.testFile == "");
};

TEST_CASE("Config Modules", "[Configs]") {
  std::string content =
      "m main\n"
      "m Cat\n"
      "m Dog\n";
  cfg::Config config = cfg::getConfig(content);
  REQUIRE(config.aflatVersion == "");
  REQUIRE(config.modules.size() == 3);
  REQUIRE(config.modules[0] == "main");
  REQUIRE(config.modules[1] == "Cat");
  REQUIRE(config.modules[2] == "Dog");
  REQUIRE(config.cFiles.size() == 0);
  REQUIRE(config.testFile == "");
};

TEST_CASE("Config CFiles", "[Configs]") {
  std::string content =
      "c main\n"
      "c Cat\n"
      "c Dog\n";
  cfg::Config config = cfg::getConfig(content);
  REQUIRE(config.aflatVersion == "");
  REQUIRE(config.modules.size() == 0);
  REQUIRE(config.cFiles.size() == 3);
  REQUIRE(config.cFiles[0] == "main");
  REQUIRE(config.cFiles[1] == "Cat");
  REQUIRE(config.cFiles[2] == "Dog");
  REQUIRE(config.testFile == "");
};

TEST_CASE("Config TestFile", "[Configs]") {
  std::string content = "t test\n";
  cfg::Config config = cfg::getConfig(content);
  REQUIRE(config.aflatVersion == "");
  REQUIRE(config.modules.size() == 0);
  REQUIRE(config.cFiles.size() == 0);
  REQUIRE(config.testFile == "test");
};

TEST_CASE("Ini Build", "[Configs]") {
  std::string content =
      "[build]\n"
      "output = ./bin/main\n"
      "debug = true\n"
      "main = src/main\n"
      "test = src/test\n";
  cfg::Config config = cfg::getConfig(content);
  REQUIRE(config.outPutFile == "./bin/main");
  REQUIRE(config.debug == true);
  REQUIRE(config.entryPoint == "src/main");
  REQUIRE(config.testFile == "src/test");
};

TEST_CASE("Ini Dependencies", "[Configs]") {
  std::string content =
      "[build]\nmain = main\n\n[dependencies]\ncollections = \"./src/answer.af\"\n";
  cfg::Config config = cfg::getConfig(content);
  REQUIRE(config.modules.size() == 1);
  REQUIRE(config.modules[0] == "answer");
};