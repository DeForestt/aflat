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
aflat ./libraries/std/src/Utils/Properties.af ./libraries/std/Properties.s
mv ./libraries/std/Properties.s ./libraries/std/Utils_Properties.s
aflat ./libraries/std/src/Utils/Object.af ./libraries/std/Object.s
mv ./libraries/std/Object.s ./libraries/std/Utils_Object.s
aflat ./libraries/std/src/HTTP/Endpoint.af ./libraries/std/HTTP_Endpoint.s
aflat ./libraries/std/src/HTTP/Server.af ./libraries/std/HTTP_Server.s
aflat ./libraries/std/src/HTTP/Endpoints.af ./libraries/std/HTTP_Endpoints.s
aflat ./libraries/std/src/Web/Content.af ./libraries/std/Web_Content.s
aflat ./libraries/std/src/Web/Content/Bind.af ./libraries/std/Web_Content_Bind.s
aflat ./libraries/std/src/JSON.af ./libraries/std/JSON.s
aflat ./libraries/std/src/JSON/Parse.af ./libraries/std/Parse.s
mv ./libraries/std/Parse.s ./libraries/std/JSON_Parse.s
aflat ./libraries/std/src/JSON/Property.af ./libraries/std/JSON_Property.s
gcc -g -no-pie -S -o ./libraries/std/request.s ./libraries/std/src/request.c
