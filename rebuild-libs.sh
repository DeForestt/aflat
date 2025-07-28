function aflat {
    ./bin/aflat $1 $2 $3
}

# Function to compile a specific library
function compile_single {
    case "$1" in
        "Collections") aflat ./libraries/std/src/Collections.af -o ./libraries/std/Collections.s ;;
        "concurrency") aflat ./libraries/std/src/concurrency.af -o ./libraries/std/concurrency.s ;;
        "DateTime") aflat ./libraries/std/src/DateTime.af -o ./libraries/std/DateTime.s ;;
        "files") aflat ./libraries/std/src/files.af -o ./libraries/std/files.s ;;
        "io") aflat ./libraries/std/src/io.af -o ./libraries/std/io.s ;;
        "math") aflat ./libraries/std/src/math.af -o ./libraries/std/math.s ;;
        "std-cmp") aflat ./libraries/std/src/std-cmp.af -o ./libraries/std/std-cmp.s ;;
        "std") aflat ./libraries/std/src/std.af -o ./libraries/std/std.s ;;
        "strings") aflat ./libraries/std/src/strings.af -o ./libraries/std/strings.s ;;
        "String") aflat ./libraries/std/src/String.af -o ./libraries/std/String.s ;;
        "ATest") aflat ./libraries/std/src/ATest.af -o ./libraries/std/ATest.s ;;
        "HTTP") aflat ./libraries/std/src/HTTP.af -o ./libraries/std/HTTP.s ;;
        "CLArgs") aflat ./libraries/std/src/CLArgs.af -o ./libraries/std/CLArgs.s ;;
        "System") aflat ./libraries/std/src/System.af -o ./libraries/std/System.s ;;
        "Memory") aflat ./libraries/std/src/Memory.af -o ./libraries/std/Memory.s ;;
        "Result") 
            aflat ./libraries/std/src/Utils/Result.af -o ./libraries/std/Result.s
            mv ./libraries/std/Result.s ./libraries/std/Utils_Result.s ;;
        "result") 
            aflat ./libraries/std/src/Utils/result.af -o ./libraries/std/result.s
            mv ./libraries/std/result.s ./libraries/std/Utils_result.s ;;
        "Functions") 
            aflat ./libraries/std/src/Utils/Functions.af -o ./libraries/std/Functions.s
            mv ./libraries/std/Functions.s ./libraries/std/Utils_Functions.s ;;
        "Observable") 
            aflat ./libraries/std/src/Utils/Observable.af -o ./libraries/std/Observable.s
            mv ./libraries/std/Observable.s ./libraries/std/Utils_Observable.s ;;
        "Map") 
            aflat ./libraries/std/src/Utils/Map.af -o ./libraries/std/Map.s
            mv ./libraries/std/Map.s ./libraries/std/Utils_Map.s ;;
        "Option") 
            aflat ./libraries/std/src/Utils/Option.af -o ./libraries/std/Option.s
            mv ./libraries/std/Option.s ./libraries/std/Utils_Option.s ;;
        "option") 
            aflat ./libraries/std/src/Utils/option.af -o ./libraries/std/option.s
            mv ./libraries/std/option.s ./libraries/std/Utils_option.s ;;
        "Properties") 
            aflat ./libraries/std/src/Utils/Properties.af -o ./libraries/std/Properties.s
            mv ./libraries/std/Properties.s ./libraries/std/Utils_Properties.s ;;
        "Object") 
            aflat ./libraries/std/src/Utils/Object.af -o ./libraries/std/Object.s
            mv ./libraries/std/Object.s ./libraries/std/Utils_Object.s ;;
        "Error") 
            aflat ./libraries/std/src/Utils/Error.af -o ./libraries/std/Error.s
            mv ./libraries/std/Error.s ./libraries/std/Utils_Error.s ;;
        "Defer") 
            aflat ./libraries/std/src/Utils/Defer.af -o ./libraries/std/Defer.s
            mv ./libraries/std/Defer.s ./libraries/std/Utils_Defer.s ;;
        "unordered_map") aflat ./libraries/std/src/Collections/unordered_map.af -o ./libraries/std/unordered_map.s ;;
        "Error_Render") aflat ./libraries/std/src/Utils/Error/Render.af -o ./libraries/std/Error_Render.s ;;
        "HTTP_Endpoint") aflat ./libraries/std/src/HTTP/Endpoint.af -o ./libraries/std/HTTP_Endpoint.s ;;
        "HTTP_Middleware") aflat ./libraries/std/src/HTTP/Middleware.af -o ./libraries/std/HTTP_Middleware.s ;;
        "HTTP_Server") aflat ./libraries/std/src/HTTP/Server.af -o ./libraries/std/HTTP_Server.s ;;
        "HTTP_Endpoints") aflat ./libraries/std/src/HTTP/Endpoints.af -o ./libraries/std/HTTP_Endpoints.s ;;
        "Web_Content") aflat ./libraries/std/src/Web/Content.af -o ./libraries/std/Web_Content.s ;;
        "Web_Content_Bind") aflat ./libraries/std/src/Web/Content/Bind.af -o ./libraries/std/Web_Content_Bind.s ;;
        "JSON") aflat ./libraries/std/src/JSON.af -o ./libraries/std/JSON.s ;;
        "JSON_Parse") 
            aflat ./libraries/std/src/JSON/Parse.af -o ./libraries/std/Parse.s
            mv ./libraries/std/Parse.s ./libraries/std/JSON_Parse.s ;;
        "JSON_Property") aflat ./libraries/std/src/JSON/Property.af -o ./libraries/std/JSON_Property.s ;;
        "Iterator") aflat ./libraries/std/src/Collections/Iterator.af -o ./libraries/std/Iterator.s ;;
        "Scroller") aflat ./libraries/std/src/Collections/Scroller.af -o ./libraries/std/Scroller.s ;;
        "Enumerator") aflat ./libraries/std/src/Collections/Enumerator.af -o ./libraries/std/Enumerator.s ;;
        "Vector") aflat ./libraries/std/src/Collections/Vector.af -o ./libraries/std/vector.s ;;
        "request") gcc -g -no-pie -S -o ./libraries/std/request.s ./libraries/std/src/request.c ;;
        *)
            echo "Unknown library: $1"
            echo "Available libraries:"
            echo "  Collections, concurrency, DateTime, files, io, math, std-cmp, std"
            echo "  strings, String, ATest, HTTP, CLArgs, System, Memory"
            echo "  Result, result, Functions, Observable, Map, Option, option"
            echo "  Properties, Object, Error, Defer, unordered_map, Error_Render"
            echo "  HTTP_Endpoint, HTTP_Middleware, HTTP_Server, HTTP_Endpoints"
            echo "  Web_Content, Web_Content_Bind, JSON, JSON_Parse, JSON_Property"
            echo "  Iterator, Scroller, Enumerator, Vector, request"
            exit 1 ;;
    esac
    echo "Successfully compiled $1"
}

# Check if a specific library was requested
if [ $# -eq 1 ]; then
    compile_single "$1"
    exit 0
fi

# Otherwise, compile all libraries in parallel

# Compile core libraries in parallel
aflat ./libraries/std/src/Collections.af -o ./libraries/std/Collections.s &
aflat ./libraries/std/src/concurrency.af -o ./libraries/std/concurrency.s &
aflat ./libraries/std/src/DateTime.af -o ./libraries/std/DateTime.s &
aflat ./libraries/std/src/files.af -o ./libraries/std/files.s &
aflat ./libraries/std/src/io.af -o ./libraries/std/io.s &
aflat ./libraries/std/src/math.af -o ./libraries/std/math.s &
aflat ./libraries/std/src/std-cmp.af -o ./libraries/std/std-cmp.s &
aflat ./libraries/std/src/std.af -o ./libraries/std/std.s &
aflat ./libraries/std/src/strings.af -o ./libraries/std/strings.s &
aflat ./libraries/std/src/String.af -o ./libraries/std/String.s &
aflat ./libraries/std/src/ATest.af -o ./libraries/std/ATest.s &
aflat ./libraries/std/src/HTTP.af -o ./libraries/std/HTTP.s &
aflat ./libraries/std/src/CLArgs.af -o ./libraries/std/CLArgs.s &
aflat ./libraries/std/src/System.af -o ./libraries/std/System.s &
aflat ./libraries/std/src/Memory.af -o ./libraries/std/Memory.s &

# Wait for the first batch to complete
wait

# Handle Utils files that need renaming (run sequentially to avoid conflicts)
aflat ./libraries/std/src/Utils/Result.af -o ./libraries/std/Result.s
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

# Compile remaining libraries in parallel
aflat ./libraries/std/src/Collections/unordered_map.af -o ./libraries/std/unordered_map.s &

aflat ./libraries/std/src/Utils/Error/Render.af -o ./libraries/std/Error_Render.s &
aflat ./libraries/std/src/HTTP/Endpoint.af -o ./libraries/std/HTTP_Endpoint.s &
aflat ./libraries/std/src/HTTP/Middleware.af -o ./libraries/std/HTTP_Middleware.s &
aflat ./libraries/std/src/HTTP/Server.af -o ./libraries/std/HTTP_Server.s &
aflat ./libraries/std/src/HTTP/Endpoints.af -o ./libraries/std/HTTP_Endpoints.s &
aflat ./libraries/std/src/Web/Content.af -o ./libraries/std/Web_Content.s &
aflat ./libraries/std/src/Web/Content/Bind.af -o ./libraries/std/Web_Content_Bind.s &
aflat ./libraries/std/src/JSON.af -o ./libraries/std/JSON.s &
aflat ./libraries/std/src/JSON/Property.af -o ./libraries/std/JSON_Property.s &
aflat ./libraries/std/src/Collections/Iterator.af -o ./libraries/std/Iterator.s &
aflat ./libraries/std/src/Collections/Scroller.af -o ./libraries/std/Scroller.s &
aflat ./libraries/std/src/Collections/Enumerator.af -o ./libraries/std/Enumerator.s &
aflat ./libraries/std/src/Collections/Vector.af -o ./libraries/std/vector.s &

# Handle JSON/Parse separately since it needs renaming
aflat ./libraries/std/src/JSON/Parse.af -o ./libraries/std/Parse.s
mv ./libraries/std/Parse.s ./libraries/std/JSON_Parse.s

# Compile C file
gcc -g -no-pie -S -o ./libraries/std/request.s ./libraries/std/src/request.c &

# Wait for all background processes to complete
wait

echo "All libraries compiled successfully!"

