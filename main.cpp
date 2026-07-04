#include <vector>
#include <algorithm>
#include <random>
#include <ncurses.h>
#include "tetrominos.h"

using namespace std;
using tetromino = vector<vector<bool>>;
using pos = unsigned int;

constexpr pos rows = 20, columns = 10;

vector<vector<bool>> matrix(rows, vector<bool>(columns, 0));

class tetro {
    public:
        tetromino shape;
        
        void insert(pos x, pos y) {
            if(collides) return;

            clear(pos_x, pos_y);

            loop(x, y, [this](pos x, pos y) {
                if(shape[y][x]) {
                    matrix[y][x] = 1;
                }
            });

            pos_x = x;
            pos_y = y;
        }
        
        void rotate() {
            tetromino origin = shape;

            loop(0, 0, [&](pos x, pos y) {
                shape[x][size-1-y] = origin[y][x];
            });

            vector<vector<bool>> matrix_origin = matrix;

            clear(pos_x, pos_y);

            if(collides(pos_x, pos_y)) {
                shape = origin;
                matrix = matrix_origin;
                return;
            } else {
                insert(pos_x, pos_y);
            }
        }

    pos pos_x, pos_y;
    pos size = shape.size();

    template <typename function>
    void loop(pos startx, pos starty, function func) {
        for(pos row = starty; row < size; ++row) {
            for(pos col = startx; col < size; ++col) {
                func(row, col);
            }
        }
    }

    bool collides(pos x, pos y) {
        loop(x, y, [this](pos x, pos y) {
            if(shape[y][x]) {
                if(matrix[y][x] == 1 || y < 0 || y >= rows || x < 0 || x >= columns) {
                    return true;
                }
            }
        });
        return false;
    }

    void clear(pos x, pos y) {
        loop(x, y, [](pos x, pos y) {
            matrix[y][x] = 0;
        });
    }
};
