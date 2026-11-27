
import pathlib, subprocess, sys, tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]
PRELUDE = '.needs <std>\nimport deque from "Collections/deque";\nimport vector from "Collections/Vector";\n\ntypes(T)\nclass Probe {\n  fn init() -> Self { return my; };\n  when (T is primitive) fn kind() -> int { return 1; };\n  when (T is not primitive) fn kind() -> int { return 2; };\n};\n\ntypes(T)\nclass AdrProbe {\n  fn init() -> Self { return my; };\n  when (T is adr) fn kind() -> int { return 1; };\n  when (T is not adr) fn kind() -> int { return 2; };\n};\n\nclass Plain {\n  fn init() -> Self { return my; };\n};\n\n'
CHECKS = {'checkU32Deque': 'fn checkU32Deque() -> void {\n    let queue = new deque::<u32>(2);\n    const u32 two = 2000000000;\n    const u32 big = two + two;\n    const u32 seven = 7;\n    assert(queue.push_back(two).unwrap(), "u32 deque push two");\n    assert(queue.push_back(big).unwrap(), "u32 deque push big");\n    assert(queue.push_front(seven).unwrap(), "u32 deque push front grows");\n    assert(queue.peek_front().unwrap() == seven, "u32 deque front");\n    assert(queue.peek_back().unwrap() == big, "u32 deque back");\n    assert(queue.pop_back().unwrap() == big, "u32 deque pop back");\n    assert(queue.pop_front().unwrap() == seven, "u32 deque pop front");\n    assert(queue.pop_back().unwrap() == two, "u32 deque pop last");\n    delete queue;\n};', 'checkI16Deque': 'fn checkI16Deque() -> void {\n    let queue = new deque::<i16>(2);\n    const i16 first = -300;\n    const i16 second = 30000;\n    const i16 third = -12345;\n    const i16 fourth = 22222;\n    assert(queue.push_back(first).unwrap(), "i16 deque push first");\n    assert(queue.push_back(second).unwrap(), "i16 deque push second");\n    assert(queue.push_back(third).unwrap(), "i16 deque push third grows");\n    assert(queue.push_front(fourth).unwrap(), "i16 deque push front");\n    assert(queue.pop_front().unwrap() == fourth, "i16 deque pop front");\n    assert(queue.pop_back().unwrap() == third, "i16 deque pop back");\n    assert(queue.pop_front().unwrap() == first, "i16 deque pop first");\n    assert(queue.pop_back().unwrap() == second, "i16 deque pop second");\n    delete queue;\n};', 'checkU64Vector': 'fn checkU64Vector() -> void {\n    let values = new vector::<u64>();\n    const u64 first = #4294967296;\n    const u64 second = first * #3;\n    values.push_back(first);\n    values.push_back(second);\n    values.push_back(9);\n    assert(values.get(0).unwrap() == first, "u64 vector first");\n    assert(values.get(1).unwrap() == second, "u64 vector second");\n    assert(values.get(2).unwrap() == 9, "u64 vector third");\n    assert(values.pop_back().unwrap() == 9, "u64 vector pop");\n    delete values;\n};', 'checkI8Vector': 'fn checkI8Vector() -> void {\n    let values = new vector::<i8>();\n    const i8 first = -128;\n    const i8 second = 127;\n    values.push_back(first);\n    values.push_back(second);\n    assert(values.get(0).unwrap() == first, "i8 vector first");\n    assert(values.get(1).unwrap() == second, "i8 vector second");\n    delete values;\n};', 'checkWhenPredicates': 'fn checkWhenPredicates() -> void {\n    let p1 = new Probe::<u32>();\n    assert(p1.kind() == 1, "u32 is primitive");\n    delete p1;\n    let p2 = new Probe::<Plain>();\n    assert(p2.kind() == 2, "a class is not primitive");\n    delete p2;\n    let a1 = new AdrProbe::<usize>();\n    assert(a1.kind() == 1, "usize satisfies is adr");\n    delete a1;\n    let a3 = new AdrProbe::<int>();\n    assert(a3.kind() == 2, "int is not adr");\n    delete a3;\n};'}

STAGES = {
  "u64_a": """fn u64_a() -> void {
    let values = new vector::<u64>();
    const u64 first = #4294967296;
    values.push_back(first);
    assert(values.get(0).unwrap() == first, "u64 vector first");
    delete values;
};
""",
  "u64_b": """fn u64_b() -> void {
    let values = new vector::<u64>();
    const u64 first = #4294967296;
    const u64 second = first * #3;
    values.push_back(first);
    values.push_back(second);
    assert(values.get(0).unwrap() == first, "u64 vector first");
    assert(values.get(1).unwrap() == second, "u64 vector second");
    delete values;
};
""",
  "u64_c": """fn u64_c() -> void {
    let values = new vector::<u64>();
    const u64 first = #4294967296;
    values.push_back(first);
    values.push_back(9);
    assert(values.get(1).unwrap() == 9, "u64 vector literal");
    delete values;
};
""",
  "u64_d": """fn u64_d() -> void {
    let values = new vector::<u64>();
    const u64 first = #4294967296;
    values.push_back(first);
    assert(values.pop_back().unwrap() == first, "u64 vector pop");
    delete values;
};
""",
}
CHECKS.update(STAGES)

def main():
    name = sys.argv[1]
    source = PRELUDE + CHECKS[name] + "\nfn main() -> int { " + name + "(); return 0; };\n"
    aflat = ROOT / "bin" / "aflat"
    with tempfile.TemporaryDirectory() as tmp:
        work = pathlib.Path(tmp)
        (work / "src").mkdir()
        (work / "bin").mkdir()
        (work / "aflat.cfg").write_text("[build]\nmain = main\n")
        (work / "src" / "main.af").write_text(source)
        subprocess.run([str(aflat), "--no-cache", "build"], cwd=work, check=True)
        ran = subprocess.run([str(work / "bin" / "a.out")], cwd=work)
        print(f"probe {name}: exit={ran.returncode} expected=0")
        return 0 if ran.returncode == 0 else 1

if __name__ == "__main__":
    raise SystemExit(main())
