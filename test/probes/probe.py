
import pathlib, subprocess, sys, tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]

PROBES = {
  "bare": ("""fn main() -> int { return 5; };
""", 5),
  "bare_needs": (""".needs <std>
fn main() -> int { return 5; };
""", 5),
  "len_again": (""".needs <std>
import {len} from "strings" under st;
fn main() -> int { return st.len("again"); };
""", 5),
  "len_one": (""".needs <std>
import {len} from "strings" under st;
fn main() -> int { return st.len("one"); };
""", 3),
  "hash_one_bucket": (""".needs <std>
import {hash} from "strings" under st;
fn main() -> int {
    mutable int h = st.hash("one");
    if h < 0 { h = h * -1; };
    return h % 8;
};
""", 7),
  "hash_again_bucket": (""".needs <std>
import {hash} from "strings" under st;
fn main() -> int {
    mutable int h = st.hash("again");
    if h < 0 { h = h * -1; };
    return h % 8;
};
""", 5),
  "hash_six_bucket": (""".needs <std>
import {hash} from "strings" under st;
fn main() -> int {
    mutable int h = st.hash("six");
    if h < 0 { h = h * -1; };
    return h % 8;
};
""", 2),
  "hash_again_stable": (""".needs <std>
import {hash} from "strings" under st;
fn main() -> int {
    if st.hash("again") == st.hash("again") { return 0; };
    return 1;
};
""", 0),
  "str_at_again_4": (""".needs <std>
import {str_at} from "strings" under st;
fn main() -> int { return st.str_at("again", 4) as int; };
""", 110),
  "str_at_again_0": (""".needs <std>
import {str_at} from "strings" under st;
fn main() -> int { return st.str_at("again", 0) as int; };
""", 97),
  "str_comp_again": (""".needs <std>
import {str_comp} from "strings" under st;
fn main() -> int { return st.str_comp("again", "again"); };
""", 1),
  "char_div": (""".needs <std>
fn main() -> int {
    const char c = 'a';
    return c / 2;
};
""", 48),
  "char_mod": (""".needs <std>
fn main() -> int {
    const char c = 'a';
    return c % 2;
};
""", 1),
  "byte_div": (""".needs <std>
fn main() -> int {
    const byte b = 200;
    return b / 2;
};
""", 100),
  "map_one": (""".needs <std>
import unordered_map from "Collections/unordered_map";
fn main() -> int {
    let map = new unordered_map::<adr, int>();
    map.set("one", 1);
    if map.get("one").isNone() { return 1; };
    if map.get("one").unwrap() != 1 { return 2; };
    delete map;
    return 0;
};
""", 0),
  "map_six": (""".needs <std>
import unordered_map from "Collections/unordered_map";
fn main() -> int {
    let map = new unordered_map::<adr, int>();
    map.set("six", 6);
    if map.get("six").isNone() { return 1; };
    if map.get("six").unwrap() != 6 { return 2; };
    delete map;
    return 0;
};
""", 0),

  "byteat_0": (""".needs <std>
import {byteAt} from "strings" under st;
fn main() -> int { return st.byteAt("again", 0); };
""", 97),
  "byteat_1": (""".needs <std>
import {byteAt} from "strings" under st;
fn main() -> int { return st.byteAt("again", 1); };
""", 103),
  "byteat_4": (""".needs <std>
import {byteAt} from "strings" under st;
fn main() -> int { return st.byteAt("again", 4); };
""", 110),
  "str_at_eq_a": (""".needs <std>
import {str_at} from "strings" under st;
fn main() -> int {
    if st.str_at("again", 0) == 'a' { return 0; };
    return 1;
};
""", 0),
  "str_at_eq_n": (""".needs <std>
import {str_at} from "strings" under st;
fn main() -> int {
    if st.str_at("again", 4) == 'n' { return 0; };
    return 1;
};
""", 0),
  "hash_full_one": (""".needs <std>
import {hash} from "strings" under st;
fn main() -> int {
    if st.hash("one") == 1675433975 { return 0; };
    return 1;
};
""", 0),
  "map_again": (""".needs <std>
import unordered_map from "Collections/unordered_map";
fn main() -> int {
    let map = new unordered_map::<adr, int>();
    map.set("again", 4);
    if map.get("again").isNone() { return 1; };
    if map.get("again").unwrap() != 4 { return 2; };
    delete map;
    return 0;
};
""", 0),
}

def main():
    name = sys.argv[1]
    source, expected = PROBES[name]
    aflat = ROOT / "bin" / "aflat"
    with tempfile.TemporaryDirectory() as tmp:
        work = pathlib.Path(tmp)
        (work / "src").mkdir()
        (work / "bin").mkdir()
        (work / "aflat.cfg").write_text("[build]\nmain = main\n")
        (work / "src" / "main.af").write_text(source)
        subprocess.run([str(aflat), "--no-cache", "build"], cwd=work, check=True)
        ran = subprocess.run([str(work / "bin" / "a.out")], cwd=work)
        print(f"probe {name}: exit={ran.returncode} expected={expected}")
        return 0 if ran.returncode == expected else 1

if __name__ == "__main__":
    raise SystemExit(main())
