#!/usr/bin/env bash
set -euo pipefail

function build_af {
    local src="$1"
    local out="$2"

    # Rebuild if the source was modified more recently than the output
    if [[ ! -f "$out" ]] || [[ "$src" -nt "$out" ]]; then
        echo "[build] $src -> $out"
        ./bin/aflat "$src" -o "$out"
    else
        echo "[cached] $out"
    fi
}

function build_c {
    local src="$1"
    local out="$2"

    if [[ ! -f "$out" ]] || [[ "$src" -nt "$out" ]]; then
        echo "[build] $src -> $out"
        gcc -g -no-pie -S -o "$out" "$src"
    else
        echo "[cached] $out"
    fi
}

build_af ./libraries/std/src/Collections.af ./libraries/std/Collections.s
build_af ./libraries/std/src/concurrency.af ./libraries/std/concurrency.s
build_af ./libraries/std/src/DateTime.af ./libraries/std/DateTime.s
build_af ./libraries/std/src/files.af ./libraries/std/files.s
build_af ./libraries/std/src/io.af ./libraries/std/io.s
build_af ./libraries/std/src/math.af ./libraries/std/math.s
build_af ./libraries/std/src/std-cmp.af ./libraries/std/std-cmp.s
build_af ./libraries/std/src/std.af ./libraries/std/std.s
build_af ./libraries/std/src/strings.af ./libraries/std/strings.s
build_af ./libraries/std/src/String.af ./libraries/std/String.s
build_af ./libraries/std/src/ATest.af ./libraries/std/ATest.s
build_af ./libraries/std/src/HTTP.af ./libraries/std/HTTP.s
build_af ./libraries/std/src/CLArgs.af ./libraries/std/CLArgs.s
build_af ./libraries/std/src/System.af ./libraries/std/System.s
build_af ./libraries/std/src/Utils/Result.af ./libraries/std/Utils_Result.s
build_af ./libraries/std/src/Memory.af ./libraries/std/Memory.s

build_af ./libraries/std/src/Utils/result.af ./libraries/std/Utils_result.s
build_af ./libraries/std/src/Utils/Functions.af ./libraries/std/Utils_Functions.s
build_af ./libraries/std/src/Utils/Observable.af ./libraries/std/Utils_Observable.s
build_af ./libraries/std/src/Utils/Map.af ./libraries/std/Utils_Map.s
build_af ./libraries/std/src/Utils/Option.af ./libraries/std/Utils_Option.s
build_af ./libraries/std/src/Utils/option.af ./libraries/std/Utils_option.s
build_af ./libraries/std/src/Utils/Properties.af ./libraries/std/Utils_Properties.s
build_af ./libraries/std/src/Utils/Object.af ./libraries/std/Utils_Object.s
build_af ./libraries/std/src/Utils/Error.af ./libraries/std/Utils_Error.s
build_af ./libraries/std/src/Utils/Defer.af ./libraries/std/Utils_Defer.s

build_af ./libraries/std/src/Collections/unordered_map.af ./libraries/std/unordered_map.s

build_af ./libraries/std/src/Utils/Error/Render.af ./libraries/std/Error_Render.s
build_af ./libraries/std/src/HTTP/Endpoint.af ./libraries/std/HTTP_Endpoint.s
build_af ./libraries/std/src/HTTP/Middleware.af ./libraries/std/HTTP_Middleware.s
build_af ./libraries/std/src/HTTP/Server.af ./libraries/std/HTTP_Server.s
build_af ./libraries/std/src/HTTP/Endpoints.af ./libraries/std/HTTP_Endpoints.s
build_af ./libraries/std/src/Web/Content.af ./libraries/std/Web_Content.s
build_af ./libraries/std/src/Web/Content/Bind.af ./libraries/std/Web_Content_Bind.s
build_af ./libraries/std/src/JSON.af ./libraries/std/JSON.s
build_af ./libraries/std/src/JSON/Parse.af ./libraries/std/JSON_Parse.s
build_af ./libraries/std/src/JSON/Property.af ./libraries/std/JSON_Property.s
build_af ./libraries/std/src/Collections/Iterator.af ./libraries/std/Iterator.s
build_af ./libraries/std/src/Collections/Scroller.af ./libraries/std/Scroller.s
build_af ./libraries/std/src/Collections/Enumerator.af ./libraries/std/Enumerator.s

build_af ./libraries/std/src/Collections/Vector.af ./libraries/std/vector.s

build_c ./libraries/std/src/request.c ./libraries/std/request.s

