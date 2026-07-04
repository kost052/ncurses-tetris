#include <vector>
#include <algorithm>
#include <random>
#include <ncurses.h>
#include "tetrominos.h"

using namespace std;
using tetromino = vector<vector<bool>>;
using pos = unsigned int;

constexpr int enter = 10;

constexpr pos rows = 20, columns = 10;

vector<vector<bool>> matrix(rows, vector<bool>(columns, 0));

void draw(pos x = 0, pos y = 0) {
    clear();
    for(const vector<bool> &row : matrix) {
        move(y, x);
        for(const bool &col : row) {
            if(col) {
                addstr("[]");
            } else {
                addstr(" .");
            }
        }
        ++y;
    }
    move(y, x);
    refresh();
}

struct Tetro {
    tetromino shape;
    pos size;
    bool falling = false;

    pos pos_x = 0, pos_y = 0;

    template <typename function>
    void loop(pos startx, pos starty, function func) {
        for(pos row = starty; row < size; ++row) {
            for(pos col = startx; col < size; ++col) {
                func(row, col);
            }
        }
    }

    bool collides(pos x, pos y) {
        for(pos row = y; row < size; ++row) {
            for(pos col = x; col < size; ++col) {
                if(shape[row][col]) {
                    if(matrix[y+row][x+col] == 1 || row < 0 || row >= rows || col < 0 || col >= columns) {
                        return true;
                    }
            }   }
        }
        return false;
    }

    void clear(pos x, pos y) {
        loop(x, y, [](pos col, pos row) {
            matrix[row][col] = 0;
        });
    }

    void insert(pos x, pos y) {
        if(collides(x, y)) return;

        clear(pos_x, pos_y);

        loop(0, 0, [&](pos row, pos col) {
            if(shape[row][col]) {
                matrix[y+row][x+col] = 1;
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
};

void shuffle_tetros() {
    random_device rng;
    shuffle(tetros.begin(), tetros.end(), rng);
}

int main() {
    initscr();
    cbreak();
    noecho();

    bool running = true, started = false;

    pos i = 0;
    shuffle_tetros();

    while(running) {
        draw();
        
        switch(getch()) {
            case enter:
                started = true;
                break;
            case 'q':
                running = false;
                break;
        }

        if(started) {
            if(i == tetros.size()) {
                shuffle_tetros();
                i = 0;                
            }
            
            Tetro tetro;
            tetro.shape = tetros[i];
            tetro.size = tetro.shape.size();
            tetro.falling = true;
            //tetro.insert((columns-tetro.size)/2+1, 0);
            tetro.insert(1, 1);
            while(tetro.falling) {
                draw();
                getch();
            }
        }
    }
}