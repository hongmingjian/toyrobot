# -*- coding: utf-8 -*-

import ply.lex as lex
import ply.yacc as yacc

t_ignore  = ' \t'
def t_error(t):
    if t.value[0] in ('\r', '\n'):
        t.lexer.skip(1)
    else:
        #print("Illegal character '%s'" % t.value[0])
        t.lexer.skip(1)
        pass

def p_error(t):
    #print(t)
    pass

tokens = (
    'KEYWORD',
    'FACE',
    'NUMBER',
    'COMMA',
)

keywords = (
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
    t_COMMA = r','

    def t_NUMBER(t):
        r'[+-]?\d+'
        try:
            t.value = int(t.value)
        except ValueError as e:
            print(e)
            t.value = -1
        return t

    def t_FACE(t):
        r'[A-Z][A-Z]*'
        if t.value in keywords:
            t.type = 'KEYWORD'
        elif t.value in faces:
            t.type = 'FACE'

        return t

    return lex.lex()

def TRParser(robot, data):

    output = []

    def p_lp1(p):
        'lines : lines line'
    def p_lp2(p):
        'lines : line'

    def p_ll1(p):
        'line : KEYWORD term'
        x = p[2][0]
        y = p[2][1]
        face = p[2][2]

        if x < 0 or x >= 5 or y < 0 or y >= 5 or (face not in faces):
            print('Ignoring %s' % str(p[1:]))
            return

        robot[0] = x
        robot[1] = y
        robot[2] = face

    def p_ll2(p):
        'line : KEYWORD'
        if robot[0] < 0 or robot[1] < 0 or robot[2] not in faces:
            print('Ignoring %s' % str(p[1:]))
            return

        if p[1] == 'REPORT':
            print(robot[0], robot[1], robot[2])
        elif p[1] == 'MOVE':
            if robot[2] == 'NORTH':
                if robot[1] < 4:
                    robot[1] += 1
            elif robot[2] == 'SOUTH':
                if robot[1] > 0:
                    robot[1] -= 1
            elif robot[2] == 'EAST':
                if robot[0] < 4:
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
            print(p[1:])

    def p_lt2(p):
        'term : number COMMA number COMMA FACE'
        p[0] = (p[1], p[3], p[5])

    def p_lf1(p):
        'number : NUMBER'
        p[0] = p[1]

    yacc.yacc(debug=0, write_tables=0).parse(input=data, lexer=TRLexer())
    return output

if __name__=='__main__':

    robot = [-1, -1, 'NORTH'] #XXX [0] -> x, [1] -> y, [2] -> face

    try:
        with open('cmds.txt', 'rt') as f:
            text = f.read()
            TRParser(robot, text)

    except (OSError, UnicodeDecodeError) as reason:
        print(reason)
