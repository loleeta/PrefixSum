CPPFLAGS = -std=c++11 -Wall -Werror -pedantic -ggdb -pthread

Heaper : Heaper.cpp
	g++ $(CPPFLAGS) $< -o $@

all : Heaper
	@echo "Made it all!"

clean : 
	$(RM) $(ALL_OBJS) Heaper
