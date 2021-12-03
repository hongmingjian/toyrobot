# -*- coding: utf-8 -*-

import ply.lex as lex
import ply.yacc as yacc

def p_error(t):
    print("%d: Syntax error '%s'" % (t.lexer.lineno, t.value))
    pass

tokens = (
    'ID',
    'ACTION',
    'FACE',
    'NUMBER',
    'newline',
)

actions = (
    'PLACE',
    'MOVE',
    'LEFT',
    'RIGHT',
    'REPORT',
)

faces = (
    'NORTH',
    'WEST',
    'SOUTH',
    'EAST',
)

def TRLexer():
    literals = [',']
    t_ignore = ' \t'

    def t_error(t):
        print("%d: Illegal character '%s'" % (t.value[0], t.lexer.lineno))
        pass

    def t_newline(t):
        r'\n+'
        t.lexer.lineno += len(t.value)
        return t

    def t_NUMBER(t):
        r'[+-]?\d+'
        try:
            t.value = int(t.value)
        except ValueError as e:
            print(e)
            t.value = -1
        return t

    def t_ID(t):
        r'[^ \t\n,]+'
        if t.value in actions:
            t.type = 'ACTION'
        elif t.value in faces:
            t.type = 'FACE'
        else:
            #print("%d: Unknown token '%s'" % (t.lexer.lineno, t.value))
            pass

        return t

    return lex.lex()

def TRParser(robot, data):
    def p_lp1(p):
        "lines : lines line newline"
    def p_lp2(p):
        "lines : line newline"

    def p_ll1(p):
        'line : ACTION term'
        x = p[2][0]
        y = p[2][1]
        face = p[2][2]

        if x < 0 or x >= 5 or y < 0 or y >= 5 or (face not in faces):
            print('%d: Bad argument %s' % (p.lineno(1), str(p[2])))
            return

        robot[0] = x
        robot[1] = y
        robot[2] = face

    def p_ll2(p):
        'line : ACTION'
        if robot[0] < 0 or robot[0] >= 5 or robot[1] < 0 or robot[1] >= 5 or robot[2] not in faces:
            print('%d: Ignoring "%s"' % (p.lineno(1), p[1]))
            return

        if p[1] == 'REPORT':
            print('%d, %d, %s' % (robot[0], robot[1], robot[2]))
        elif p[1] == 'MOVE':
            if robot[2] == 'NORTH':
                if robot[1] < (5-1):
                    robot[1] += 1
            elif robot[2] == 'SOUTH':
                if robot[1] > 0:
                    robot[1] -= 1
            elif robot[2] == 'EAST':
                if robot[0] < (5-1):
                    robot[0] += 1
            elif robot[2] == 'WEST':
                if robot[0] > 0:
                    robot[0] -= 1
        elif p[1] == 'LEFT':
            if robot[2] == 'NORTH':
                robot[2] = 'WEST'
            elif robot[2] == 'SOUTH':
                robot[2] = 'EAST'
            elif robot[2] == 'EAST':
                robot[2] = 'NORTH'
            elif robot[2] == 'WEST':
                robot[2] = 'SOUTH'
        elif p[1] == 'RIGHT':
            if robot[2] == 'NORTH':
                robot[2] = 'EAST'
            elif robot[2] == 'SOUTH':
                robot[2] = 'WEST'
            elif robot[2] == 'EAST':
                robot[2] = 'SOUTH'
            elif robot[2] == 'WEST':
                robot[2] = 'NORTH'
        else:
            # XXX - p[1] must be PLACE
            print('%d: Missing arguments "%s"' % (p.lineno(1), p[1]))

    def p_lt2(p):
        'term : NUMBER "," NUMBER "," FACE'
        p[0] = (p[1], p[3], p[5])

    yacc.yacc(debug=0, write_tables=0).parse(input=data, lexer=TRLexer())
    return

if __name__=='__main__':

    import sys

    robot = [-1, -1, 'NORTH'] #XXX [0] -> x, [1] -> y, [2] -> face

    text = sys.stdin.read()
    TRParser(robot, text)
