all:
	# c++ -std=c++17 -g -Wall -Werror -Wextra -Werror=no-unused-but-set-variable  main.cpp -o main
	c++ -std=c++17 -g main.cpp -o main
	./main