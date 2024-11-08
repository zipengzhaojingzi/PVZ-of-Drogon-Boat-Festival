


#define _CRT_SECURE_NO_WARNINGS 1
/*
������־
1.��������Ŀ������Ŀģ��
2.�����ز�
3.ʵ���ʼ����Ϸ����
4.ʵ����Ϸ�����Ĺ�����
*/

#include <stdio.h>
#include <graphics.h> //easyxͼ��ͷ�ļ���Ҫ��װeasyxͼ�ο�
#include "tools.h"
#include <time.h>//���������������
#include <math.h>
#include "vector2.h"//��������
#include <windows.h>
//#include <iostream>
//#include <thread>

#include <mmsystem.h>//������Чͷ�ļ�
#pragma comment(lib,"winmm.lib")

#define WIN_WIDTH 900 //���ڿ��
#define WIN_HEIGHT 600 //���ڸ߶�
#define ZM_MAX 40//���ֽ�ʬ����
#define bosstime 60	//key time
int createzmspeed = 1;	//speed of create zm 
bool last = 0;

enum { WAN_DOU, XIANG_RI_KUI, ZONGZI, PEPPER, ZHI_WU_COUNT };//��ö�ٱ�������¼���е�ֲ������
int plantmoney[ZHI_WU_COUNT] = { 100, 50,55,125 };//ÿ��ֲ���money/����

//ʱ����ṹ��
struct CheckTime
{
	int PlantCD[ZHI_WU_COUNT];//ÿ��ֲ����ȴʱ��
	int PlantCDtime[ZHI_WU_COUNT];//CD����ʱ
	int TimeFlag;//ʱ����λ,��1ʱ��ʼ����ʱ
};

struct CheckTime CD = { {30,30,10,70},{0,0,0,0},0 };//CD�ṹ�帳��ֵ


IMAGE ImgBg;//��ʾ����ͼƬ
IMAGE ImgBar;//���ƿ�
IMAGE ImgCards[ZHI_WU_COUNT * 2];
IMAGE imgshovel;
IMAGE imgshovelslot;
IMAGE imgpool;	//boss.status=1

bool shoveluse = 0;

IMAGE* imgZhiWu[ZHI_WU_COUNT][20];

int curX, curY;//��ǰ��x��y�������ƶ������е�λ��
int curZhiWu; //��ǰѡ���ֲ�0��û��ѡ�У�1��ѡ���˵�һ��ֲ��
int catchplantstatus = 0;//���ץס״̬��0��ûץס��1��ץס

enum { GOING, WIN, FAIL };
int killCount;//�Ѿ�ɱ���Ľ�ʬ����
int zmCount;//�Ѿ����ֵĽ�ʬ����
int gameStatus;//��Ϸ״̬

struct zhiwu
{
	int type;  //0����ʾû��ֲ�1����ʾ��һ��ֲ��
	int FrameIndex; //����֡���
	bool catched;//�Ƿ񱻽�ʬ����
	int deadTime;//����������fg

	int timer;//��ʱ��
	int x, y;//��ʾֲ���λ��

	int money;
	int shootTime;
};

struct zhiwu map[5][9];//��ͼ�����о��У�����ͼʱ��Ҫ��������

enum { SUNSHINE_DOWN, SUNSHINE_GROUND, SUNSHINE_COLLECT, SUNSHINE_PRODUCT };//ö�ٱ�����¼���⵱ǰ״̬


struct SunshineBall
{
	int x, y;//��������Ʈ������е�����λ�ã�x���䣩
	int FrameIndex;//��ʾ��ǰ֡���
	int destY;//Ʈ��λ�õ�Y����
	bool used;//�����Ƿ���ʹ��
	int timer;

	float xoff;
	float yoff;

	float t;//���������ߵ�ʱ��� 0~1
	vector2 p1, p2, p3, p4;
	vector2 pCur;//��ǰʱ���������λ��
	float speed;//��������ٶ�
	int status;//��ǰ�������״̬
};

//�أ�Ԥ��׼��һ������أ�ʮ����
struct SunshineBall balls[10];
//����һ��ͼƬ����洢����֡
IMAGE imgSunshineBall[29];
int sunshine;
int ZhiWuDraw;//ֲ���־
int TotalCount = 0;//�ܼ�ʱ����Լÿ������1
int TotalTimeFlag = 0;//��ʱ���־λ



//������ʬ
struct zm
{
	int x, y;
	int FrameIndex;//��ʬͼƬ��֡���
	bool used;//��ʬ��ʹ��״̬
	int speed;//��ʬ���ƶ��ٶ�
	int row;//��ʬ������
	int blood;//��ʬѪ��
	bool dead;//��ʬ�Ƿ�����
	bool eating;//��ʬ���ڳ�ֲ���״̬
	int color;	//zongzi magic 0:normal, 1:green, 2:pink, 3:red, 4:keep away from
	bool shield;	//add shield
};

//������ʬ��
struct zm zms[ZM_MAX];

//��ʬͼƬ����
IMAGE imgZM[22];//��ʬ���ߵ�ͼƬ����
IMAGE imgZM1[22];//green��ʬ���ߵ�ͼƬ����
IMAGE imgZM2[22];//pink��ʬ���ߵ�ͼƬ����
IMAGE imgZMDead[20];//��ʬ������ͼƬ����
IMAGE imgzmfall[20];//��ʬfall down��ͼƬ����
IMAGE imgdeadbrain[20];//��ʬbrain is fall��ͼƬ����
IMAGE imgZMEat[21];//��ʬ��ֲ���ͼƬ����
IMAGE imgZMEat1[21];//green��ʬ��ֲ���ͼƬ����
IMAGE imgZMEat2[21];//pink��ʬ��ֲ���ͼƬ����
IMAGE imgshield;


//ʷ�ٷ�ͼƬ����
IMAGE imgSteve1[31];//ʷ�ٷ�˵����ͼƬ����
IMAGE imgSteve2[21];//ʷ�ٷ�ҡ�ε�ͼƬ����
IMAGE imgSteve3[56];//ʷ�ٷ�ץ��ͼƬ����
IMAGE imgStevesay[3];//ʷ�ٷ�˵��ͼƬ����
IMAGE imgZmNote[3];//��ʬ˵��ͼƬ����

//BOSS
struct boss
{
	int x = 0, y = 0;
	int status = 0;
	int FrameIndex = 0;
}boss;
IMAGE* imgboss[3][16];//boss has 3 status


//dragon boat 
int row_cargo = 8;	//car is used
struct car
{
	int x, y;
	int speed;
	int FrameIndex;
} dragon_boat[5];

IMAGE imgdragon_boat[20];

//�ӵ�����������
struct bullet
{
	int x, y;//�ӵ�����
	int row;//�ӵ����ڵ�����
	bool used;//�ӵ���ǰ״̬
	int speed;//�ӵ��ٶ�
	int blast;//�ӵ��Ƿ�����ը
	int FrameIndex;//�ӵ�֡���
};


struct bullet bullets[30];//�����ӵ���
IMAGE imgBulletNormal;//�ӵ�������״̬
IMAGE imgBullBlast[4];//�ӵ���ը֡ͼƬ
IMAGE burn[10];		//burn fire
IMAGE imgZmStand[11];//������ʬվ����ͼƬ����

void playMusicStart();//��������
void playMusicWin();//ʤ������
void playMusicLose();//ʧ������
void playMusicComing();//��ʬ��Ϯ����
void PlayMusicCherrybomb();//������ը����
void PlayMusicSteveSayshort1();//steve�۱Ȳ���
void PlayMusicSteveSayshort2();//steve�Ա��Ա�
void PlayMusicSteveSayshort3();//steve�Ա��Աȱ�
void PlayMusicSteveSaylong1();
void PlayMusicSteveSaylong2();
void PlayMusicgroan2();//��ʬ̾Ϣ
void PlayMusic_recite1();
void PlayMusic_recite2();
void PlayMusic_recite3();
void PlayMusic_chomp();//�Զ�����Ч
void PlayMusic_scream();//steve���
void PlayMusic_GragenBoat();//������ˮ��Ч

//�ж��ļ����Ƿ����
bool FileExist(const char* name)
{
	FILE* fp = fopen(name, "r");
	if (fp)
	{
		fclose(fp);
	}

	return fp != NULL;

}

//��Ϸ��ʼ��
void GameInit()
{
	//������Ϸ����ͼƬ
	//���ַ����޸�Ϊ"���ַ���"
	loadimage(&ImgBg, "res/bg.jpg");
	loadimage(&ImgBar, "res/bar5.png");
	loadimage(&imgpool, "res/pool.jpg");
	loadimage(&imgshovel, "res/Screen/shovel.png");
	loadimage(&imgshovelslot, "res/Screen/shovelSlot.png");

	memset(imgZhiWu, 0, sizeof(imgZhiWu));//��ʼ��Ϊ0
	memset(map, 0, sizeof(map));//��ʼ����ͼΪ0

	killCount = 0;
	zmCount = 0;
	gameStatus = GOING;
	ZhiWuDraw = 0;

	//��ʼ��ֲ�￨��
	char name[64];

	for (int i = 0; i < ZHI_WU_COUNT; i++)
	{
		//����ֲ�￨�Ƶ��ļ���
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&ImgCards[i], name);
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", (i + 1) * 10);
		loadimage(&ImgCards[i + ZHI_WU_COUNT], name);

		for (int j = 0; j < 20; j++)
		{
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);
			//���ж�����ļ��Ƿ����
			if (FileExist(name))
			{
				imgZhiWu[i][j] = new IMAGE;
				loadimage(imgZhiWu[i][j], name);
			}
			else
			{
				break;
			}
		}
	}
	curZhiWu = 0;

	//init boss
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			sprintf_s(name, sizeof(name), "res/boss/boss%d/%d.png", i + 1, j + 1);
			//���ж�����ļ��Ƿ����
			if (FileExist(name))
			{
				imgboss[i][j] = new IMAGE;
				loadimage(imgboss[i][j], name);
			}
			else
			{
				break;
			}
		}
	}

	sunshine = 200;//��ʼ����ֵΪ50

	memset(balls, 0, sizeof(balls));//����������
	for (int i = 0; i < 29; i++)
	{
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		loadimage(&imgSunshineBall[i], name);
	}

	//�����������
	srand(time(NULL));

	//������Ϸͼ�δ���
	initgraph(WIN_WIDTH, WIN_HEIGHT);

	//��������
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 28;
	f.lfWeight = 14;
	strcpy(f.lfFaceName, "Segoe UI Black");
	f.lfQuality = ANTIALIASED_QUALITY;//�����Ч��
	settextstyle(&f);
	setbkmode(TRANSPARENT);//�������屳��͸��
	setcolor(BLACK);//����������ɫ

	//��ʼ����ʬ����
	memset(zms, 0, sizeof(zms));


	for (int i = 0; i < 22; i++)
	{
		sprintf_s(name, sizeof(name), "res/zm/%d.png", i + 1);
		loadimage(&imgZM[i], name);
		sprintf_s(name, sizeof(name), "res/zm_green/%d.png", i + 1);
		loadimage(&imgZM1[i], name);
		sprintf_s(name, sizeof(name), "res/zm_pink/%d.png", i + 1);
		loadimage(&imgZM2[i], name);
	}
	loadimage(&imgshield, "res/shield.png");

	//boom! and�� burn!
	for (int i = 0; i < 10; i++)
	{
		sprintf_s(name, sizeof(name), "res/burn/%d.png", i + 9);
		loadimage(&burn[i], name);
	}
	//�ӵ�
	loadimage(&imgBulletNormal, "res/bullets/bullet_normal.png");
	memset(bullets, 0, sizeof(bullets));

	//��ʼ���㶹�ӵ���֡ͼƬ����
	loadimage(&imgBullBlast[3], "res/bullets/bullet_blast.png");
	for (int i = 0; i < 3; i++)
	{
		float k = (i + 1) * 0.2;
		loadimage(&imgBullBlast[i], "res/bullets/bulet_blast.png",
			imgBullBlast[3].getwidth() * k,
			imgBullBlast[3].getheight() * k, true);
	}

	//init image of drogan_boat
	memset(dragon_boat, 0, sizeof(dragon_boat));
	for (int i = 0; i < 5; i++) {
		dragon_boat[i].x = 0;
		dragon_boat[i].y = 140 + i * 85;
	}
	for (int i = 0; i < 20; i++)
	{
		sprintf_s(name, sizeof(name), "res/dragon boat/%d.png", i + 1);
		loadimage(&imgdragon_boat[i], name);
	}

	//��ʬ����
	for (int i = 0; i < 20; i++)
	{
		sprintf_s(name, sizeof(name), "res/zm_dead/%d.png", i + 1);
		loadimage(&imgZMDead[i], name);
		sprintf_s(name, sizeof(name), "res/zm_fall/%d.png", i + 1);
		loadimage(&imgzmfall[i], name);
		sprintf_s(name, sizeof(name), "res/zm_brain_fall/%d.png", i + 1);
		loadimage(&imgdeadbrain[i], name);
	}

	//��ʬ��ֲ��
	for (int i = 0; i < 21; i++)
	{
		sprintf_s(name, "res/zm_eat/%d.png", i + 1);
		loadimage(&imgZMEat[i], name);
		sprintf_s(name, "res/zm_green_eat/%d.png", i + 1);
		loadimage(&imgZMEat1[i], name);
		sprintf_s(name, "res/zm_pink_eat/%d.png", i + 1);
		loadimage(&imgZMEat2[i], name);
	}

	//��ʬվ��
	for (int i = 0; i < 11; i++)
	{
		sprintf_s(name, sizeof(name), "res/zm_stand/%d.png", i + 1);
		loadimage(&imgZmStand[i], name);
	}

	//ʷ�ٷ�˵��
	for (int i = 0; i < 31; i++) {
		sprintf_s(name, sizeof(name), "res/steve_say/%d.png", i + 1);
		loadimage(&imgSteve1[i], name);
	}

	//ʷ�ٷ�ҡ��
	for (int i = 0; i < 21; i++) {
		sprintf_s(name, sizeof(name), "res/steve_shake/%d.png", i + 1);
		loadimage(&imgSteve2[i], name);
	}

	//ʷ�ٷ�ץ
	for (int i = 0; i < 56; i++) {
		sprintf_s(name, sizeof(name), "res/steve_catched/%d.png", i + 1);
		loadimage(&imgSteve3[i], name);
	}

	//ʷ�ٷ�˵�����ݶԻ���
	for (int i = 0; i < 3; i++) {
		sprintf_s(name, sizeof(name), "res/stevesay/%d.png", i + 1);
		loadimage(&imgStevesay[i], name);
	}

	//��ʬ˵�����ݶԻ���
	for (int i = 0; i < 3; i++) {
		sprintf_s(name, sizeof(name), "res/ZmNote/%d.png", i + 1);
		loadimage(&imgZmNote[i], name);
	}

}

//���ƽ�ʬ
void DrawZM()
{
	int zmCount = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0; i < zmCount; i++)
	{
		if (zms[i].used)
		{
			//IMAGE* img = &imgZM[zms[i].FrameIndex];
			//IMAGE* img = (zms[i].dead) ? imgZMDead : imgZM;
			IMAGE* img = NULL;
			IMAGE* img2 = NULL;
			if (zms[i].dead) {

				img = imgzmfall;
				if (zms[i].color == 3) {
					img = imgZMDead;
				}

			}
			else if (zms[i].eating && zms[i].color == 0) img = imgZMEat;
			else if (zms[i].color == 1) {
				if (zms[i].eating) img = imgZMEat1;
				else img = imgZM1;
				//printf("%d", i);
			}

			else if (zms[i].color == 2) {
				if (zms[i].eating) img = imgZMEat2;
				else img = imgZM2;
			}

			else  img = imgZM;

			img += zms[i].FrameIndex;
			putimagePNG(zms[i].x, zms[i].y - img->getheight(), img);
			if (zms[i].shield)
				putimagePNG(zms[i].x + 78, zms[i].y - 90, &imgshield);
			//head falls
			if (zms[i].dead && zms[i].color != 3) {
				//img = imgZMDead;
				img2 = imgdeadbrain;
				img2 += zms[i].FrameIndex;
				putimagePNG(zms[i].x + 50, zms[i].y - img->getheight() - 20, img2);

			}
		}
	}
}

//��Ⱦ����
void DrawSunshine()
{
	//��Ⱦ�������ֵ
	if (ZhiWuDraw)//ֲ������ֲ��״̬
	{
		if (ZhiWuDraw == WAN_DOU + 1)
			sunshine -= plantmoney[WAN_DOU];

		else if (ZhiWuDraw == XIANG_RI_KUI + 1)
			sunshine -= 50;

		else if (ZhiWuDraw == ZONGZI + 1)	sunshine -= plantmoney[ZONGZI];
		else if (ZhiWuDraw == PEPPER + 1)	sunshine -= plantmoney[PEPPER];

		ZhiWuDraw = 0;
	}
	char ScoreText[8];
	sprintf_s(ScoreText, sizeof(ScoreText), "%d", sunshine);
	outtextxy(148, 67, ScoreText);//�������ֵ����

	//���ÿ��ֲ�￨�Ƶ��µĵ���ʱ
	for (int i = 0; i < ZHI_WU_COUNT; i++)
	{
		sprintf_s(ScoreText, sizeof(ScoreText), "%d", CD.PlantCDtime[i]);
		int x = 180 + (i + 1) * 65;//CDʱ����õ�λ��
		int y = 3;
		outtextxy(x, y, ScoreText);//���CD
	}

	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++)
	{
		//if (balls[i].used || balls[i].xoff)
		if (balls[i].used)
		{
			IMAGE* img = &imgSunshineBall[balls[i].FrameIndex];
			//putimagePNG(balls[i].x, balls[i].y, img);
			putimagePNG(balls[i].pCur.x, balls[i].pCur.y, img);
		}
	}

}

//��Ⱦֲ�￨��
void DrawCards()
{
	for (int i = 0; i < ZHI_WU_COUNT; i++)
	{
		int x = 210 + i * 65;//ֲ�￨�Ʒ��õ�λ��
		int y = 5;
		if (CD.PlantCDtime[i] == 0 && sunshine >= plantmoney[i])
			putimagePNG(x, y, &ImgCards[i]);
		else
			putimagePNG(x, y, &ImgCards[i + ZHI_WU_COUNT]);
	}
}

//boom!!
void boom(int row, int k) {

	int zCount = sizeof(zms) / sizeof(zms[0]);
	if (map[row][k].FrameIndex > 8) {

		//boom!!!
		for (int a = 0; a < zCount; a++)
		{

			if (zms[a].used == 0 || zms[a].dead == 1 || zms[a].color == 2)continue;
			if (zms[a].row == row) {
				zms[a].blood -= 300;								//die!!!
				zms[a].speed = 0;
				zms[a].color = 3;
			}

		}
		if (map[row][k].FrameIndex > 15) {
			map[row][k].type = 0;
			map[row][k].FrameIndex = 0;
			mciSendString("close res/audio/cherrybomb.wav", NULL, NULL, NULL);

		}
	}
}

//draw boss
void DrawBoss() {
	static int count = 0;
	boss.FrameIndex++;
	if (boss.status > 0) {
		if (imgboss[boss.status - 1][boss.FrameIndex - 1] == NULL)
		{
			boss.FrameIndex = 1;
		}
		putimagePNG(boss.x, boss.y, imgboss[boss.status - 1][boss.FrameIndex - 1]);
		count++;
		if (count > 128) {
			count = 0;
			boss.status = 0;
		}
	}
}

//��Ⱦֲ��
void DrawZhiWu()
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (map[i][j].type > 0)
			{
				//int x = 256 + j * 81;
				//int y = 179 + i * 102+14;
				int zhiWuType = map[i][j].type - 1;
				int index = map[i][j].FrameIndex;
				//putimagePNG(x, y, imgZhiWu[zhiWuType][index]);
				if (map[i][j].type != 4 || index <= 7)
					putimagePNG(map[i][j].x, map[i][j].y, imgZhiWu[zhiWuType][index]);
				else
				{
					//	PlayMusicCherrybomb();			�ڻ��ƺ����ﾡ��ֻ�û��ƣ���Ϊ��Ҫ���棬�����׿�,ok
						//PlaySound("res/audio/cherrybomb.wav", NULL, SND_FILENAME | SND_ASYNC);
					int x, y, n;
					x = 135;
					y = 100 + i * 87;
					n = map[i][j].FrameIndex - 8;
					putimagePNG(x, y, &burn[n]);
				}
			}
		}
	}

	//��Ⱦ �϶������е�ֲ��
	if (curZhiWu > 0 && catchplantstatus == 1)
	{
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);
	}
	if (shoveluse)	//using shovel
	{
		putimagePNG(curX - imgshovel.getwidth() / 2, curY - imgshovel.getheight() / 2, &imgshovel);
	}
	else putimagePNG(750, 20, &imgshovel);
}

//draw car
void DrawCar() {
	int dragon_boatMax = sizeof(dragon_boat) / sizeof(dragon_boat[0]);
	for (int i = 0; i < dragon_boatMax; i++)
	{
		int index = dragon_boat[i].FrameIndex;
		putimagePNG(dragon_boat[i].x, dragon_boat[i].y, &imgdragon_boat[index]);
	}
}
//��Ⱦ�㶹
void DrawBullets()
{
	int bulletMax = sizeof(bullets) / sizeof(bullets[0]);
	for (int i = 0; i < bulletMax; i++)
	{
		if (bullets[i].used)
		{
			if (bullets[i].blast)
			{
				IMAGE* img = &imgBullBlast[bullets[i].FrameIndex];
				putimagePNG(bullets[i].x, bullets[i].y, img);
				/*	printf("%d", i);*/
			}
			else
			{
				putimagePNG(bullets[i].x, bullets[i].y, &imgBulletNormal);
			}
		}
	}
}
int pool;
//���´���
void UpdataWindow()
{
	BeginBatchDraw();//��ʼ����

	putimage(-112, 0, &ImgBg);//��Ӧ���봰����ߺ��ϱߵľ���
	putimagePNG(120, 0, &ImgBar);//͸����ͼ
	putimagePNG(750, 20, &imgshovelslot);
	putimagePNG(750, 20, &imgshovel);	//shovel

	//boss
	if (TotalCount > bosstime - 2) {
		putimage(133 + pool * 74, 135, &imgpool);
		for (int i = 0; i < 5; i++) {
			map[i][pool].type = 0;
		}
	}
	DrawBoss();

	DrawCards();//��Ⱦֲ�￨��
	DrawZhiWu();//��Ⱦֲ��
	DrawCar();//draw car

	DrawSunshine();//��Ⱦ����

	DrawZM();//��Ⱦ��ʬ

	DrawBullets();//��Ⱦ�㶹

	EndBatchDraw();//����˫����
}


//�ռ�����
void CollectSunshine(ExMessage* msg)
{
	int count = sizeof(balls) / sizeof(balls[0]);
	//������
	int w = imgSunshineBall[0].getwidth();
	int h = imgSunshineBall[0].getheight();
	for (int i = 0; i < count; i++)
	{
		if (balls[i].used)
		{
			//int x = balls[i].x;
			//int y = balls[i].y;
			int x = balls[i].pCur.x;
			int y = balls[i].pCur.y;
			if (msg->x > x && msg->x < x + w && msg->y>y && msg->y < y + h)
			{
				//balls[i].used = false;
				balls[i].status = SUNSHINE_COLLECT;
				//sunshine += 25;
				//mciSendString("play res/sunshine.mp3", 0, 0, 0);//�ռ�����ʱ����Ч.....ȱ�㣺�������˲��ܲ�����һ��
				PlaySound("res/sunshine.wav", NULL, SND_FILENAME | SND_ASYNC);
				//�����������ƫ����
				//float destY = 0;
				//float destX = 262;
				//float angle = atan((y - destY) / (x - destX));//���ʱ���������ԭ�������ֵ
				//balls[i].xoff = 4 * cos(angle);
				//balls[i].yoff = 4 * sin(angle);//ÿ���ƶ��ĸ����ص�
				balls[i].p1 = balls[i].pCur;
				balls[i].p4 = vector2(160, 0);
				balls[i].t = 0;
				float distance = dis(balls[i].p1 - balls[i].p4);
				float off = 8;//��������ɻ�ȥ���ٶȣ����������׵�֡����˿��
				balls[i].speed = 1.0 / (distance / off);
			}
		}
	}
}

void UserClick()
{
	ExMessage msg;

	//�жϵ�ǰ������Ϣ������Ϣ����Ϊ��
	if (peekmessage(&msg))
	{
		if (msg.message == WM_LBUTTONDOWN)
		{
			if (msg.x > 210 && msg.x < 210 + 65 * ZHI_WU_COUNT && msg.y>5 && msg.y < 96)
			{
				int index = (msg.x - 210) / 65;
				curZhiWu = index + 1;
				if (sunshine >= plantmoney[index] && CD.PlantCDtime[index] == 0)	// judge if there is enough money 
					catchplantstatus = 1;
				curX = msg.x;
				curY = msg.y;
			}if (msg.x > 750 && msg.x < 820 && msg.y>20 && msg.y < 94) {	//use shovel
				curX = msg.x;
				curY = msg.y;
				shoveluse = 1;
			}
			else
			{
				CollectSunshine(&msg);
			}
		}
		else if (msg.message == WM_MOUSEMOVE && (catchplantstatus == 1 || shoveluse))
		{
			curX = msg.x;
			curY = msg.y;
		}

		else if (msg.message == WM_LBUTTONUP && (catchplantstatus == 1 || shoveluse))
		{
			if (catchplantstatus == 1) {
				if (msg.x > 120 && msg.x < 795 && msg.y > 135 && msg.y < 570)
				{
					int row = (msg.y - 135) / 87;
					int col = (msg.x - 135) / 74;
					if (map[row][col].type == 0)
					{
						map[row][col].type = curZhiWu;
						map[row][col].FrameIndex = 0;
						map[row][col].shootTime = 0;

						map[row][col].x = 135 + col * 74;
						map[row][col].y = 135 + row * 87;
						ZhiWuDraw = curZhiWu;
						CD.PlantCDtime[ZhiWuDraw - 1] = CD.PlantCD[ZhiWuDraw - 1];

						//printf("%d\n", curZhiWu);
					}

				}

				curZhiWu = 0;
				catchplantstatus = 0;
			}
			else if (shoveluse) {
				shoveluse = 0;
				int row = (msg.y - 135) / 87;
				int col = (msg.x - 135) / 74;
				if (map[row][col].type != 0)
				{
					map[row][col].type = 0;
					map[row][col].FrameIndex = 0;

					//printf("%d\n", curZhiWu);
				}
			}
		}
	}
}


//��������
void CreateSunshine()
{
	static int count = 0;
	static int fre = 400;
	count++;
	if (count >= fre)
	{
		fre = 200 + rand() % 200;
		count = 0;
		//���������ȡһ������ʹ�õ�����
		int ballMax = sizeof(balls) / sizeof(balls[0]);
		int i;
		for (i = 0; i < ballMax && balls[i].used; i++);
		if (i >= ballMax) return;

		balls[i].used = true;
		balls[i].FrameIndex = 0;
		//balls[i].x = 260 + rand() % (900 - 260);//260~900
		//balls[i].y = 60;
		//balls[i].destY = 200 + (rand() % 4) * 90; //�����������ÿһ��
		balls[i].timer = 0;
		//balls[i].xoff = 0;
		//balls[i].yoff = 0;//��ʼ��ƫ����
		balls[i].status = SUNSHINE_DOWN;
		balls[i].t = 0;
		balls[i].p1 = vector2(260 - 112 + rand() % (900 - 260 + 112), 60);
		balls[i].p4 = vector2(balls[i].p1.x, 200 + (rand() % 4) % 90);
		int off = 2;
		float distance = balls[i].p4.y - balls[i].p1.y;
		balls[i].speed = 1.0 / (distance / off);
	}
	//���տ���������
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (map[i][j].type == XIANG_RI_KUI + 1)
			{
				map[i][j].timer++;//��ʱ��
				if (map[i][j].timer > 400)
				{
					map[i][j].timer = 0;

					int k;
					for (k = 0; k < ballMax && balls[k].used; k++);
					if (k >= ballMax)return;

					balls[k].used = true;
					balls[k].p1 = vector2(map[i][j].x, map[i][j].y);
					int w = (100 + rand() % 50) * (rand() % 2 ? 1 : -1);
					balls[k].p4 = vector2(map[i][j].x + w, map[i][j].y + imgZhiWu[XIANG_RI_KUI][0]->getheight() - imgSunshineBall[0].getheight());
					balls[k].p2 = vector2(balls[k].p1.x + w * 0.3, balls[k].p1.y - 100);
					balls[k].p3 = vector2(balls[k].p1.x + w * 0.7, balls[k].p1.y - 100);
					balls[k].status = SUNSHINE_PRODUCT;
					balls[k].speed = 0.05;
					balls[k].t = 0;
				}
			}
		}
	}
}

//��������
void UpdataSunshine()
{
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++)
	{
		if (balls[i].used)
		{
			balls[i].FrameIndex = (balls[i].FrameIndex + 1) % 29;
			if (balls[i].status == SUNSHINE_DOWN)//�����Ǵ�ֱ�½���״̬
			{
				struct SunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);//���µ�ǰλ��
				if (sun->t >= 1)
				{
					sun->status = SUNSHINE_GROUND;
					sun->timer = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_GROUND)//��������ص�״̬
			{
				balls[i].timer++;
				if (balls[i].timer > 100)//100ms��������ʧ
				{
					balls[i].used = false;
					balls[i].timer = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_COLLECT)//�����������ռ���״̬
			{
				struct SunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);
				if (sun->t > 1)
				{
					sun->used = false;
					sunshine += 25;//�ռ���������������ֵ����
				}
			}
			else if (balls[i].status == SUNSHINE_PRODUCT)//��������������״̬
			{
				struct SunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = calcBezierPoint(sun->t, sun->p1, sun->p2, sun->p3, sun->p4);
				if (sun->t > 1)
				{
					sun->status = SUNSHINE_GROUND;
					sun->timer = 0;
				}
			}


			//balls[i].FrameIndex = (balls[i].FrameIndex + 1) % 29;
			//if (balls[i].timer == 0)
			//{
			//	balls[i].y += 2;
			//}
			//
			////���䵽�����������ʱ��
			//if (balls[i].y >= balls[i].destY)
			//{
			//	balls[i].timer++;
			//	if (balls[i].timer > 100)
			//	{
			//		balls[i].used = false;
			//	}
			//}
		}
		//else if(balls[i].xoff)
		//{
		//	float destY = 0;
		//	float destX = 262;
		//	float angle = atan((balls[i].y - destY) / (balls[i].x - destX));
		//	balls[i].xoff = 4 * cos(angle);
		//	balls[i].yoff = 4 * sin(angle);

		//	balls[i].x -= balls[i].xoff;
		//	balls[i].y -= balls[i].yoff;
		//	if (balls[i].y < 0 || balls[i].x < 262)
		//	{
		//		balls[i].xoff = 0;
		//		balls[i].yoff = 0;
		//		sunshine += 25;
		//	}
		//}
	}

}

//������ʬ
void CreateZM()
{
	if (zmCount >= ZM_MAX) {
		return;
	}

	static int zmFre = 200;//��ʬ��ʼƵ��
	static int count = 0;//�������õĴ���


	count++;
	if (count > zmFre)
	{
		count = 0;
		zmFre = rand() % 200 + 300 / createzmspeed;

		int i;
		int zmMax = sizeof(zms) / sizeof(zms[0]);
		for (i = 0; i < zmMax && (zms[i].used); i++);
		if (i < zmMax)
		{
			memset(&zms[i], 0, sizeof(zms[i]));
			zms[i].used = true;
			zms[i].x = WIN_WIDTH;
			zms[i].row = rand() % 5;//0~2
			zms[i].y = 215 + (zms[i].row) * 87;
			zms[i].speed = 2;//��ʬ�ٶ�
			zms[i].blood = 200;//��ʬ��ʼѪ��Ϊ100��
			if (last) {
				zms[i].speed += 2;	//third section 
				zms[i].blood += 300;
				zms[i].shield = 1;
			}
			zms[i].dead = false;//add
			zmCount++;
		}
	}
}

//update car
void UpdateCar(int row) {

	for (int i = 0; i < 5; i++) {

		dragon_boat[i].FrameIndex = (dragon_boat[i].FrameIndex + 1) % 20;
		dragon_boat[i].x += dragon_boat[i].speed;
	}

}

//���½�ʬ
void UpdataZM()
{
	int zmMax = sizeof(zms) / sizeof(zms[0]);

	static int count = 0;
	count++;
	if (count > 2)
	{
		count = 0;
		//���½�ʬ��λ��
		for (int i = 0; i < zmMax; i++)
		{
			if (zms[i].used)
			{
				/*		printf("%d", zms[i].row);
						printf("%d", row_cargo);*/
				if (zms[i].blood <= 200)	zms[i].shield = 0;
				if (dragon_boat[zms[i].row].x > zms[i].x - 56 && dragon_boat[zms[i].row].x < zms[i].x
					&& zms[i].color != 2 && dragon_boat[zms[i].row].speed != 0) {
					zms[i].blood -= 500;
				}
				if (zms[i].color == 1)
					zms[i].blood -= 2;
				zms[i].x -= zms[i].speed;
				if (zms[i].blood <= 0 && zms[i].dead == false)
				{
					zms[i].dead = true;
					zms[i].speed = 0;
					zms[i].FrameIndex = 0;

				}
				if (zms[i].x < 56)
				{
					row_cargo = zms[i].row;
					//printf("GAME OVER\n");
					//MessageBox(NULL, "over", "over", 0);//���Ż�
					//exit(0);//���Ż�
					dragon_boat[row_cargo].speed = 8;

				}
				if (zms[i].x < -56)	gameStatus = FAIL;
			}
		}
		UpdateCar(row_cargo);
	}

	static int count2 = 0;
	count2++;
	if (count2 > 2)
	{
		count2 = 0;
		for (int i = 0; i < zmMax; i++)
		{
			if (zms[i].used)
			{
				if (zms[i].dead)
				{
					zms[i].FrameIndex++;
					if (zms[i].FrameIndex >= 20)
					{
						zms[i].used = false;
						if (zms[i].color != 2)
							killCount++;

					}
				}
				//��ʬ��ֲ��Ķ���
				else if (zms[i].eating)
				{
					zms[i].FrameIndex = (zms[i].FrameIndex + 1) % 21;
					//PlaySound("res/audio/chomp.wav", NULL, SND_FILENAME | SND_ASYNC);
				}
				else    //zm walking
				{
					zms[i].FrameIndex = (zms[i].FrameIndex + 1) % 22;
				}

			}
			/*else if (zms[i].color == 2) {
				zms[i].FrameIndex = (zms[i].FrameIndex + 1) % 22;
			}*/
			if (killCount == ZM_MAX)
			{
				gameStatus = WIN;
			}
		}
	}
}

//�����ӵ�
void shoot()
{
	static int count = 0;
	if (++count < 2)return;
	count = 0;//��ʱ�������ӵ�����Ƶ��

	int lines[5] = { 0 };
	int zmCount = sizeof(zms) / sizeof(zms[0]);
	int bulletMax = sizeof(bullets) / sizeof(bullets[0]);
	int dangerX = WIN_WIDTH - imgZM[0].getwidth() / 5;	//make plant shoot early
	for (int i = 0; i < zmCount; i++)
	{
		if (zms[i].used && zms[i].color != 2 && zms[i].x < dangerX)
		{
			lines[zms[i].row] = 1;
		}
	}

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (map[i][j].type == WAN_DOU + 1 && lines[i])
			{
				//static int count2 = 0;
				//count2++;
				map[i][j].shootTime++;
				//if (count2 > 20)//�����ӵ����Ƶ��
				if (map[i][j].shootTime > 20)
				{
					//count2 = 0;
					map[i][j].shootTime = 0;

					int k;
					for (k = 0; k < bulletMax && bullets[k].used; k++);
					if (k < bulletMax)
					{
						bullets[k].used = true;
						bullets[k].row = i;
						bullets[k].speed = 8;

						bullets[k].blast = false;
						bullets[k].FrameIndex = 0;

						int zwX = 135 + j * 74;
						int zwY = 135 + i * 87;
						bullets[k].x = zwX + imgZhiWu[map[i][j].type - 1][0]->getwidth() - 20;
						bullets[k].y = zwY + 5;
						PlaySound("res/throw.wav", NULL, SND_FILENAME | SND_ASYNC);
					}
				}
			}
		}
	}
}


//�����㶹�ӵ�
void UpdataBullets()
{
	static int count = 0;
	if (++count < 2)return;
	count = 0;//��ʱ�������ӵ�������
	int countMax = sizeof(bullets) / sizeof(bullets[0]);
	for (int i = 0; i < countMax; i++)
	{
		if (bullets[i].used)
		{

			bullets[i].x += bullets[i].speed;
			if (bullets[i].x > WIN_WIDTH)
			{
				bullets[i].used = false;
			}

			//��ʵ���ӵ�����ײ���
			if (bullets[i].blast)
			{
				bullets[i].FrameIndex++;
				if (bullets[i].FrameIndex >= 4)
				{
					bullets[i].used = false;
				}
			}
		}
	}
}

//�ӵ��Խ�ʬ����ײ���
void CheckBullet_To_Zm()
{
	int bCount = sizeof(bullets) / sizeof(bullets[0]);
	int zCount = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0; i < bCount; i++)
	{
		if (bullets[i].used == false || bullets[i].blast)continue;
		for (int k = 0; k < zCount; k++)
		{
			if (zms[k].used == false || zms[k].color == 2)continue;//���
			int x1 = zms[k].x + 90;
			int x2 = zms[k].x + 120;
			int x = bullets[i].x;
			if (zms[k].dead == false && bullets[i].row == zms[k].row && x > x1 && x < x2 && zms[k].color != 2)
			{
				/*if(zms[k].color==1)				zms[k].blood -= 20;*/
				zms[k].blood -= 20;
				bullets[i].blast = true;
				bullets[i].speed = 0;

				break;
			}
		}
	}
}
//zm_pink eat zm_normal
void Checkzmpink_To_Zm()
{

	int zCount = sizeof(zms) / sizeof(zms[0]);

	for (int k = 0; k < zCount; k++)
	{
		if (zms[k].color != 2 || zms[k].dead == true)continue;//���
		for (int i = 0; i < zCount; i++) {
			if (zms[i].y == zms[k].y && i != k) {

				int x1 = zms[i].x - 15;
				int x2 = zms[i].x + 15;
				if (zms[k].x > x1 && zms[k].x < x2) {
					if (zms[i].dead == false && zms[k].dead == false)
					{
						/*if(zms[k].color==1)				zms[k].blood -= 20;*/
						zms[k].blood -= 2;
						zms[k].eating = true;	//eating 
						//PlayMusic_chomp();
						zms[k].speed = 0;
						zms[i].blood -= 2;
						zms[i].eating = true;	//eating 
						zms[i].speed = 0;
						break;	//can't eat two 
					}
					else if (zms[i].dead) {
						zms[k].eating = false;
						//mciSendString("close res/audio/chomp.wav", NULL, NULL, NULL);
						zms[k].speed = -2;
						continue;//find next;
					}
					else if (zms[k].dead) {	//pink dies
						zms[i].eating = false;
						zms[i].speed = 2;
						break;
					}
				}
			}



		}
	}
}

//void CheckZmTo_zmpink()
//{
//
//	int zCount = sizeof(zms) / sizeof(zms[0]);
//
//	for (int k = 0; k < zCount; k++)
//	{
//		if (zms[k].color != 2)continue;//���
//		for (int i = 0; i < zCount; i++) {
//			if (zms[i].y == zms[k].y && i != k) {
//
//				int x1 = zms[k].x - 15;
//				int x2 = zms[k].x + 15;
//				if (zms[i].dead == false && zms[i].x > x1 && zms[i].x < x2 && zms[k].color == 2)
//				{
//					/*if(zms[k].color==1)				zms[k].blood -= 20;*/
//					zms[k].blood -= 2;
//					zms[i].eating = true;	//eating 
//					zms[i].speed = 0;
//					break;	//can't eat two 
//				}
//				else {
//					zms[k].eating = false;
//					zms[k].speed = -2;
//					continue;	//find next;
//				}
//
//			}
//		}
//	}
//}

//void boom(int row,int k){
//
//		int zCount = sizeof(zms) / sizeof(zms[0]);
//		if (map[row][k].FrameIndex > 8) {	
//			int x, y, n;
//			x = 144;
//			y = 150 + row * 102 ;
//			n = map[row][k].FrameIndex - 9;
//			putimagePNG(x, y, &burn[n]);
//			//boom!!!
//			for (int a = 0; a < zCount; a++)
//			{
//
//				if (zms[a].dead || zms[a].color == 2)continue;
//				if (zms[a].row == row) {
//					zms[a].blood-=300;								//die!!!
//					zms[a].speed = 0;
//					zms[a].color = 3;
//				}
//
//			}
//			if (map[row][k].FrameIndex > 15) {
//				map[row][k].type = 0;
//				map[row][k].FrameIndex = 0;
//			}
//		}
//	
//}

//��ʬ��ֲ�����ײ���
void CheckZM_To_ZhiWu()
{
	int zCount = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0; i < zCount; i++)
	{
		if (zms[i].dead == true || zms[i].color == 2)continue;
		zms[i].eating = false;
		zms[i].speed = 2;

		int row = zms[i].row;
		for (int k = 0; k < 9; k++)
		{
			if (map[row][k].type == 0 || map[row][k].type == 4)continue;

			int zhiWuX = 135 + k * 74;
			int x1 = zhiWuX + 10;
			int x2 = zhiWuX + 60;
			int x3 = zms[i].x + 80;
			if (x3 > x1 && x3 < x2)
			{
				if (map[row][k].catched)
				{

					zms[i].eating = true;
					/*PlayMusic_chomp();*/
					zms[i].speed = 0;
					//zms[i].FrameIndex++;
					map[row][k].deadTime++;
					//if (zms[i].FrameIndex > 100)
					/*printf("%d\n", map[row][k].deadTime);*/
					if (map[row][k].deadTime >= 90)
					{
						zms[i].eating = false;
						//mciSendString("close res/audio/chomp.wav", NULL, NULL, NULL);
						zms[i].FrameIndex = 0;
						zms[i].speed = 2;

						if (map[row][k].type == 3) {
							zms[i].color = rand() % 4;
							map[row][k].deadTime = 0;
							map[row][k].type = 0;
							map[row][k].FrameIndex = 0;
							map[row][k].catched = false;

							switch (zms[i].color)
							{
							case 3: {
								map[row][k].type = 4;
								//red
								break;
							}
							case 1 || 0: {
								break;
							}
							case 2: {
								zms[i].speed = -2;
								zms[i].blood = 200;
								killCount++;
								break;
							}
							}
						}
						else {
							map[row][k].deadTime = 0;
							map[row][k].type = 0;
							map[row][k].FrameIndex = 0;
							map[row][k].catched = false;
						}

					}
				}
				else
				{
					map[row][k].catched = true;
					map[row][k].deadTime = 0;
					zms[i].FrameIndex = 0;
				}
			}
		}
	}
}

int ChompMusicOpen = 0;//�Զ�����Ч���
//�Զ������
void EatingMusic_chaeck() {
	int sz = sizeof(zms) / sizeof(zms[0]);
	int flag = 0;

	for (int i = 0; i < sz; i++) {
		if (zms[i].eating && zms[i].dead == 0) {
			if (ChompMusicOpen == 0)
				PlayMusic_chomp();
			flag = 1;
			ChompMusicOpen = 1;
			break;
		}
	}
	if (flag == 0) {
		mciSendString("close res/audio/chomp.mp3", NULL, NULL, NULL);
		ChompMusicOpen = 0;
	}
}

int GragenboatMusicOpen[sizeof(dragon_boat) / sizeof(dragon_boat[0])] = { 0 };//������Ч���
//���ۿ������
void GragenboatGO_Check() {
	int sz = sizeof(dragon_boat) / sizeof(dragon_boat[0]);

	for (int i = 0; i < sz; i++) {
		if (dragon_boat[i].x != 0 && GragenboatMusicOpen[i] == 0) {
			//PlayMusic_GragenBoat();
			PlaySound("res/zombie_entering_water", NULL, SND_FILENAME | SND_ASYNC);
			GragenboatMusicOpen[i] = 1;
		}
	}
}


//��ײ���
void CollisiomCheck()
{
	CheckBullet_To_Zm(); //�ӵ��Խ�ʬ����ײ���
	CheckZM_To_ZhiWu();//��ʬ��ֲ�����ײ���
	Checkzmpink_To_Zm();//check if pink zm eats zm and zm eats pink zm 
	EatingMusic_chaeck();//�Զ������
	GragenboatGO_Check();//���ۿ������
}

void UndataZhiWu()
{
	static int count = 0;
	if (++count < 2)return;
	count = 0;
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (map[i][j].type > 0)
			{
				map[i][j].FrameIndex++;
				int zhiWuType = map[i][j].type - 1;
				int index = map[i][j].FrameIndex;
				if (map[i][j].type == 4) {
					int row = i;
					int k = j;
					if (index == 7)	PlayMusicCherrybomb();// play music boom
					if (index > 8)
						boom(row, k);
				}
				else if (imgZhiWu[zhiWuType][index] == NULL)
				{
					map[i][j].FrameIndex = 0;
				}
			}
		}
	}
}

//������Ϸ
void UpdataGame()
{

	UndataZhiWu();

	CreateSunshine();//��������
	UpdataSunshine();//��������״̬



	CreateZM();//������ʬ
	UpdataZM();//���½�ʬ��״̬

	shoot();//�����㶹�ӵ�
	UpdataBullets();//�����㶹�ӵ�
	CollisiomCheck();//ʵ���㶹�ӵ�����ײ���  check should be earlier than update zm !

}

int Isview = 0;
//�����˵�
void StartUI()
{
	IMAGE imgBg, imgMenu1, imgMenu2, imgIsview;
	loadimage(&imgBg, "res/menu.png");
	loadimage(&imgMenu1, "res/menu1.png");
	loadimage(&imgMenu2, "res/menu2.png");
	loadimage(&imgIsview, "res/isview.png");
	int flag = 0;

	while (1)
	{
		BeginBatchDraw();
		putimage(0, 0, &imgBg);
		putimagePNG(474, 75, flag ? &imgMenu2 : &imgMenu1);
		if (Isview == 0)
			putimagePNG(0, 0, &imgIsview);


		ExMessage msg;
		if (peekmessage(&msg))
		{
			if (msg.message == WM_LBUTTONDOWN && msg.x > 0 && msg.x < 300 && msg.y > 0 && msg.y < 150)
			{
				Isview = 1;
			}
			if (msg.message == WM_LBUTTONDOWN && msg.x > 474 && msg.x < 474 + 300 && msg.y > 75 && msg.y < 75 + 100)
			{
				flag = 1;
				//EndBatchDraw();
			}
			else if (msg.message == WM_LBUTTONUP && flag == 1)
			{
				EndBatchDraw();
				break;
			}
		}
		EndBatchDraw();
	}
}

//����Ѳ��
void ViewScence()
{
	int xMin = WIN_WIDTH - ImgBg.getwidth();//900-1400=-500
	vector2 points[9] = { {550,80 },{530,160},{630,170},{530,200},
						 {515,270},{565,370},{605,340},{705,280},{690,340} };//��ʼ�Ľ�ʬ����λ��

	int index[9];
	for (int i = 0; i < 9; i++)
	{
		index[i] = rand() % 11;//�����������ʼ֡���
	}

	int count = 0;
	for (int x = 0; x >= xMin; x -= 2)
	{
		BeginBatchDraw();//����,��ֹ����
		putimage(x, 0, &ImgBg);//��Ⱦ����ͼƬ��ÿ�������ƶ�2����

		count++;
		for (int k = 0; k < 9; k++)
		{
			putimagePNG(points[k].x - xMin + x, points[k].y, &imgZmStand[0]);
			if (count >= 10)
			{
				index[k] = (index[k] + 1) % 11;//ÿʮ�ξ��л�һ֡
			}
		}
		if (count >= 10)count = 0;

		EndBatchDraw();//��������
		Sleep(5);
	}

	//ͣ��1s����
	for (int i = 0; i < 100; i++)
	{
		BeginBatchDraw();

		putimage(xMin, 0, &ImgBg);
		for (int k = 0; k < 9; k++)
		{
			putimagePNG(points[k].x, points[k].y, &imgZmStand[index[k]]);
			index[k] = (index[k] + 1) % 11;
		}

		EndBatchDraw();
		Sleep(30);
	}

	for (int x = xMin; x <= -112; x += 2)
	{
		BeginBatchDraw();

		putimage(x, 0, &ImgBg);

		count++;
		for (int k = 0; k < 9; k++)
		{
			putimagePNG(points[k].x - xMin + x, points[k].y, &imgZmStand[index[k]]);
			if (count >= 10)
			{
				index[k] = (index[k] + 1) % 11;
			}
			if (count >= 10)count = 0;
		}

		EndBatchDraw();
		Sleep(5);
	}

	Sleep(100);

	//ʷ�ٷ�˵��
	for (int j = 0; j < 3; j++)
	{
		switch (j)
		{
		case 0:PlayMusicSteveSaylong1(); break;
		case 1:PlayMusicSteveSayshort2(); break;
		case 2:PlayMusicSteveSaylong2(); break;
		default:
			break;
		}
		for (int i = 0; i < 31; i++)
		{
			BeginBatchDraw();
			putimage(-112, 0, &ImgBg);
			putimagePNG(140, 80, &imgStevesay[j]);
			putimagePNG(0, 0, &imgSteve1[i]);
			// printf("%d", i);
			EndBatchDraw();
			Sleep(100);
		}
	}

	Sleep(50);

	//ʷ�ٷ�ҡ��
	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 21; i++)
		{
			BeginBatchDraw();
			putimage(-112, 0, &ImgBg);
			putimagePNG(200, 100, &imgZmNote[j]);
			putimagePNG(0, 0, &imgSteve2[i]);
			// printf("%d", i);
			EndBatchDraw();
			Sleep(100);
		}
	}

	Sleep(50);

	//ʷ�ٷ�ץ
	for (int i = 0; i < 56; i++)
	{
		if (i == 33)PlayMusic_scream();//steve���
		BeginBatchDraw();
		putimage(-112, 0, &ImgBg);
		putimagePNG(0, 0, &imgSteve3[i]);
		// printf("%d", i);
		EndBatchDraw();
		Sleep(80);
	}
	Sleep(200);
}

//���Ʋ۵���
void BarsDown()
{
	int height = ImgBar.getheight();
	for (int y = -height; y <= 0; y++) {
		BeginBatchDraw();//��ʼ����

		putimage(-112, 0, &ImgBg);
		putimagePNG(120, y, &ImgBar);
		putimagePNG(750, y + 20, &imgshovelslot);

		for (int i = 0; i < ZHI_WU_COUNT; i++)
		{
			int x = 210 + i * 65;
			putimage(x, 6 + y, &ImgCards[i]);
			putimagePNG(750, 26 + y, &imgshovel);
		}

		EndBatchDraw();//��������
		Sleep(5);
	}
}

//����Ƿ�ʤ��
bool CheckOver()
{
	int ret = false;
	if (gameStatus == WIN) {
		Sleep(2000);
		playMusicWin();
		loadimage(0, "res/win2.png");
		mciSendString("close res/bg.mp3", NULL, NULL, NULL);//�رձ�������
		ret = true;
	}
	else if (gameStatus == FAIL) {
		Sleep(2000);
		playMusicLose();
		loadimage(0, "res/ZombiesAteYourBrains.png");
		mciSendString("close res/bg.mp3", NULL, NULL, NULL);//�رձ�������
		ret = true;
	}
	return ret;
}

//ʱ���⺯��
void Time_Check()
{
	if (CD.TimeFlag)
	{
		for (int i = 0; i < ZHI_WU_COUNT; i++)
		{
			if (CD.PlantCDtime[i] > 0) CD.PlantCDtime[i]--; //����ȴʱ�����0�����Ӧֲ�����ȴʱ���Լ�
		}
		CD.TimeFlag = 0;
	}

	//��⿪������
	if (TotalCount == 8)
	{
		playMusicComing();
	}

	//��⽩ʬ�߳�
	if (TotalTimeFlag)
	{
		if (TotalCount % bosstime > bosstime - 2 && TotalCount / bosstime < 3) {	//key time
			boss.status += TotalCount / bosstime + 1;
			switch (boss.status) {
			case 1: {
				pool = rand() % 3;
				boss.x = 100 + pool * 74;
				boss.y = 30;
				boss.FrameIndex = 0;

				PlayMusic_recite1();
				//PlaySound("res/audio/recite/1.wav", NULL, SND_FILENAME | SND_ASYNC);
				break;
			};
			case 2: {
				boss.x = 750;
				boss.y = 300;
				boss.FrameIndex = 0;
				int i = 0, n = 0;
				for (i = 0; i < ZM_MAX; i++) {
					if (zms[i].used != true)
					{
						memset(&zms[i], 0, sizeof(zms[i]));
						zms[i].used = true;
						zms[i].x = WIN_WIDTH - rand() % 40;
						zms[i].row = rand() % 5;//0~2
						zms[i].y = 215 + (zms[i].row) * 87;
						zms[i].speed = 2;//��ʬ�ٶ�
						zms[i].blood = 200;//��ʬ��ʼѪ��Ϊ100��
						zms[i].dead = false;//add
						n++;
						zmCount++;
					}
					if (n > 10)
						break;
				}
				PlayMusic_recite2();
				//PlaySound("res/audio/recite/2.wav", NULL, SND_FILENAME | SND_ASYNC);
				break;
			}
			case 3: {
				boss.x = 750;
				boss.y = 300;
				boss.FrameIndex = 0;
				for (int i = 0; i < ZM_MAX; i++) {
					last = 1;
					createzmspeed = 3;
					if (zms[i].color != 2 && zms[i].dead != 1)
						zms[i].blood = 500;
					zms[i].shield = 1;
				}
				PlayMusic_recite3();
				//PlaySound("res/audio/recite/3.wav", NULL, SND_FILENAME | SND_ASYNC);
				break;
			}
			default: break;
			};
		}
		TotalTimeFlag = 0;
	}

}


int main(void)
{
	// ��¼�ϴβ������ֵ�ʱ��
	//DWORD lastPlayTime = 0;

	GameInit();//��Ϸ��ʼ��

	playMusicStart();

	StartUI();//��ʼ����

	//�Ƿ���������Ѳ�ӣ�0��������1����
	if (Isview == 0)
	{
		ViewScence();//����Ѳ��
	}


	BarsDown();//���Ƶ���

	int timer = 0;
	int flag = true;
	long double lasttime = 0;//��һ�μ�����ʱ��

	while (1)
	{
		//��ʱ���
		time_t current_time;

		time(&current_time);
		if (lasttime != current_time)
		{
			//printf("ִ��һ��%ld\n", TotalCount);
			TotalCount++;
			CD.TimeFlag = 1;
			TotalTimeFlag = 1;
		}
		lasttime = current_time;//�����ϴ�ʱ��
		Time_Check();//ִ��ʱ����

		UserClick();//���ʹ�ü��

		timer += getDelay();
		if (timer > 30)//��ʱ����ÿ30����ִ��һ��
		{
			flag = true;
			timer = 0;
		}
		if (flag)
		{
			flag = false;
			//UpdataGame();
			UpdataWindow();
			UpdataGame();
			if (CheckOver())break;
		}
	}
	system("pause");
	return 0;
}

void playMusicStart() {
	//PlaySound("res/bg.mav", NULL, SND_FILENAME | SND_ASYNC);
	mciSendString("open res/bg.mp3", NULL, NULL, NULL);
	mciSendString("play res/bg.mp3 repeat", NULL, NULL, NULL);
}

void playMusicWin() {
	//PlaySound("res/bg.mav", NULL, SND_FILENAME | SND_ASYNC);
	mciSendString("open res/win.mp3", NULL, NULL, NULL);
	mciSendString("play res/win.mp3", NULL, NULL, NULL);
}

void playMusicLose() {
	//PlaySound("res/bg.mav", NULL, SND_FILENAME | SND_ASYNC);
	mciSendString("open res/lose.mp3", NULL, NULL, NULL);
	mciSendString("play res/lose.mp3", NULL, NULL, NULL);
}

void playMusicComing() {
	mciSendString("open res/audio/awooga.wav", NULL, NULL, NULL);
	mciSendString("play res/audio/awooga.wav", NULL, NULL, NULL);
}

void PlayMusicCherrybomb() {
	//PlaySound("res/audio/cherrybomb.wav", NULL, SND_FILENAME | SND_ASYNC);
	mciSendString("open res/audio/cherrybomb.wav", NULL, NULL, NULL);
	mciSendString("play res/audio/cherrybomb.wav", NULL, NULL, NULL);
}

//steve�۱Ȳ���
void PlayMusicSteveSayshort1() {
	mciSendString("open res/audio/crazydaveshort1.mp3", NULL, NULL, NULL);
	mciSendString("play res/audio/crazydaveshort1.mp3", NULL, NULL, NULL);
}

//steve�Ա��Ա�
void PlayMusicSteveSayshort2() {
	mciSendString("open res/audio/crazydaveshort2.mp3", NULL, NULL, NULL);
	mciSendString("play res/audio/crazydaveshort2.mp3", NULL, NULL, NULL);
}

void PlayMusicSteveSayshort3() {
	mciSendString("open res/audio/crazydaveshort3.mp3", NULL, NULL, NULL);
	mciSendString("play res/audio/crazydaveshort3.mp3", NULL, NULL, NULL);
}

void PlayMusicSteveSaylong1() {
	mciSendString("open res/audio/crazydavelong1.mp3", NULL, NULL, NULL);
	mciSendString("play res/audio/crazydavelong1.mp3", NULL, NULL, NULL);
}


void PlayMusicSteveSaylong2() {
	mciSendString("open res/audio/crazydavelong2.mp3", NULL, NULL, NULL);
	mciSendString("play res/audio/crazydavelong2.mp3", NULL, NULL, NULL);
}

void PlayMusicgroan2() {
	mciSendString("open res/audio/groan2.mp3", NULL, NULL, NULL);
	mciSendString("play res/audio/groan2.mp3", NULL, NULL, NULL);
}

void PlayMusic_recite1() {
	mciSendString("open res/audio/recite/1.wav", NULL, NULL, NULL);
	mciSendString("play res/audio/recite/1.wav", NULL, NULL, NULL);
}


void PlayMusic_recite2() {
	mciSendString("open res/audio/recite/2.wav", NULL, NULL, NULL);
	mciSendString("play res/audio/recite/2.wav", NULL, NULL, NULL);
}

void PlayMusic_recite3() {
	mciSendString("open res/audio/recite/3.wav", NULL, NULL, NULL);
	mciSendString("play res/audio/recite/3.wav", NULL, NULL, NULL);
}

void PlayMusic_chomp() {
	mciSendString("open res/audio/chomp.mp3", NULL, NULL, NULL);
	mciSendString("play res/audio/chomp.mp3 repeat", NULL, NULL, NULL);
}


void PlayMusic_scream() {
	mciSendString("open res/audio/scream.mp3", NULL, NULL, NULL);
	mciSendString("play res/audio/scream.mp3", NULL, NULL, NULL);
}

void PlayMusic_GragenBoat() {
	mciSendString("open res/audio/zombie_entering_water.mp3", NULL, NULL, NULL);
	mciSendString("play res/audio/zombie_entering_water.mp3", NULL, NULL, NULL);
}