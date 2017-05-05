JSON = -ljson
all: judge
judge: judge.cpp
	g++ judge.cpp -o judge $(JSON)