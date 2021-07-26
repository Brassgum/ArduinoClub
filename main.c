#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>

#define MEMORY_SPACE (1024)

typedef struct Crossroads
{
	bool bDirections[4]; //DIRECTION;
	unsigned int Distance;
	//int X, Y;
	enum eDirection eFirstDir;
}CrossNode, * pCrossNode;

typedef struct  VisitedNode
{
	int X, Y;
}Visited, * pVisited;

const char* DIRECTION[4] =
{
	"UP", "RIGHT", "DOWN", "LEFT"
};

const char* MAZE[11] =
{
	"###################", //  0
	"#               ###", //  1
	"# ### ### ##### ###", //  2
	"# ##  ### #     ###", //  3
	"# ####### # #######", //  4
	"#       # # #      ", //  5
	"####### ## ## #####", //  6
	"#       ## ## #####", //  7
	"### ####          #", //  8
	"#        ##########", //  9
	"###################", // 10
};

const unsigned int MAZE_X = 20, MAZE_Y = 11;
const char BODY = '@';

enum eDirection
{
	UP, RIGHT, DOWN, LEFT
}eBodyDir;

char Screen[11][20];
unsigned int X, Y;
bool bInMaze;
bool First;
pCrossNode cBP, cSP;
pVisited vBP, vSP;

bool Setup(void);
void Input(void);
void Update(void);
void Draw(void);
void PullDown(void);

_inline void Forward(void)
{
	switch (eBodyDir)
	{
	case UP:
		Y++;
		break;
	case RIGHT:
		X++;
		break;
	case DOWN:
		Y--;
		break;
	case LEFT:
		X--;
		break;
	}
	cSP->Distance++;
}

_inline void Backward(void)
{
	switch (eBodyDir ^ 2)
	{
	case UP:
		Y++;
		break;
	case RIGHT:
		X++;
		break;
	case DOWN:
		Y--;
		break;
	case LEFT:
		X--;
		break;
	}
	cSP->Distance--;
}

bool CheckForward(void)
{
	switch (eBodyDir)
	{
	case UP:
		return '#' != MAZE[Y + 1][X];
	case RIGHT:
		return '#' != MAZE[Y][X + 1];
	case DOWN:
		return '#' != MAZE[Y - 1][X];
	case LEFT:
		return '#' != MAZE[Y][X - 1];
	}
}

bool CheckBackward(void)
{
	switch (eBodyDir ^ 2)
	{
	case UP:
		return '#' != MAZE[Y + 1][X];
	case RIGHT:
		return '#' != MAZE[Y][X + 1];
	case DOWN:
		return '#' != MAZE[Y - 1][X];
	case LEFT:
		return '#' != MAZE[Y][X - 1];
	}
}

_inline void TurnRight(void)
{
	eBodyDir = (eBodyDir + 1) & 3;
}
_inline void TurnLeft(void)
{
	eBodyDir = (eBodyDir + 3) & 3;
}

//bool array
int CheckNerdy(void)
{
	//4byte
	bool nerdy[4];
	nerdy[eBodyDir] = CheckForward();
	nerdy[eBodyDir ^ 2] = CheckBackward();
	TurnRight();
	nerdy[eBodyDir] = CheckForward();
	nerdy[eBodyDir ^ 2] = CheckBackward();
	TurnLeft();
	return *(int*)nerdy;
}

bool Setup(void)
{
	X = 1; Y = 1;
	cBP = (CrossNode*)malloc(sizeof(CrossNode) * ((MEMORY_SPACE - 200) / sizeof(CrossNode)));
	vBP = (Visited*)malloc(sizeof(Visited) * ((MEMORY_SPACE - 800) / sizeof(CrossNode)));
	eBodyDir = 0;
	bInMaze = false;
	if (cBP && vBP)
	{
		cSP = cBP;
		vSP = vBP;
		*(int*)cSP->bDirections = CheckNerdy();
		cSP->Distance = 0;
		//cSP->X = X; cSP->Y = Y;
		vSP->X = X; vSP->Y = Y;
		bInMaze = true;
	}
}

void PullDown(void)
{
	free(cBP);
	printf("\nprogram end");
}

void Input(void)
{
	if (GetAsyncKeyState(VK_SPACE))
	{
		system("pause");
	}
}

void Update(void)
{
	bool bNerdy[4];

	if (cSP->Distance)
	{
		*(int*)bNerdy = CheckNerdy();
		bNerdy[eBodyDir ^ 2] = false;

		switch (*(int*)bNerdy)
		{
		default:
			//cross road
			//check equal pos
			for (pVisited i = vBP; i <= vSP; i++)
			{
				if (X == i->X && Y == i->Y)
				{
					goto BACK_TO_LAST_CROSSROAD;
				}
			}
			cSP++;
			vSP++;
			*(int*)cSP->bDirections = *(int*)bNerdy;
			cSP->Distance = 0;
			vSP->X = X;
			vSP->Y = Y;
			cSP->eFirstDir = eBodyDir;
			//move
			while (!bNerdy[eBodyDir])
			{
				TurnRight();
			}
			cSP->bDirections[eBodyDir] = false;
			Forward();
			break;
		case (0):
		BACK_TO_LAST_CROSSROAD:
			while (cSP->Distance)
			{
				*(int*)bNerdy = CheckNerdy();
				bNerdy[eBodyDir] = false;
				while (!bNerdy[eBodyDir ^ 2])
				{
					TurnLeft();
				}
				Backward();
#if defined _DEBUG
				//screen update
				for (size_t i = 0; i < MAZE_Y; i++)
				{
					for (size_t j = 0; j < (MAZE_X); j++)
					{
						Screen[i][j] = MAZE[i][j];
					}
				}
				Screen[Y][X] = BODY;
				Draw();
#endif
			}
			break;
		case (1):
		case (1 << 8):
		case (1 << 16):
		case (1 << 24):
			if (vBP->X == X && vBP->Y == Y)
			{
				goto BACK_TO_LAST_CROSSROAD;
			}
			//one way
			while (!bNerdy[eBodyDir])
			{
				TurnRight();
			}
			Forward();
			break;
		}
	SWITCH_END0:;
	}
	else
	{
		if (!*(int*)cSP->bDirections)
		{
			if (cSP == cBP)
			{
				bInMaze = false;
				return;
			}

			while (cSP->eFirstDir != eBodyDir)
			{
				TurnLeft();
			}
			cSP--;
			goto BACK_TO_LAST_CROSSROAD;
		}
		while (!cSP->bDirections[eBodyDir])
		{
			TurnRight();
		}
		cSP->bDirections[eBodyDir] = false;
		Forward();
	}
	if (!(0 < X && 0 < Y && X < MAZE_X - 2 && Y < MAZE_Y - 1)) { bInMaze = false; }
	//screen update
	for (size_t i = 0; i < MAZE_Y; i++)
	{
		for (size_t j = 0; j < (MAZE_X); j++)
		{
			Screen[i][j] = MAZE[i][j];
		}
	}
	Screen[Y][X] = BODY;
}

void Draw(void)
{
	system("cls");
	for (size_t i = 0; i < MAZE_Y; i++)
	{
		printf("%s\n", Screen[MAZE_Y - 1 - i]);
	}
}

int main(void)
{
	char re[3];
START:
	Setup();
	while (bInMaze)
	{
		Input();
		Update();
		if (X == 9 && Y == 1)
		{
			_asm nop;
		}
		Draw();
	}
	PullDown();
	scanf_s("%s", re, 3);
	if (re[0] == 'r') { goto START; }
	return 0;
}
