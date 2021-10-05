import pygame
from pygame.locals import *
import subprocess


WIDTH = 600
HEIGHT = 600
BOX_WIDTH = WIDTH // 8
BOX_HEIGHT = HEIGHT // 8
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)


def handle_events():
    for event in pygame.event.get():
        if event.type == QUIT:
            quit()
            

def interface_with_program():
    proc.stdin.write(b'e4\n')
    proc.stdin.flush()
    print(proc.stdout.readline())


def draw_screen():
     screen.fill(WHITE)
     
     # draw grid
     is_white = True
     for x in range(8):
        for y in range(8):
            rect = pygame.Rect(x * BOX_WIDTH, y * BOX_HEIGHT, BOX_WIDTH, BOX_HEIGHT)
            if is_white:
                pygame.draw.rect(screen, WHITE, rect)
            else:
                pygame.draw.rect(screen, BLACK, rect)
            is_white = not is_white
        is_white = not is_white
        
     # draw peices
     # TODO
     
     pygame.display.flip()


def quit():
    pygame.quit()
    proc.terminate()
    exit(0)


pygame.init()
screen = pygame.display.set_mode((WIDTH, HEIGHT))
path = "/Library/Application Support/Adobe/Adobe PCD/cache/abc/board"

proc = subprocess.Popen([path],
                        stdin=subprocess.PIPE,
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE)

proc.stdin.write(b'y\n')
proc.stdin.flush()

while 1:
    handle_events()
    
    interface_with_program()

    draw_screen()
   
    
    