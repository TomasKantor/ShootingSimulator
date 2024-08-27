import csv
import matplotlib.pyplot as plt
from math import sqrt


file_path = 'output.txt'
numbers_list = []
pos_y = []
pos_x = []

with open(file_path, mode='r') as csv_file:
    csv_reader = csv.reader(csv_file)
    for row in csv_reader:
        numbers_list.append([float(item.strip()) for item in row])

    for row in numbers_list:
        pos_y.append(row[1])
        x = sqrt(row[0]**2 + row[2]**2)
        pos_x.append(x)


    curves_x = []
    curves_y = []
    current_x = []
    current_y = []
    for x, y in zip(pos_x, pos_y):
        if x == 0:
            curves_x.append(current_x)
            curves_y.append(current_y)
            current_x = [x]
            current_y = [y]
        else:
            current_x.append(x)
            current_y.append(y)


    max_distance = max(max(pos_x), max(pos_y))
    min_distance = min(pos_y)
    plt.ylim(min(pos_y), max_distance)

    plt.plot(pos_x[:1], pos_y[:1], 'gx')
    plt.plot(pos_x[1:2], pos_y[1:2], 'rx')

    for i, (x, y) in enumerate(zip(curves_x[2:], curves_y[2:])):
        plt.plot(x, y, label=f'Curve {i+1}')
    plt.legend()
    plt.xlabel('Horizontal position')
    plt.ylabel('Vertical Position')
    plt.title('Ballistic curve')
    plt.savefig('plot.png')