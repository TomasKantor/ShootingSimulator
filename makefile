all: build run

build: main.cpp
	g++-11 -o main.out main.cpp

run:
	./main.out > output.txt

visualize:
	python3 visualize.py

clean:
	rm main