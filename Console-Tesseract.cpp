#include <stdio.h>
#include <windows.h>
#include <cmath>

#define C_VERT static_cast<char>(219)
#define C_EDGE '#'
#define C_CLEAR ' '

#define F_NUM_VERT 16
#define F_NUM_EDGE 32
#define F_SIZE 60.0f

#define S_WIDTH 100
#define S_HEIGHT 100
#define S_CAMERA_Z 150.0f
#define S_CAMERA_W 50.0f

typedef struct
{
	float x, y;
}VECT2;
typedef struct
{
	float x, y, z;
}VECT3;
typedef struct
{
	float x, y, z, w;
}VECT4;

class FIGURE
{
public:
	VECT2 vertexes2[F_NUM_VERT] = {};
	VECT4 vertexes4[F_NUM_VERT] = {
		{ 0, 0, 0, 0 }, { 1, 0, 0, 0 }, { 1, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 1, 0, 1, 0 }, { 1, 1, 1, 0 }, { 0, 1, 1, 0 },
		{ 0, 0, 0, 1 }, { 1, 0, 0, 1 }, { 1, 1, 0, 1 }, { 0, 1, 0, 1 }, { 0, 0, 1, 1 }, { 1, 0, 1, 1 }, { 1, 1, 1, 1 }, { 0, 1, 1, 1 }
	};
	unsigned char edges[F_NUM_EDGE][2] = {
		{ 0, 1 },  { 1, 2 },  { 2, 3 },   { 3, 0 },  { 4, 5 },   { 5, 6 },   { 6, 7 },   { 7, 4 },   { 0, 4 },  { 1, 5 },  { 2, 6 },   { 3, 7 },
		{ 8, 9 },  { 9, 10 }, { 10, 11 }, { 11, 8 }, { 12, 13 }, { 13, 14 }, { 14, 15 }, { 15, 12 }, { 8, 12 }, { 9, 13 }, { 10, 14 }, { 11, 15 },
		{ 0, 8 },  { 1, 9 },  { 2, 10 },  { 3, 11 }, { 4, 12 },  { 5, 13 },  { 6, 14 },  { 7, 15 }
	};

	FIGURE()
	{
		for (int i = 0; i < F_NUM_VERT; i++)
		{
			vertexes4[i].x = F_SIZE * vertexes4[i].x + (S_WIDTH - F_SIZE) / 2.0f;
			vertexes4[i].y = F_SIZE * vertexes4[i].y + (S_HEIGHT - F_SIZE) / 2.0f;
			vertexes4[i].z = F_SIZE * vertexes4[i].z;
			vertexes4[i].w = F_SIZE * vertexes4[i].w;
		}
	}
	void rotate(float angle)
	{
		VECT4 vect4;

		for (int i = 0; i < F_NUM_VERT; i++)
		{
			vertexes4[i].x -= S_WIDTH / 2.0f;
			vertexes4[i].y -= S_HEIGHT / 2.0f;
			vertexes4[i].z -= F_SIZE / 2.0f;
			vertexes4[i].w -= F_SIZE / 2.0f;

			vect4 = vertexes4[i];
			
			vertexes4[i].x = cos(angle) * vect4.x - sin(angle) * vect4.z;
			vertexes4[i].z = sin(angle) * vect4.x + cos(angle) * vect4.z;	
			vertexes4[i].y = cos(angle) * vect4.y - sin(angle) * vect4.w;
			vertexes4[i].w = sin(angle) * vect4.y + cos(angle) * vect4.w;
		
			vertexes4[i].x += S_WIDTH / 2.0f;
			vertexes4[i].y += S_HEIGHT / 2.0f;
			vertexes4[i].z += F_SIZE / 2.0f;
			vertexes4[i].w += F_SIZE / 2.0f;
		}
	}
};

class SCREEN
{
private:
	char char_pixel, buffer[S_WIDTH * S_HEIGHT + 1];
	HANDLE hout = nullptr;

	void set_pixel(int x, int y)
	{
		if (x >= 0 && y >= 0 && x < S_WIDTH && y < S_HEIGHT)
		{
			buffer[y * S_WIDTH + x] = char_pixel;
		}
	}
	void set_line(float x1, float y1, float x2, float y2)
	{
		float d = (fabs(x2 - x1) > fabs(y2 - y1)) ? fabs(x2 - x1) : fabs(y2 - y1);
		float sx = (x2 - x1) / d;
		float sy = (y2 - y1) / d;

		for (int i = 0; i < d; i++, x1 += sx, y1 += sy)
		{
			set_pixel(
				static_cast<int>(round(x1)),
				static_cast<int>(round(y1))
			);
		}
	}
	VECT2 vect4_to_vect2(VECT4& vect4)
	{
		VECT3 vect3;
		VECT2 vect2;
		float k;

		if (vect4.w > 0)
		{
			k = S_CAMERA_W / (S_CAMERA_W + vect4.w);
			vect3.x = 50.0f + (vect4.x - 50.0f) * k;
			vect3.y = 50.0f + (vect4.y - 50.0f) * k;
			vect3.z = 50.0f + (vect4.z - 50.0f) * k;
		}
		else
		{
			k = S_CAMERA_W / (S_CAMERA_W - vect4.w);
			vect3.x = 50.0f + (vect4.x - 50.0f) / k;
			vect3.y = 50.0f + (vect4.y - 50.0f) / k;
			vect3.z = 50.0f + (vect4.z - 50.0f) / k;
		}
		if (vect3.z > 0)
		{
			k = S_CAMERA_Z / (S_CAMERA_Z + vect3.z);
			vect2.x = 50.0f + (vect3.x - 50.0f) * k;
			vect2.y = 50.0f + (vect3.y - 50.0f) * k;
		}
		else
		{
			k = S_CAMERA_Z / (S_CAMERA_Z - vect3.z);
			vect2.x = 50.0f + (vect3.x - 50.0f) / k;
			vect2.y = 50.0f + (vect3.y - 50.0f) / k;
		}

		return vect2;
	}
public:
	SCREEN()
	{
		CONSOLE_FONT_INFOEX cfi = {};
		cfi.dwFontSize = { 5, 4 };
		cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
		hout = GetStdHandle(STD_OUTPUT_HANDLE);
		SetCurrentConsoleFontEx(hout, false, &cfi);

		system("mode con cols=100 lines=100");
		system("title Tesseract");
		system("color 0f");

		buffer[S_WIDTH * S_HEIGHT] = '\0';
		char_pixel = C_CLEAR;
	}
	void set_figure(FIGURE& figure)
	{
		// Project 4D vector on 2D screen
		for (int i = 0; i < F_NUM_VERT; i++)
		{
			figure.vertexes2[i] = vect4_to_vect2(figure.vertexes4[i]);
		}

		// Set edges
		char_pixel = C_EDGE;
		for (int i = 0; i < F_NUM_EDGE; i++)
		{
			set_line(
				figure.vertexes2[figure.edges[i][0]].x,
				figure.vertexes2[figure.edges[i][0]].y,
				figure.vertexes2[figure.edges[i][1]].x,
				figure.vertexes2[figure.edges[i][1]].y
			);
		}

		// Set vertexes
		char_pixel = C_VERT;
		for (int i = 0; i < F_NUM_VERT; i++)
		{
			set_pixel(
				static_cast<int>(round(figure.vertexes2[i].x)),
				static_cast<int>(round(figure.vertexes2[i].y))
			);
		}
	}
	void clear()
	{
		memset(buffer, C_CLEAR, S_WIDTH * S_HEIGHT);
	}
	void print()
	{
		SetConsoleCursorPosition(hout, { 0, 0 });
		printf("%s", buffer);
	}
};

int main()
{
	SCREEN scr;
	FIGURE fig;

	while (GetKeyState(VK_ESCAPE) >= 0)
	{
		fig.rotate(0.015f);
		scr.clear();
		scr.set_figure(fig);
		scr.print();
	}

	return 0;
}