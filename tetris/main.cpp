#include <iostream>
#include <Windows.h>
#include <time.h>
#include <vector>
#include <cwchar>
#include <stdio.h>


using namespace std;

wstring tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18;

unsigned char *pField = nullptr;

int nScreenWidth = 60; // Console Screen Size X (columns)
int nScreenHeight = 40; // Console Screen Size y (row)

int Rotate(int px, int py, int r){
    switch (r%4){
        case 0: return py * 4 + px; // 0 degrees
        case 1: return 12 + py - (px * 4); // 90 deg
        case 2: return 15 - (py * 4) - px; // 180 deg
        case 3: return 3 - py + (px * 4); // 270
    }
    return 0;
}

bool DoesPieceFit(int nTet, int nRot, int nPosX, int nPosY){

for(int px = 0; px < 4; px++)
    for(int py = 0; py < 4; py++)
    {
        //get index into piece
        int pi = Rotate(px, py, nRot);
        // get index into field
        int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

        if(nPosX + px >= 0 && nPosX + px < nFieldWidth){
            if(nPosY + py >= 0 && nPosY + py < nFieldHeight){
                if(tetromino[nTet][pi] == L'X' && pField[fi] != 0)
                    return false; // fail on first hit
            }
        }
    }

    return true;
}

int main()
{
    // Creat assets
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");

    tetromino[1].append(L"..X.");
    tetromino[1].append(L".XX.");
    tetromino[1].append(L".X..");
    tetromino[1].append(L"....");

    tetromino[2].append(L".X..");
    tetromino[2].append(L".XX.");
    tetromino[2].append(L"..X.");
    tetromino[2].append(L"....");

    tetromino[3].append(L"....");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L"....");

    tetromino[4].append(L"..X.");
    tetromino[4].append(L".XX.");
    tetromino[4].append(L"..X.");
    tetromino[4].append(L"....");

    tetromino[5].append(L"....");
    tetromino[5].append(L".XX.");
    tetromino[5].append(L"..X.");
    tetromino[5].append(L"..X.");

    tetromino[6].append(L"....");
    tetromino[6].append(L".XX.");
    tetromino[6].append(L".X..");
    tetromino[6].append(L".X..");

    pField = new unsigned char[nFieldWidth*nFieldHeight];// play field buffer
    for (int x = 0; x < nFieldWidth; x++)// Board boundary
        for(int y = 0; y < nFieldHeight; y++)
            pField[y*nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1 ) ? 9 : 0;

    //wcout<< tetromino[0]; // much faster than sys32 default console

    wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];
    for(int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL,CONSOLE_TEXTMODE_BUFFER, NULL );
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    bool bGameOver = false;

    int nCurPiece = 0;
    int nCurRot = 0;
    int nCurX = nFieldWidth/2;
    int nCurY = 0;
    bool bKey[4];
    bool bRotateHold = false;

    int nSpeed = 20;
    int nSpeedCounter = 0;
    bool nForceDown = false;
    int nPieceCount = 0;
    int nScore = 0;

    vector<int> vLines;

    while (!bGameOver){
        // Timing
        Sleep(0130);
        nSpeedCounter++;
        bool nForceDown = (nSpeedCounter == nSpeed);

        //input
        for (int k = 0; k < 4; k++)                              //<-  -> Dwn Z
            bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k])))!= 0;
        //game logic
        if(bKey[1]){
            if(DoesPieceFit(nCurPiece, nCurRot, nCurX - 1, nCurY)){
                nCurX = nCurX - 1;
            }
        }
        nCurX += (bKey[0] && DoesPieceFit(nCurPiece, nCurRot, nCurX + 1, nCurY))? 1: 0;



        if(bKey[2]){
            if(DoesPieceFit(nCurPiece, nCurRot, nCurX , nCurY + 1)){
                nCurY = nCurY + 1;
            }
        }
        if(bKey[3]){
            nCurRot += (!bRotateHold && DoesPieceFit(nCurPiece, nCurRot + 1, nCurX, nCurY))? 1: 0;
            bRotateHold = true;
        }
        else
            bRotateHold = false;

        if(nForceDown){
            if(DoesPieceFit(nCurPiece, nCurRot, nCurX, nCurY + 1))
                nCurY++; // it can, So do it!
            else{
                // lock the piece in the field
                for (int px = 0; px < 4; px++)
                    for (int py = 0; py < 4; py++)
                        if(tetromino[nCurPiece][Rotate(px, py, nCurRot)] == L'X')
                            //screen[(nCurY + py + 2) * nScreenWidth + (nCurX + px + 2)] = nCurPiece + 65;
                            pField[(nCurY + py) * nFieldWidth+(nCurX + px)] = nCurPiece+1;

                nPieceCount++;
                if(nPieceCount % 10 == 0)
                    if(nSpeed >= 10) nSpeed--;

                // check have we got any lines
                for(int py = 0; py < 4; py++)
                    if(nCurY + py < nFieldHeight - 1)
                {
                    bool bLine = true;
                    for(int px = 1; px < nFieldWidth - 1; px++)
                        bLine &= (pField[(nCurY + py) * nFieldWidth + px]) != 0;

                    if(bLine)
                    {
                        //Remove line, set to =
                        for (int px = 1; px < nFieldWidth - 1; px++)
                            pField[(nCurY + py) * nFieldWidth + px] = 8;

                        vLines.push_back(nCurY + py);
                    }
                }

                nScore += 25;
                if(!vLines.empty()) nScore += (1 << vLines.size()) * 100;

                // choose next piece
                nCurX = nFieldWidth /2;
                nCurY = 0;
                nCurRot = 0;
                nCurPiece = rand() % 7;

                // if piece does not fit

                bGameOver = !DoesPieceFit(nCurPiece, nCurRot, nCurX, nCurY);
            }
              nSpeedCounter = 0;
        }// end if (dForceDown)
        //Draw Field
        for(int x = 0; x < nFieldWidth; x++)
            for(int y = 0; y < nFieldHeight; y++)
                screen[(y+2) * nScreenWidth + (x+2)] = L" ABCDEFG=#"[pField[y*nFieldWidth + x]];
        //Draw current Piece
        for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++)
                if(tetromino[nCurPiece][Rotate(px, py, nCurRot)] == L'X')
                    screen[(nCurY + py + 2) * nScreenWidth + (nCurX + px + 2)] = nCurPiece + 65;

        // Draw Score
        //swprintf_s(&screen[2*nScreenWidth+nFieldWidth+6], 16, L"SCORE: %8d", nScore);

        if(!vLines.empty()){
           // display to screen
            WriteConsoleOutputCharacter(hConsole, (const char*)screen, nScreenWidth*nScreenHeight, {0,0}, &dwBytesWritten);
            Sleep(0200);

            for (auto &v : vLines)
                for(int px = 1; px < nFieldWidth - 1; px++){
                    for(int py = v; py > 0; py--)
                        pField[py * nFieldWidth + px] = pField[(py - 1)* nFieldWidth + px];
                    pField[px] = 0;
                }
                vLines.clear();
        }

        // Display Frame
       WriteConsoleOutputCharacter(hConsole, (const char*)screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

    }

CloseHandle(hConsole);
cout << "Game Over!! Score:" << nScore << endl;

    return 0;
}
