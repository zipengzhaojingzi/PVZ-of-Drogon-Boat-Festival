


#define _CRT_SECURE_NO_WARNINGS 1
/*
开发日志
1.创建新项目（空项目模板
2.导入素材
3.实现最开始的游戏场景
4.实现游戏顶部的工具栏
*/

#include <stdio.h>
#include <graphics.h> //easyx图形头文件需要安装easyx图形库
#include "tools.h"
#include <time.h>//用来配置随机种子
#include <math.h>
#include "vector2.h"//向量函数
#include <windows.h>
//#include <iostream>
//#include <thread>

#include <mmsystem.h>//播放音效头文件
#pragma comment(lib,"winmm.lib")

#define WIN_WIDTH 900 //窗口宽度
#define WIN_HEIGHT 600 //窗口高度
#define ZM_MAX 40//本局僵尸个数
#define bosstime 60	//key time
int createzmspeed = 1;	//speed of create zm 
bool last = 0;

enum { WAN_DOU, XIANG_RI_KUI, ZONGZI, PEPPER, ZHI_WU_COUNT };//用枚举变量来记录已有的植物数量
int plantmoney[ZHI_WU_COUNT] = { 100, 50,55,125 };//每种植物的money/阳光

//时间检测结构体
struct CheckTime
{
	int PlantCD[ZHI_WU_COUNT];//每种植物冷却时间
	int PlantCDtime[ZHI_WU_COUNT];//CD倒计时
	int TimeFlag;//时间检测位,置1时开始倒计时
};

struct CheckTime CD = { {30,30,10,70},{0,0,0,0},0 };//CD结构体赋初值


IMAGE ImgBg;//表示背景图片
IMAGE ImgBar;//卡牌框
IMAGE ImgCards[ZHI_WU_COUNT * 2];
IMAGE imgshovel;
IMAGE imgshovelslot;
IMAGE imgpool;	//boss.status=1

bool shoveluse = 0;

IMAGE* imgZhiWu[ZHI_WU_COUNT][20];

int curX, curY;//当前的x和y坐标在移动过程中的位置
int curZhiWu; //当前选择的植物，0：没有选中，1：选择了第一种植物
int catchplantstatus = 0;//鼠标抓住状态，0：没抓住，1：抓住

enum { GOING, WIN, FAIL };
int killCount;//已经杀掉的僵尸个数
int zmCount;//已经出现的僵尸个数
int gameStatus;//游戏状态

struct zhiwu
{
	int type;  //0，表示没有植物，1：表示第一种植物
	int FrameIndex; //序列帧序号
	bool catched;//是否被僵尸捕获
	int deadTime;//死亡计数器fg

	int timer;//计时器
	int x, y;//表示植物的位置

	int money;
	int shootTime;
};

struct zhiwu map[5][9];//地图，五行九列，换地图时需要调整这里

enum { SUNSHINE_DOWN, SUNSHINE_GROUND, SUNSHINE_COLLECT, SUNSHINE_PRODUCT };//枚举变量记录阳光当前状态


struct SunshineBall
{
	int x, y;//阳光球在飘落过程中的坐标位置（x不变）
	int FrameIndex;//显示当前帧序号
	int destY;//飘落位置的Y坐标
	bool used;//阳光是否在使用
	int timer;

	float xoff;
	float yoff;

	float t;//贝塞尔曲线的时间点 0~1
	vector2 p1, p2, p3, p4;
	vector2 pCur;//当前时刻阳光球的位置
	float speed;//阳光球的速度
	int status;//当前阳光球的状态
};

//池：预先准备一个阳光池，十个等
struct SunshineBall balls[10];
//定义一个图片数组存储阳光帧
IMAGE imgSunshineBall[29];
int sunshine;
int ZhiWuDraw;//植物标志
int TotalCount = 0;//总计时器，约每秒自增1
int TotalTimeFlag = 0;//总时间标志位



//创建僵尸
struct zm
{
	int x, y;
	int FrameIndex;//僵尸图片的帧序号
	bool used;//僵尸的使用状态
	int speed;//僵尸的移动速度
	int row;//僵尸所在行
	int blood;//僵尸血量
	bool dead;//僵尸是否死亡
	bool eating;//僵尸正在吃植物的状态
	int color;	//zongzi magic 0:normal, 1:green, 2:pink, 3:red, 4:keep away from
	bool shield;	//add shield
};

//创建僵尸池
struct zm zms[ZM_MAX];

//僵尸图片数组
IMAGE imgZM[22];//僵尸行走的图片数组
IMAGE imgZM1[22];//green僵尸行走的图片数组
IMAGE imgZM2[22];//pink僵尸行走的图片数组
IMAGE imgZMDead[20];//僵尸死亡的图片数组
IMAGE imgzmfall[20];//僵尸fall down的图片数组
IMAGE imgdeadbrain[20];//僵尸brain is fall的图片数组
IMAGE imgZMEat[21];//僵尸吃植物的图片数组
IMAGE imgZMEat1[21];//green僵尸吃植物的图片数组
IMAGE imgZMEat2[21];//pink僵尸吃植物的图片数组
IMAGE imgshield;


//史蒂夫图片数组
IMAGE imgSteve1[31];//史蒂夫说话的图片数组
IMAGE imgSteve2[21];//史蒂夫摇晃的图片数组
IMAGE imgSteve3[56];//史蒂夫被抓的图片数组
IMAGE imgStevesay[3];//史蒂夫说话图片数组
IMAGE imgZmNote[3];//僵尸说话图片数组

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

//子弹的数据类型
struct bullet
{
	int x, y;//子弹坐标
	int row;//子弹所在的行数
	bool used;//子弹当前状态
	int speed;//子弹速度
	int blast;//子弹是否发生爆炸
	int FrameIndex;//子弹帧序号
};


struct bullet bullets[30];//创建子弹池
IMAGE imgBulletNormal;//子弹正常的状态
IMAGE imgBullBlast[4];//子弹爆炸帧图片
IMAGE burn[10];		//burn fire
IMAGE imgZmStand[11];//建立僵尸站立的图片数组

void playMusicStart();//开场音乐
void playMusicWin();//胜利音乐
void playMusicLose();//失败音乐
void playMusicComing();//僵尸来袭音乐
void PlayMusicCherrybomb();//辣椒爆炸音乐
void PlayMusicSteveSayshort1();//steve哇比博布
void PlayMusicSteveSayshort2();//steve略比略比
void PlayMusicSteveSayshort3();//steve略比略比比
void PlayMusicSteveSaylong1();
void PlayMusicSteveSaylong2();
void PlayMusicgroan2();//僵尸叹息
void PlayMusic_recite1();
void PlayMusic_recite2();
void PlayMusic_recite3();
void PlayMusic_chomp();//吃东西音效
void PlayMusic_scream();//steve尖叫
void PlayMusic_GragenBoat();//龙舟下水音效

//判断文件名是否存在
bool FileExist(const char* name)
{
	FILE* fp = fopen(name, "r");
	if (fp)
	{
		fclose(fp);
	}

	return fp != NULL;

}

//游戏初始化
void GameInit()
{
	//加载游戏背景图片
	//把字符集修改为"多字符集"
	loadimage(&ImgBg, "res/bg.jpg");
	loadimage(&ImgBar, "res/bar5.png");
	loadimage(&imgpool, "res/pool.jpg");
	loadimage(&imgshovel, "res/Screen/shovel.png");
	loadimage(&imgshovelslot, "res/Screen/shovelSlot.png");

	memset(imgZhiWu, 0, sizeof(imgZhiWu));//初始化为0
	memset(map, 0, sizeof(map));//初始化地图为0

	killCount = 0;
	zmCount = 0;
	gameStatus = GOING;
	ZhiWuDraw = 0;

	//初始化植物卡牌
	char name[64];

	for (int i = 0; i < ZHI_WU_COUNT; i++)
	{
		//生成植物卡牌的文件名
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&ImgCards[i], name);
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", (i + 1) * 10);
		loadimage(&ImgCards[i + ZHI_WU_COUNT], name);

		for (int j = 0; j < 20; j++)
		{
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);
			//先判断这个文件是否存在
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
			//先判断这个文件是否存在
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

	sunshine = 200;//初始阳光值为50

	memset(balls, 0, sizeof(balls));//把阳光池清空
	for (int i = 0; i < 29; i++)
	{
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		loadimage(&imgSunshineBall[i], name);
	}

	//配置随机种子
	srand(time(NULL));

	//创建游戏图形窗口
	initgraph(WIN_WIDTH, WIN_HEIGHT);

	//设置字体
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 28;
	f.lfWeight = 14;
	strcpy(f.lfFaceName, "Segoe UI Black");
	f.lfQuality = ANTIALIASED_QUALITY;//抗锯齿效果
	settextstyle(&f);
	setbkmode(TRANSPARENT);//设置字体背景透明
	setcolor(BLACK);//设置字体颜色

	//初始化僵尸数据
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

	//boom! and！ burn!
	for (int i = 0; i < 10; i++)
	{
		sprintf_s(name, sizeof(name), "res/burn/%d.png", i + 9);
		loadimage(&burn[i], name);
	}
	//子弹
	loadimage(&imgBulletNormal, "res/bullets/bullet_normal.png");
	memset(bullets, 0, sizeof(bullets));

	//初始化豌豆子弹的帧图片数组
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

	//僵尸死亡
	for (int i = 0; i < 20; i++)
	{
		sprintf_s(name, sizeof(name), "res/zm_dead/%d.png", i + 1);
		loadimage(&imgZMDead[i], name);
		sprintf_s(name, sizeof(name), "res/zm_fall/%d.png", i + 1);
		loadimage(&imgzmfall[i], name);
		sprintf_s(name, sizeof(name), "res/zm_brain_fall/%d.png", i + 1);
		loadimage(&imgdeadbrain[i], name);
	}

	//僵尸吃植物
	for (int i = 0; i < 21; i++)
	{
		sprintf_s(name, "res/zm_eat/%d.png", i + 1);
		loadimage(&imgZMEat[i], name);
		sprintf_s(name, "res/zm_green_eat/%d.png", i + 1);
		loadimage(&imgZMEat1[i], name);
		sprintf_s(name, "res/zm_pink_eat/%d.png", i + 1);
		loadimage(&imgZMEat2[i], name);
	}

	//僵尸站立
	for (int i = 0; i < 11; i++)
	{
		sprintf_s(name, sizeof(name), "res/zm_stand/%d.png", i + 1);
		loadimage(&imgZmStand[i], name);
	}

	//史蒂夫说话
	for (int i = 0; i < 31; i++) {
		sprintf_s(name, sizeof(name), "res/steve_say/%d.png", i + 1);
		loadimage(&imgSteve1[i], name);
	}

	//史蒂夫摇晃
	for (int i = 0; i < 21; i++) {
		sprintf_s(name, sizeof(name), "res/steve_shake/%d.png", i + 1);
		loadimage(&imgSteve2[i], name);
	}

	//史蒂夫被抓
	for (int i = 0; i < 56; i++) {
		sprintf_s(name, sizeof(name), "res/steve_catched/%d.png", i + 1);
		loadimage(&imgSteve3[i], name);
	}

	//史蒂夫说话内容对话框
	for (int i = 0; i < 3; i++) {
		sprintf_s(name, sizeof(name), "res/stevesay/%d.png", i + 1);
		loadimage(&imgStevesay[i], name);
	}

	//僵尸说话内容对话框
	for (int i = 0; i < 3; i++) {
		sprintf_s(name, sizeof(name), "res/ZmNote/%d.png", i + 1);
		loadimage(&imgZmNote[i], name);
	}

}

//绘制僵尸
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

//渲染阳光
void DrawSunshine()
{
	//渲染阳光分数值
	if (ZhiWuDraw)//植物在种植的状态
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
	outtextxy(148, 67, ScoreText);//输出阳光值分数

	//输出每种植物卡牌底下的倒计时
	for (int i = 0; i < ZHI_WU_COUNT; i++)
	{
		sprintf_s(ScoreText, sizeof(ScoreText), "%d", CD.PlantCDtime[i]);
		int x = 180 + (i + 1) * 65;//CD时间放置的位置
		int y = 3;
		outtextxy(x, y, ScoreText);//输出CD
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

//渲染植物卡牌
void DrawCards()
{
	for (int i = 0; i < ZHI_WU_COUNT; i++)
	{
		int x = 210 + i * 65;//植物卡牌放置的位置
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

//渲染植物
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
					//	PlayMusicCherrybomb();			在绘制函数里尽量只用绘制，因为他要缓存，很容易卡,ok
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

	//渲染 拖动过程中的植物
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
//渲染豌豆
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
//更新窗口
void UpdataWindow()
{
	BeginBatchDraw();//开始缓冲

	putimage(-112, 0, &ImgBg);//对应距离窗口左边和上边的距离
	putimagePNG(120, 0, &ImgBar);//透明底图
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

	DrawCards();//渲染植物卡牌
	DrawZhiWu();//渲染植物
	DrawCar();//draw car

	DrawSunshine();//渲染阳光

	DrawZM();//渲染僵尸

	DrawBullets();//渲染豌豆

	EndBatchDraw();//结束双缓冲
}


//收集阳光
void CollectSunshine(ExMessage* msg)
{
	int count = sizeof(balls) / sizeof(balls[0]);
	//阳光宽度
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
				//mciSendString("play res/sunshine.mp3", 0, 0, 0);//收集阳光时的音效.....缺点：播放完了才能播放下一个
				PlaySound("res/sunshine.wav", NULL, SND_FILENAME | SND_ASYNC);
				//设置阳光球的偏移量
				//float destY = 0;
				//float destX = 262;
				//float angle = atan((y - destY) / (x - destX));//点击时的阳光距离原点的正弦值
				//balls[i].xoff = 4 * cos(angle);
				//balls[i].yoff = 4 * sin(angle);//每次移动四个像素点
				balls[i].p1 = balls[i].pCur;
				balls[i].p4 = vector2(160, 0);
				balls[i].t = 0;
				float distance = dis(balls[i].p1 - balls[i].p4);
				float off = 8;//调节阳光飞回去的速度，但调大容易掉帧，不丝滑
				balls[i].speed = 1.0 / (distance / off);
			}
		}
	}
}

void UserClick()
{
	ExMessage msg;

	//判断当前有无消息，有消息返回为真
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


//创建阳光
void CreateSunshine()
{
	static int count = 0;
	static int fre = 400;
	count++;
	if (count >= fre)
	{
		fre = 200 + rand() % 200;
		count = 0;
		//从阳光池中取一个可以使用的阳光
		int ballMax = sizeof(balls) / sizeof(balls[0]);
		int i;
		for (i = 0; i < ballMax && balls[i].used; i++);
		if (i >= ballMax) return;

		balls[i].used = true;
		balls[i].FrameIndex = 0;
		//balls[i].x = 260 + rand() % (900 - 260);//260~900
		//balls[i].y = 60;
		//balls[i].destY = 200 + (rand() % 4) * 90; //阳光随机落在每一层
		balls[i].timer = 0;
		//balls[i].xoff = 0;
		//balls[i].yoff = 0;//初始化偏移量
		balls[i].status = SUNSHINE_DOWN;
		balls[i].t = 0;
		balls[i].p1 = vector2(260 - 112 + rand() % (900 - 260 + 112), 60);
		balls[i].p4 = vector2(balls[i].p1.x, 200 + (rand() % 4) % 90);
		int off = 2;
		float distance = balls[i].p4.y - balls[i].p1.y;
		balls[i].speed = 1.0 / (distance / off);
	}
	//向日葵生产阳光
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (map[i][j].type == XIANG_RI_KUI + 1)
			{
				map[i][j].timer++;//计时器
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

//更新阳光
void UpdataSunshine()
{
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++)
	{
		if (balls[i].used)
		{
			balls[i].FrameIndex = (balls[i].FrameIndex + 1) % 29;
			if (balls[i].status == SUNSHINE_DOWN)//阳光是垂直下降的状态
			{
				struct SunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);//更新当前位置
				if (sun->t >= 1)
				{
					sun->status = SUNSHINE_GROUND;
					sun->timer = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_GROUND)//阳光是落地的状态
			{
				balls[i].timer++;
				if (balls[i].timer > 100)//100ms后阳光消失
				{
					balls[i].used = false;
					balls[i].timer = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_COLLECT)//阳光是正在收集的状态
			{
				struct SunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);
				if (sun->t > 1)
				{
					sun->used = false;
					sunshine += 25;//收集到阳光后调整阳光值分数
				}
			}
			else if (balls[i].status == SUNSHINE_PRODUCT)//阳光是在生产的状态
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
			////降落到地面后启动定时器
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

//创建僵尸
void CreateZM()
{
	if (zmCount >= ZM_MAX) {
		return;
	}

	static int zmFre = 200;//僵尸初始频率
	static int count = 0;//函数调用的次数


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
			zms[i].speed = 2;//僵尸速度
			zms[i].blood = 200;//僵尸初始血量为100；
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

//更新僵尸
void UpdataZM()
{
	int zmMax = sizeof(zms) / sizeof(zms[0]);

	static int count = 0;
	count++;
	if (count > 2)
	{
		count = 0;
		//更新僵尸的位置
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
					//MessageBox(NULL, "over", "over", 0);//待优化
					//exit(0);//待优化
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
				//僵尸吃植物的动作
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

//发射子弹
void shoot()
{
	static int count = 0;
	if (++count < 2)return;
	count = 0;//定时器调节子弹发射频度

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
				//if (count2 > 20)//调节子弹射出频率
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


//更新豌豆子弹
void UpdataBullets()
{
	static int count = 0;
	if (++count < 2)return;
	count = 0;//定时器调节子弹的移速
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

			//待实现子弹的碰撞检测
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

//子弹对僵尸的碰撞检测
void CheckBullet_To_Zm()
{
	int bCount = sizeof(bullets) / sizeof(bullets[0]);
	int zCount = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0; i < bCount; i++)
	{
		if (bullets[i].used == false || bullets[i].blast)continue;
		for (int k = 0; k < zCount; k++)
		{
			if (zms[k].used == false || zms[k].color == 2)continue;//标记
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
		if (zms[k].color != 2 || zms[k].dead == true)continue;//标记
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
//		if (zms[k].color != 2)continue;//标记
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

//僵尸对植物的碰撞检测
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

int ChompMusicOpen = 0;//吃东西音效标记
//吃东西检测
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

int GragenboatMusicOpen[sizeof(dragon_boat) / sizeof(dragon_boat[0])] = { 0 };//龙舟音效标记
//龙舟开动检测
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


//碰撞检测
void CollisiomCheck()
{
	CheckBullet_To_Zm(); //子弹对僵尸的碰撞检测
	CheckZM_To_ZhiWu();//僵尸对植物的碰撞检测
	Checkzmpink_To_Zm();//check if pink zm eats zm and zm eats pink zm 
	EatingMusic_chaeck();//吃东西检测
	GragenboatGO_Check();//龙舟开动检测
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

//更新游戏
void UpdataGame()
{

	UndataZhiWu();

	CreateSunshine();//创建阳光
	UpdataSunshine();//更新阳光状态



	CreateZM();//创建僵尸
	UpdataZM();//更新僵尸的状态

	shoot();//发射豌豆子弹
	UpdataBullets();//更新豌豆子弹
	CollisiomCheck();//实现豌豆子弹的碰撞检测  check should be earlier than update zm !

}

int Isview = 0;
//启动菜单
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

//开场巡视
void ViewScence()
{
	int xMin = WIN_WIDTH - ImgBg.getwidth();//900-1400=-500
	vector2 points[9] = { {550,80 },{530,160},{630,170},{530,200},
						 {515,270},{565,370},{605,340},{705,280},{690,340} };//初始的僵尸所在位置

	int index[9];
	for (int i = 0; i < 9; i++)
	{
		index[i] = rand() % 11;//设置随机的起始帧序号
	}

	int count = 0;
	for (int x = 0; x >= xMin; x -= 2)
	{
		BeginBatchDraw();//缓冲,防止抖动
		putimage(x, 0, &ImgBg);//渲染背景图片，每两毫秒移动2像素

		count++;
		for (int k = 0; k < 9; k++)
		{
			putimagePNG(points[k].x - xMin + x, points[k].y, &imgZmStand[0]);
			if (count >= 10)
			{
				index[k] = (index[k] + 1) % 11;//每十次就切换一帧
			}
		}
		if (count >= 10)count = 0;

		EndBatchDraw();//结束缓冲
		Sleep(5);
	}

	//停留1s左右
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

	//史蒂夫说话
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

	//史蒂夫摇晃
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

	//史蒂夫被抓
	for (int i = 0; i < 56; i++)
	{
		if (i == 33)PlayMusic_scream();//steve尖叫
		BeginBatchDraw();
		putimage(-112, 0, &ImgBg);
		putimagePNG(0, 0, &imgSteve3[i]);
		// printf("%d", i);
		EndBatchDraw();
		Sleep(80);
	}
	Sleep(200);
}

//卡牌槽掉落
void BarsDown()
{
	int height = ImgBar.getheight();
	for (int y = -height; y <= 0; y++) {
		BeginBatchDraw();//开始缓冲

		putimage(-112, 0, &ImgBg);
		putimagePNG(120, y, &ImgBar);
		putimagePNG(750, y + 20, &imgshovelslot);

		for (int i = 0; i < ZHI_WU_COUNT; i++)
		{
			int x = 210 + i * 65;
			putimage(x, 6 + y, &ImgCards[i]);
			putimagePNG(750, 26 + y, &imgshovel);
		}

		EndBatchDraw();//结束缓冲
		Sleep(5);
	}
}

//检查是否胜利
bool CheckOver()
{
	int ret = false;
	if (gameStatus == WIN) {
		Sleep(2000);
		playMusicWin();
		loadimage(0, "res/win2.png");
		mciSendString("close res/bg.mp3", NULL, NULL, NULL);//关闭背景音乐
		ret = true;
	}
	else if (gameStatus == FAIL) {
		Sleep(2000);
		playMusicLose();
		loadimage(0, "res/ZombiesAteYourBrains.png");
		mciSendString("close res/bg.mp3", NULL, NULL, NULL);//关闭背景音乐
		ret = true;
	}
	return ret;
}

//时间检测函数
void Time_Check()
{
	if (CD.TimeFlag)
	{
		for (int i = 0; i < ZHI_WU_COUNT; i++)
		{
			if (CD.PlantCDtime[i] > 0) CD.PlantCDtime[i]--; //若冷却时间大于0，则对应植物的冷却时间自减
		}
		CD.TimeFlag = 0;
	}

	//检测开场音乐
	if (TotalCount == 8)
	{
		playMusicComing();
	}

	//检测僵尸高潮
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
						zms[i].speed = 2;//僵尸速度
						zms[i].blood = 200;//僵尸初始血量为100；
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
	// 记录上次播放音乐的时间
	//DWORD lastPlayTime = 0;

	GameInit();//游戏初始化

	playMusicStart();

	StartUI();//开始界面

	//是否跳过开场巡视，0不调过，1跳过
	if (Isview == 0)
	{
		ViewScence();//开场巡视
	}


	BarsDown();//卡牌掉落

	int timer = 0;
	int flag = true;
	long double lasttime = 0;//上一次计数的时间

	while (1)
	{
		//总时间戳
		time_t current_time;

		time(&current_time);
		if (lasttime != current_time)
		{
			//printf("执行一次%ld\n", TotalCount);
			TotalCount++;
			CD.TimeFlag = 1;
			TotalTimeFlag = 1;
		}
		lasttime = current_time;//保留上次时间
		Time_Check();//执行时间检测

		UserClick();//鼠标使用检测

		timer += getDelay();
		if (timer > 30)//定时器，每30毫秒执行一次
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

//steve哇比博布
void PlayMusicSteveSayshort1() {
	mciSendString("open res/audio/crazydaveshort1.mp3", NULL, NULL, NULL);
	mciSendString("play res/audio/crazydaveshort1.mp3", NULL, NULL, NULL);
}

//steve略比略比
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