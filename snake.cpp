#include<iostream>
#include<cstddef>
#include<conio.h>
#include<windows.h>
#include<ctime>
#include<thread>
#include<conio.h>
#define STD_OUTPUT_HANDLE ((DWORD)-11)
using namespace std;
//Gsoy 2019/5/1 编写完成；
//Gsoy 2019/5/2 优化部分代码，加强数据的抽象与封装，
//尝试使用非阻塞函数监听按键，导致掉尾更加严重，不过蛇移动动画变得性感，原因可能是擦尾的延迟，
//使用非阻塞函数监听按键同时将多线程改为多进程来自动移动蛇尾，导致卡顿，掉尾严重；
//已知BUG 多线程下有时蛇尾无法擦除大几率是发生在吃食物以后，蛇与食物随机生成算法问题导致有几率生成在边界上；

bool listenKey();
void gotoxy(int x,int y){
    COORD pos={short(x),short(y)};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
}
struct coordinate{
    public:
    int x,y;
};
class Map{
    public:
    int width,height;
    void draw();
};
class Snake{
    friend class Food;
    protected:
    int tail[2],length;
    coordinate body[];
    public:
    void draw(),move(),clearTail(),create(),addBody();
    bool isAlive();
};
class Food:public coordinate{
    public:
    int score=0;
    void create(),draw();
    bool take();
};
enum Direction{UP,DOWN,LEFT,RIGHT} snake_dir;
void initialize(Map),gameOver(Map);

Food food;
Map map;
Snake snake;
int main(){
    map.width=60;
    map.height=30;
    initialize(map);
    food.create();
    food.draw();
    snake.create();
    snake.draw();
    thread snakeMove([](){
        while(true){
            Sleep(300);
            snake.move();
            if(snake.isAlive()){
                if(food.take()){
                    snake.addBody();
                    snake.draw();
                    food.create();
                    food.draw();
                }
                else{
                    snake.draw();
                    snake.clearTail();
                }
            }
            else{
                gameOver(map);
                break;
            }
        }
    });//创建线程用来自动移动蛇
    snakeMove.detach();
    while(true){
        if(listenKey()){
            snake.move();
            if(snake.isAlive()){
                if(food.take()){
                    snake.addBody();
                    snake.draw();
                    food.create();
                    food.draw();
                }
                else{
                    snake.draw();
                    snake.clearTail();
                }
            }
            else{
                gameOver(map);
                break;
            }
        }
    }
    system("pause");
    return 0;
}

void initialize(Map m){
    char buffer[32];
    sprintf_s(buffer,"mode con cols=%d lines=%d",m.width,m.height);
    system(buffer);//设置窗口大小
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO CursorInfo;
    GetConsoleCursorInfo(handle,&CursorInfo);
    CursorInfo.bVisible = false;
    SetConsoleCursorInfo(handle, &CursorInfo);//设置控制台状态
    map.draw();
}
void gameOver(Map m){
    gotoxy(m.width/2-6,m.height/3);
    cout<<"--GAME OVER--";
    gotoxy(m.width/2-6,m.height/3+1);
    cout<<" Score is "<<food.score;
    gotoxy(m.width/2-6,m.height/3+3);
}
bool listenKey(){
    char ch=getch();
    if(strchr("wasd",ch)!=NULL){//通过获取输入值是否包含在字符串内屏蔽其他按键
        switch(ch){
            case'w':
            if(snake_dir==DOWN){
                return false;
                break;
            }
            snake_dir=UP;
            break;
            case's':
            if(snake_dir==UP){
                return false;
                break;
            }
            snake_dir=DOWN;
            break;
            case'a':
            if(snake_dir==RIGHT){
                return false;
                break;
            }
            snake_dir=LEFT;
            break;
            case'd':
            if(snake_dir==LEFT){
                return false;
                break;
            }
            snake_dir=RIGHT;
            break;
        }
        return true;
    }
    else
       return false;
}
void Map::draw(){
    for(int i=0;i<width;i++){
        cout<<"#";
    }
    cout<<endl;
    for(int i=0;i<height-2;i++){
        cout<<"#";
        for(int i=0;i<width-2;i++){
            cout<<" ";
        }
        cout<<"#"<<endl;
    }
    for(int i=0;i<width;i++){
        cout<<"#";
    }
}
void Snake::draw(){
    for(int i=0;i<length;i++){
        gotoxy(body[i].x,body[i].y);
        cout<<"@";
    }
}
void Snake::clearTail(){
    gotoxy(tail[0],tail[1]);
    cout<<" ";
}
void Snake::move(){
    tail[0]=body[length-1].x;
    tail[1]=body[length-1].y;//移动前将蛇尾保存下来
    switch(snake_dir){
        case UP:
        for(int i=length-1;i>0;i--){
            body[i].x=body[i-1].x;
            body[i].y=body[i-1].y;
        }
        body[0].y--;
        break;
        case DOWN:
        for(int i=length-1;i>0;i--){
            body[i].x=body[i-1].x;
            body[i].y=body[i-1].y;
        }
        body[0].y++;
        break;
        case LEFT:
        for(int i=length-1;i>0;i--){
            body[i].x=body[i-1].x;
            body[i].y=body[i-1].y;
        }
        body[0].x--;
        break;
        case RIGHT:
        for(int i=length-1;i>0;i--){
            body[i].x=body[i-1].x;
            body[i].y=body[i-1].y;
        }
        body[0].x++;
        break;
    }//将蛇身每一节的坐标都改成前一节的坐标，头单独向某个方向移动坐标，实现蛇的移动
}
void Snake::create(){
    srand((unsigned)time(NULL)+10);
    length=3;
    for(int i=0,x=rand()%(map.width-1)+1,y=rand()%(map.height-1)+1;i<length;i++){
        body[i].x=x-i;
        body[i].y=y;
    }//利用地图大小随机生成蛇
    snake_dir=RIGHT;
}
void Snake::addBody(){
    body[length].x=tail[0];
    body[length].y=tail[1];
    length++;
}
bool Snake::isAlive(){
    for(int i=1;i<length;i++){
        if(body[0].x==body[i].x&&body[0].y==body[i].y){
            return false;
            break;
        }
    }
    if(body[0].x==0||body[0].y==0||body[0].x>=map.width-1||body[0].y>=map.height-1)
        return false;
    else
        return true;
}
void Food::create(){
    srand((unsigned)time(NULL));
    x=rand()%(map.width-1)+1;
    y=rand()%(map.height-1)+1;
    for(int i=0;i<snake.length;i++){
        if(x==snake.body[i].x&&y==snake.body[i].y){
            srand((unsigned)time(NULL)+1);
            x=rand()%(map.width-1)+1;
            y=rand()%(map.height-1)+1;
            break;
        }
    }//食物不能够生成在蛇身上
}
void Food::draw(){
    gotoxy(x,y);
    cout<<"$";
}
bool Food::take(){
    if(snake.body[0].x==x&&snake.body[0].y==y){
        score++;
        return true;
    }
    else
        return false;
}