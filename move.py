class Move:
    """
    Represents a move in Corintho
    """

    def __init__(self, mtype, row1, col1, ptype=0, row2=0, col2=0):
        """
        bool,int,int(,int,int,int)
        """
        self.mtype = mtype
        # Place
        if mtype:
            self.ptype = ptype
            self.row1 = row1
            self.col1 = col1
        # Move
        else:
            self.row1 = row1
            self.col1 = col1
            self.row2 = row2
            self.col2 = col2

    def __eq__(self, obj):
        if self.mtype != obj.mtype:
            return False
        # Place
        if self.mtype:
            return (
                self.ptype == obj.ptype
                and self.row1 == obj.row1
                and self.col1 == obj.col1
            )
        return (
            self.row1 == obj.row1
            and self.col1 == obj.col1
            and self.row2 == obj.row2
            and self.col2 == obj.col2
        )

    def __str__(self):
        """
        -> str
        String representation of move
        """
        output = ""
        # Place
        if self.mtype:
            output += "P"
            if self.ptype == 0:
                output += "B"
            elif self.ptype == 1:
                output += "C"
            elif self.ptype == 2:
                output += "A"
            output += str(self.row1)
            output += str(self.col1)
        # Move
        else:
            output += "M"
            output += str(self.row1)
            output += str(self.col1)
            if self.row2 - self.row1 == 1:
                output += "D"
            elif self.row2 - self.row1 == -1:
                output += "U"
            elif self.col2 - self.col1 == 1:
                output += "R"
            elif self.col2 - self.col1 == -1:
                output += "L"
        return output
