import sys
import time

def read_trace_file(trace_file):
    with open(trace_file, "r") as file:
        for line in file:
            print(line.strip())
            sys.stdout.flush()
            time.sleep(1)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python run.py <trace_file>")
        sys.exit(1)

    trace_file = sys.argv[1]
    read_trace_file(trace_file)
