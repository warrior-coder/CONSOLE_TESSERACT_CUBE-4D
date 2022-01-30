#include <stdio.h>
#include <windows.h>
#include <math.h>
#pragma warning(disable : 4996) // отключаем предупреждения о небезопасных функциях


// определяем размеры консоли
#define CON_WIDTH 96
#define CON_HEIGHT 64

// определяем свойства фигуры
#define F_VERTEX_COUNT 16U
#define F_EDGE_COUNT 32U
#define F_EDGE_LENGTH 22.0f

// определяем расстояния до камеры
#define CAMERA_Z 80.0f
#define CAMERA_W 25.0f


// структуры координат на плоскости, в пространстве и в гиперпространстве
typedef struct _FLOAT_PAIR
{
	float x, y;
}FLOAT_PAIR;

typedef struct _FLOAT_TRIPLE
{
	float x, y, z;
}FLOAT_TRIPLE;

typedef struct _FLOAT_QUAD
{
	float x, y, z, w;
}FLOAT_QUAD;

// структура, хранящая индексы начала и конца линии
typedef struct _LINE_PATH_INDEX
{
	int begin, end;
}LINE_PATH_INDEX;


// объявляем глобальные переменные
FLOAT_PAIR vertexes2[F_VERTEX_COUNT];
FLOAT_QUAD vertexes4[F_VERTEX_COUNT] = {
	{ -1, -1, -1, -1 }, {  1, -1, -1, -1 }, {  1,  1, -1, -1 }, { -1,  1, -1, -1 }, { -1, -1,  1, -1 }, {  1, -1,  1, -1 }, {  1,  1,  1, -1 }, { -1,  1,  1, -1 },
	{ -1, -1, -1,  1 }, {  1, -1, -1,  1 }, {  1,  1, -1,  1 }, { -1,  1, -1,  1 }, { -1, -1,  1,  1 }, {  1, -1,  1,  1 }, {  1,  1,  1,  1 }, { -1,  1,  1,  1 }
};
LINE_PATH_INDEX indexes[F_EDGE_COUNT] = {
	{  0,  1 }, {  1,  2 }, {  2,  3 }, {  3,  0 }, {  4,  5 }, {  5,  6 }, {  6,  7 }, {  7,  4 }, {  0,  4 }, {  1,  5 }, {  2,  6 }, {  3,  7 },
	{  8,  9 }, {  9, 10 }, { 10, 11 }, { 11,  8 }, { 12, 13 }, { 13, 14 }, { 14, 15 }, { 15, 12 }, {  8, 12 }, {  9, 13 }, { 10, 14 }, { 11, 15 },
	{  0,  8 }, {  1,  9 }, {  2, 10 }, {  3, 11 }, {  4, 12 }, {  5, 13 }, {  6, 14 }, {  7, 15 }
};
char buffer[CON_WIDTH * CON_HEIGHT];
HANDLE hout = NULL;
COORD coord = { 0, 0 };


// прототипы функций
void consoleInit();
void figrueInit();
void figureRotate(float, float);
FLOAT_PAIR perspectiveProject(FLOAT_QUAD*);
void bufferDrawLine(float, float, float, float);
void bufferDrawFigure();
void bufferClear();
void bufferShow();


// точка входа консольного приложения 
int main()
{
	const float angleXZ = 0.8f / 180.0f * 3.14159f;
	const float angleYW = 1.0f / 180.0f * 3.14159f;

	consoleInit();
	figrueInit();

	// основной цикл программы
	while (TRUE)
	{
		bufferClear(); // очищаем буфер
		figureRotate(angleXZ, angleYW); // вращаем фигуру
		bufferDrawFigure(); // рисуем фигуру в буфер
		bufferShow(); // выводим буфер в консоль
		
		Sleep(4);
	}

	return 0;
}


// реализация функций
void consoleInit()
{
	// устанавливаем шрифт консоли
	CONSOLE_FONT_INFOEX cfi = {
		sizeof(CONSOLE_FONT_INFOEX),
		0,
		{ 8, 8 },
		0,
		FW_BOLD,
		L"Consolas"
	};

	hout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetCurrentConsoleFontEx(hout, FALSE, &cfi);

	// устанавливаем режим консоли
	char command[128];

	sprintf(command, "mode con cols=%d lines=%d & title Console Tesseract & color 09", CON_WIDTH, CON_HEIGHT);
	system(command);
}

void figrueInit()
{
	// масштабируем фигуру
	for (int i = 0; i < F_VERTEX_COUNT; i++)
	{
		vertexes4[i].x *= F_EDGE_LENGTH / 2.0f;
		vertexes4[i].y *= F_EDGE_LENGTH / 2.0f;
		vertexes4[i].z *= F_EDGE_LENGTH / 2.0f;
		vertexes4[i].w *= F_EDGE_LENGTH / 2.0f;
	}
}

void figureRotate(float angleXZ, float angleYW)
{
	FLOAT_QUAD v4;

	for (int i = 0; i < F_VERTEX_COUNT; i++)
	{
		v4 = vertexes4[i];

		// вращение в плоскости XZ
		vertexes4[i].x = cos(angleXZ) * v4.x - sin(angleXZ) * v4.z;
		vertexes4[i].z = sin(angleXZ) * v4.x + cos(angleXZ) * v4.z;

		// вращение в плоскости YW
		vertexes4[i].y = cos(angleYW) * v4.y - sin(angleYW) * v4.w;
		vertexes4[i].w = sin(angleYW) * v4.y + cos(angleYW) * v4.w;
	}
}

FLOAT_PAIR perspectiveProject(FLOAT_QUAD* v4)
{
	FLOAT_TRIPLE v3;
	FLOAT_PAIR v2;
	float k;

	// проецируем на пространство
	k = CAMERA_W / (CAMERA_W + v4->w);
	v3.x = v4->x * k;
	v3.y = v4->y * k;
	v3.z = v4->z * k;

	// проецируем на плоскость
	k = CAMERA_Z / (CAMERA_Z + v3.z);
	v2.x = v3.x * k;
	v2.y = v3.y * k;

	// смещаем начало координат в центр консоли
	v2.x += CON_WIDTH / 2.0f;
	v2.y += CON_HEIGHT / 2.0f;

	return v2;
}

void bufferDrawLine(float x1, float y1, float x2, float y2)
{
	float d = fabsf(x2 - x1) > fabsf(y2 - y1) ? fabsf(x2 - x1) : fabsf(y2 - y1);
	float dx = (x2 - x1) / d;
	float dy = (y2 - y1) / d;
	int x, y;

	for (int i = 0; i < d; i++, x1 += dx, y1 += dy)
	{
		if ((x = (int)x1) >= 0 && (y = (int)y1) >= 0 && x < CON_WIDTH && y < CON_HEIGHT)
		{
			buffer[y * CON_WIDTH + x] = '@';
		}
	}
}

void bufferDrawFigure()
{
	// проецируем вершины гиперпространства на плоскость консоли
	for (int i = 0; i < F_VERTEX_COUNT; i++)
	{
		vertexes2[i] = perspectiveProject(vertexes4 + i);
	}

	// рисуем ребра гиперкуба
	for (int i = 0; i < F_EDGE_COUNT; i++)
	{
		bufferDrawLine(
			vertexes2[indexes[i].begin].x,
			vertexes2[indexes[i].begin].y,
			vertexes2[indexes[i].end].x,
			vertexes2[indexes[i].end].y
		);
	}
}

void bufferClear()
{
	memset(buffer, ' ', CON_WIDTH * CON_HEIGHT);
}

void bufferShow()
{
	SetConsoleCursorPosition(hout, coord);
	fwrite(buffer, sizeof(char), CON_WIDTH * CON_HEIGHT, stdout);
}