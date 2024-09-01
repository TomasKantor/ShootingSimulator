import csv
import matplotlib.pyplot as plt
from math import sqrt
import json
import sys

if len(sys.argv) < 2:
    print('Usage: python visualize.py <file_path>')
    sys.exit(1)

file_path = sys.argv[1]

def convert_to_2d(pos):
    return sqrt(pos[0]*pos[0]+pos[2]*pos[2]), pos[1]

with open(file_path, mode='r') as f:
    data = json.load(f)

    start_h, start_v = convert_to_2d(data["start"])
    plt.plot(start_h, start_v, 'gx')

    target_h, target_v = convert_to_2d(data["target"])
    plt.plot(target_h, target_v, 'rx')


    for i, curve in enumerate(data["curves"]):
        curve_x, curve_y = zip(*[convert_to_2d(pos) for pos in curve])
        plt.plot(curve_x, curve_y, label=f'Curve {i+1}')

    plt.axis('equal')
    plt.legend()
    plt.xlabel('Horizontal position')
    plt.ylabel('Vertical Position')
    plt.title('Ballistic curve')
    plt.savefig('plot.png')