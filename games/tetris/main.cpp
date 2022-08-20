#include <iostream>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>

using namespace std;

void swap(int &a, int &b)
{
    int t = a;
    a = b;
    b = t;
}
// 获取[min, max]之间的随机数
int getrand(int min, int max)
{
    return (min + rand() % (max - min + 1));
}

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destory_win(WINDOW *local_win);

int game_win_height = 30;
int game_win_width = 45;

int hint_win_height = 10;
int hint_win_width = 20;

// 定义game主窗口，下一个窗口，得分窗口
WINDOW *game_win, *hint_win, *score_win;
// 获取键盘输入的键值
int key;

class Piece
{
public:
    int score;
    // 方块的形状
    int shape;
    int next_shape;
    // 方块出现的位置
    int head_x;
    int head_y;
    // 方块的高和宽
    int size_h;
    int size_w;

    int next_size_h;
    int next_size_w;
    // 每个元素代表一个最小的正方块
    int box_shape[4][4];
    int next_box_shape[4][4];
    // 表示地图
    int box_map[30][45];

    bool game_over;

public:
    // 初始化地图
    void initial();
    // 设置方块的形状
    void set_shape(int &cshape, int box_shape[][4], int &size_w, int &size_h);

    void score_next();
    void judge();
    // 移动方块 上键表示旋转
    void move();
    // 旋转方块
    void rotate();
    // 判断是否结束
    bool isaggin();
    // 判断第几行是否已经满了
    bool exsqr(int row);
};

int main()
{

    initscr();
    // raw();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    refresh();

    game_win = create_newwin(game_win_height, game_win_width, 0, 0);
    wborder(game_win, '*', '*', '*', '*', '*', '*', '*', '*');
    wrefresh(game_win);

    hint_win = create_newwin(hint_win_height, hint_win_width, 0, game_win_width + 10);
    mvprintw(0, game_win_width + 10 + 2, "%s", "Next");
    refresh();

    score_win = create_newwin(hint_win_height, hint_win_width, 20, game_win_width + 10);
    mvprintw(20, game_win_width + 10 + 2, "%s", "Score");
    refresh();

    Piece *pp = new Piece;
    pp->initial();

    while (1)
    {
        pp->move();
        if (pp->game_over)
            break;
    }

    destory_win(game_win);
    destory_win(hint_win);
    destory_win(score_win);
    delete pp;
    system("clear");

    int row, col;
    getmaxyx(stdscr, row, col);
    mvprintw(row / 2, col / 2, "%s", "GAMER OVER ! \n ");
    mvprintw(row / 2 + 2, col / 2 - 2, "%s", "Wait 5s to return tthe erminal ! \n ");
    refresh();

    sleep(5);
    endwin();
    return 0;
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);
    wrefresh(local_win);
    return local_win;
}

void destory_win(WINDOW *local_win)
{
    wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(local_win);
    delwin(local_win);
}

void Piece::initial()
{
    score = 0;
    game_over = false;
    for (int i = 0; i < game_win_height; i++)
        for (int j = 0; j < game_win_width; j++)
        {
            if (i == 0 || i == game_win_height - 1 || j == 0 || j == game_win_width - 1)
            {
                box_map[i][j] = 1;
            }
            else
                box_map[i][j] = 0;
        }

    srand((unsigned)time(0));
    shape = getrand(0, 6);
    set_shape(shape, box_shape, size_w, size_h);

    next_shape = getrand(0, 6);
    set_shape(next_shape, next_box_shape, next_size_w, next_size_h);

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (next_box_shape[i][j] == 1)
            {
                mvwaddch(hint_win, (hint_win_height - size_h) / 2 + i, (hint_win_width - size_w) / 2 + j, '#');
                wrefresh(hint_win);
            }

    mvwprintw(score_win, hint_win_height / 2, hint_win_width / 2 - 2, "%d", score);
    wrefresh(score_win);
}

void Piece::set_shape(int &cshape, int shape[][4], int &size_w, int &size_h)
{
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            shape[i][j] = 0;
    switch (cshape)
    {
    case 0:
        size_h = 1;
        size_w = 4;
        shape[0][0] = 1;
        shape[0][1] = 1;
        shape[0][2] = 1;
        shape[0][3] = 1;
        break;
    case 1:
        size_h = 2;
        size_w = 3;
        shape[0][0] = 1;
        shape[1][0] = 1;
        shape[1][1] = 1;
        shape[1][2] = 1;
        break;
    case 2:
        size_h = 2;
        size_w = 3;
        shape[0][2] = 1;
        shape[1][0] = 1;
        shape[1][1] = 1;
        shape[1][2] = 1;
        break;
    case 3:
        size_h = 2;
        size_w = 3;
        shape[0][1] = 1;
        shape[0][2] = 1;
        shape[1][0] = 1;
        shape[1][1] = 1;
        break;

    case 4:
        size_h = 2;
        size_w = 3;
        shape[0][0] = 1;
        shape[0][1] = 1;
        shape[1][1] = 1;
        shape[1][2] = 1;
        break;

    case 5:
        size_h = 2;
        size_w = 2;
        shape[0][0] = 1;
        shape[0][1] = 1;
        shape[1][0] = 1;
        shape[1][1] = 1;
        break;

    case 6:
        size_h = 2;
        size_w = 3;
        shape[0][1] = 1;
        shape[1][0] = 1;
        shape[1][1] = 1;
        shape[1][2] = 1;
        break;
    }

    head_x = game_win_width / 2;
    head_y = 1;

    if (isaggin()) /* GAME OVER ! */
        game_over = true;
}

void Piece::rotate()
{
    int temp[4][4] = {0};
    int temp_piece[4][4] = {0};
    int i, j, tmp_size_h, tmp_size_w;

    tmp_size_w = size_w;
    tmp_size_h = size_h;

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            temp_piece[i][j] = box_shape[i][j];

    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            temp[j][i] = box_shape[i][j];
    i = size_h;
    size_h = size_w;
    size_w = i;
    for (i = 0; i < size_h; i++)
        for (j = 0; j < size_w; j++)
            box_shape[i][size_w - 1 - j] = temp[i][j];

    if (isaggin())
    {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                box_shape[i][j] = temp_piece[i][j];
        size_w = tmp_size_w;
        size_h = tmp_size_h;
    }
    else
    {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
            {
                if (temp_piece[i][j] == 1)
                {
                    mvwaddch(game_win, head_y + i, head_x + j, ' ');
                    wrefresh(game_win);
                }
            }
        for (int i = 0; i < size_h; i++)
            for (int j = 0; j < size_w; j++)
            {
                if (this->box_shape[i][j] == 1)
                {
                    mvwaddch(game_win, head_y + i, head_x + j, '#');
                    wrefresh(game_win);
                }
            }
    }
}

void Piece::move()
{
    fd_set set;
    FD_ZERO(&set);
    FD_SET(0, &set);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 500000;

    if (select(1, &set, NULL, NULL, &timeout) == 0)
    {
        head_y++;
        if (isaggin())
        {
            head_y--;
            for (int i = 0; i < size_h; i++)
                for (int j = 0; j < size_w; j++)
                    if (box_shape[i][j] == 1)
                        box_map[head_y + i][head_x + j] = 1;
            score_next();
        }
        else
        {
            for (int i = size_h - 1; i >= 0; i--)
                for (int j = 0; j < size_w; j++)
                {
                    if (this->box_shape[i][j] == 1)
                    {
                        mvwaddch(game_win, head_y - 1 + i, head_x + j, ' ');
                        mvwaddch(game_win, head_y + i, head_x + j, '#');
                    }
                }
            wrefresh(game_win);
        }
    }

    if (FD_ISSET(0, &set))
    {
        while ((key = getch()) == -1)
            ;

        if (key == KEY_LEFT)
        {
            head_x--;
            if (isaggin())
                head_x++; // undo
            else
            {
                for (int i = 0; i < size_h; i++)
                    for (int j = 0; j < size_w; j++)
                    {
                        if (this->box_shape[i][j] == 1)
                        {
                            mvwaddch(game_win, head_y + i, head_x + j + 1, ' ');
                            mvwaddch(game_win, head_y + i, head_x + j, '#');
                        }
                    }
                wrefresh(game_win);
            }
        }

        if (key == KEY_RIGHT)
        {
            head_x++;
            if (isaggin())
                head_x--;
            else
            {
                for (int i = 0; i < size_h; i++)
                    for (int j = size_w - 1; j >= 0; j--)
                    {
                        if (this->box_shape[i][j] == 1)
                        {
                            mvwaddch(game_win, head_y + i, head_x + j - 1, ' ');
                            mvwaddch(game_win, head_y + i, head_x + j, '#');
                        }
                    }
                wrefresh(game_win);
            }
        }

        if (key == KEY_DOWN)
        {
            head_y++;
            if (isaggin())
            {
                head_y--;
                for (int i = 0; i < size_h; i++)
                    for (int j = 0; j < size_w; j++)
                        if (box_shape[i][j] == 1)
                            box_map[head_y + i][head_x + j] = 1;

                score_next();
            }
            else
            {
                for (int i = size_h - 1; i >= 0; i--)
                    for (int j = 0; j < size_w; j++)
                    {
                        if (this->box_shape[i][j] == 1)
                        {
                            mvwaddch(game_win, head_y - 1 + i, head_x + j, ' ');
                            mvwaddch(game_win, head_y + i, head_x + j, '#');
                        }
                    }
                wrefresh(game_win);
            }
        }

        if (key == KEY_UP)
            rotate();

        if (head_x + size_w + 1 > game_win_width)
            head_x = game_win_width - size_w - 1;
        if (head_x < 1)
            head_x = 1;
    }
}

bool Piece::isaggin()
{
    for (int i = 0; i < size_h; i++)
        for (int j = 0; j < size_w; j++)
        {
            if (box_shape[i][j] == 1)
            {
                if (head_y + i > game_win_height - 2)
                    return true;
                if (head_x + j > game_win_width - 2 || head_x + i - 1 < 0)
                    return true;
                if (box_map[head_y + i][head_x + j] == 1)
                    return true;
            }
        }
    return false;
}

bool Piece::exsqr(int row)
{
    for (int j = 1; j < game_win_width - 1; j++)
        if (box_map[row][j] == 1)
            return true;
    return false;
}

void Piece::judge()
{
    int i, j;
    int line = 0;
    bool full;
    for (i = 1; i < game_win_height - 1; i++)
    {
        full = true;
        for (j = 1; j < game_win_width - 1; j++)
        {
            if (box_map[i][j] == 0)
                full = false;
        }
        if (full)
        {
            line++;
            score += 50;
            for (j = 1; j < game_win_width - 1; j++)
                box_map[i][j] = 0;
        }
    }
    if (line != 0)
    {
        for (i = game_win_height - 2; i >= 2; i--)
        {
            int s = i;
            if (exsqr(i) == 0)
            {
                while (s > 1 && exsqr(--s) == 0)
                    ;
                for (j = 1; j < game_win_width - 1; j++)
                {
                    box_map[i][j] = box_map[s][j];
                    box_map[s][j] = 0;
                }
            }
        }

        for (int i = 1; i < game_win_height - 1; i++)
            for (int j = 1; j < game_win_width - 1; j++)
            {
                if (box_map[i][j] == 1)
                {
                    mvwaddch(game_win, i, j, '#');
                    wrefresh(game_win);
                }
                else
                {
                    mvwaddch(game_win, i, j, ' ');
                    wrefresh(game_win);
                }
            }
    }
}

void Piece::score_next()
{
    score += 10;
    judge();

    mvwprintw(score_win, hint_win_height / 2, hint_win_width / 2 - 2, "%d", score);
    wrefresh(score_win);

    set_shape(next_shape, box_shape, size_w, size_h);

    this->next_shape = getrand(0, 6);
    set_shape(next_shape, next_box_shape, next_size_w, next_size_h);

    for (int i = 1; i < hint_win_height - 1; i++)
        for (int j = 1; j < hint_win_width - 1; j++)
        {
            mvwaddch(hint_win, i, j, ' ');
            wrefresh(hint_win);
        }
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (next_box_shape[i][j] == 1)
            {
                mvwaddch(hint_win, (hint_win_height - size_h) / 2 + i, (hint_win_width - size_w) / 2 + j, '#');
                wrefresh(hint_win);
            }
}
