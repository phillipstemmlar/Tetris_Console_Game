// Tetris.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <chrono>
#include <thread>
#include<vector>

using namespace std;

//Functions
void CreateAssets();
int Rotate(int x, int y, int r);
void Update();
bool DoesBlockFit(int Tblock, int rot, int xPos, int yPos);

//Assets
wstring TetrisShapes[7];
int fieldWidth  = 12;
int fieldHeight = 18;
unsigned char* Field = nullptr;

//Screen
int screenHeight = 30;
int screenWidth  = 80;
wchar_t* Screen = nullptr;

//Update
bool GameOver = false;

//Score
int Score = 0;

int main()
{
	CreateAssets();
	Update();
    return 0;
}

void CreateAssets()
{
	TetrisShapes[0].append(L"..X.");
	TetrisShapes[0].append(L"..X.");
	TetrisShapes[0].append(L"..X.");
	TetrisShapes[0].append(L"..X.");

	TetrisShapes[1].append(L"....");
	TetrisShapes[1].append(L".XX.");
	TetrisShapes[1].append(L"..X.");
	TetrisShapes[1].append(L"..X.");

	TetrisShapes[2].append(L".XX.");
	TetrisShapes[2].append(L".X..");
	TetrisShapes[2].append(L".X..");
	TetrisShapes[2].append(L"....");

	TetrisShapes[3].append(L"..X.");
	TetrisShapes[3].append(L".XX.");
	TetrisShapes[3].append(L"..X.");
	TetrisShapes[3].append(L"....");

	TetrisShapes[4].append(L".XX.");
	TetrisShapes[4].append(L".XX.");
	TetrisShapes[4].append(L"....");
	TetrisShapes[4].append(L"....");

	TetrisShapes[5].append(L".X..");
	TetrisShapes[5].append(L".XX.");
	TetrisShapes[5].append(L"..X.");
	TetrisShapes[5].append(L"....");

	TetrisShapes[6].append(L"..X.");
	TetrisShapes[6].append(L".XX.");
	TetrisShapes[6].append(L".X..");
	TetrisShapes[6].append(L"....");

	Field = new unsigned char[fieldHeight * fieldWidth];	//Create Field
	for (int x = 0; x < fieldWidth; x++){
		for (int y = 0; y < fieldHeight; y++)
		{
			int c = 0;
			if ( (x == 0) || (x == fieldWidth -1) || y == fieldHeight -1)
			{
				c = 9;
			}
			Field[y*fieldWidth + x] = c;
		}
	}
}


void Update()
{

	Screen = new wchar_t[screenHeight * screenWidth];
	for (int n = 0; n < screenHeight*screenWidth; n++) { Screen[n] = L' '; }
	HANDLE Console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(Console);
	DWORD dwBytesWritten = 0;
	
	HWND hwnd = GetConsoleWindow();
	RECT rect = { 100, 100, 600, 775 };
	MoveWindow(hwnd, rect.top, rect.left, rect.bottom - rect.top, rect.right - rect.left, TRUE);

	//GAME LOGIC STUFF ==========================================================

	GameOver = false;

	int CurPiece = 0;
	int CurPosX = (fieldWidth / 2) -1;
	int CurPosY = 0;
	int CurRot = 0;

	bool RotHold = false;
	bool bkey[4];

	int speed = 20;
	int speedCounter = 0;
	int nPieces = 0;
	bool forceDown = false;

	vector<int> Lines;

	while (!GameOver)
	{
		//GAME TIMING ===========================================================
		srand(time(0));
		this_thread::sleep_for(30ms); //Game Tick
		speedCounter++;
		forceDown = (speedCounter == speed);

		//INPUT =================================================================
		for (int k = 0 ; k < 4; k++)
		{														   //R    L  D  Z
			bkey[k] = ( (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0);
		}

		//GAME LOGIC ============================================================
		if (bkey[1]) {	//L
			if (DoesBlockFit(CurPiece, CurRot, CurPosX -1, CurPosY) )
			{
				CurPosX = CurPosX - 1;
			}
		}
		if (bkey[0]) {	//R
			if (DoesBlockFit(CurPiece, CurRot, CurPosX + 1, CurPosY))
			{
				CurPosX = CurPosX + 1;
			}
		}
		if (bkey[2]) {	//D
			if (DoesBlockFit(CurPiece, CurRot, CurPosX, CurPosY +1))
			{
				CurPosY = CurPosY + 1;
			}
		}
		if (bkey[3]) {  //Z
			if (!RotHold && DoesBlockFit(CurPiece, CurRot+1, CurPosX, CurPosY))
			{
				CurRot++;
			}
			RotHold = true;
		}
		else { RotHold = false; }

		if (forceDown)
		{
			if (DoesBlockFit(CurPiece, CurRot, CurPosX, CurPosY + 1)) 
			{ CurPosY = CurPosY + 1; } // if it can still go down
			else
			{
				//lock piece into field
				for (int x = 0; x < 4; x++) {
					for (int y = 0; y < 4; y++)
					{
						if (TetrisShapes[CurPiece][Rotate(x, y, CurRot)] == L'X')
						{
							Field[(CurPosY + y) * fieldWidth + (CurPosX + x)] = CurPiece + 1;		//First character is a blank space
						}
					}
				}

				nPieces++;
				if (nPieces % 10 == 0)
				{
					if (speed >= 10)
					{
						speed--;
					}
				}

				//check full hr lines
				for (int y = 0; y < 4; y++)
				{
					if (CurPosY + y < fieldHeight - 1)
					{
						bool line = true;
						for (int x = 0; x < fieldWidth - 1; x++)
						{

							line &= ( Field[(CurPosY + y)*fieldWidth + x] != 0 );

						}

						if (line)
						{
							for (int x = 1; x < fieldWidth-1; x++)
							{
								Field[(CurPosY + y)*fieldWidth + x] = 8;
							}

							Lines.push_back(CurPosY + y);
						}

					}
				}

				Score += 25;
				if (!Lines.empty() )
				{
					Score += ( 1 << Lines.size() * 100 );
				}

				//Choose next piece
				CurPosX = (fieldWidth / 2) - 1;
				CurPosY = 0;
				CurRot = 0;
				CurPiece = rand() % 7;


				//if next piece not fit => game over
				GameOver = !DoesBlockFit(CurPiece, CurRot, CurPosX, CurPosY);
			}

			speedCounter = 0;

		}


		//RENDER OUTPUR =========================================================

		//Draw field
		for (int x = 0; x < fieldWidth; x++) {
			for (int y = 0; y < fieldHeight; y++)
			{
				Screen[(y + 4)*screenWidth + (x + 4)] = L" ABCDEFG=#"[Field[y * fieldWidth + x]];
			}
		}

		//Draw Current Piece
		for (int x = 0; x < 4; x++) {
			for (int y = 0; y < 4; y++)
			{
				if ( TetrisShapes[CurPiece][Rotate(x,y, CurRot)] == L'X')
				{
					Screen[(CurPosY + y + 4)*screenWidth + (CurPosX + x + 4)] = CurPiece + 65;	//65 is A so that adjusts 0 to be A etc.
				}
			}
		}

		if (!Lines.empty())
		{
			//Display frame
			WriteConsoleOutputCharacter(Console, Screen, screenWidth * screenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(100ms);	//delay a bit

			for (auto &v : Lines)
			{
				for (int x = 1; x < fieldWidth - 1; x++)
				{
					for (int y = v; y > 0; y--)
					{
						Field[y*fieldWidth + x] = Field[(y -1)* fieldWidth + x];
					}
					Field[x] = 0;
				}
			}
			Lines.clear();

		}

		//Draw Score
		swprintf_s(&Screen[2* screenWidth + screenHeight + 6], 16, L"SCORE: %8d", Score);

		//Display frame
		WriteConsoleOutputCharacter(Console, Screen, screenWidth * screenHeight, { 0,0 }, &dwBytesWritten);
	}

	//Game Over
	CloseHandle(Console);
	cout << "GAME OVER!" << endl << endl << "Score: " << Score << endl;
	system("pause");

	delete[] Field;
	Field = nullptr;


}

bool DoesBlockFit(int TShape, int rot, int xPos, int yPos)
{
	for (int x = 0; x < 4; x++) {
		for(int y = 0; y < 4; y++)
		{
			//index of piece
			int i_p = Rotate(x,y, rot);

			//index of field
			int i_f = (yPos + y)*fieldWidth + (xPos + x);

			if ( (xPos + x >= 0) && (xPos + x < fieldWidth) ) {
				if ( (yPos + y >= 0) && (yPos + y < fieldHeight) )
				{
					if ( (TetrisShapes[TShape][i_p] == L'X') && (Field[i_f] != 0) )
					{
						return false; //Fall on first hit
					}
				}
			}


		}
	}
	return true;
}

int Rotate(int x, int y, int r)
{
	switch (r % 4)
	{
	case 0: return (y * 4) + x;			//0
	case 1: return 12 + y - (x * 4);	//90
	case 2: return 15 - (y * 4) - x;	//180
	case 3: return 3 - y + (x * 4);		//270
	}
	return 0;

}