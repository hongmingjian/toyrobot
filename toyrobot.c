#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NR_ELEMENTS(x) (sizeof(x)/sizeof(x[0]))
#define WIDTH  5
#define HEIGHT 5

enum FACE {NORTH=0, WEST, SOUTH, EAST, NR_FACES};
struct robot {
    int x, y;
    enum FACE face;
};

const char *g_face_names[] = {
    "NORTH",
    "WEST",
    "SOUTH",
    "EAST"
};

const char *g_cmd_names[] = {
    "PLACE",
    "MOVE",
    "LEFT",
    "RIGHT",
    "REPORT"
};

static int name_to_index(const char *names[], int n, char *name)
{
    for(int i = 0; i < n; i++)
        if(!strcmp(names[i], name))
            return i;
    return -1;
}

/**
 * Trim the space in a string of range [q, *pp)
 */
static char *string_trim(char *q, char **pp)
{
    while(1) {
        if(!*q)
            break;
        if(!isspace(*q))
            break;
        ++q;
    }

    while(1) {
        if(*pp <= q)
            break;
        if(!isspace(*(*pp-1)))
            break;
        --(*pp);
    }

    return q;
}

/**
 * Validate the range [q, p) for atoi
 */
static int validate_atoi(char *q, char *p)
{
    q = string_trim(q, &p);
    if(q == p)
        return 0;

    if(*q == '-' || *q == '+')
        q++;
    for(; q < p; q++) {
        if(!isdigit(*q))
            return 0;
    }

    return 1;
}

static int place(struct robot *r, char *args)
{
	int x, y, face;

    char *q, *p, *endp = args + strlen(args);
	q = string_trim(args, &endp);
    p = strchr(q, ',');
    if(!p || !validate_atoi(q, p)) {
        return -2;
    }
    x = atoi(q);

    q = p + 1;
    p = strchr(q, ',');
    if(!p || !validate_atoi(q, p)) {
        return -2;
    }
    y = atoi(q);

    q = string_trim(p+1, &endp);
    face = name_to_index(g_face_names, NR_ELEMENTS(g_face_names), q);

    if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT || face < 0) {
        return -2;
    }

    r->x = x;
    r->y = y;
    r->face = face;

	return 0;
}

static int move(struct robot *r, char *args)
{
	if(r->x < 0 || r->x >= WIDTH || r->y < 0 || r->y >= HEIGHT) {
        return -1;
    }
	if(*args) {
		return -2;
	}

    switch(r->face) {
    case NORTH:
        if(r->y < HEIGHT-1)
            r->y++;
        break;
    case SOUTH:
        if(r->y > 0)
            r->y--;
        break;
    case EAST:
        if(r->x < WIDTH-1)
            r->x++;
        break;
    case WEST:
        if(r->x > 0)
            r->x--;
        break;
    default:
//        fprintf(stderr, "Wrong face value %d\n", r->face);
		return -10;
        break;
    }
	return 0;
}

static int left(struct robot *r, char *args)
{
	if(r->x < 0 || r->x >= WIDTH || r->y < 0 || r->y >= HEIGHT) {
        return -1;
    }
	if(*args) {
		return -2;
	}

    r->face++;
    r->face %= NR_FACES;
	return 0;
}

static int right(struct robot *r, char *args)
{
	if(r->x < 0 || r->x >= WIDTH || r->y < 0 || r->y >= HEIGHT) {
        return -1;
    }
	if(*args) {
		return -2;
	}

    /**
     * XXX - enum may be unsigned
     *       make sure r->face >= 0
     */
    if(!r->face)
        r->face += NR_FACES;

    r->face--;
	return 0;
}

static int report(struct robot *r, char *args)
{
	if(r->x < 0 || r->x >= WIDTH || r->y < 0 || r->y >= HEIGHT) {
        return -1;
    }
	if(*args) {
		return -2;
	}

    printf("%d, %d, %s\n", r->x, r->y, g_face_names[r->face]);
	return 0;
}

int (*g_cmd_actions[])(struct robot *r, char *args) = {
	place,
    move,
    left,
    right,
    report
};

int main(int argc, char *argv[])
{
    char *p, *line, buf[64], *endp;
    struct robot robot1 = {-1, -1, NORTH};
    int cmd, line_num = 0;

    do {
        line_num++;

        if(!fgets(&buf[0], NR_ELEMENTS(buf), stdin))
            break;

        endp = &buf[0] + strlen(buf);
        line = string_trim(&buf[0], &endp);
        if(line == endp)
            continue;
        *endp = '\0';

		p = line;
		while(1) {
            if(!*p)
				break;
			if(isspace(*p))
				break;
			++p;
		}
		if(*p) {
			*p = '\0';
			++p;
		}

		cmd = name_to_index(g_cmd_names, NR_ELEMENTS(g_cmd_names), line);
		if(cmd < 0) {
            fprintf(stderr, "%d: Unknown command \"%s\"\n", line_num, line);
            continue;
        }
        switch(g_cmd_actions[cmd](&robot1, p)) {
		case 0:
			break;
		case -1:
			printf("%d: Ignoring command %s\n", line_num, line);
			break;
		case -2:
			printf("%d: Bad argument %s\n", line_num, line);
			break;
		default:
			printf("%d: Unknown error %s\n", line_num, line);
			break;
		}
    } while(1);

    return 0;
}

