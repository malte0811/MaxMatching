#!/usr/bin/python3
import argparse
from os import listdir
import subprocess
import time

parser = argparse.ArgumentParser()
parser.add_argument("binary")
parser.add_argument("test_folder")

args = parser.parse_args()

known_optima = {
    "ar9152.dmx": 4349,
    "ch71009.dmx": 35025,
    "ei8246.dmx": 4123,
    "fixed.dmx": 500,
    "gr9882.dmx": 4931,
    "K2.dmx": 1,
    "K3.dmx": 1,
    "K4.dmx": 2,
    "lu980.dmx": 490,
    "P3.dmx": 2,
    "pbd984.dmx": 492,
    "peterson.dmx": 5,
    "pma343.dmx": 171,
    "queen10_10.dmx": 50,
    "queen11_11.dmx": 60,
    "queen16_16.dmx": 128,
    "queen27_27.dmx": 364,
    "queen4_4.dmx": 8,
    "queen5_5.dmx": 12,
    "queen6_6.dmx": 18,
    "queen7_7.dmx": 24,
    "queen8_8.dmx": 32,
    "queen9_9.dmx": 40,
    "simple.dmx": 2,
    "xqf131.dmx": 64
}

for file in listdir(args.test_folder):
    print("Running on " + file)
    start = time.time()
    output: bytes = subprocess.check_output([args.binary, args.test_folder + "/" + file])
    duration = time.time() - start
    first_line: str = output.splitlines()[0].decode('utf-8')
    num_edges = int(first_line.split(" ")[-1])
    if file not in known_optima:
        print("Unknown instance "+file+": "+str(num_edges)+" matching edges found in "+str(duration)+" s")
    elif num_edges != known_optima[file]:
        print("Wrong number of edges for "+file+": expected "+str(known_optima[file])+", found "+str(num_edges))
    else:
        print("Solution matching expected optimum for "+file+" found in "+str(duration)+" s")
