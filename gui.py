import pygame
from pygame.locals import *
import subprocess
import os


WIDTH = 600
HEIGHT = 600
BOX_WIDTH = WIDTH // 8
BOX_HEIGHT = HEIGHT // 8
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
DARK_YELLOW = (138, 130, 33)
LIGHT_YELLOW = (240, 232, 129)
RED = (255, 0, 0)

IMAGE_NAMES = (
    "bb.png", "bn.png", "bq.png", "wb.png", "wn.png", "wq.png",
    "bk.png", "bp.png", "br.png", "wk.png",	"wp.png", "wr.png"
)
DIRNAME = os.path.dirname(__file__)


class Board:
    def __init__(self, path):
        self.proc = subprocess.Popen([path, "--gui"],
                        stdin=subprocess.PIPE,
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE)

        self.board = [
            ["r", "n", "b", "q", "k", "b", "n", "r"],
            ["p", "p", "p", "p", "p", "p", "p", "p"],
            [" ", " ", " ", " ", " ", " ", " ", " "],
            [" ", " ", " ", " ", " ", " ", " ", " "],
            [" ", " ", " ", " ", " ", " ", " ", " "],
            [" ", " ", " ", " ", " ", " ", " ", " "],
            ["P", "P", "P", "P", "P", "P", "P", "P"],
            ["R", "N", "B", "Q", "K", "B", "N", "R"] 
        ]
        self.selected_coords = (0, 0)
        self.is_piece_selected = False
        self.game_over = False
        self.checkmate_square = -1


        self.screen = pygame.display.set_mode((WIDTH, HEIGHT))
        self.images = {}

        for image in IMAGE_NAMES:
            if image[0] == "b":
                name = image[1]
            else:
                name = image[1].upper()

            self.images[name] = pygame.transform.scale(
                pygame.image.load(os.path.join(DIRNAME, "images", image)),
                (BOX_WIDTH, BOX_HEIGHT)
            )

    def pos_to_coords(self, pos):
        return (pos[0] // BOX_WIDTH, pos[1] // BOX_HEIGHT)

    def handle_events(self):
        for event in pygame.event.get():
            if event.type == QUIT:
                quit()
            elif event.type == MOUSEBUTTONUP:
                if not self.is_piece_selected:
                    self.selected_coords = self.pos_to_coords(event.pos)
                    self.is_piece_selected = True
                else:
                    new_coords = self.pos_to_coords(event.pos)
                    
                    piece = self.board[self.selected_coords[1]][self.selected_coords[0]]
                    flags = "0"  # only used for promotion
                    """
                    queen = 2
                    rook = 3
                    bishop = 4
                    knight = 5
                    """
                    # if piece == "p":
                    #     if new_coords[1] == 7:
                    #         print("black promoting")
                    if piece == "P":
                        if (new_coords[1] == 0):
                            flags = input("Promotion: ")
                        
                    self.send_input(
                        str(self.selected_coords[0]) + str(self.selected_coords[1]) +
                        str(new_coords[0]) + str(new_coords[1]) + flags
                    )
                    self.is_piece_selected = False

    def send_input(self, x):
        if self.proc.poll() is None:
            self.proc.stdin.write(x.encode("utf-8") + b"\n")
            self.proc.stdin.flush()
            self.read_proc()

    def read_proc(self):
        for line in iter(self.proc.stdout.readline, b""):
            print(line.decode("utf-8"), end="")
            if line == b"Enter Move (enter q to quit):\n":
                break
            elif line == b"----- NEW BOARD -----\n":
                for y in range(8):
                    line = self.proc.stdout.readline().decode("utf-8")
                    self.board[y] = line.replace("\n", "").split("-")[:8]
            elif line == b"White Wins!\n":
                self.game_over = True
                for x in range(8):
                    for y in range(8):
                        if self.board[y][x] == "k":
                            self.checkmate_square = (x, y)
            elif line == b"Black Wins!\n":
                self.game_over = True
                for x in range(8):
                    for y in range(8):
                        if self.board[y][x] == "K":
                            self.checkmate_square = (x, y)
            elif line == b"The game is a draw\n":
                print(line.decode("utf-8"), end="")
                self.game_over = True

    def draw_screen(self):
        self.screen.fill(BLACK)

        # draw grid
        is_white = True
        for x in range(8):
            for y in range(8):
                rect = pygame.Rect(x * BOX_WIDTH, y * BOX_HEIGHT, BOX_WIDTH, BOX_HEIGHT)

                if self.is_piece_selected and self.selected_coords == (x, y):
                    if is_white:
                        pygame.draw.rect(self.screen, LIGHT_YELLOW, rect)
                    else:
                        pygame.draw.rect(self.screen, DARK_YELLOW, rect)
                elif is_white:
                    pygame.draw.rect(self.screen, WHITE, rect)

                if (x, y) == self.checkmate_square:
                    pygame.draw.rect(self.screen, RED, rect)

                if self.board[y][x] != " ":
                    self.screen.blit(self.images[self.board[y][x]], rect)

                is_white = not is_white
            is_white = not is_white

        pygame.display.flip()


    def quit(self):
        pygame.quit()
        self.proc.terminate()
        exit(0)


pygame.init()

path = os.path.join(DIRNAME, "board")
b = Board(path)
b.read_proc()

clock = pygame.time.Clock()
while clock.tick(30):
    b.handle_events()

    b.draw_screen()
