function aflat {
    ./bin/aflat $1 $2
}
aflat ./libraries/std/src/Collections.af ./libraries/std/Collections.s
aflat ./libraries/std/src/concurrency.af ./libraries/std/concurrency.s
aflat ./libraries/std/src/DateTime.af ./libraries/std/DateTime.s
aflat ./libraries/std/src/files.af ./libraries/std/files.s
aflat ./libraries/std/src/io.af ./libraries/std/io.s
aflat ./libraries/std/src/math.af ./libraries/std/math.s
aflat ./libraries/std/src/std-cmp.af ./libraries/std/std-cmp.s
aflat ./libraries/std/src/std.af ./libraries/std/std.s
aflat ./libraries/std/src/strings.af ./libraries/std/strings.s
aflat ./libraries/std/src/String.af ./libraries/std/String.s
aflat ./libraries/std/src/ATest.af ./libraries/std/ATest.s
aflat ./libraries/std/src/HTTP.af ./libraries/std/HTTP.s
aflat ./libraries/std/src/CLArgs.af ./libraries/std/CLArgs.s
aflat ./libraries/std/src/System.af ./libraries/std/System.s
aflat ./libraries/std/src/Utils/Result.af ./libraries/std/Result.s
# rename Result.s to Utils_Result.s
mv ./libraries/std/Result.s ./libraries/std/Utils_Result.s
aflat ./libraries/std/src/Utils/Functions.af ./libraries/std/Functions.s
mv ./libraries/std/Functions.s ./libraries/std/Utils_Functions.s
aflat ./libraries/std/src/Utils/Map.af ./libraries/std/Map.s
mv ./libraries/std/Map.s ./libraries/std/Utils_Map.s
gcc -g -no-pie -S -o ./libraries/std/request.s ./libraries/std/src/request.c
