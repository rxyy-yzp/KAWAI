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
	//�ò�ͬ���ݱ�ʾ��ͬ���ද���10�ֶ��ÿ��10��
	int i, j;
	for (i = 1;i < MAP_SIZE + 1;i++)
	{
		for (j = 1;j < MAP_SIZE + 1;j++)
		{
			map[i][j] = i;
		}
	}
	//����ͼƬ
	loadimage(&img_bk, "bk.jpg", WIDTH + 100, WIDTH + 50);
	IMAGE animal;
	loadimage(&animal, "animal.bmp");
	//�и�ͼƬ
	SetWorkingImage(&animal);//���ù�����
	for (i = 0;i < TOTAL_ANIMAL_NUM;i++)
	{
		for (j = 0;j < 2;j++)
		{
			getimage(&img_animal[i][j], j * IMG_SIZE, i * IMG_SIZE, IMG_SIZE, IMG_SIZE);
		}
	}
	SetWorkingImage();//�ָ�Ĭ�Ϲ�����
	//��������
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
				putimage(j * IMG_SIZE, i * IMG_SIZE, &img_animal[map[i][j]][1], SRCAND);//����ͼ
				putimage(j * IMG_SIZE, i * IMG_SIZE, &img_animal[map[i][j]][0], SRCPAINT);//ԭͼ 
			}
			
		}
	}
}

//��ȡ�����Ϣ
void mouseEvent()
{
	if (MouseHit())
	{
		MOUSEMSG msg = GetMouseMsg();
		//�ж��������λ��(begin,end)
		if (msg.uMsg == WM_LBUTTONDOWN && state == BEGIN)
		{
			//����������ת�ɶ�Ӧ��������±�
			begin.col = msg.x / IMG_SIZE;
			begin.row = msg.y / IMG_SIZE;
			state = END;//�ı�״̬
		}
		else if (msg.uMsg == WM_LBUTTONDOWN && state == END)
		{
			end.col = msg.x / IMG_SIZE;
			end.row = msg.y / IMG_SIZE;
			state = BEGIN;//�ı�״̬
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
	//�Ƿ���ͬһˮƽ����
	if (begin.row != end.row)
	{
		return false;
	}
	//�������֮���Ǹ���col�����С,��С�ı��������
	int minC = min(begin.col, end.col);
	int maxC = max(begin.col, end.col);
	for (col = minC + 1; col < maxC; col++)
	{
		//�ж��ǲ����ж���
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
	//�Ƿ���ͬһ��ֱ���� (row�Ƿ���ͬ)
	if (begin.col != end.col)
	{
		return false;
	}
	//�������֮���Ǹ���row�����С,��С�ı��������
	int minR = min(begin.row, end.row);
	int maxR = max(begin.row, end.row);
	for (row = minR + 1; row < maxR; row++)
	{
		//�ж��ǲ����ж���
		if (isBlock(row, begin.col))
		{
			return false;
		}
	}
	return true;
}

bool turn_once(struct Index begin, struct Index end)
{
	//�ҵ��������㶼�й�ϵ����������±�,��������
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
	//1.ˮƽ��������
	flag = horizon(begin, end);
	if (flag)
	{
		return true;
	}
	//2.��ֱ��������
	flag = vertical(begin, end);
	if (flag)
	{
		return true;
	}
	//3.һ��ת��
	flag = turn_once(begin, end);
	if (flag)
	{
		return true;
	}
	//4.����ת��
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
	SetWindowTextA(txz, "������");
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
					//��ʼ˫�����ͼ
					BeginBatchDraw();
					gameDraw();
					//����˫�����ͼ
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