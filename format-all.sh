find ./src ./test \( -name '*.c' -o -name '*.cpp' -o -name '*.h' -o -name '*.hpp' \) \
  -exec clang-format -style=Google -i {} +
