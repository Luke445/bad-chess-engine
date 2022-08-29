"""
this file is used to generate precalculated move positions
"""


MIN_BOARD = 0;
MAX_BOARD = 63;

KNIGHT_OFFSETS = [17, 10, -15, -6, 15, 6, -17, -10]

def getMovesForPos(pos):
    out = []
    sectionIndex = 0;

    # --- bishop moves ---
    # up left
    for next in range(pos-9, MIN_BOARD-1, -9):
        if next & 0b111 == 7:
            break
        out.append(next)
    out.insert(sectionIndex, (len(out) + 1))
    sectionIndex = len(out)
    # up right
    for next in range(pos-7, MIN_BOARD-1, -7):
        if next & 0b111 == 0:
            break
        out.append(next)
    out.insert(sectionIndex, (len(out) + 1))
    sectionIndex = len(out)
    # down left
    for next in range(pos+7, MAX_BOARD+1, 7):
        if next & 0b111 == 7:
            break
        out.append(next)
    out.insert(sectionIndex, (len(out) + 1))
    sectionIndex = len(out)
    # down right
    for next in range(pos+9, MAX_BOARD+1, 9):
        if next & 0b111 == 0:
            break
        out.append(next)
    out.insert(sectionIndex, (len(out) + 1))
    sectionIndex = len(out)


    # --- rook moves ---
    # up
    for next in range(pos-8, MIN_BOARD-1, -8):
        out.append(next)
    out.insert(sectionIndex, (len(out) + 1))
    sectionIndex = len(out)
    # left
    for next in range(pos+1, MAX_BOARD+1, 1):
        if next & 0b111 == 0:
            break
        out.append(next)
    out.insert(sectionIndex, (len(out) + 1))
    sectionIndex = len(out)
    # right
    for next in range(pos-1, MIN_BOARD-1, -1):
        if next & 0b111 == 7:
            break
        out.append(next)
    out.insert(sectionIndex, (len(out) + 1))
    sectionIndex = len(out)
    # down
    for next in range(pos+8, MAX_BOARD+1, 8):
        out.append(next)
    out.insert(sectionIndex, (len(out) + 1))
    sectionIndex = len(out)


    # --- knight moves ---
    for offset in KNIGHT_OFFSETS:
        next = pos + offset
        if next in range(0, 64) and abs((next & 0b111) - (pos & 0b111)) <= 2:
            out.append(next)
    out.insert(sectionIndex, (len(out) + 1))
    sectionIndex = len(out)

    return out






for i in range(64):
    moves = getMovesForPos(i);
    print(f"const int pos{i}[{len(moves)}] = {{{ ', '.join(map(str, moves)) }}};")

print("const int *moveLookup[64] = {", end="")
for i in range(63):
    print(f"pos{i}, ", end="")
print("pos63};")





