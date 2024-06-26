import copy

def liveNeighbors(pole, rows, colls):
    count = 0
    for i in range(rows - 1, rows + 2):  # okolí --> 3x3
        for j in range(colls - 1, colls + 2):
            if 0 <= i < len(pole) and 0 <= j < len(pole):
                if pole[i][j] == 1:
                    count += 1
    return count

def update(alive: set, size: (int, int), iter_n: int) -> set:
    a, b = size
    pole = [[0 for i in range(a)] for j in range(b)]

    for x, y in alive:
        pole[x][y] = 1

    for i in range(iter_n):
        kpole = copy.deepcopy(pole)
        for r in range(len(pole)):
            for c in range(len(pole)):
                live = liveNeighbors(pole, r, c) - pole[r][c]
                kpole[r][c] = 0
                if (live == 2 or live==3) and pole[r][c] == 1:
                    kpole[r][c] = 1
                if live == 3 and pole[r][c]==0:
                    kpole[r][c] = 1
        pole = kpole

    newAlive=list()

    for i in range(a):
        for j in range(b):
            if pole[i][j]==1:
                newAlive.append((i, j))

    newAlive=set(newAlive)
    print(newAlive)
    return newAlive

def draw(alive: set, size: (int, int)) -> str:
    a, b = size
    board = """"""
    for x in range(a + 2):
        for y in range(b + 2):
            if x == 0:
                if y == 0:
                    board += '+'
                elif y == b+1:
                    board += '+\n'
                else:
                    board += '-'
            elif x == a + 1:
                if y == 0 or y == b+1:
                    board += '+'
                else:
                    board += '-'
            else:
                if y == 0:
                    board += '|'
                elif y == b+1:
                    board += '|\n'
                else:
                    found = False
                    for r, c in alive:
                        if r == x - 1 and c == y-1:
                            board += 'X'
                            found = True
                            break

                    if not found:
                        board += ' '

    return board

#draw({(1,1),(0,1),(0,0),(2,0),(0,3)}, (4,4))
