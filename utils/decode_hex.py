#! /usr/bin/env python3

import argparse

def parse_options():
    parser = argparse.ArgumentParser()
    parser.add_argument("hex", type=str)
    return parser.parse_args()

def decode(hex_str: str):
    value = int(hex_str, 16) if hex_str.startswith("0x") else int(hex_str)
    return bin(value)

def active_bits(bin_str: str):
    active_bits = []
    for i, e in enumerate(reversed(bin_str[2:])):
        if e == "1":
            active_bits.append(i)
    return sorted(active_bits, reverse=True)

def main():
    args = parse_options()
    bin_str = decode(args.hex)
    print(bin_str)
    print(active_bits(bin_str))

if __name__ == "__main__":
    main()

