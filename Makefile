all: toyrobot

toyrobot: toyrobot.c
	$(CC) -o $@ $<

.PHONY: test
test: toyrobot
	cat cmds.txt | ./toyrobot	
	cat cmds.txt | python toyrobot.py	

.PHONY: clean
clean:
	$(RM) toyrobot
