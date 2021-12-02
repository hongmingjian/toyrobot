all: toyrobot

toyrobot: toyrobot.c
	$(CC) -o $@ $<

.PHONY: test
test: toyrobot
	cat cmds.txt | ./toyrobot	

.PHONY: clean
clean:
	$(RM) toyrobot
