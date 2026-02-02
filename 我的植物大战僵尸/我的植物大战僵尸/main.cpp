#include<stdio.h>
#include<graphics.h>
#include<time.h>
#include<math.h>
#include"tools.h"
#include"vector2.h"

#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")

#define WIN_WIDTH 900
#define WIN_HIGHT 600
#define ZM_MAX 10

enum {WAN_DOU,XIANG_RI_KUI,DA_ZUI_HUA,ZHI_WU_COUST};

IMAGE imgBg;
IMAGE imgBar;
IMAGE imgCards[ZHI_WU_COUST];
IMAGE* imgZhiWu[ZHI_WU_COUST][20];


int curX,curY;//当前选中的植物，在移动过程中的位置
int curZhiWu;//  0:没有选中，1：选择了第一种植物

enum {GOING,WIN,FAIL};
int killCount;	//已经死亡的僵尸个数
int zmCount;	//已经出现的僵尸个数
int gameStauts;

struct zhiwu {
	int type;//  0:没有植物，1：选择了第一种植物
	int frameindex;//序列帧的序号

	bool catched;//是否被僵尸捕获
	int deadTime;//死亡计数器

	int timer;
	int x, y;

	int shootTime;
};

struct zhiwu map[3][9];

enum{SUNSHINE_DOWN,SUNSHINE_GROUND,SUNSHINE_COLLECT,SUNSHINE_PRODUCT};


struct sunshineBall {
	int x, y;//阳光球在飘落过程中的坐标
	int frameindex;//当前显示的图片帧的序号
	int destY;//飘落目标位置的y坐标
	bool used;//是否使用
	int timer;

	float xoff;
	float yoff;

	float t;//贝塞尔曲线时间点0..1
	vector2 p1, p2, p3, p4;
	vector2 pCur;//当前时刻阳光球位置
	float speed;
	int status;//当前的状态

};

//池
struct sunshineBall balls[10];
IMAGE imgSunshineBall[29];
int sunshine;

struct zm {
	int x, y;
	int frameindex;
	bool used;
	int speed;
	int row;
	int blood;
	bool dead;
	bool eating;//是否吃植物
};
struct zm zms[10];
IMAGE imgZM[22];
IMAGE imgZMDead[20];
IMAGE imgZMEat[21];

//子弹数据类型
struct bullet {
	int x, y;
	int row;
	bool used;
	int speed;
	bool blast; //是否发生爆炸
	int frameIndex;//帧序号
};
struct bullet bullets[30];
IMAGE imgBulletNormal;
IMAGE imgBullBlast[4];
IMAGE imgZmStand[11];

bool fileExist(const char* name) {
	FILE* fp = fopen(name, "r");
	if (fp == NULL) {
		return false;
	}
	else {
		fclose(fp);
		return true;
	}
}

void gameInit() {
	//加载背景图片
	loadimage(&imgBg, "res/bg.jpg");
	loadimage(&imgBar, "res/bar5.png");

	memset(imgZhiWu, 0, sizeof(imgZhiWu));
	memset(map, 0, sizeof(map));

	killCount = 0;
	zmCount = 0;
	gameStauts = GOING;

	//初始化植物卡牌
	char name[64];
	for (int i = 0; i < ZHI_WU_COUST; i++) {
		//生成植物卡牌文件名
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&imgCards[i], name);

		for (int j = 0; j < 20; j++) {
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j+1);
			//先判断文件是否存在
			if (fileExist(name)) {
				imgZhiWu[i][j] = new IMAGE;//c++
				loadimage(imgZhiWu[i][j], name);
			}
			else
			{
				break;
			}
		}
	}
	curZhiWu = 0;
	sunshine = 50;

	memset(balls, 0, sizeof(balls));
	for (int i = 0; i < 29; i++) {
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		loadimage(&imgSunshineBall[i], name);
	}

	//配置随机种子
	srand(time(NULL));

	//创建游戏窗口
	initgraph(WIN_WIDTH, WIN_HIGHT,1);

	//设置字体
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWeight = 15;
	strcpy( f.lfFaceName, "Segoe UI Black");
	f.lfQuality = ANTIALIASED_QUALITY;//抗锯齿效果
	settextstyle(&f);
	setbkmode(TRANSPARENT);
	setcolor(BLACK);

	//初始化僵尸数据
	memset(zms, 0, sizeof(zms));
	for (int i = 0; i < 22; i++) {
		sprintf_s(name,sizeof(name), "res/zm/%d.png", i + 1);
		loadimage(&imgZM[i], name);
	}

	loadimage(&imgBulletNormal, "res/bullets/bullet_normal.png");
	memset(bullets, 0, sizeof(bullets));

	//初始化豌豆子弹的帧图片数组
	loadimage(&imgBullBlast[3], "res/bullets/bullet_blast.png");
	for (int i = 0; i < 3; i++) {
		float k = (i + 1) * 0.2;
		loadimage(&imgBullBlast[i], "res/bullets/bullet_blast.png", 
			imgBullBlast[3].getwidth() * k ,
			imgBullBlast[3].getheight() * k , true);

	}

	for (int i = 0; i < 10; i++) {
		sprintf_s(name, sizeof(name), "res/zm_dead/%d.png", i + 1);
		loadimage(&imgZMDead[i], name);
	
	}

	for (int i = 0; i < 21; i++) {
		sprintf_s(name, sizeof(name), "res/zm_eat/%d.png", i + 1);
		loadimage(&imgZMEat[i], name);
	}

	for (int i = 0; i < 11; i++) {
		sprintf_s(name, sizeof(name), "res/zm_stand/%d.png", i + 1);
		loadimage(&imgZmStand[i], name);
	}

}

void drawZM() {
	int zmCount = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0; i < zmCount; i++) {
		if (zms[i].used) {
			/*IMAGE* img = &imgZM[zms[i].frameindex];*/
			/*IMAGE* img = (zms[i].dead) ? imgZMDead : imgZM;*/
			IMAGE* img = NULL;
			if (zms[i].dead)img = imgZMDead;
			else if (zms[i].eating)img = imgZMEat;
			else img = imgZM;

			img += zms[i].frameindex;

			putimagePNG(zms[i].x, zms[i].y - img->getheight(), img);
		}
	}
}

void drawSunshine() {
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++) {
		/*if (balls[i].used || balls[i].xoff) {*/
		if (balls[i].used ) {
			
			IMAGE* img = &imgSunshineBall[balls[i].frameindex];
			/*putimagePNG(balls[i].x, balls[i].y, img);*/
			putimagePNG(balls[i].pCur.x, balls[i].pCur.y, img);
		}
	}

	char scoreText[8];
	sprintf_s(scoreText, sizeof(scoreText), "%d", sunshine);
	outtextxy(182, 67, scoreText);//输出分数
}

void drawCards() {
	for (int i = 0; i < ZHI_WU_COUST; i++) {
		int x = 238 + i * 65;
		int y = 6;
		putimage(x, y, &imgCards[i]);
	}
}

void drawZhiWu() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				//int x = 256 + j * 81;
				//int y = 179 + i * 102 + 7;
				int zhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameindex;
				putimagePNG(map[i][j].x, map[i][j].y, imgZhiWu[zhiWuType][index]);

			}
		}
	}

	//渲染拖动过程中的植物
	if (curZhiWu > 0) {
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);
	}
}

void drawBullets() {
	int bulletMax = sizeof(bullets) / sizeof(bullets[0]);
	for (int i = 0; i < bulletMax; i++) {
		if (bullets[i].used) {
			if (bullets[i].blast) {
				IMAGE* img = &imgBullBlast[bullets[i].frameIndex];
				putimagePNG(bullets[i].x, bullets[i].y, img);

			}
			else
			{
				putimagePNG(bullets[i].x, bullets[i].y, &imgBulletNormal);

			}
		}//生成子弹
	}
}

void updateWindow() {
	BeginBatchDraw();//开始缓冲

	putimage(-112, 0, &imgBg);
	putimagePNG(150, 0, &imgBar);

	drawCards();
	drawZhiWu();
	drawSunshine();
	drawZM();
	drawBullets();

	EndBatchDraw();//结束双缓冲
}

void collectSunshine(ExMessage* msg) {
	int count = sizeof(balls) / sizeof(balls[0]);
	int w = imgSunshineBall[0].getwidth();
	int h = imgSunshineBall[0].getheight();
	for (int i = 0; i < count; i++) {
		if (balls[i].used)
		{
			//int x = balls[i].x;
			//int y = balls[i].y;
			int x = balls[i].pCur.x;
			int y = balls[i].pCur.y;

			if (msg->x > x && msg->x < x + w &&
				msg->y > y && msg->y < y + h) {
				//balls[i].used = false;
				balls[i].status = SUNSHINE_COLLECT;
				//sunshine += 25;
				//mciSendString("play res/sunshine.mp3", 0, 0, 0);
				PlaySound("res/sunshine.wav", NULL, SND_FILENAME | SND_ASYNC);

				//设置阳光球偏移量
				//float destY = 0;
				//float destX = 162;
				//float angle = atan((y - destY) / (x - destX));
				//balls[i].xoff = 4 * cos(angle);
				//balls[i].yoff = 4 * sin(angle);
				balls[i].p1 = balls[i].pCur;
				balls[i].p4 = vector2(162, 0);
				balls[i].t = 0;
				float distance = dis(balls[i].p1 - balls[i].p4);
				float off = 8;
				balls[i].speed = 1.0 / (distance / off);
				break;
			}
		}
		

	}
}

void userClick() {
	ExMessage msg;
	static int status = 0;
	if (peekmessage(&msg)) {
		if (msg.message == WM_LBUTTONDOWN) {
			if (msg.x > 238 && msg.x< 238 + 65*ZHI_WU_COUST && msg.y < 96)
			{
				int index = (msg.x - 238) / 65;
				status = 1;
				curZhiWu = index + 1;
			}
			else {
				collectSunshine(&msg);
			}
		}
		else if(msg.message == WM_MOUSEMOVE && status ==1){
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP && status == 1) {
			if (msg.x > 256-112 && msg.y > 179 && msg.y < 489) {
				int row = (msg.y - 179) /102;
				int col = (msg.x - 256+112)/ 81;
				
				if (map[row][col].type == 0) 
				{
					map[row][col].type = curZhiWu;
					map[row][col].frameindex = 0;
					map[row][col].shootTime = 0;

					map[row][col].x = 256-112 + col * 81;
					map[row][col].y= 179 + row * 102 + 7;


				}
				

			}

			curZhiWu = 0;
			status = 0;
		}
	}
}

void createSunshine() {
	static int count = 0;
	static int fre = 400;
	count++;
	if(count>=fre){
		fre = 200 + rand() % 200;
		count = 0;
		//从阳光池中取一个可以使用的阳光
	int ballMax = sizeof(balls) / sizeof(balls[0]);

	int i;
	for (i = 0; i < ballMax && balls[i].used; i++);
	if (i >= ballMax)return;

	balls[i].used = true;
	balls[i].frameindex = 0;
	//balls[i].x =160 + rand() % (900 - 160);
	//balls[i].y = 60;
	//balls[i].destY = 200 + (rand() % 4) * 90;
	balls[i].timer = 0;
	//balls[i].xoff = 0;
	//balls[i].yoff = 0;
	balls[i].status = SUNSHINE_DOWN;
	balls[i].t = 0;
	balls[i].p1 = vector2(160-112 + rand() % (900 - 160+112), 60);
	balls[i].p4 = vector2(balls[i].p1.x, 200 + (rand() % 4) * 90);
	int off = 2;
	float distance = balls[i].p4.y - balls[i].p1.y;
	balls[i].speed = 1.0 / (distance / off);
	}
	//向日葵生产阳光
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type==XIANG_RI_KUI+1){
				map[i][j].timer++;
				if (map[i][j].timer > 200) {
					map[i][j].timer = 0;
					int k;
					for ( k = 0; k < ballMax && balls[k].used; k++);
					if (k >= ballMax)return;
					balls[k].used = true;
					balls[k].p1 = vector2(map[i][j].x, map[i][j].y);
					int w = (100 + rand() % 50) * (rand() % 2 ? 1 : -1);
					balls[k].p4 = vector2(map[i][j].x + w, 
						map[i][j].y + imgZhiWu[XIANG_RI_KUI][0]->getheight() - 
						imgSunshineBall[0].getheight());
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




void updateSunshine() {
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {
			balls[i].frameindex = (balls[i].frameindex + 1) % 29;
			if (balls[i].status==SUNSHINE_DOWN){
				struct sunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);
				if (sun->t >= 1) {
					sun->status = SUNSHINE_GROUND;
					sun->timer = 0;
				}
				
			}
			else if (balls[i].status == SUNSHINE_GROUND) {
				balls[i].timer++;
				if (balls[i].timer > 100) {
					balls[i].used = false;
					balls[i].timer = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_COLLECT) {
				struct sunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);
				if (sun->t > 1) {
					sun->used = false;
					sunshine += 25;
				}

			}
			else if (balls[i].status == SUNSHINE_PRODUCT) {
				struct sunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = calcBezierPoint(sun->t, sun->p1, sun->p2, sun->p3, sun->p4);
				if (sun->t > 1) {
					sun->status = SUNSHINE_GROUND;
					sun->timer = 0;
				}
			}



			//balls[i].frameindex = (balls[i].frameindex + 1) % 29;
			//if (balls[i].timer == 0) {
			//	balls[i].y += 2;
			//}	
			//if (balls[i].y >= balls[i].destY) {
			//	//balls[i].used = false;
			//	balls[i].timer++;
			//	if (balls[i].timer > 100) {
			//		balls[i].used = false;
			//	}
			//}
		}
		//else if (balls[i].xoff) {
		//	float destY = 0;
		//	float destX = 162;
		//	float angle = atan((balls[i].y - destY) / (balls[i].x - destX));
		//	balls[i].xoff = 4 * cos(angle);
		//	balls[i].yoff = 4 * sin(angle);
		//	balls[i].x -= balls[i].xoff;
		//	balls[i].y -= balls[i].yoff;
		//	if (balls[i].y < 0 || balls[i].x < 162) {
		//		balls[i].xoff = 0;
		//		balls[i].yoff = 0;
		//		sunshine += 25;
		//	}
		//}
	}
}

void createZM() {
	if (zmCount>=ZM_MAX)
	{
		return;
	}
	static int zmFre = 200;
	static int count = 0;
	count++;
	if (count > zmFre) {
		count = 0;
		zmFre = rand() % 200 + 300;

		int i;
		int zmMax = sizeof(zms) / sizeof(zms[0]);
		for (i = 0; i < zmMax && zms[i].used; i++);
		if (i < zmMax) {
			memset(&zms[i], 0, sizeof(zms[i]));
			zms[i].used = true;
			zms[i].x = WIN_WIDTH;
			zms[i].row = rand() % 3;
			zms[i].y = 172 + (1 + zms[i].row) * 100;
			zms[i].speed = 1;
			zms[i].blood = 100;
			zms[i].dead = false;//add
			zms[i].eating = false;
			zmCount++;
		}
		else {
			printf("创建僵尸失败！\n");
		}

	}
	
}

void updateZM() {
	int zmMax = sizeof(zms) / sizeof(zms[0]);

	static int count = 0;
	count++;
	if (count > 2*2) {
		count = 0;
		//更新僵尸位置
		for (int i = 0; i < zmMax; i++) {
			if (zms[i].used) {
				zms[i].x -= zms[i].speed;
				if (zms[i].x < 56) {
					//printf("GAME OVER\n");
					//MessageBox(NULL, "over", "over", 0);//待优化
					//exit(0);//待优化

					gameStauts = FAIL;
				}
			}
		}
	}

	static int count2 = 0;
	count2++;
	if (count2 > 4*2) {
		count2 = 0;
		for (int i = 0; i < zmMax; i++) {
			if (zms[i].used) {
				if (zms[i].dead) {
					zms[i].frameindex++;
					if (zms[i].frameindex >= 20) {
						zms[i].used = false;
						killCount++;
						if (killCount==ZM_MAX)
						{
							gameStauts = WIN;
						}
					}
				}
				else if (zms[i].eating) {
					zms[i].frameindex = (zms[i].frameindex + 1) % 21;
				}
				else {
					zms[i].frameindex = (zms[i].frameindex + 1) % 22;
				}
			}
		}
	}
}

void shoot() {
	static int count = 0;
	if (++count < 3)return;
	count = 0;

	int lines[3] = { 0 };
	int zmCount = sizeof(zms) / sizeof(zms[0]);
	int bulletMax = sizeof(bullets) / sizeof(bullets[0]);
	int dangerX = WIN_WIDTH - imgZM[0].getwidth()/3;
	for(int i= 0; i < zmCount; i++){
		if (zms[i].used && zms[i].x < dangerX) {
			lines[zms[i].row] = 1;
		}
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type == WAN_DOU + 1 && lines[i]) {
				//static int count2 = 0;
				//count2++;
				map[i][j].shootTime++;
				if (map[i][j].shootTime >20){
					map[i][j].shootTime = 0;

					int k;
					for (k = 0; k < bulletMax && bullets[k].used; k++);
						if (k < bulletMax) {
							bullets[k].used = true;
							bullets[k].row = i;
							bullets[k].speed = 4;

							//int x = 256 + j * 81;
							//int y = 179 + i * 102 + 7;

							bullets[k].blast = false;
							bullets[k].frameIndex = 0;

							int zwX = 256-112 + j * 81;
							int zwY = 179 + i * 102 + 7;
							bullets[k].x = zwX + imgZhiWu[map[i][j].type - 1][0]->getwidth()-10;
							bullets[k].y = zwY + 5;

						}
					
				}
			}
		}
	}
}


void updateBullets() {
	static int count = 0;
	if (++count < 2)return;
	count = 0;

	int countMax = sizeof(bullets) / sizeof(bullets[0]);
	for (int i = 0; i < countMax; i++) {
		if (bullets[i].used) {
			bullets[i].x += bullets[i].speed;
			if (bullets[i].x > WIN_WIDTH) {
				bullets[i].used = false;
			}

			//待实现子弹的碰撞检测
			if (bullets[i].blast) {
				bullets[i].frameIndex++;
				if (bullets[i].frameIndex >= 4) {
					bullets[i].used = false;
				}

			}
		}
	}
}

void checkBullet2Zm() {
	int bCount = sizeof(bullets) / sizeof(bullets[0]);
	int zCount = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0; i < bCount; i++) {
		if (bullets[i].used == false || bullets[i].blast)continue;
		for (int k = 0; k < zCount; k++) {
			if (zms[k].used == false)continue;
			int x1 = zms[k].x + 80;
			int x2 = zms[k].x + 110;
			int x = bullets[i].x;
			if (zms[k].dead == false && bullets[i].row == zms[k].row && x > x1 && x < x2) {
				zms[k].blood -= 5;
				bullets[i].blast = true;
				bullets[i].speed = 0;

				if (zms[k].blood <= 0) {
					zms[k].dead = true;
					zms[k].speed = 0;
					zms[k].frameindex = 0;
				}

				break;
			}
		}
	}

}

void checkZm2ZhiWu() {
	int zmCount = sizeof(zms)/sizeof(zms[0]);
	for (int i = 0; i < zmCount; i++) {
		if (zms[i].dead)continue;

		int row = zms[i].row;
		for (int k = 0; k < 9; k++) {
			if (map[row][k].type == 0)continue;
			
			int zhiWuX = 256-112 + k * 81;
			int x1 = zhiWuX + 10;
			int x2 = zhiWuX + 60;
			int x3 = zms[i].x + 80;
			if (x3 > x1 && x3 < x2) {
				if (map[row][k].catched) {
					/*zms[i].frameindex++;*/
					map[row][k].deadTime++;
					if (map[row][k].deadTime > 100) {
						map[row][k].deadTime = 0;
						map[row][k].type = 0;
						zms[i].eating = false;
						zms[i].frameindex = 0;
						zms[i].speed = 1;
					}
				}
				else {
					map[row][k].catched = true;
					map[row][k].deadTime = 0;
					zms[i].eating = true;
					zms[i].speed = 0;
					zms[i].frameindex = 0;

				}
			}
		}
	}
}

void collisionCheck() {
	checkBullet2Zm();//子弹对僵尸的碰撞检测
	checkZm2ZhiWu();//僵尸对植物的碰撞检测
	
}

void updateZhiWu() {
	static int count = 0;
	if (++count < 2)return;
	count = 0;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				map[i][j].frameindex++;
				int zhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameindex;
				if (imgZhiWu[zhiWuType][index] == NULL) {
					map[i][j].frameindex = 0;
				}
			}
		}
	}
}

void updateGame() {
	updateZhiWu();

	createSunshine();//创建阳光
	updateSunshine();//更新阳光

	createZM();//创建僵尸
	updateZM();//更新僵尸的状态
	shoot();//发射子弹
	updateBullets();//更新子弹

	collisionCheck();//实现豌豆与僵尸碰撞检测

}

void startUI() {
	IMAGE imgBj, imgMenu1, imgMenu2;
	loadimage(&imgBj, "res/menu.png");
	loadimage(&imgMenu1, "res/menu1.png");
	loadimage(&imgMenu2, "res/menu2.png");
	int flag = 0;

	while (1) {
		BeginBatchDraw();
		putimage(0, 0, &imgBj);
		putimagePNG(474, 75, flag ? &imgMenu2 : &imgMenu1);

		ExMessage msg;
		if (peekmessage(&msg)) {
			if (msg.message == WM_LBUTTONDOWN &&
				msg.x > 474 && msg.x < 474 + 300 &&
				msg.y > 75 && msg.y < 75 + 140)
			{
				flag = 1;

			}
			else if (msg.message == WM_LBUTTONUP && flag == 1) {
				EndBatchDraw();
				break;
			}
		}
		EndBatchDraw();
	}
}

void viewScence() {
	int xMin = WIN_WIDTH - imgBg.getwidth();
	vector2 points[9] = {
		{530, 80},{530,160},{630,170},{530,200},{515,270},
		{565,370},{605,340},{705,280},{690,340} };
	int index[9];
	for (int i = 0; i < 9; i++)
	{
		index[i] = rand() % 11;
	}

	int count = 0;
	for (int x = 0; x >= xMin; x--) {
		BeginBatchDraw();
		putimage(x, 0, &imgBg);
		 
		count++;
		for (int k = 0; k < 9; k++){
			putimagePNG(points[k].x - xMin + x,
				points[k].y,
				&imgZmStand[index[k]]);
			if (count >= 10) {
				index[k] = (index[k] + 1) % 11;
			}
		}
		if (count >= 10)count = 0;

		EndBatchDraw();
		Sleep(5);
	}

	//停留1S左右
	for (int i = 0; i < 100; i++)
	{
		BeginBatchDraw();

		putimage(xMin, 0, &imgBg);
		for (int k = 0; k < 9; k++)
		{
			putimagePNG(points[k].x, points[k].y, &imgZmStand[index[k]]);
			index[k] = (index[k] + 1) % 11;
		}

		EndBatchDraw();
		Sleep(30);
	}

	for (int x = xMin; x <= -112; x += 2) {
		BeginBatchDraw();

		putimage(x, 0, &imgBg);

		count++;
		for (int k = 0; k < 9; k++)
		{
			putimagePNG(points[k].x - xMin + x, points[k].y, &imgZmStand[index[k]]);
			if (count>=10)
			{
				index[k] = (index[k] + 1) % 11;
			}
			if (count >= 10)count = 0;
		}

		EndBatchDraw();
		Sleep(5);
	}

}

void barsDown() {
	int height = imgBar.getheight();

	for (int y = -height; y <= 0; y++) {
		BeginBatchDraw();

		putimage(-112, 0, &imgBg);
		putimagePNG(150, y, &imgBar);

		for (int i = 0; i < ZHI_WU_COUST; i++) {
			int x = 238 + i * 65;
			
			putimage(x, 6+y, &imgCards[i]);
		}

		EndBatchDraw();
		Sleep(10);
	}
}

bool checkOver() {
	int ret = false;
	if (gameStauts==WIN)
	{
		Sleep(2000); 
		loadimage(0, "res/win.png");
		ret = true;
	}
	else if (gameStauts==FAIL)
	{
		Sleep(2000);
		loadimage(0, "res/fail2.png");
		ret = true;
	}
	return ret;
}

int main(void) {
	gameInit();

	startUI();
	viewScence();

	barsDown();

	int timer = 0;
	bool flag = true;
	while (1) {
		userClick();
		timer += getDelay();
		if (timer > 10) {
			flag = true;
			timer = 0;
		}

		if (flag){
			flag = false;
			updateWindow();
			updateGame();
			if(checkOver())break;
		}
	}

	system("pause");
	return 0;

}