#!/usr/bin/env python
# -*- coding: utf-8 -*- 
"""
invasores.py

MAC420 EP1 - Evandro Fernandes Giovanini (nUSP 5173890)
Baseado no raquetebol.py escrito por Carlos Hitoshi Morimoto.

Este programa é uma implementação do jogo básico descrito pelo EP1 de MAC420.
Nele, o jogador controla uma nave que fica na parte inferior da tela e que pode
atirar nos monstros, que nascem na parte superior da tela. O objetivo do jogo é 
matar o maior número possível de monstros antes que 4 naves sejam destruídas.

Controles: ASD (JKL) controlam a direção da nave. Espaço atira nos monstros,  
N faz nascer um novo monstro mais poderoso e P pausa.

Controles de depuração: O entra no modo de depuração, I avança um passo.

Algumas mudanças pequenas foram feitas em relação ao enunciado. 
- Os controles foram ligeiramente alterados (como descrito acima) para não 
haver conflitos entre teclas;
- Quando o usuário perde, o jogo é reiniciado automaticamente e seu recorde
atualizado. Assim, a tecla N faz sempre a mesma função, simplificando a 
interface de usuário do programa. 

"""

import sys
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
from random import randint

# CONSTANTES
WIDTH     = 400
HEIGHT    = 500
DELTATIME =  20
DELTASPEED = 0.001
PROBABILIDADE = 1

TIRO_D     = 0.02

NAVE_W     = 0.08
NAVE_H     = 0.06

MONSTRO_W = 0.08
MONSTRO_H = 0.08

### CORES 

CORTIRO  = [0.45, 0.15, 0.0]
CORTIROM = [186.0/256.0, 148.0/256.0, 125.0/256.0]
CORBG    = (0.9, 0.9, 0.7, 1.0)
CORNAVE  = [0.4, 0.1, 0.0]
CORMONS  = [186.0/256.0, 148.0/256.0, 125.0/256.0]
COR_HUD  = [0.3, 0.1, 0.0]

# classes

class Status:
    """Mantem o estado do jogo"""
    def __init__(self):
        self.w = WIDTH
        self.h = HEIGHT
        self.debug = False
        self.step = False
        # Parametros de uma partida
        self.pontos = 0
        self.record = 0
        self.nivel = 0
        self.naves = 3
        self.motion = 0
        self.tiros = 0
        self.tirosMon = 0

gState = Status()

class Coisa:
    def __init__(self, t):
        self.x, self.y = 0.0, 0.0
        self.tipo = t  
        # a gente nao usa o tipo, mas é bonitinho dar um nome pras coisas
        self.ligado = True
        self.sv = 1
        self.sh = 1
    
    def setSpeed(self, s, m):
        self.speed = s
        self.minSpeed = m

    def slower(self):
        self.speed -= DELTASPEED
        if self.speed < minSpeed:
            self.speed = minSpeed

    def faster(self):
        self.speed += DELTASPEED

    def setDir(self, h, v): # sentido do tiro
        self.sv = v
        self.sh = h

class Tiro (Coisa):
    def __init__(self):
        Coisa("Tiro")

# tiro do monstro
class TiroMon (Coisa):
    def __init__(self):
        Coisa("TiroMon")
        
class Monstro (Coisa):
    def __init__(self):
        Coisa("Monstro")
        self.nivel = 1
        self.ligado = False
        self.speed = 0


class Scene ():
    """Mantem informacoes sobre a cena"""
    def __init__(self):
        self.Nave = Coisa("Nave")
        self.Monstro = Monstro()
        self.Tiro = Tiro()
        self.TiroMon = TiroMon()

gScene = Scene()


def PrintHelp (): 
    print ("\n====================================")
    print ("Tecla :-  Funcao \n")
    print ("A S D :- movimentos da nave para canhotos ")
    print ("J K L :- mesmo que as teclas A S D, mas para destros")
    print ("espaço :- dar um tiro")
    print ("H :- mostra essa mensagem ")
    print ("N :- cria um novo monstro. ")
    print ("P :- pausa ou continua o jogo ")
    print ("O :- entra no modo depuração ")
    print ("I :- mostra um passo da animação ")
    print ("Q :- sair do jogo ")
    print ("\n====================================\n")


# Função que desenha os objetos do jogo

def display ():
    # limpa a tela
    glClearColor(CORBG[0], CORBG[1], CORBG[2], CORBG[3])
    glClear(GL_COLOR_BUFFER_BIT)

    # desenho da nave
    if gScene.Nave.ligado:
        glColor(CORNAVE)
        glBegin(GL_TRIANGLES)
        glVertex3f(gScene.Nave.x + NAVE_W/2.0, gScene.Nave.y + NAVE_H, 0)
        glVertex3f(gScene.Nave.x, gScene.Nave.y, 0)
        glVertex3f(gScene.Nave.x + NAVE_W, gScene.Nave.y, 0)
        glEnd()
        #glRectf(gScene.Nave.x, gScene.Nave.y, gScene.Nave.x + NAVE_W, gScene.Nave.y + NAVE_H)

    # tiro
    if gScene.Tiro.ligado:
        glColor(CORTIRO)
        glRectf(gScene.Tiro.x, gScene.Tiro.y,
                gScene.Tiro.x+TIRO_D, gScene.Tiro.y+TIRO_D)

    # monstro
    if gScene.Monstro.ligado:
        glColor(CORMONS)
        glRectf(gScene.Monstro.x, gScene.Monstro.y,
                gScene.Monstro.x+MONSTRO_W, gScene.Monstro.y + MONSTRO_H)
    # olhos e boca do monstro
    if gScene.Monstro.ligado:
        glColor(CORBG)
        # boca
        glRectf(gScene.Monstro.x + 2.0*MONSTRO_W/8.0, gScene.Monstro.y + 2.0*MONSTRO_H/8.0,
                gScene.Monstro.x + 6.0*MONSTRO_W/8.0, gScene.Monstro.y + 3.0*MONSTRO_H/8.0)
        # olhos
        glRectf(gScene.Monstro.x + 2.0*MONSTRO_W/8.0, gScene.Monstro.y + 5.0*MONSTRO_H/8.0,
                gScene.Monstro.x + 3.0*MONSTRO_W/8.0, gScene.Monstro.y + 6.0*MONSTRO_H/8.0)
        
        glRectf(gScene.Monstro.x + 5.0*MONSTRO_W/8.0, gScene.Monstro.y + 5.0*MONSTRO_H/8.0,
                gScene.Monstro.x + 6.0*MONSTRO_W/8.0, gScene.Monstro.y + 6.0*MONSTRO_H/8.0)

    # tiro do monstro
    if gScene.TiroMon.ligado:
        glColor(CORTIROM)
        glRectf(gScene.TiroMon.x, gScene.TiroMon.y,
                gScene.TiroMon.x+TIRO_D, gScene.TiroMon.y+TIRO_D)

    # SCORE 
    glColor(COR_HUD)
    glRectf(0, gScene.Nave.y - 0.07, 1, gScene.Nave.y - 0.075)
    
    glRasterPos2f(0.03,0.07)
    for c in "Pontos: %5d"%gState.pontos:
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, ord(c))

    glColor(COR_HUD)
    glRasterPos2f(0.03,0.03)
    for c in "Recorde: %5d"%gState.record:
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, ord(c))

    glRasterPos2f(0.6,0.07)
    for c in "Nivel: %5d"%gState.nivel:
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, ord(c))

    glRasterPos2f(0.6,0.03)
    for c in "Naves: %5d"%gState.naves:
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, ord(c))
    
    glutSwapBuffers()


def reshape(w, h):
    gState.h = h
    gState.w = w

    glViewport(0,0,w,h)


# Funções que criam e apagam objetos do jogo

def criaNave():
    gScene.Nave.x = 0.45
    gScene.Nave.y = 0.2
    gScene.Nave.setSpeed(0.01, 0.01)

def destroiNave():
    gScene.Nave.x = 0.45
    gScene.Nave.y = 0.2
    gScene.Nave.setSpeed(0.01, 0.01)
    gState.naves -= 1

def criaMonstro():
    if gScene.Monstro.ligado == False:
        gState.nivel += 1
        gScene.Monstro.sh = 1
        gScene.Monstro.sv = 1
        # Cria o mosntro em uma posicao aleatoria
        # entre 0.1 e 0.9
        gScene.Monstro.x = randint(1,9)/10.0 - MONSTRO_W/2
        gScene.Monstro.y = 1.0 - MONSTRO_H
        gScene.Monstro.ligado = True
        if (gState.nivel == 0):
            gScene.Monstro.setSpeed(0.009, 0.009)
        else:
            gScene.Monstro.faster()
        return True
    else:
        print "Já existe um monstro ativo no jogo."
        return False

def destroiMonstro():
    gScene.Monstro.ligado = False
    gScene.Monstro.y = -1
    destroiTiroMon()

def criaTiro():
    gScene.Tiro.sh = 0
    gScene.Tiro.sv = 1
    gScene.Tiro.x = gScene.Nave.x + NAVE_W/2.0
    gScene.Tiro.y = gScene.Nave.y + NAVE_H
    gScene.Tiro.ligado = True
    gScene.Tiro.setSpeed(0.01, 0.01)

def destroiTiro():
    gScene.Tiro.x = -1
    gScene.Tiro.y = -1
    gScene.Tiro.ligado = False
    gState.tiros = 0

def criaTiroMon():
    gScene.TiroMon = TiroMon()
    gScene.TiroMon.sh = 0
    gScene.TiroMon.sv = -1
    gScene.TiroMon.x = gScene.Monstro.x + MONSTRO_W/2.0
    gScene.TiroMon.y = gScene.Monstro.y 
    gScene.TiroMon.ligado = True
    gScene.TiroMon.setSpeed(0.01, 0.01)

def destroiTiroMon():
    gScene.TiroMon.x = -1
    gScene.TiroMon.y = -1
    gScene.TiroMon.ligado = False
    gState.tirosMon = 0


def verificaFim():
    # se o jogo terminou, guarda o recorde e recomeça
    if (gState.naves == -1):
        if (gState.pontos > gState.record):
            gState.record = gState.pontos
        gState.pontos = 0
        gState.nivel = 0
        gState.naves = 3
        destroiMonstro()
        destroiTiro()
        destroiTiroMon()

# Função do passo da execução do programa    
def step():
    # update nave 
    gScene.Nave.x += gState.motion * gScene.Nave.speed

    if (gScene.Nave.x < 0):
        gState.motion = 0
        gScene.Nave.x = 0
    if (gScene.Nave.x > 1 - NAVE_W):
        gState.motion = 0
        gScene.Nave.x = 1 - NAVE_W

    # update monstro
    if (gScene.Monstro.ligado):
        gScene.Monstro.x += gScene.Monstro.sh * gScene.Monstro.speed
        # monstro chegou ao lado esq
        if (gScene.Monstro.x < 0):
            gScene.Monstro.sh  *=  -1
            gScene.Monstro.x  *= -1
            gScene.Monstro.y = gScene.Monstro.y - MONSTRO_H/2
        # monstro chegou ao lado dir
        elif (gScene.Monstro.x > 1 - MONSTRO_W):
            gScene.Monstro.sh  *=  -1
            gScene.Monstro.x  *= 1
            gScene.Monstro.y = gScene.Monstro.y - MONSTRO_H/2
        # monstro bateu na nave
        elif (gScene.Monstro.y + MONSTRO_H < gScene.Nave.y + 1.5 * NAVE_H):
            destroiNave()
            verificaFim()
            destroiTiro()
            destroiMonstro()
            destroiTiroMon()

    # update tiro
    if (gScene.Tiro.ligado):
        gScene.Tiro.y += gScene.Tiro.sv * gScene.Tiro.speed
        # Tiro chegou ao topo sem atingir nada.
        if (gScene.Tiro.y > 1 - TIRO_D):
            destroiTiro()

        # detectar se houve hit entre tiro e monstro
        #if abs(gScene.Tiro.x - gScene.Monstro.x) <= MONSTRO_W and abs(gScene.Tiro.y - gScene.Monstro.y) <= MONSTRO_H:
        #if abs(gScene.Tiro.x - gScene.Monstro.x) <= MONSTRO_W and abs(gScene.Tiro.y) > abs(gScene.Monstro.y):
        if (gScene.Monstro.x < gScene.Tiro.x) and \
           (gScene.Monstro.x + MONSTRO_W > gScene.Tiro.x + TIRO_D) and \
           (gScene.Monstro.y < gScene.Tiro.y + TIRO_D):
           destroiTiro()
           gState.pontos += 1
           destroiMonstro()

    # update tiro do monstro
    if (gScene.TiroMon.ligado):
        gScene.TiroMon.y += gScene.TiroMon.sv * gScene.TiroMon.speed
        if (gScene.TiroMon.y + TIRO_D < gScene.Nave.y):
            destroiTiroMon()

        # detectar se houve hit entre tiroMon e nave
        if (gScene.Nave.x < gScene.TiroMon.x) and \
           (gScene.Nave.x + NAVE_W > gScene.TiroMon.x + TIRO_D) and \
           (gScene.Nave.y > gScene.TiroMon.y):
           destroiTiroMon()
           destroiNave()
           verificaFim()

    # atirar com probabilidade P
    if (randint(0,100) < PROBABILIDADE):
        if (gScene.TiroMon.ligado == False and gState.tirosMon == 0):
            gScene.TiroMon.ligado = True
            gState.tirosMon = gState.tirosMon + 1
            gScene.TiroMon.sh = 0
            gScene.TiroMon.sv = -1
            gScene.TiroMon.x = gScene.Monstro.x + MONSTRO_W/4.0
            gScene.TiroMon.y = gScene.Monstro.y 

    if (gState.debug):
        print("Posição: nave = [%5.2g, %5.2g] \t monstro = [%5.2g, %5.2g]\n"%(gScene.Nave.x, gScene.Nave.y, gScene.Monstro.x, gScene.Monstro.y))


def animacao (id):
    if gState.debug:
        if gState.step:
            gState.step = False
            step()
    else:
        step()
    
    glutPostRedisplay()
    glutTimerFunc(DELTATIME, animacao, 0)



# Funções de menu, teclado e mouse

def MainMenu (op):
    global gState
    if op == 0:
        if (gState.naves >= 0):
            if (criaMonstro() == True):
                criaTiroMon()

    elif op == 1:  # debug on/off 
        if gState.debug == True:
            print("Modo debug foi desligado\n")
            gState.debug = False
        else:
            print("Modo debug foi ligado\n")
            print("Use a tecla I para passo-a-passo, ou a tecla O para continuar.\n")
            gState.step = False
            gState.debug = True

    elif op == 2:  # help option 
        PrintHelp()

    elif op == 3:  # quit option 
        sys.exit (0)


def mouse (btn, st, x, y):
    if btn == GLUT_LEFT_BUTTON:
        if st == GLUT_DOWN and gState.debug:
            gState.step = True


def keyboard (key, x, y):
    if key == 'q' or key == 'Q':
        sys.exit(0)
  
    if key == 'h' or key == 'H':
        PrintHelp()

    elif key == ' ' or key == ' ':
        if (gScene.Tiro.ligado == False and gState.tiros == 0):
            criaTiro()

    elif key == 'a' or key == 'A' or key == 'j' or key == 'J':
        gState.motion = -1

    elif key == 's' or key == 'S' or key == 'k' or key == 'K':
        gState.motion = 0

    elif key == 'd' or key == 'D' or key == 'l' or key == 'L':
        gState.motion = 1

    elif key == 'i' or key == 'I':
        if gState.debug == True:
            gState.step = True
      
    elif key == 'o' or key == 'O':
        if gState.debug == True:
            print("Modo debug foi desligado\n")
            gState.debug = False
        else:
            print("Modo debug foi ligado\n")
            print("Use a tecla I para passo-a-passo, ou a tecla O para continuar.\n")
            gState.step = False
            gState.debug = True

    elif key == 'n' or key == 'N':
        if (gState.naves >= 0):
            if (criaMonstro() == True):
                criaTiroMon()


# Função de inicialização dos objetos

def myInit():
    # inicializa a parte gráfica
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluOrtho2D(0.0, 1.0, 0.0, 1.0)
    glMatrixMode(GL_MODELVIEW)

    PrintHelp()

    # estado inicial do jogo
    gState = Status()
    gState.pontos = 0
    gState.nivel = 1
    gState.naves = 3

    # cria e inicializa objetos
    gScene.Nave = Coisa(gScene.Nave)
    criaNave()

    gScene.Tiro = Tiro()
    gScene.Tiro.ligado = False

    gScene.Monstro = Monstro()
    criaMonstro()

    gScene.TiroMon = TiroMon()
    criaTiroMon()


if __name__ == "__main__":
    glutInit( sys.argv )
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB)
    glutInitWindowSize(WIDTH, HEIGHT)
    glutCreateWindow("Invasores")

    myInit()

    # callbacks
    glutDisplayFunc(display)
    glutReshapeFunc(reshape)
    glutMouseFunc(mouse)
    glutKeyboardFunc(keyboard)

    glutTimerFunc(DELTATIME, animacao, 0) # comecar animacao

    # criacao do menu e submenus
    glutCreateMenu(MainMenu)
    glutAddMenuEntry("Novo monstro [n]", 0)
    glutAddMenuEntry("Pausa/continua [p]", 1)
    glutAddMenuEntry("Ajuda [h]", 2)
    glutAddMenuEntry("Saida [q]", 3)
    glutAttachMenu(GLUT_RIGHT_BUTTON)
    
    glutMainLoop()


