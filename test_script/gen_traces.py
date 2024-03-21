import random
import sys

def read_user_init_file(file_path):
    traces = []
    with open(file_path, "r") as file:
        for line in file:
            priv_key, addr, value = line.strip().split(",")
            traces.append(addr)
    return traces

def generate_traces(traces, num_traces):
    with open("traces.txt", "w") as file:
        for _ in range(num_traces):
            from_addr = random.choice(traces)
            to_addr = random.choice(traces)
            while to_addr == from_addr:  # Ensure from and to addresses are different
                to_addr = random.choice(traces)
            value = round(random.uniform(1, 12), 2)  # Random float with two decimal places
            file.write(f"{from_addr} {to_addr} {value}\n")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python gen_traces.py <user_init_file> <num_traces>")
        sys.exit(1)

    user_init_file = sys.argv[1]  # User initialization file path
    num_traces = int(sys.argv[2])  # Number of traces to generate

    traces = read_user_init_file(user_init_file)
    generate_traces(traces, num_traces)
