set -euo pipefail

AF_FLAGS=()
MAX_JOBS="${AFLAT_LIB_JOBS:-2}"

function usage {
    echo "Usage: $0 [-d] [-j jobs] [library]"
    echo "  -d    Compile libraries with aflat's debug flag"
    echo "  -j    Maximum parallel library builds (default: 2)"
    exit 1
}

while getopts ":dj:" opt; do
    case "$opt" in
        d) AF_FLAGS=(-d) ;;
        j) MAX_JOBS="$OPTARG" ;;
        *) usage ;;
    esac
done

shift $((OPTIND - 1))

if [ $# -gt 1 ]; then
    usage
fi

if [[ ! "$MAX_JOBS" =~ ^[1-9][0-9]*$ ]]; then
    echo "Invalid job count: $MAX_JOBS" >&2
    usage
fi

function aflat {
    ./bin/aflat "${AF_FLAGS[@]}" "$@"
}

job_pids=()
job_names=()
job_status=0

function wait_for_oldest_job {
    local pid="${job_pids[0]}"
    local name="${job_names[0]}"

    if wait "$pid"; then
        echo "Successfully compiled $name"
    else
        local status=$?
        echo "Failed to compile $name (exit status $status)" >&2
        job_status=1
    fi

    job_pids=("${job_pids[@]:1}")
    job_names=("${job_names[@]:1}")
}

function queue_library {
    local name="$1"

    compile_single "$name" &
    job_pids+=("$!")
    job_names+=("$name")

    if [ "${#job_pids[@]}" -ge "$MAX_JOBS" ]; then
        wait_for_oldest_job
    fi
}

function wait_for_jobs {
    while [ "${#job_pids[@]}" -gt 0 ]; do
        wait_for_oldest_job
    done

    local status="$job_status"
    job_status=0
    return "$status"
}

# Function to compile a specific library
function compile_single {
    case "$1" in
        "Tuple") aflat ./libraries/std/src/Collections/Tuple.af -o ./libraries/std/Tuple.s ;;
        "concurrency") aflat ./libraries/std/src/concurrency.af -o ./libraries/std/concurrency.s ;;
        "DateTime") aflat ./libraries/std/src/DateTime.af -o ./libraries/std/DateTime.s ;;
        "files") aflat ./libraries/std/src/files.af -o ./libraries/std/files.s ;;
        "io") aflat ./libraries/std/src/io.af -o ./libraries/std/io.s ;;
        "math") aflat ./libraries/std/src/math.af -o ./libraries/std/math.s ;;
        "std-cmp") aflat ./libraries/std/src/std-cmp.af -o ./libraries/std/std-cmp.s ;;
        "std") aflat ./libraries/std/src/std.af -o ./libraries/std/std.s ;;
        "Allocator") gcc -O2 -mstackrealign -mincoming-stack-boundary=3 -S -o ./libraries/std/allocator.s ./libraries/std/src/allocator_runtime.c ;;
        "strings") aflat ./libraries/std/src/strings.af -o ./libraries/std/strings.s ;;
        "String") aflat ./libraries/std/src/String.af -o ./libraries/std/String.s ;;
        "uni_string") aflat ./libraries/std/src/uni_string.af -o ./libraries/std/uni_string.s ;;
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
        "JSON_Property_Fields") aflat ./libraries/std/src/JSON/Property/Fields.af -o ./libraries/std/JSON_Property_Fields.s ;;
        "Vector") aflat ./libraries/std/src/Collections/Vector.af -o ./libraries/std/vector.s ;;
        "Heap") aflat ./libraries/std/src/Collections/Heap.af -o ./libraries/std/Heap.s ;;
        "request") gcc -g -no-pie -S -o ./libraries/std/request.s ./libraries/std/src/request.c ;;
        "Async") gcc -O2 -mstackrealign -mincoming-stack-boundary=3 -S -o ./libraries/std/async.s ./libraries/std/src/async_runtime.c ;;
        *)
            echo "Unknown library: $1"
            echo "Available libraries:"
            echo "  concurrency, DateTime, files, io, math, std-cmp, std, Allocator"
            echo "  strings, String, uni_string, ATest, HTTP, CLArgs, System, Memory"
            echo "  Result, result, Functions, Observable, Map, Option, option"
            echo "  Properties, Object, Error, Defer, unordered_map, Error_Render"
            echo "  HTTP_Endpoint, HTTP_Middleware, HTTP_Server, HTTP_Endpoints"
            echo "  Web_Content, Web_Content_Bind, JSON, JSON_Parse, JSON_Property"
            echo "  JSON_Property_Fields"
            echo "  Vector, request, Async, Tuple, Heap"
            exit 1 ;;
    esac
}

# Check if a specific library was requested
if [ $# -eq 1 ]; then
    compile_single "$1"
    echo "Successfully compiled $1"
    exit 0
fi

# Otherwise, compile all libraries with bounded parallelism.
echo "Compiling standard libraries with up to $MAX_JOBS parallel jobs"

# Compile core libraries in parallel
for library in \
    concurrency DateTime files io math std-cmp std strings String uni_string \
    ATest HTTP CLArgs System Memory; do
    queue_library "$library"
done

# Wait for the first batch to complete
wait_for_jobs

# Handle Utils files that need renaming (run sequentially to avoid conflicts)
for library in \
    Result result Functions Observable Map Option option Properties Object Error Defer; do
    compile_single "$library"
    echo "Successfully compiled $library"
done

# Compile remaining libraries in parallel
for library in \
    unordered_map Error_Render HTTP_Endpoint HTTP_Middleware HTTP_Server \
    HTTP_Endpoints Web_Content Web_Content_Bind JSON JSON_Property \
    JSON_Property_Fields Vector Heap Tuple JSON_Parse request Async Allocator; do
    queue_library "$library"
done

# Wait for all background processes to complete
wait_for_jobs

echo "All libraries compiled successfully!"
