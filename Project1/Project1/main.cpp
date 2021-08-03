#include"stdio.h"
#include"time.h"
#include"graphics.h"
#include"conio.h"
#include "windows.h"
#include "mmsystem.h"
#pragma comment(lib, "winmm.lib")

#define MAP_SIZE 10
#define IMG_SIZE 39
#define WIDTH (MAP_SIZE+2)*IMG_SIZE
#define TOTAL_ANIMAL_NUM 42

int map[MAP_SIZE + 2][MAP_SIZE + 2];

IMAGE img_bk;
IMAGE img_animal[TOTAL_ANIMAL_NUM][2];
IMAGE img_fm, imgwin1, imgwin2;

struct Index
{
	int row;
	int col;
}begin = { -1,-1 }, end = { -1,-1 };

enum Cur
{
	BEGIN,
	END
};

Cur state = BEGIN;

void gameInit()
{
	srand((unsigned)time(NULL));
	//用不同数据表示不同种类动物，共10种动物，每种10个
	int i, j;
	for (i = 1;i < MAP_SIZE + 1;i++)
	{
		for (j = 1;j < MAP_SIZE + 1;j++)
		{
			map[i][j] = i;
		}
	}
	//加载图片
	loadimage(&img_bk, "bk.jpg", WIDTH + 100, WIDTH + 50);
	IMAGE animal;
	loadimage(&animal, "animal.bmp");
	//切割图片
	SetWorkingImage(&animal);//设置工作区
	for (i = 0;i < TOTAL_ANIMAL_NUM;i++)
	{
		for (j = 0;j < 2;j++)
		{
			getimage(&img_animal[i][j], j * IMG_SIZE, i * IMG_SIZE, IMG_SIZE, IMG_SIZE);
		}
	}
	SetWorkingImage();//恢复默认工作区
	//打乱数组
	for (i = 1;i < MAP_SIZE + 1;i++)
	{
		for (j = 1;j < MAP_SIZE + 1;j++)
		{
			int x = rand() % MAP_SIZE + 1;
			int y = rand() % MAP_SIZE + 1;
			int swap = map[i][j];
			map[i][j] = map[x][y];
			map[x][y] = swap;
		}
	}
}

void gameDraw()
{
	int i, j;
	putimage(0, 0, &img_bk);
	for (i = 1;i < MAP_SIZE + 1;i++)
	{
		for (j = 1;j < MAP_SIZE + 1;j++)
		{
			if (map[i][j] != 0)
			{
				putimage(j * IMG_SIZE, i * IMG_SIZE, &img_animal[map[i][j]][1], SRCAND);//掩码图
				putimage(j * IMG_SIZE, i * IMG_SIZE, &img_animal[map[i][j]][0], SRCPAINT);//原图 
			}
			
		}
	}
}

//获取鼠标消息
void mouseEvent()
{
	if (MouseHit())
	{
		MOUSEMSG msg = GetMouseMsg();
		//判断鼠标点击的位置(begin,end)
		if (msg.uMsg == WM_LBUTTONDOWN && state == BEGIN)
		{
			//把鼠标的坐标转成对应的数组的下标
			begin.col = msg.x / IMG_SIZE;
			begin.row = msg.y / IMG_SIZE;
			state = END;//改变状态
		}
		else if (msg.uMsg == WM_LBUTTONDOWN && state == END)
		{
			end.col = msg.x / IMG_SIZE;
			end.row = msg.y / IMG_SIZE;
			state = BEGIN;//改变状态
		}
		if (msg.uMsg == WM_LBUTTONDOWN)
		{
			printf("begin(%d,%d) end(%d,%d)\n", begin.row, begin.col, end.row, end.col);
		}
	}
}

bool isBlock(int row,int col)
{
	return map[row][col];
}

bool horizon(struct Index begin, struct Index end)
{
	int col;
	/*if (map[begin.row][begin.col] != map[end.row][end.col])
	{
		return false;
	}*/
	//是否在同一水平方向
	if (begin.row != end.row)
	{
		return false;
	}
	//求出两个之间那个的col大或者小,从小的遍历到大的
	int minC = min(begin.col, end.col);
	int maxC = max(begin.col, end.col);
	for (col = minC + 1; col < maxC; col++)
	{
		//判断是不是有动物
		if (isBlock(begin.row, col))
		{
			return false;
		}
	}
	return true;
}

bool vertical(struct Index begin, struct Index end)
{
	int row;
	/*if (map[begin.row][begin.col] != map[end.row][end.col])
	{
		return false;
	}*/
	//是否在同一垂直方向 (row是否相同)
	if (begin.col != end.col)
	{
		return false;
	}
	//求出两个之间那个的row大或者小,从小的遍历到大的
	int minR = min(begin.row, end.row);
	int maxR = max(begin.row, end.row);
	for (row = minR + 1; row < maxR; row++)
	{
		//判断是不是有动物
		if (isBlock(row, begin.col))
		{
			return false;
		}
	}
	return true;
}

bool turn_once(struct Index begin, struct Index end)
{
	//找到与两个点都有关系的两个点的下标,保存起来
	struct Index temp1 = { begin.row,end.col }, temp2 = { end.row,begin.col };
	bool flag = false;
	if (!isBlock(temp1.row, temp1.col))
	{
		flag |= horizon(begin, temp1) && vertical(end, temp1);
	}
	if (flag)
	{
		return true;
	}
	if (!isBlock(temp2.row, temp2.col))
	{
		flag |= vertical(begin, temp2) && horizon(end, temp2);
	}
	if (flag)
	{
		return true;
	}
	return flag;
}

bool turn_twice(struct Index begin, struct Index end)
{
	int i, j;
	int row1;
	int col1;
	struct Index temp;
	row1 = begin.row;
	col1 = begin.col;
	for (i = 0;i < row1;i++)
	{
		temp.row = i;
		temp.col = col1;
		if (!isBlock(i, col1))
		{
			if (vertical(begin, temp) && turn_once(end, temp))
			{
				return true;
			}
		}
	}
	for (i = row1 + 1;i <= MAP_SIZE + 1;i++)
	{
		temp.row = i;
		temp.col = col1;
		if (!isBlock(i, col1))
		{
			if (vertical(begin, temp) && turn_once(end, temp))
			{
				return true;
			}
		}
	}
	for (j = 0;j < col1;j++)
	{
		temp.row = row1;
		temp.col = j;
		if (!isBlock(row1 ,j))
		{
			if (horizon(begin, temp) && turn_once(end, temp))
			{
				return true;
			}
		}
	}
	for (j = col1 + 1;j <= MAP_SIZE + 1;j++)
	{
		temp.row = row1;
		temp.col = j;
		if (!isBlock(row1, j))
		{
			if (horizon(begin, temp) && turn_once(end, temp))
			{
				return true;
			}
		}
	}
	return false;
}

bool earse(struct Index begin, struct Index end)
{
	int flag = false;
	if (begin.row == end.row && begin.col == end.col)
	{
		return false;
	}
	if (map[begin.row][begin.col] != map[end.row][end.col])
	{
		return false;
	}
	//1.水平方向消除
	flag = horizon(begin, end);
	if (flag)
	{
		return true;
	}
	//2.垂直方向消除
	flag = vertical(begin, end);
	if (flag)
	{
		return true;
	}
	//3.一个转角
	flag = turn_once(begin, end);
	if (flag)
	{
		return true;
	}
	//4.两个转角
	flag = turn_twice(begin, end);
	if (flag)
	{
		return true;
	}
	return flag;
}

void showMap()
{
	int i, j;
	for (i = 0;i < MAP_SIZE + 2;i++)
	{
		for (j = 0;j < MAP_SIZE + 2;j++)
		{
			printf("%-2d", map[i][j]);
		}
		printf("\n");
	}
}

int ifVictory()
{
	int i, j;
	for (i = 1;i < MAP_SIZE + 1;i++)
	{
		for (j = 1;j < MAP_SIZE + 1;j++)
		{
			if (map[i][j] != 0)
			{
				return 0;
			}
		}
	}
	return 1;
}

int main()
{
	int fg;
	MOUSEMSG m;
	HWND txz = initgraph(WIDTH, WIDTH);
	SetWindowTextA(txz, "连连看");
	mciSendString("open yydjtc.mp3 alias BGM", 0, 0, 0);
	mciSendString("play BGM repeat", 0, 0, 0);
	loadimage(&img_fm, "ksbk.jpg", WIDTH, WIDTH);
	do {
		cleardevice();
		putimage(0, 0, &img_fm);
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN)
		{
			if (m.x >= 120 && m.x <= 360 && m.y >= 390 && m.y <= 435)
			{
				cleardevice();
				gameInit();
				showMap();
				while (1)
				{
					//开始双缓冲绘图
					BeginBatchDraw();
					gameDraw();
					//结束双缓冲绘图
					EndBatchDraw();
					mouseEvent();
					fg = earse(begin, end);
					if (fg && begin.row != -1 && end.row != -1)
					{
						map[begin.row][begin.col] = 0;
						map[end.row][end.col] = 0;
						begin.row = -1;
						begin.col = -1;
						end.row = -1;
						end.col = -1;
					}
					if (ifVictory() == 1)
					{
						cleardevice();
						loadimage(&imgwin1, "v1.jpg", WIDTH, WIDTH);
						loadimage(&imgwin2, "v2.jpg", WIDTH, WIDTH);
						putimage(0, 0, &imgwin1);
						Sleep(300);
						putimage(0, 0, &imgwin2);
						Sleep(300);
						putimage(0, 0, &imgwin1);
						Sleep(300);
						putimage(0, 0, &imgwin2);
						system("pause");
						break;
					}
				}
			}
		}
		
		
	} while (1);
	return 0;
}