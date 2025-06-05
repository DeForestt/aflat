import time

def main():
    start = time.perf_counter()
    s = 0
    for i in range(10_000_000):
        s += i
    duration = time.perf_counter() - start
    print(s)
    print(f"time: {duration:.3f}")

if __name__ == "__main__":
    main()
