"""
this file is used to generate precalculated data for move generation functions
"""


MIN_BOARD = 0;
MAX_BOARD = 63;

KNIGHT_OFFSETS = [17, 10, -15, -6, 15, 6, -17, -10]
KING_OFFSETS = [-9, -8, -7, -1, 1, 7, 8, 9]

def FILE(x):
    return x & 7
def RANK(x):
    return x >> 3

def getBishopMovesForPos(pos):
    out = []
    sectionIndex = 0;

    # up left
    for next in range(pos-9, MIN_BOARD-1, -9):
        if FILE(next) == 7:
            break
        out.append(next)
    out.insert(sectionIndex, (len(out) + 1))
    sectionIndex = len(out)
    # up right
    for next in range(pos-7, MIN_BOARD-1, -7):
        if FILE(next) == 0:
            break
        out.append(next)
    out.insert(sectionIndex, (len(out) + 1))
    sectionIndex = len(out)
    # down left
    for next in range(pos+7, MAX_BOARD+1, 7):
        if FILE(next) == 7:
            break
        out.append(next)
    out.insert(sectionIndex, (len(out) + 1))
    sectionIndex = len(out)
    # down right
    for next in range(pos+9, MAX_BOARD+1, 9):
        if FILE(next) == 0:
            break
        out.append(next)
    out.insert(sectionIndex, (len(out) + 1))
    sectionIndex = len(out)

    return out


def getRookMovesForPos(pos):
    out = []
    sectionIndex = 0;

    # up
    for next in range(pos-8, MIN_BOARD-1, -8):
        out.append(next)
    out.insert(sectionIndex, (len(out) + 1))
    sectionIndex = len(out)
    # left
    for next in range(pos+1, MAX_BOARD+1, 1):
        if FILE(next) == 0:
            break
        out.append(next)
    out.insert(sectionIndex, (len(out) + 1))
    sectionIndex = len(out)
    # right
    for next in range(pos-1, MIN_BOARD-1, -1):
        if FILE(next) == 7:
            break
        out.append(next)
    out.insert(sectionIndex, (len(out) + 1))
    sectionIndex = len(out)
    # down
    for next in range(pos+8, MAX_BOARD+1, 8):
        out.append(next)
    out.insert(sectionIndex, (len(out) + 1))
    sectionIndex = len(out)

    return out

def getKnightMovesForPos(pos):
    out = []
    # --- knight moves ---
    for offset in KNIGHT_OFFSETS:
        next = pos + offset
        if next in range(0, 64) and abs(FILE(next) - FILE(pos)) <= 2:
            out.append(next)
    out.insert(0, (len(out) + 1))

    return out

def getKingMovesForPos(pos):
    out = []
    # --- king moves ---
    for offset in KING_OFFSETS:
        next = pos + offset
        if next in range(0, 64) and abs(FILE(next) - FILE(pos)) <= 1:
            out.append(next)
    out.insert(0, (len(out) + 1))

    return out

def getMovesForPos2(pos):
    array = []
    out = 0

    # --- rook moves ---
    # up
    for next in range(pos-8, MIN_BOARD-1, -8):
        out |= (1 << (next))
    # left
    for next in range(pos+1, MAX_BOARD+1, 1):
        if FILE(next) == 0:
            break
        out |= (1 << (next))
    # right
    for next in range(pos-1, MIN_BOARD-1, -1):
        if FILE(next) == 7:
            break
        out |= (1 << (next))
    # down
    for next in range(pos+8, MAX_BOARD+1, 8):
        out |= (1 << (next))

    array.append(out)
    out = 0

    # --- bishop moves ---
    # up left
    for next in range(pos-9, MIN_BOARD-1, -9):
        if FILE(next) == 7:
            break
        out |= (1 << (next))
    # up right
    for next in range(pos-7, MIN_BOARD-1, -7):
        if FILE(next) == 0:
            break
        out |= (1 << (next))
    # down left
    for next in range(pos+7, MAX_BOARD+1, 7):
        if FILE(next) == 7:
            break
        out |= (1 << (next))
    # down right
    for next in range(pos+9, MAX_BOARD+1, 9):
        if FILE(next) == 0:
            break
        out |= (1 << (next))

    array.append(out);
    out = 0

    # --- knight moves ---
    for offset in KNIGHT_OFFSETS:
        next = pos + offset
        if next in range(0, 64) and abs(FILE(next) - FILE(pos)) <= 2:
            out |= (1 << (next))
    array.append(out)
    out = 0

    # --- king moves ---
    for offset in KING_OFFSETS:
        next = pos + offset
        if next in range(0, 64) and abs(FILE(next) - FILE(pos)) <= 1:
            out |= (1 << (next))
    array.append(out)
    out = 0

    # --- white pawn moves ---
    if pos > 16:
        if (FILE(pos) != 0):
            out |= (1 << (pos - 9))
        if (FILE(pos) != 7):
            out |= (1 << (pos - 7))
    array.append(out)
    out = 0

    # --- black pawn moves ---
    if pos < 48:
        if (FILE(pos) != 0):
            out |= (1 << (pos + 7))
        if (FILE(pos) != 7):
            out |= (1 << (pos + 9))
    array.append(out)

    return array

def printBitBoard(board):
    for i in range(8):
        tmp = bin(board & 0xFF)[2:]
        tmp = "0" * (8 - len(tmp)) + tmp
        for char in tmp:
            print(char, end=" ")
        print()
        board >>= 8

# checks
for i in range(64):
    moves = getMovesForPos2(i);

    print(f"const uint64_t checksPos{i}[{len(moves)}] = {{{ ', '.join(map(hex, moves)) }}};")

print("const uint64_t *checkBitBoards[64] = {", end="")
for i in range(63):
    print(f"checksPos{i}, ", end="")
print("checksPos63};")

print()

# bishops
for i in range(64):
    moves = getBishopMovesForPos(i);

    print(f"const int8_t bishopsPos{i}[{len(moves)}] = {{{ ', '.join(map(str, moves)) }}};")

print("const int8_t *bishopMoves[64] = {", end="")
for i in range(63):
    print(f"bishopsPos{i}, ", end="")
print("bishopsPos63};")

print()

# rooks
for i in range(64):
    moves = getRookMovesForPos(i);

    print(f"const int8_t rooksPos{i}[{len(moves)}] = {{{ ', '.join(map(str, moves)) }}};")

print("const int8_t *rookMoves[64] = {", end="")
for i in range(63):
    print(f"rooksPos{i}, ", end="")
print("rooksPos63};")

print()

# knights
for i in range(64):
    moves = getKnightMovesForPos(i);

    print(f"const int8_t knightsPos{i}[{len(moves)}] = {{{ ', '.join(map(str, moves)) }}};")

print("const int8_t *knightMoves[64] = {", end="")
for i in range(63):
    print(f"knightsPos{i}, ", end="")
print("knightsPos63};")

print()

# kings
for i in range(64):
    moves = getKingMovesForPos(i);

    print(f"const int8_t kingsPos{i}[{len(moves)}] = {{{ ', '.join(map(str, moves)) }}};")

print("const int8_t *kingMoves[64] = {", end="")
for i in range(63):
    print(f"kingsPos{i}, ", end="")
print("kingsPos63};")




