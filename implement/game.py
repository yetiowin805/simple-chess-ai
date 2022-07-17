import numpy as np
from copy import deepcopy
from implement.board import Board
from implement.move import Move


class Game:
    """
    Represents Coritho game
    A board with player pieces, current player, and outcome
    """

    def __init__(self):
        self.board = Board()
        # Which player is playing
        self.to_play = 0
        # Number of pieces for each player
        self.pieces = [[4, 4, 4], [4, 4, 4]]
        # Outcome when game is done. 1 is first player win. None means the game is ongoing
        self.outcome = None

    def __str__(self):
        return (
            str(self.board) + "\n" + str(self.pieces) + "\n" + str(self.to_play) + "\n"
        )

    def is_legal(self, move):
        """
        Move -> bool
        Checks if move is legal
        """
        # Place
        if move.mtype:
            if self.pieces[self.to_play][move.ptype] == 0 or not self.board.can_place(
                move.ptype, move.row1, move.col1
            ):
                return False
            return True
        # Move
        return self.board.can_move(move.row1, move.col1, move.row2, move.col2)

    def get_legal_moves(self):
        """
        -> array
        Returns a list of all legal moves
        """

        lines = self.board.lines
        # More than 2 lines, no legal moves
        if len(lines) > 2:
            return np.zeros(96)
        moves = np.ones(96)
        # Need to break / extend lines
        if len(lines) > 0:
            for line in lines:
                cur = np.zeros(96)
                if line[0][0] == "r":
                    target = int(line[0][1])
                    sources = []
                    if target == 0:
                        sources = [1]
                    elif target == 1:
                        sources = [0, 2]
                    elif target == 2:
                        sources = [1, 3]
                    # 3
                    else:
                        sources = [2]
                    # Move moves
                    for source in sources:
                        cur[Move.encode_move(target, 1, source, 1)] = 1
                        cur[Move.encode_move(target, 2, source, 2)] = 1
                        cur[Move.encode_move(source, 1, target, 1)] = 1
                        cur[Move.encode_move(source, 2, target, 2)] = 1
                        if line[0][2] == "l":
                            cur[Move.encode_move(target, 0, source, 0)] = 1
                            cur[Move.encode_move(source, 0, target, 0)] = 1
                            # Extend
                            if self.board.top(source, 3) == line[1]:
                                cur[Move.encode_move(source, 3, target, 3)] = 1
                        elif line[0][2] == "r":
                            cur[Move.encode_move(target, 3, source, 3)] = 1
                            cur[Move.encode_move(source, 3, target, 3)] = 1
                            # Extend
                            if self.board.top(source, 0) == line[1]:
                                cur[Move.encode_move(source, 0, target, 0)] = 1
                    if line[0][2] == "l":
                        cur[Move.encode_move(target, 2, target, 3)] = 1
                    elif line[0][2] == "r":
                        cur[Move.encode_move(target, 1, target, 0)] = 1
                    # Place moves, not on capitals
                    if line[1] < 2:
                        cur[Move.encode_place(line[1] + 1, target, 1)] = 1
                        cur[Move.encode_place(line[1] + 1, target, 2)] = 1
                        if line[0][2] == "l":
                            cur[Move.encode_place(line[1] + 1, target, 0)] = 1
                        elif line[0][2] == "r":
                            cur[Move.encode_place(line[1] + 1, target, 3)] = 1
                    # Extend, can place capital
                    if line[0][2] == "l":
                        cur[Move.encode_place(line[1], target, 3)] = 1
                    elif line[0][2] == "r":
                        cur[Move.encode_place(line[1], target, 0)] = 1
                elif line[0][0] == "c":
                    target = int(line[0][1])
                    sources = []
                    if target == 0:
                        sources = [1]
                    elif target == 1:
                        sources = [0, 2]
                    elif target == 2:
                        sources = [1, 3]
                    # 3
                    else:
                        sources = [2]
                    # Move moves
                    for source in sources:
                        cur[Move.encode_move(1, target, 1, source)] = 1
                        cur[Move.encode_move(2, target, 2, source)] = 1
                        cur[Move.encode_move(1, source, 1, target)] = 1
                        cur[Move.encode_move(2, source, 2, target)] = 1
                        if line[0][2] == "u":
                            cur[Move.encode_move(0, target, 0, source)] = 1
                            cur[Move.encode_move(0, source, 0, target)] = 1
                            # Extend
                            if self.board.top(3, source) == line[1]:
                                cur[Move.encode_move(3, source, 3, target)] = 1
                        elif line[0][2] == "d":
                            cur[Move.encode_move(3, target, 3, source)] = 1
                            cur[Move.encode_move(3, source, 3, target)] = 1
                            # Extend
                            if self.board.top(0, source) == line[1]:
                                cur[Move.encode_move(0, source, 0, target)] = 1
                    if line[0][2] == "u":
                        cur[Move.encode_move(2, target, 3, target)] = 1
                    elif line[0][2] == "d":
                        cur[Move.encode_move(1, target, 0, target)] = 1
                    # Place moves, not on capitals
                    if line[1] < 2:
                        cur[Move.encode_place(line[1] + 1, 1, target)] = 1
                        cur[Move.encode_place(line[1] + 1, 2, target)] = 1
                        if line[0][2] == "u":
                            cur[Move.encode_place(line[1] + 1, 0, target)] = 1
                        elif line[0][2] == "d":
                            cur[Move.encode_place(line[1] + 1, 3, target)] = 1
                    # Extend, can place capital
                    if line[0][2] == "u":
                        cur[Move.encode_place(line[1], 3, target)] = 1
                    elif line[0][2] == "d":
                        cur[Move.encode_place(line[1], 0, target)] = 1
                # Long diagonals
                elif line[0][0] == "d":
                    # Flip column if necessary
                    f = lambda x: x
                    if line[0][1] == "1":
                        f = lambda x: 3 - x
                    # Move moves
                    cur[Move.encode_move(1, f(1), 0, f(1))] = 1
                    cur[Move.encode_move(1, f(1), 0, f(2))] = 1
                    cur[Move.encode_move(1, f(1), 0, f(1))] = 1
                    cur[Move.encode_move(1, f(1), 0, f(0))] = 1
                    cur[Move.encode_move(0, f(1), 0, f(1))] = 1
                    cur[Move.encode_move(1, f(2), 0, f(1))] = 1
                    cur[Move.encode_move(2, f(1), 0, f(1))] = 1
                    cur[Move.encode_move(1, f(0), 0, f(1))] = 1
                    if line[0][2] == "u":
                        cur[Move.encode_move(0, f(0), 0, f(1))] = 1
                        cur[Move.encode_move(0, f(0), 1, f(0))] = 1
                        cur[Move.encode_move(0, f(1), 0, f(0))] = 1
                        cur[Move.encode_move(1, f(0), 0, f(0))] = 1
                        # Extend
                        if self.board.top(2, f(3)) == line[1]:
                            cur[Move.encode_move(2, f(3), 3, f(3))] = 1
                        if self.board.top(3, f(2)) == line[1]:
                            cur[Move.encode_move(3, f(2), 3, f(3))] = 1
                    elif line[0][2] == "d":
                        cur[Move.encode_move(3, f(3), 2, f(3))] = 1
                        cur[Move.encode_move(3, f(3), 3, f(2))] = 1
                        cur[Move.encode_move(2, f(3), 3, f(3))] = 1
                        cur[Move.encode_move(3, f(2), 3, f(3))] = 1
                        # Extend
                        if self.board.top(0, f(1)) == line[1]:
                            cur[Move.encode_move(0, f(1), 0, f(0))] = 1
                        if self.board.top(1, f(0)) == line[1]:
                            cur[Move.encode_move(1, f(0), 0, f(0))] = 1
                    # Place moves, not capitals
                    if line[1] < 2:
                        cur[Move.encode_place(line[1] + 1, 1, f(1))] = 1
                        cur[Move.encode_place(line[1] + 1, 2, f(2))] = 1
                        if line[0][2] == "u":
                            cur[Move.encode_place(line[1] + 1, 0, f(0))] = 1
                        elif line[0][2] == "d":
                            cur[Move.encode_place(line[1] + 1, 3, f(3))] = 1
                    # Place capital, extend
                    if line[0][2] == "u":
                        cur[Move.encode_place(line[1], 3, f(3))] = 1
                    elif line[0][2] == "d":
                        cur[Move.encode_place(line[1], 0, f(0))] = 1
                # Short diagonals
                else:
                    # Flip column if necessary
                    f = lambda x: x
                    if line[0][1] in ["1", "2"]:
                        f = lambda x: 3 - x
                    # Shift down if necessary
                    s = lambda x: x
                    if line[0][1] in ["2", "3"]:
                        s = lambda x: 3 - x
                    # Move moves
                    cur[Move.encode_move(s(0), f(3), s(0), f(2))] = 1
                    cur[Move.encode_move(s(1), f(2), s(0), f(2))] = 1
                    cur[Move.encode_move(s(0), f(1), s(0), f(2))] = 1
                    cur[Move.encode_move(s(0), f(1), s(1), f(1))] = 1
                    cur[Move.encode_move(s(1), f(2), s(1), f(1))] = 1
                    cur[Move.encode_move(s(2), f(1), s(1), f(1))] = 1
                    cur[Move.encode_move(s(1), f(0), s(1), f(1))] = 1
                    cur[Move.encode_move(s(1), f(0), s(2), f(0))] = 1
                    cur[Move.encode_move(s(2), f(1), s(2), f(0))] = 1
                    cur[Move.encode_move(s(3), f(0), s(2), f(0))] = 1
                    cur[Move.encode_move(s(0), f(2), s(0), f(3))] = 1
                    cur[Move.encode_move(s(0), f(2), s(1), f(2))] = 1
                    cur[Move.encode_move(s(0), f(2), s(0), f(1))] = 1
                    cur[Move.encode_move(s(1), f(1), s(0), f(1))] = 1
                    cur[Move.encode_move(s(1), f(1), s(1), f(2))] = 1
                    cur[Move.encode_move(s(1), f(1), s(2), f(1))] = 1
                    cur[Move.encode_move(s(1), f(1), s(1), f(0))] = 1
                    cur[Move.encode_move(s(2), f(0), s(1), f(0))] = 1
                    cur[Move.encode_move(s(2), f(0), s(2), f(1))] = 1
                    cur[Move.encode_move(s(2), f(0), s(3), f(0))] = 1
                    # Place moves, not capitals
                    if line[1] < 2:
                        cur[Move.encode_place(line[1] + 1, s(0), f(2))] = 1
                        cur[Move.encode_place(line[1] + 1, s(1), f(1))] = 1
                        cur[Move.encode_place(line[1] + 1, s(2), f(0))] = 1
                # First line
                if len(moves) == 0:
                    moves = cur
                else:
                    for i, move in enumerate(cur):
                        if not move:
                            moves[i] = 0
        legal_moves = np.zeros(96)
        for i, move in enumerate(moves):
            if move and self.is_legal(Move(i)):
                legal_moves[i] = 1
        return legal_moves

    def do_move(self, move_id):
        """
        Move -> bool,float
        Does move
        Current player loses if move is illegal
        Returns if game is done and the outcome
        """
        # Illegal move
        # if not self.is_legal(move):
        #     self.outcome = 1 - self.to_play
        #     return self.outcome
        # Place, remove piece from arsenal
        move = Move(move_id)
        if move.mtype:
            self.pieces[self.to_play][move.ptype] -= 1
        self.board.do_move(move)
        self.to_play = 1 - self.to_play
        # Previous player win
        if max(self.get_legal_moves()) == 0:
            if len(self.board.lines) > 0:
                self.outcome = 2 * self.to_play - 1
            else:
                self.outcome = 0
            return self.outcome
        return None

    def get_vector(self):
        """
        -> array
        Returns vector representation of game in canonical form
        """

        canonical_pieces = [
            self.pieces[self.to_play],
            self.pieces[1 - self.to_play],
        ]
        return np.concatenate(
            [
                np.array(self.board.spaces).flatten(),
                np.array(self.board.frozen).flatten(),
                np.array(canonical_pieces).flatten() / 4,
            ]
        )
