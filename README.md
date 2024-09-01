# ShootingSimulator

## Simulation

### Build

This project requires catch2 framework to be installed on the system.
To build the project you need cmake and to run these commands:

`cmake .`

`cmake --build .`

To build the project I needed to use g++-11. I set the required compiler as follows:

`export CXX=g++-11`


### Run

To run the simulation simply run:

`../ShootingSimulator input.json output.json`

where input.json contains params of simulations.

## Visualization

To better check the results of simulation a simple visualization tool was created using python and matplotlib.

### Install

`pip install -r requirements.txt`

or

`pip install matplotlib`

### Run

`python3 visualize.py output.json`

![plot](./plot.png)