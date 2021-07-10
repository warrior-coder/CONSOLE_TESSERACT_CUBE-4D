#include <stdio.h>
#include <windows.h>
#include <cmath>

#define C_VERT '#'
#define C_EDGE '.'
#define C_CLEAR ' '

#define F_NUM_VERT 16
#define F_NUM_EDGE 32
#define F_SIZE 70.0f

#define S_WIDTH 100
#define S_HEIGHT 100

typedef struct VECT2
{
	float x, y;
};
typedef struct VECT3
{
	float x, y, z;
};
typedef struct VECT4
{
	float x, y, z, w;
};

class FIGURE
{
public:
	VECT4 vertexes4[F_NUM_VERT] = {
		// Cube W = 0
		{ 0, 0, 0, 0 },
		{ 1, 0, 0, 0 },
		{ 1, 1, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 0, 1, 0 },
		{ 1, 0, 1, 0 },
		{ 1, 1, 1, 0 },
		{ 0, 1, 1, 0 },

		// Cube W = 1
		{ 0, 0, 0, 1 },
		{ 1, 0, 0, 1 },
		{ 1, 1, 0, 1 },
		{ 0, 1, 0, 1 },
		{ 0, 0, 1, 1 },
		{ 1, 0, 1, 1 },
		{ 1, 1, 1, 1 },
		{ 0, 1, 1, 1 }
	};
	VECT3 vertexes3[F_NUM_VERT];
	VECT2 vertexes2[F_NUM_VERT];
	char edges[F_NUM_EDGE][2] = {
		// Cube W = 0
		{ 0, 1 },
		{ 1, 2 },
		{ 2, 3 },
		{ 3, 0 },
		{ 4, 5 },
		{ 5, 6 },
		{ 6, 7 },
		{ 7, 4 },
		{ 0, 4 },
		{ 1, 5 },
		{ 2, 6 },
		{ 3, 7 },

		// Cube W = 1
		{ 8, 9 },
		{ 9, 10 },
		{ 10, 11 },
		{ 11, 8 },
		{ 12, 13 },
		{ 13, 14 },
		{ 14, 15 },
		{ 15, 12 },
		{ 8, 12 },
		{ 9, 13 },
		{ 10, 14 },
		{ 11, 15 },

		// Cubes connection
		{ 0, 8 },
		{ 1, 9 },
		{ 2, 10 },
		{ 3, 11 },
		{ 4, 12 },
		{ 5, 13 },
		{ 6, 14 },
		{ 7, 15 }
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
		/*
		Rotation matrix around XY example:

		| cos, -sin,  0,    0 |      | X |     | cos * X - sin * Y |
		| sin,  cos,  0,    0 |  *   | Y |  =  | sin * X + cos * Y |
		| 0,    0,    1,    0 |      | Z |     |       1 * Z       |
		| 0,    0,    0,    1 |      | W |     |       1 * W       |

		*/

		VECT4 vect4;

		for (int i = 0; i < F_NUM_VERT; i++)
		{
			vertexes4[i].x -= S_WIDTH / 2.0f;
			vertexes4[i].y -= S_HEIGHT / 2.0f;
			vertexes4[i].z -= F_SIZE / 2.0f;
			vertexes4[i].w -= F_SIZE / 2.0f;

			vect4 = vertexes4[i];
			
			//Around XZ plane :
			vertexes4[i].x = cos(angle) * vect4.x - sin(angle) * vect4.z;
			vertexes4[i].z = sin(angle) * vect4.x + cos(angle) * vect4.z;	
			
			//Around YW plane :
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
	char pixel, buffer[S_WIDTH * S_HEIGHT + 1]; // One extra char for '\0'
	float camera_z, camera_w;

public:
	SCREEN()
	{
		system("mode con cols=100 lines=100");
		system("title Console Tesseract");
		system("color 0f");

		pixel = C_CLEAR;
		buffer[S_WIDTH * S_HEIGHT] = '\0';
		camera_z = 150.0f;
		camera_w = 100.0f;
	}
	void set_figure(FIGURE& figure)
	{
		// Convert 4D -> 3D -> 2D
		for (int i = 0; i < F_NUM_VERT; i++)
		{
			figure.vertexes3[i] = vect4_to_vect3(figure.vertexes4[i]);
			figure.vertexes2[i] = vect3_to_vect2(figure.vertexes3[i]);
		}

		// Set edges
		pixel = C_EDGE;
		for (int i = 0; i < F_NUM_EDGE; i++)
		{
			set_line(figure.vertexes2[figure.edges[i][0]].x, figure.vertexes2[figure.edges[i][0]].y, figure.vertexes2[figure.edges[i][1]].x, figure.vertexes2[figure.edges[i][1]].y);
		}

		// Set vertexes
		pixel = C_VERT;
		for (int i = 0; i < F_NUM_VERT; i++)
		{
			set_pixel(figure.vertexes2[i].x, figure.vertexes2[i].y);
		}
	}
	void set_pixel(int x, int y)
	{
		if (x >= 0 && y >= 0 && x < S_WIDTH && y < S_HEIGHT)
		{
			buffer[y * S_WIDTH + x] = pixel;
		}
	}
	void set_line(float x1, float y1, float x2, float y2)
	{
		float dx = fabs(x2 - x1);
		float dy = fabs(y2 - y1);
		float sx, sy;

		if (dx > dy)
		{
			sx = (x2 - x1) / dx;
			sy = (y2 - y1) / dx;
			
			for (int i = 0; i < dx; i++, x1 += sx, y1 += sy)
			{
				set_pixel((int)x1, (int)y1);
			}
		}
		else
		{
			sx = (x2 - x1) / dy;
			sy = (y2 - y1) / dy;

			for (int i = 0; i < dy; i++, x1 += sx, y1 += sy)
			{
				set_pixel((int)x1, (int)y1);
			}
		}
	}
	void clear()
	{
		memset(buffer, C_CLEAR, S_WIDTH * S_HEIGHT);
	}
	void print()
	{
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 });
		printf("%s", buffer);
	}
	VECT3 vect4_to_vect3(VECT4& vect4)
	{
		if (vect4.w == 0)
		{
			return { vect4.x, vect4.y, vect4.z };
		}
		else if (vect4.w > 0)
		{
			VECT3 vect3;
			float k = camera_w / (camera_w + vect4.w);

			vect3.x = 50.0f + (vect4.x - 50.0f) * k;
			vect3.y = 50.0f + (vect4.y - 50.0f) * k;
			vect3.z = 50.0f + (vect4.z - 50.0f) * k;

			return vect3;
		}
		else
		{
			VECT3 vect3;
			float k = camera_w / (camera_w - vect4.w);

			vect3.x = 50.0f + (vect4.x - 50.0f) / k;
			vect3.y = 50.0f + (vect4.y - 50.0f) / k;
			vect3.z = 50.0f + (vect4.z - 50.0f) / k;

			return vect3;
		}
	}
	VECT2 vect3_to_vect2(VECT3& vect3)
	{
		if (vect3.z == 0)
		{
			return { vect3.x, vect3.y };
		}
		else if (vect3.z > 0)
		{
			VECT2 vect2;
			float k = camera_z / (camera_z + vect3.z);

			vect2.x = 50.0f + (vect3.x - 50.0f) * k;
			vect2.y = 50.0f + (vect3.y - 50.0f) * k;

			return vect2;
		}
		else
		{
			VECT2 vect2;
			float k = camera_z / (camera_z - vect3.z);

			vect2.x = 50.0f + (vect3.x - 50.0f) / k;
			vect2.y = 50.0f + (vect3.y - 50.0f) / k;

			return vect2;
		}
	}
};

int main()
{
	SCREEN scr;
	FIGURE fig;

	

	while (true)
	{
		scr.clear();
		scr.set_figure(fig);
		fig.rotate(0.01f);
		scr.print();

		if (GetKeyState(VK_ESCAPE) < 0) return 0;
	}
}