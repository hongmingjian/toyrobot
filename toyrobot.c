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
    "MOVE",
    "LEFT",
    "RIGHT",
    "REPORT"
//    "PLACE"  /*XXX - PLACE is treated differently*/
};

static int name_to_index(const char *names[], int n, char *name)
{
    for(int i = 0; i < n; i++)
        if(!strncmp(names[i], name, strlen(names[i])))
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
static void move(struct robot *r)
{
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
        fprintf(stderr, "Error: Wrong face value %d\n", r->face);
        break;
    }
}

static void left(struct robot *r)
{
    r->face++;
    r->face %= NR_FACES;
}

static void right(struct robot *r)
{
    /**
     * XXX - enum may be unsigned
     *       make sure r->face >= 0
     */
    if(!r->face)
        r->face += NR_FACES;

    r->face--;
}

static void report(struct robot *r)
{
    printf("%d, %d, %s\n", r->x, r->y, g_face_names[r->face]);
}

void (*g_cmd_actions[])(struct robot *r) = {
    move,
    left,
    right,
    report
};

int main(int argc, char *argv[])
{
    char *line, buf[64], *endp;
    struct robot robot1 = {-1, -1, NORTH};
    int line_num = 0;

    do {
        line_num++;

        if(!fgets(&buf[0], NR_ELEMENTS(buf), stdin))
            break;

        endp = &buf[0] + strlen(buf);
        line = string_trim(&buf[0], &endp);
        if(line == endp)
            continue;
        *endp = '\0';

        if(!strncmp(line, "PLACE", 5)) {
            int x, y, face;
            char *q = line+5, *p;

            if(!isblank(*q)) {
                fprintf(stderr, "Error:%d: Bad command \"%s\"\n", line_num, line);
                continue;
            }

            p = strchr(q, ',');
            if(!p || !validate_atoi(q, p)) {
                fprintf(stderr, "Error:%d: Bad command \"%s\"\n", line_num, line);
                continue;
            }
            x = atoi(q);

            q = p + 1;
            p = strchr(q, ',');
            if(!p || !validate_atoi(q, p)) {
                fprintf(stderr, "Error:%d: Bad command \"%s\"\n", line_num, line);
                continue;
            }
            y = atoi(q);

            q = string_trim(p+1, &endp);
            face = name_to_index(g_face_names, NR_ELEMENTS(g_face_names), q);

            if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT || face < 0) {
                fprintf(stderr, "Error:%d: Bad argument \"%s\"\n", line_num, line);
                continue;
            }

            robot1.x = x;
            robot1.y = y;
            robot1.face = face;
        } else {
            if(robot1.x < 0 || robot1.x >= WIDTH || robot1.y < 0 || robot1.y >= HEIGHT) {
                fprintf(stderr, "Warning:%d: Ignoring command \"%s\"\n", line_num, line);
                continue;
            }

            int cmd = name_to_index(g_cmd_names, NR_ELEMENTS(g_cmd_names), line);
            if(cmd < 0) {
                fprintf(stderr, "Error:%d: Unknown command \"%s\"\n", line_num, line);
                continue;
            }

            g_cmd_actions[cmd](&robot1);
        }
    } while(1);

    return 0;
}

