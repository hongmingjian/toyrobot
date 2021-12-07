all: toyrobot1 toyrobot2

toyrobot1: toyrobot.c
	$(CC) -o $@ $<

toyrobot2: toyrobot.cpp
	$(CXX) -std=c++11 -o $@ $<

.PHONY: test
test: all
#	cat cmds.txt | ./toyrobot1
	cat cmds.txt | ./toyrobot2
#	cat cmds.txt | python toyrobot.py	

.PHONY: clean
clean:
	$(RM) toyrobot1 toyrobot2
