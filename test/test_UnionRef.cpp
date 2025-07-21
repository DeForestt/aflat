#include <filesystem>
#include <fstream>

#include "Configs.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

TEST_CASE("union alias reference generics", "[union][generics][ref]") {
  SUCCEED("reference union placeholder");
}
