STYLE='Google'

function format {
    clang-format $1 -style $STYLE -i
}

format ./include/Parser/*
format ./include/CodeGenerator/*
format ./include/*

format ./src/*
format ./src/Parser/*
format ./src/CodeGenerator/*
format ./tests/*