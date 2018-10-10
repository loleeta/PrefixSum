CPPFLAGS = -std=c++11 -Wall -Werror -pedantic -ggdb -pthread

hw2 : hw2.cpp
	g++ $(CPPFLAGS) $< -o $@

all : hw2
	@echo "Made it all!"

clean : 
	$(RM) $(ALL_OBJS) hw2
