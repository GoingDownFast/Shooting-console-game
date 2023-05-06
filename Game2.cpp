#include <iostream>
#include <windows.h>
#include <conio.h>
#include <vector>
#include <time.h>

using std::cout; using std::vector;

HANDLE Console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD Cursor;
CONSOLE_CURSOR_INFO ci = { 10, false };
//SetConsoleCursorInfo(Console, &ci); // does not recognize

struct projectile 
{
    int x, y, dir;
}p, e;

constexpr int screenw = 120;
constexpr int screenh = 30;
int shipx = screenw / 4;   // bottom left corner
int enemyx = screenw / 4;  // top left corner

vector<projectile> my_ammo;
vector<projectile> enemy_ammo;

void gotoxy(int x, int y);
void drawBorder();
void updateScore(int& score);
void drawShip();
void drawEnemy(int& dir, int& steps, int& inc);
void fire();
void enemyFire();
void enemyProjectile();
void collision(int& c_score, bool& ded);
void game();

void gotoxy(const int x,const int y)  // navigation
{
    Cursor.X = x;
    Cursor.Y = y;
    SetConsoleCursorPosition(Console, Cursor);
}

void drawBorder() 
{
    for (int i = 0;i < screenh;i++) {
        gotoxy(0, i); cout << "HHH";
        gotoxy(screenw / 2, i); cout << "HHH";
    }
    gotoxy(screenw / 2 + 10, 7); cout << "a - left";
    gotoxy(screenw / 2 + 10, 8); cout << "d - right";
    gotoxy(screenw / 2 + 10, 9); cout << "s - shoot";
    gotoxy(screenw / 2 + 29, screenh / 2); cout << "ENEMY";
    gotoxy(screenw / 2 + 17, screenh / 2 + 1); cout << "|";
    gotoxy(screenw / 2 + 43, screenh / 2 + 1); cout << "|";
    gotoxy(screenw / 2 + 18, screenh / 2 + 1); for (int i = 0; i < 25;i++) cout << "/"; // enemy life bar = 25
}

void updateScore(int& score) 
{
    score++;
    gotoxy(screenw / 2 + 43 - score, screenh / 2 + 1); cout << " ";
}

void drawShip() 
{
    gotoxy(shipx, screenh-3); cout << "    Y ";
    gotoxy(shipx, screenh-2); cout << "  MMMMM ";
    gotoxy(shipx, screenh-1); cout << " MMMMMMM ";
}

void drawEnemy(int& dir, int& steps, int& inc) 
{   
    enemyx += dir;
    if (enemyx < 5 || enemyx > (screenw / 2) - 12) dir *= -1; // wall bump
    if (steps < inc) {  // random route not finished
        gotoxy(enemyx, 0); cout << " OOOOOOOO ";
        gotoxy(enemyx, 1); cout << "  OOOOOO  ";
        gotoxy(enemyx, 2); cout << "   OOOO   ";
        steps++;
    }
    else { // generate new random route
        dir *= - 1;
        inc = rand() % 30 + 15;
        steps = 0;
    }
}

void fire() 
{
    for (int i = 0;i < my_ammo.size();i++) {
        my_ammo[i].y--;
        gotoxy(my_ammo[i].x, my_ammo[i].y); cout << "^";
        gotoxy(my_ammo[i].x, my_ammo[i].y + 1); cout << " ";
        if (my_ammo[i].y == 0) {    // reached end of screen
            gotoxy(my_ammo[i].x, my_ammo[i].y); cout << " ";
            my_ammo.erase(my_ammo.begin());
        }
    }
}

void enemyFire() 
{     
    int d = rand() % 3 - 1; // rand gen. of projectile direction
    e = { enemyx + 7, 4,  d };
    enemy_ammo.push_back(e);
}

void enemyProjectile() 
{
    for (int i = 0; i < enemy_ammo.size();i++) {
            if (enemy_ammo[i].x < 5 || enemy_ammo[i].x > screenw / 2 - 3) { // wall bump
                enemy_ammo[i].dir *= -1;
                gotoxy(enemy_ammo[i].x, enemy_ammo[i].y); cout << "*";
                gotoxy(enemy_ammo[i].x + enemy_ammo[i].dir, enemy_ammo[i].y - 1); cout << " ";
            }
            else {
                gotoxy(enemy_ammo[i].x, enemy_ammo[i].y); cout << "*";
                gotoxy(enemy_ammo[i].x - enemy_ammo[i].dir, enemy_ammo[i].y - 1); cout << " ";
            }
            enemy_ammo[i].x += enemy_ammo[i].dir; enemy_ammo[i].y++;
        if (enemy_ammo[i].y == screenh-1) { // reached end of screen
            gotoxy(enemy_ammo[i].x - enemy_ammo[i].dir, enemy_ammo[i].y - 1); cout << " ";
            enemy_ammo.erase(enemy_ammo.begin()+i);
        }
    }
}

void collision(int& c_score, bool& ded) 
{
    for (int i = 0;i < my_ammo.size();i++) { // player hits enemy
        if ((my_ammo[i].x > enemyx + 3 && my_ammo[i].x < enemyx + 7 && my_ammo[i].y == 2) || 
            (my_ammo[i].x > enemyx + 2 && my_ammo[i].x < enemyx + 8 && my_ammo[i].y == 1) ||
            (my_ammo[i].x > enemyx + 1 && my_ammo[i].x < enemyx + 9 && my_ammo[i].y == 0)) {
            gotoxy(my_ammo[i].x, my_ammo[i].y); cout << "X";
            Sleep(8);
            updateScore(c_score);
            if (c_score == 25) ded = true;
            }
        
        for (int j = 0;j < enemy_ammo.size();j++) { // projectile hits projectile
            if (my_ammo[i].x == enemy_ammo[j].x && my_ammo[i].y == enemy_ammo[j].y) {
                gotoxy(enemy_ammo[j].x - enemy_ammo[j].dir, enemy_ammo[j].y - 1); cout << " ";
                gotoxy(my_ammo[i].x, my_ammo[i].y + 1); cout << " ";
                gotoxy(my_ammo[i].x, my_ammo[i].y); cout << "X";
                Sleep(8);
                gotoxy(my_ammo[i].x, my_ammo[i].y); cout << " ";
                my_ammo.erase(my_ammo.begin() + i); enemy_ammo.erase(enemy_ammo.begin() + j);
            }
        }
    }
    for (int i = 0;i < enemy_ammo.size();i++) { // enemy hits player (Game Over)
        if ((enemy_ammo[i].x == shipx + 4 && enemy_ammo[i].y == screenh - 3) ||
            (enemy_ammo[i].x > shipx + 2 && enemy_ammo[i].x < shipx + 7 && enemy_ammo[i].y == screenh -2 ) ||
            (enemy_ammo[i].x > shipx + 1 && enemy_ammo[i].x < shipx + 8 && enemy_ammo[i].y == screenh - 1)) {
            gotoxy(enemy_ammo[i].x, enemy_ammo[i].y); cout << "X";
            ded = true;
            }
    }
}
    
void game() {

    int enemy_dir = 1;         // enemy direction
    int enemy_inc = 5;         // random route starting length
    int enemy_steps = 0;       // distance covered
    int timer = 0;             // enemy fire interval
    int current_score = 0;
    bool game_over = false;

    system("cls");
    drawBorder();
    drawShip();
    drawEnemy(enemy_dir, enemy_steps, enemy_inc);
    while(!game_over) {
        if (_kbhit()) { // keyboard read
            char ch = _getch();
            switch (ch) {
            case 'a':   // go left
                if (shipx > 3) {
                    shipx--;
                    break;
                }
                else break;
            case 'd':   //go right
                if (shipx < screenw / 2 - 9) {
                    shipx++;
                    break;
                }
                else break;
            case 's': { // shoot
                p = { shipx + 4, screenh-2, 0};
                my_ammo.push_back(p);
            }
            }
        }
        fire();
        drawShip();
        drawEnemy(enemy_dir, enemy_steps, enemy_inc);
        if((timer++)%30 == 0) enemyFire(); // enemy fire frequency
        enemyProjectile();
        collision(current_score, game_over);
       
        Sleep(50);
    }
    gotoxy(screenw / 4, screenh / 2); cout << "Game Over";
    if (current_score == 25) { gotoxy(screenw / 4, screenh / 2 + 2); cout << "YOU WIN"; }
    else { gotoxy(screenw / 4, screenh / 2 + 1); cout << "YOU LOOSE"; }
    Sleep(1000);
    gotoxy(screenw / 2 +15, screenh - 6); cout << "Press e to exit";
    gotoxy(screenw / 2 + 15, screenh - 5); cout << "Press any key + enter to play again";
    gotoxy(screenw / 2 + 15, screenh - 4); char e = getchar();
    if (e == 'e') exit(0); if (e) game();
}

int main()
{
    game();
}