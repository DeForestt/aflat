function aflat {
    ./bin/aflat $1 $2 $3
}

aflat ./libraries/std/src/Collections.af -o ./libraries/std/Collections.s
aflat ./libraries/std/src/concurrency.af -o ./libraries/std/concurrency.s
aflat ./libraries/std/src/DateTime.af -o ./libraries/std/DateTime.s
aflat ./libraries/std/src/files.af -o ./libraries/std/files.s
aflat ./libraries/std/src/io.af -o ./libraries/std/io.s
aflat ./libraries/std/src/math.af -o ./libraries/std/math.s
aflat ./libraries/std/src/std-cmp.af -o ./libraries/std/std-cmp.s
aflat ./libraries/std/src/std.af -o ./libraries/std/std.s
aflat ./libraries/std/src/strings.af -o ./libraries/std/strings.s
aflat ./libraries/std/src/String.af -o ./libraries/std/String.s
aflat ./libraries/std/src/ATest.af -o ./libraries/std/ATest.s
aflat ./libraries/std/src/HTTP.af -o ./libraries/std/HTTP.s
aflat ./libraries/std/src/CLArgs.af -o ./libraries/std/CLArgs.s
aflat ./libraries/std/src/System.af -o ./libraries/std/System.s
aflat ./libraries/std/src/Utils/Result.af -o ./libraries/std/Result.s
aflat ./libraries/std/src/Memory.af -o ./libraries/std/Memory.s

# Rename to prevent overwrite by later files
mv ./libraries/std/Result.s ./libraries/std/Utils_Result.s

aflat ./libraries/std/src/Utils/result.af -o ./libraries/std/result.s
mv ./libraries/std/result.s ./libraries/std/Utils_result.s

aflat ./libraries/std/src/Utils/Functions.af -o ./libraries/std/Functions.s
mv ./libraries/std/Functions.s ./libraries/std/Utils_Functions.s

aflat ./libraries/std/src/Utils/Observable.af -o ./libraries/std/Observable.s
mv ./libraries/std/Observable.s ./libraries/std/Utils_Observable.s

aflat ./libraries/std/src/Utils/Map.af -o ./libraries/std/Map.s
mv ./libraries/std/Map.s ./libraries/std/Utils_Map.s

aflat ./libraries/std/src/Utils/Option.af -o ./libraries/std/Option.s
mv ./libraries/std/Option.s ./libraries/std/Utils_Option.s

aflat ./libraries/std/src/Utils/option.af -o ./libraries/std/option.s
mv ./libraries/std/option.s ./libraries/std/Utils_option.s

aflat ./libraries/std/src/Utils/Properties.af -o ./libraries/std/Properties.s
mv ./libraries/std/Properties.s ./libraries/std/Utils_Properties.s

aflat ./libraries/std/src/Utils/Object.af -o ./libraries/std/Object.s
mv ./libraries/std/Object.s ./libraries/std/Utils_Object.s

aflat ./libraries/std/src/Utils/Error.af -o ./libraries/std/Error.s
mv ./libraries/std/Error.s ./libraries/std/Utils_Error.s

aflat ./libraries/std/src/Utils/Defer.af -o ./libraries/std/Defer.s
mv ./libraries/std/Defer.s ./libraries/std/Utils_Defer.s

aflat ./libraries/std/src/Utils/Error/Render.af -o ./libraries/std/Error_Render.s
aflat ./libraries/std/src/HTTP/Endpoint.af -o ./libraries/std/HTTP_Endpoint.s
aflat ./libraries/std/src/HTTP/Middleware.af -o ./libraries/std/HTTP_Middleware.s
aflat ./libraries/std/src/HTTP/Server.af -o ./libraries/std/HTTP_Server.s
aflat ./libraries/std/src/HTTP/Endpoints.af -o ./libraries/std/HTTP_Endpoints.s
aflat ./libraries/std/src/Web/Content.af -o ./libraries/std/Web_Content.s
aflat ./libraries/std/src/Web/Content/Bind.af -o ./libraries/std/Web_Content_Bind.s
aflat ./libraries/std/src/JSON.af -o ./libraries/std/JSON.s
aflat ./libraries/std/src/JSON/Parse.af -o ./libraries/std/Parse.s
mv ./libraries/std/Parse.s ./libraries/std/JSON_Parse.s
aflat ./libraries/std/src/JSON/Property.af -o ./libraries/std/JSON_Property.s
aflat ./libraries/std/src/Collections/Iterator.af -o ./libraries/std/Iterator.s
aflat ./libraries/std/src/Collections/Scroller.af -o ./libraries/std/Scroller.s
aflat ./libraries/std/src/Collections/Enumerator.af -o ./libraries/std/Enumerator.s

aflat ./libraries/std/src/Collections/Vector.af -o ./libraries/std/vector.s

gcc -g -no-pie -S -o ./libraries/std/request.s ./libraries/std/src/request.c

