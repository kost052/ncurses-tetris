#include <ncurses.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <random>

using namespace std;
using pos = unsigned int;
using tetromino = vector<vector<bool>>;

vector<tetromino> tetros{
    {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    
    {
        {1, 1},
        {1, 1}
    },

    {
        {0, 1, 0},
        {1, 1, 1},
        {0, 0, 0}
    },

    {
        {0, 1, 1},
        {1, 1, 0},
        {0, 0, 0}
    },

    {
        {1, 1, 0},
        {0, 1, 1},
        {0, 0, 0}
    },

    {
        {0, 0, 1},
        {1, 1, 1},
        {0, 0, 0}
    },
    
    {
        {1, 0, 0},
        {1, 1, 1},
        {0, 0, 0}
    }
};

constexpr pos cols = 10, rows = 20;
constexpr pos fps = 60; 

pos pull_counter = 0;
pos level, score, lines;

vector<vector<bool>> matrix(rows, vector<bool>(cols, 0));

void print_stats(pos x = cols*2+7, pos y = 0) {
    move(y, x);

    printw("Level: %d", level);

    move(y+2, x);

    printw("Score: %d", score);

    move(y+4, x);

    printw("Lines: %d", lines);
}

void print_matrix() {
    clear();

    pos line = 0;

    for(const vector<bool> &row : matrix) {
        addstr("<!");
        for(const bool &state : row) {
            if(state) {
                addstr("[]");
            } else {
                addstr(" .");
            }
        }
        addstr("!>");

        line++;
        move(line, 0);
    }

    addstr("<!");
    for(int i = 0; i < cols; i++) {
        addstr("==");
    }
    addstr("!>");
    
    print_stats();

    refresh();
}

void update_matrix() {
    pos i = 0;

    for(vector<bool> &row : matrix) {
        bool filled = true;

        for(bool state : row) {
            if(!state) {
                filled = false;
                break;
            }    
        }

        if(filled) {
            fill(row.begin(), row.end(), 0);

            for(int j = i; j > 0; j--) {
                matrix[j] = matrix[j-1];
            }
            lines++;
            score+= 100;
        }
        i++;
    }
    print_matrix(); 
}

void clear_matrix() {
    fill(matrix.begin(), matrix.end(), vector<bool>(cols, 0));
    print_matrix();
}

class tetro {
    tetromino shape;

    public:
        int X = 0, Y = 0;
        pos size;

    bool in_bounds(int x, int y) {
        if(x >= 0 && x < cols && y >= 0 && y < rows) {
            return true; 
        } else {
            return false;
        }
    }

    void clear_tetro(vector<vector<bool>> &target = matrix) {
        int x = X, y = Y;
        for(const vector<bool> &row : shape) {
            for(const bool &col : row) {
                if(col) {
                    if(in_bounds(x, y)) {
                        target[y][x] = 0;
                    }
                }
                x++;
            }
            y++;
            x = X;
        }
    }

    public:
        bool collides(int x, int y) {
            vector<vector<bool>> matrix_cp = matrix;
            clear_tetro(matrix_cp);

            int init_x = x;

            for(const vector<bool> &row : shape) {
                for(const bool &col : row) {
                    if(col) {
                        if(!in_bounds(x, y) || matrix_cp[y][x]) {
                            return true;
                        }
                    }
                    x++;
                }
                y++;
                x = init_x;
            }
            return false;
        }

    void insert(int x, int y) {
        if(collides(x, y)) {
            return;
        }

        clear_tetro();

        X = x, Y = y;

        int init_x = x;

        for(const vector<bool> &row : shape) {
            for(const bool &col : row) {
                if(col) {
                    matrix[y][x] = 1;
                }
                x++;
            }
            y++;
            x = init_x;
        }
        print_matrix();
    }

    void rotate() {
        tetromino shape_cp = shape, rotated(size, vector<bool>(size, 0));

        for(int row = 0; row < size; row++) {
            for(int col = 0; col < size; col++) {
                if(shape[row][col]) {
                    rotated[col][size-row-1] = 1;
                }
            }
        }

        clear_tetro();

        shape = rotated;

        if(collides(X, Y)) {
            shape = shape_cp;
            insert(X, Y);
            return;
        } else {
            insert(X, Y);
        }
    }

    void shuffle_tetros() {
        random_device random;
        shuffle(tetros.begin(), tetros.end(), random);
    }

    public:
        void move(int dir) {
            switch(dir) {
                case KEY_LEFT:
                    insert(X-1, Y);
                    break;
                case KEY_RIGHT:
                    insert(X+1, Y);
                    break;
                case KEY_DOWN:
                    insert(X, Y+1);
                    score++;
                    break;
                case KEY_UP:
                    rotate();
                    break;
                case 's':
                    insert(X, Y+1);
                    break;
            }
        }

        tetro() {
            if(pull_counter == 0) {
                shuffle_tetros();
            }

            shape = tetros[pull_counter];
            size = shape.size();

            pull_counter++;
            pull_counter %= tetros.size();

            insert((cols-size+1)/2, 0);
        }
};

bool running = true, started = false;

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    timeout(1000/fps);

    while(running) {
        level = 1, score = 0, lines = 0;

        print_matrix();
        
        switch(getch()) {
            case 'e':
                started = true;
                break;
            case 'q':
                running = false;
                break;
        }

        while(started) {
            tetro part;
            
            if(part.collides((cols-part.size+1)/2, 0)) {
                started = false;
                clear_matrix();
                break;
            }

            float time_counter = 0;

            while(true) {
                part.move(getch());

                time_counter += 1000/fps;

                if(time_counter > 2000/pow(2, level)) {
                    if(part.collides(part.X, part.Y+1)) {
                        update_matrix();
                        break;
                    } else {
                        part.move('s');
                        time_counter = 0;
                    }
                }

                level = score/1000+1;
            }
        }
    }
}