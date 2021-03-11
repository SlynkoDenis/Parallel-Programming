#!/usr/bin/python3

import matplotlib.pyplot as plt
import argparse
import csv
import os


parser = argparse.ArgumentParser()
parser.add_argument("--method", type=str,
                    help="Plot graph for particular method")
cmd_args = parser.parse_args()

path_to_csv = '/home/denis/Desktop/example.csv'
# path_to_csv = os.path.join(os.path.dirname(os.path.realpath(__file__)), cmd_args.method, 'data.csv')
if not os.path.isfile(path_to_csv):
    raise ValueError(f"unknown path: {path_to_csv}")

values = []
with open(path_to_csv) as csvfile:
    spamreader = csv.reader(csvfile, delimiter=',', quotechar='|')
    for row in spamreader:
        values += [int(x) for x in row]

print(values)
