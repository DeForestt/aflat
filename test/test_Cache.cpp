#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>

#include "Configs.hpp"
#include "catch.hpp"

bool compileCFile(const std::string &path, bool debug);
bool runConfig(cfg::Config &config, const std::string &libPath, char pmode);

TEST_CASE("compileCFile cache reuse", "[cache]") {
  namespace fs = std::filesystem;
  fs::create_directories("src/tmp");
  std::ofstream ofs("src/tmp/foo.c");
  ofs << "int foo(){return 42;}";
  ofs.close();
  fs::remove_all(".cache/tmp");

  REQUIRE(compileCFile("tmp/foo", false));
  fs::path obj(".cache/tmp/foo.o");
  REQUIRE(fs::exists(obj));
  auto t1 = fs::last_write_time(obj);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  REQUIRE(compileCFile("tmp/foo", false));
  auto t2 = fs::last_write_time(obj);
  REQUIRE(t1 == t2);

  fs::remove_all("src/tmp");
  fs::remove_all(".cache/tmp");
}

TEST_CASE("module cache invalidates on header change", "[cache]") {
  namespace fs = std::filesystem;
  fs::create_directories("src/tmp");
  std::ofstream bar("src/tmp/bar.af");
  bar << "export int bar(){return 1;}";
  bar.close();
  std::ofstream foo("src/tmp/foo.af");
  foo << "import {bar} from \"./bar\" under tmp;\n";
  foo << "export int foo(){ return tmp.bar(); };";
  foo.close();
  fs::remove_all(".cache/tmp");

  std::string cfgText =
      "[build]\nmain = tmp/foo\n\n[dependencies]\nbar = \"./src/tmp/bar.af\"\n";
  cfg::Config cfg = cfg::getConfig(cfgText);
  cfg.outPutFile = "./bin/cache_test";
  REQUIRE(runConfig(cfg, "../libraries/std/", 'e'));
  fs::path obj(".cache/tmp/foo.o");
  REQUIRE(fs::exists(obj));
  auto t1 = fs::last_write_time(obj);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::ofstream mod("src/tmp/bar.af", std::ios::app);
  mod << " \n";
  mod.close();
  REQUIRE(runConfig(cfg, "../libraries/std/", 'e'));
  auto t2 = fs::last_write_time(obj);
  REQUIRE(t2 > t1);

  fs::remove_all("src/tmp");
  fs::remove_all(".cache/tmp");
  fs::remove("./bin/cache_test");
}
