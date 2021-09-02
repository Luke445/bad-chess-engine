import pygame
import os

pygame.init()

screen = pygame.display.set_mode((800, 800))
clock = pygame.time.Clock()
path = os.path.dirname(os.path.abspath(__file__)) + "/images/"
images = {
    ("K", 0): pygame.image.load(path + "black_king.png"),
    ("K", 1): pygame.image.load(path + "white_king.png"),
    ("Q", 0): pygame.image.load(path + "black_queen.png"),
    ("Q", 1): pygame.image.load(path + "white_queen.png"),
    ("R", 0): pygame.image.load(path + "black_rook.png"),
    ("R", 1): pygame.image.load(path + "white_rook.png"),
    ("B", 0): pygame.image.load(path + "black_bishop.png"),
    ("B", 1): pygame.image.load(path + "white_bishop.png"),
    ("N", 0): pygame.image.load(path + "black_knight.png"),
    ("N", 1): pygame.image.load(path + "white_knight.png"),
    ("P", 0): pygame.image.load(path + "black_pawn.png"),
    ("P", 1): pygame.image.load(path + "white_pawn.png")
}


class Board:
    def __init__(self):
        self.array = [[("R", 0), ("N", 0), ("B", 0), ("Q", 0), ("K", 0), ("B", 0), ("N", 0), ("R", 0)],
                      [("P", 0)] * 8,
                      [("X", 1)] * 8,
                      [("X", 1)] * 8,
                      [("X", 1)] * 8,
                      [("X", 1)] * 8,
                      [("P", 1)] * 8,
                      [("R", 1), ("N", 1), ("B", 1), ("Q", 1), ("K", 1), ("B", 1), ("N", 1), ("R", 1)]]
        self.selected_piece = ()
        self.is_whites_turn = True
        self.white_short_castle = True
        self.white_long_castle = True
        self.black_short_castle = True
        self.black_long_castle = True

    def get_square(self, pos):
        return self.array[pos[0]][pos[1]]

    def move_piece(self, pos1, pos2):
        self.array[pos2[0]][pos2[1]] = self.get_square(pos1)
        self.array[pos1[0]][pos1[1]] = ("X", 1)
        moved_piece = self.get_square(pos2)
        if moved_piece[0] == "P" and pos2[0] in (0, 7):
            self.array[pos2[0]][pos2[1]] = ("Q", moved_piece[1])
        elif moved_piece[0] == "K":
            if pos2 == (7, 2) and self.white_long_castle:
                self.move_piece((7, 0), (7, 3))
            elif pos2 == (7, 6) and self.white_short_castle:
                self.move_piece((7, 7), (7, 5))
            elif pos2 == (0, 2) and self.black_long_castle:
                self.move_piece((0, 0), (0, 3))
            elif pos2 == (0, 6) and self.black_short_castle:
                self.move_piece((0, 7), (0, 5))
        if moved_piece[0] in ("K", "R"):
            if moved_piece == ("K", 1):
                self.white_short_castle = False
                self.white_long_castle = False
            elif moved_piece == ("K", 0):
                self.black_short_castle = False
                self.black_long_castle = False
            elif pos1 == (0, 0):
                self.black_long_castle = False
            elif pos1 == (7, 0):
                self.white_long_castle = False
            elif pos1 == (0, 7):
                self.black_short_castle = False
            elif pos1 == (7, 7):
                self.white_short_castle = False

    def piece_clicked(self, pos):
        if self.selected_piece == ():
            square = self.get_square(pos)
            if square[0] != "X" and square[1] == self.is_whites_turn:
                self.selected_piece = pos
        else:
            if pos in self.get_valid_moves(self.selected_piece):
                self.move_piece(self.selected_piece, pos)
                self.is_whites_turn = not self.is_whites_turn
            self.selected_piece = ()

    def is_valid_square(self, pos, piece_color, pawn_mode=False):
        if pos[0] in range(8) and pos[1] in range(8):
            if pawn_mode:
                return self.get_square(pos)[0] != "X" and piece_color != self.get_square(pos)[1]
            else:
                return self.get_square(pos)[0] == "X" or piece_color != self.get_square(pos)[1]
        else:
            return False

    def _rook(self, pos):
        r, c = pos
        piece = self.get_square(pos)
        out = []
        for i in range(1, 8):
            if self.is_valid_square((r, c + i), piece[1]):
                out.append((r, c + i))
                if self.get_square((r, c + i))[0] != "X":
                    break
            else:
                break
        for i in range(1, 8):
            if self.is_valid_square((r + i, c), piece[1]):
                out.append((r + i, c))
                if self.get_square((r + i, c))[0] != "X":
                    break
            else:
                break
        for i in range(1, 8):
            if self.is_valid_square((r, c - i), piece[1]):
                out.append((r, c - i))
                if self.get_square((r, c - i))[0] != "X":
                    break
            else:
                break
        for i in range(1, 8):
            if self.is_valid_square((r - i, c), piece[1]):
                out.append((r - i, c))
                if self.get_square((r - i, c))[0] != "X":
                    break
            else:
                break
        return out

    def _bishop(self, pos):
        r, c = pos
        piece = self.get_square(pos)
        out = []
        for i in range(1, 8):
            if self.is_valid_square((r + i, c + i), piece[1]):
                out.append((r + i, c + i))
                if self.get_square((r + i, c + i))[0] != "X":
                    break
            else:
                break
        for i in range(1, 8):
            if self.is_valid_square((r - i, c + i), piece[1]):
                out.append((r - i, c + i))
                if self.get_square((r - i, c + i))[0] != "X":
                    break
            else:
                break
        for i in range(1, 8):
            if self.is_valid_square((r + i, c - i), piece[1]):
                out.append((r + i, c - i))
                if self.get_square((r + i, c - i))[0] != "X":
                    break
            else:
                break
        for i in range(1, 8):
            if self.is_valid_square((r - i, c - i), piece[1]):
                out.append((r - i, c - i))
                if self.get_square((r - i, c - i))[0] != "X":
                    break
            else:
                break
        return out

    def get_valid_moves(self, pos):
        r, c = pos
        piece = self.get_square(pos)
        out = []
        if piece[0] == "K":
            moves = [(r, c + 1), (r, c - 1), (r - 1, c + 1), (r + 1, c + 1),
                     (r + 1, c), (r - 1, c), (r + 1, c - 1), (r - 1, c - 1)]
            for move in moves:
                if self.is_valid_square(move, piece[1]):
                    out.append(move)
            if piece[1] == 1 and self.white_long_castle and self.get_square((7, 3))[0] == "X" and self.get_square((7, 2))[0] == "X" and self.get_square((7, 1))[0] == "X":
                out.append((7, 2))
            if piece[1] == 1 and self.white_short_castle and self.get_square((7, 6))[0] == "X" and self.get_square((7, 5))[0] == "X":
                out.append((7, 6))
            if piece[1] == 0 and self.black_long_castle and self.get_square((0, 3))[0] == "X" and self.get_square((0, 2))[0] == "X" and self.get_square((0, 1))[0] == "X":
                out.append((0, 2))
            if piece[1] == 0 and self.black_short_castle and self.get_square((0, 6))[0] == "X" and self.get_square((0, 5))[0] == "X":
                out.append((0, 6))
        elif piece[0] == "Q":
            out = self._rook(pos) + self._bishop(pos)
        elif piece[0] == "R":
            out = self._rook(pos)
        elif piece[0] == "B":
            out = self._bishop(pos)
        elif piece[0] == "N":
            moves = [(r + 2, c + 1), (r + 2, c - 1), (r - 2, c + 1), (r - 2, c - 1),
                     (r + 1, c + 2), (r + 1, c - 2), (r - 1, c + 2), (r - 1, c - 2)]
            for move in moves:
                if self.is_valid_square(move, piece[1]):
                    out.append(move)
        elif piece[0] == "P":
            if piece[1]:
                if self.get_square((r - 1, c))[0] == "X":
                    out.append((r - 1, c))
                if r == 6 and self.get_square((r - 2, c))[0] == "X":
                    out.append((r - 2, c))
                if self.is_valid_square((r - 1, c + 1), True, True):
                    out.append((r - 1, c + 1))
                if self.is_valid_square((r - 1, c - 1), True, True):
                    out.append((r - 1, c - 1))
            else:
                if self.get_square((r + 1, c))[0] == "X":
                    out.append((r + 1, c))
                if r == 1 and self.get_square((r + 2, c))[0] == "X":
                    out.append((r + 2, c))
                if self.is_valid_square((r + 1, c + 1), False, True):
                    out.append((r + 1, c + 1))
                if self.is_valid_square((r + 1, c - 1), False, True):
                    out.append((r + 1, c - 1))

        return out


def redraw():
    screen.fill((255, 255, 255))

    dark_square = False
    for row in range(8):
        for column in range(8):
            if dark_square:
                pygame.draw.rect(screen, (0, 0, 0), (column * 100, row * 100, 100, 100))
            dark_square = not dark_square

            piece = board.get_square((row, column))
            if piece[0] != "X":
                screen.blit(images[piece], (column * 100, row * 100))
        dark_square = not dark_square

    if board.selected_piece != ():
        for pos in board.get_valid_moves(board.selected_piece):
            pygame.draw.circle(screen, (100, 100, 100), (pos[1] * 100 + 50, pos[0] * 100 + 50), 10)

    pygame.display.flip()


board = Board()

while clock.tick(60):
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            exit()
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_q:
                exit()
            elif event.key == pygame.K_r:
                board.__init__()
            elif event.key == pygame.K_a:
                print("debug")
        elif event.type == pygame.MOUSEBUTTONDOWN:
            board.piece_clicked((event.pos[1] // 100, event.pos[0] // 100))

    redraw()
