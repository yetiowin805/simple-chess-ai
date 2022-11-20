#ifndef MOVE_H
#define MOVE_H

#include "util.h"

struct Move {

    bool mtype;
    uintf ptype, row1, col1, row2, col2;

    Move(uintf move_id);

};

uintf encode_place(uintf ptype, uintf row, uintf col);
uintf encode_move(uintf row1, uintf col1, uintf row2, uintf col2);

#endif
