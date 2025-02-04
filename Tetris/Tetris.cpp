#include <ctime>
#include <fstream>
#include <future>
#include <random>
#include <string>
#include <tchar.h>
#include <thread>
#include <windows.h>
#include <sstream>

// 時間格式化


std::string getTime() {
    std::time_t t = std::time(nullptr);  // 获取当前时间
    std::tm now;

    // 使用线程安全的 localtime_s
    localtime_s(&now, &t);

    // 使用 stringstream 格式化时间
    std::ostringstream oss;
    oss << (now.tm_year + 1900) << "/"
        << (now.tm_mon + 1) << "/"
        << now.tm_mday << "  "
        << now.tm_hour << ":"
        << now.tm_min << ":"
        << now.tm_sec;

    return oss.str();  // 返回格式化后的时间字符串
}


std::future<void> writeFileAsync(const std::string& filename, const std::string& content) {
    return std::async(std::launch::async, [filename, content]() {
        std::ofstream outFile(filename, std::ios::app);
        if (outFile.is_open()) {
            outFile << content << std::endl;
        }
        else {
        }
        });
}

//void 定義
void UpdateElapsedSeconds();
int RandomNumber(int min, int max);
void rotateList(int ori[4][4], int out[4][4], int n);
void moveShapeToTopLeft(int matrix[4][4]);
bool arelistSame(int a[4][4], int b[4][4]);
void ListMove(int matrix[4][4], int direction, int steps);
void runBlockReCheckToTetris();
void RandomBlock();
void RunBlockRe();
void rotateBlock(int type);
void moveRight();
void moveLeft();
void stringRemove();
void checkblockIN();
void movedown();
//
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

const int GRID_WIDTH = 10;   // 格子列數
const int GRID_HEIGHT = 20;  // 格子行數
const int CELL_SIZE = 20;    // 格子大小 (px)
const int BORDER_SIZE = 5;   // 邊框大小 (px)
const int TIMER_ID = 1;      // 定時器 ID
const int TIMER_INTERVAL = 65; // 定時器間隔 (ms)
bool KeyList[256] = { false }; // 用來記錄所有按鍵的狀態
//TETRIS定義
//強降checking
int SdownCheck = 0;
//遊戲時間
int Time = 0;
//方塊預存
int RandomBlockSave = 0;
//遊戲等級
int level = 0;
//最高紀錄
int topScore = 0;
//遊戲分數
int scores = 0;
//消除線數
int AllString = 0;
int removeString = 0;
// blockType
int BlockType = 0;
//RunBlock
int RunBlock[GRID_HEIGHT][GRID_WIDTH] = { 0 };
// blockSummon
bool blockSummon = false;
// game End Execute
bool End = false;
// tetris  defind
int TetrisBlock[20][10] = { 0 };
int checkTetrisBlock[20][10] = { 0 };
int blockC[7][2][4] = {
    // I形（直條型）
    {
        {1, 1, 1, 1}, // 第一行 (水平排)
        {0, 0, 0, 0}  // 第二行 (無)
    },
    // O形（正方形型）
    {
        {1, 1, 0, 0}, // 第一行
        {1, 1, 0, 0}  // 第二行
    },
    // T形
    {
        {0, 1, 0, 0}, // 第一行
        {1, 1, 1, 0}  // 第二行
    },
    // S形（鏡像Z形）
    {
        {0, 1, 1, 0}, // 第一行
        {1, 1, 0, 0}  // 第二行
    },
    // Z形（鏡像S形）
    {
        {1, 1, 0, 0}, // 第一行
        {0, 1, 1, 0}  // 第二行
    },
    // J形
    {
        {1, 0, 0, 0}, // 第一行
        {1, 1, 1, 0}  // 第二行
    },
    // L形
    {
        {0, 0, 1, 0}, // 第一行
        {1, 1, 1, 0}  // 第二行
    }
};
// 顏色對應
COLORREF colors[] = {
    RGB(255, 255, 255), // 0: 白色
    RGB(255, 0, 0),     // 1: 紅色
    RGB(0, 0, 0)        // 2: 黑色
};

//run背景執行
void run() {
    RandomBlockSave = RandomNumber(0, 6);
    //writeFileAsync("consloe.txt", "[INFO] block random " + RandomBlockSave);
    //
    blockSummon = true;
    // runTimes
    int RunTimes = 0;
    while (!End)
    {
        if (blockSummon == true)
        {
            RandomBlock();
            blockSummon = false;
            RunBlockRe();
        }
        if (RunTimes % (53 / (level + 1)) == 0)
        {
            movedown();
            RunBlockRe();
        }
        UpdateElapsedSeconds();
        Sleep(3);
        RunTimes++;
    }
    //writeFileAsync("consloe.txt", "[INFO] game end ");
    return;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT("俄羅斯方塊");
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpszClassName = szAppName;
    wndclass.lpszMenuName = NULL;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbWndExtra = 0;
    wndclass.cbClsExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT("視窗註冊失敗"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow(
        szAppName,
        TEXT("TETRIS"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        (GRID_WIDTH * CELL_SIZE) + BORDER_SIZE * 2 + 432,
        (GRID_HEIGHT * CELL_SIZE) + BORDER_SIZE * 2 + 278,
        NULL,
        NULL,
        hInstance,
        NULL);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    // 啟動定時器
    SetTimer(hwnd, TIMER_ID, TIMER_INTERVAL, NULL);
    //啟動預執行
    std::thread bgThread(run);
    while (GetMessage(&msg, NULL, 0, 0) && End == false)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    bgThread.detach();
    KillTimer(hwnd, TIMER_ID); // 停止定時器
    //資料存處
    writeFileAsync("Histroy.txt", "====" + getTime() + "====");
    writeFileAsync("Histroy.txt", "[score]" + std::to_string(scores));
    writeFileAsync("Histroy.txt", "[level]" + std::to_string(level));
    writeFileAsync("Histroy.txt", "[time]" + std::to_string(Time));
    writeFileAsync("Histroy.txt", "[string]" + std::to_string(AllString));
    writeFileAsync("Histroy.txt", "============================");
    //遊戲結束
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HDC hdcMem;
    static HBITMAP hBitmap;
    static HBRUSH hBrushYellow;
    static HBRUSH hBrushBlack;
    static RECT clientRect;

    switch (message)
    {
    case WM_CREATE:
        MessageBox(hwnd, TEXT("點擊後遊戲啟動,請準備輸入法"), TEXT("準備"), MB_OK | MB_ICONINFORMATION);
        GetClientRect(hwnd, &clientRect);
        hdcMem = CreateCompatibleDC(NULL); // 創建內存DC
        hBitmap = CreateCompatibleBitmap(GetDC(hwnd), clientRect.right, clientRect.bottom);
        SelectObject(hdcMem, hBitmap);
        hBrushYellow = CreateSolidBrush(RGB(255, 255, 0)); // 初始化黃色畫刷
        hBrushBlack = CreateSolidBrush(RGB(0, 0, 0));      // 初始化黑色畫刷
        return 0;

    case WM_TIMER: // 定時器觸發
        if (End == false) {
            // 使用雙緩衝繪製
            FillRect(hdcMem, &clientRect, (HBRUSH)(COLOR_WINDOW + 1)); // 填充背景色

            // 繪製主格子
            for (int y = 0; y < GRID_HEIGHT; ++y)
            {
                for (int x = 0; x < GRID_WIDTH; ++x)
                {
                    // 計算每個格子的位置
                    int left = 100 + BORDER_SIZE + x * CELL_SIZE;
                    int top = 100 + BORDER_SIZE + y * CELL_SIZE;
                    int right = left + CELL_SIZE;
                    int bottom = top + CELL_SIZE;

                    // 畫格子內部
                    RECT cellRect = { left, top, right, bottom };
                    HBRUSH hBrush = CreateSolidBrush(colors[RunBlock[y][x]]);
                    FillRect(hdcMem, &cellRect, hBrush);
                    DeleteObject(hBrush);

                    // 畫格子的黃色邊框
                    FrameRect(hdcMem, &cellRect, hBrushYellow);
                }
            }

            // 繪製右方的整體長方形
            int offsetX = 100 + BORDER_SIZE + GRID_WIDTH * CELL_SIZE + 50; // 主格子右方 50px 的位置
            int offsetY = 100 + BORDER_SIZE;

            // 計算整體矩形的邊界
            int left = offsetX;
            int top = offsetY;
            int right = offsetX + 6 * CELL_SIZE; // 寬度 6 個單位
            int bottom = offsetY + 20 * CELL_SIZE; // 高度 20 個單位

            // 畫整體矩形的黑色邊框
            RECT outerRect = { left, top, right, bottom };
            FrameRect(hdcMem, &outerRect, hBrushBlack); // 改為黑色邊框

            // 繪製分隔線（水平分割線）
            HPEN hPenBlack = CreatePen(PS_SOLID, 5, RGB(0, 0, 0)); // 黑色筆，5px 寬
            HPEN hOldPen = (HPEN)SelectObject(hdcMem, hPenBlack);

            // 繪製分隔線
            for (int i = 0; i < 2; ++i) {
                int separatorY = top + (4 + i * 5) * CELL_SIZE;
                MoveToEx(hdcMem, left, separatorY, NULL);
                LineTo(hdcMem, right, separatorY);
            }

            // 設置文字屬性
            SetBkMode(hdcMem, TRANSPARENT);
            SetTextColor(hdcMem, RGB(0, 0, 0));

            // 定義文字與對應數值
            struct TextItem {
                const TCHAR* label;
                int* value;
                int lineOffset;
            };
            TextItem items[] = {
                { TEXT("Top Score"), &topScore, 0 },
                { TEXT("Score"), &scores, 2 },
                { TEXT("NextBlock"), nullptr, 8 },
                { TEXT("Level"), &level, 10 },
                { TEXT("Time"), &Time, 12 },
                { TEXT("String"), &AllString, 14 },
                { TEXT("Made by 詹秉睿"), nullptr, 18 }
            };

            // 動態繪製文字與數值
            TCHAR buffer[32];
            for (const auto& item : items) {
                RECT rect = { left, top + item.lineOffset * CELL_SIZE, right, top + (item.lineOffset + 1) * CELL_SIZE };
                DrawText(hdcMem, item.label, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                if (item.value) {
                    _stprintf_s(buffer, TEXT("%d"), *item.value);
                    rect.top += CELL_SIZE;
                    rect.bottom += CELL_SIZE;
                    DrawText(hdcMem, buffer, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
            }


            // 下一個方塊的具體顯示（可以根據遊戲邏輯填充相應數據）
            // Example: 填充一個簡單方塊
            for (int y = 1; y <= 2; y++) // 示範方塊的範圍（僅示範）
            {
                for (int x = 1; x <= 4; x++)
                {
                    if (blockC[RandomBlockSave][y - 1][x - 1] == 1) {
                        int blockLeft = left + x * CELL_SIZE;
                        int blockTop = top + (y + 4) * CELL_SIZE;
                        int blockRight = blockLeft + CELL_SIZE;
                        int blockBottom = blockTop + CELL_SIZE;

                        RECT blockRect = { blockLeft, blockTop, blockRight, blockBottom };
                        HBRUSH hBrushBlock = CreateSolidBrush(RGB(255, 0, 0)); // 填充紅色示例方塊
                        FillRect(hdcMem, &blockRect, hBrushBlock);
                        DeleteObject(hBrushBlock);
                    }
                    
                }
            }

            InvalidateRect(hwnd, NULL, FALSE); // 請求重繪
            return 0;

        }
        
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, hdcMem, 0, 0, SRCCOPY); // 將內存DC的內容繪製到窗口DC
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_KEYDOWN: // 鍵盤按下訊息
        if (!KeyList[wParam]) // 如果按鍵未被記錄為按下
        {
            //writeFileAsync("consloe.txt", "[EXECUTE] KEY " + std::to_string(wParam) + " check");
            if (wParam == 'Q') {
                rotateBlock(1);
                RunBlockRe();
            }
            if (wParam == 'E') {
                rotateBlock(2);
                RunBlockRe();
            }
            if (wParam == 'A') {
                moveLeft();
                RunBlockRe();
            }
            if (wParam == 'S') {
                movedown();
                RunBlockRe();
            }
            if (wParam == 'D') {
                moveRight();
                RunBlockRe();
            }
            if (wParam == ' ') {
                //強降
                if (SdownCheck == 0) {
                    SdownCheck = 1;
                    while (SdownCheck == 1) {
                        movedown();
                        RunBlockRe();
                    }
                    SdownCheck = 0;
                }
                
            }
        }
        return 0;

    case WM_KEYUP: // 鍵盤放開訊息
        KeyList[wParam] = false; // 設置按鍵為未按下
        return 0;

    case WM_DESTROY:
        DeleteObject(hBitmap);
        DeleteObject(hBrushYellow);
        DeleteDC(hdcMem);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}



//時間
DWORD lastUpdateTime = 0; // 上次更新的時間
void UpdateElapsedSeconds()
{
    // 獲取當前時間（毫秒）
    DWORD currentTime = GetTickCount();

    // 計算與上次更新的時間差
    if (currentTime - lastUpdateTime >= 1000) // 如果超過 1 秒
    {
        Time++;        // 秒數加 1
        lastUpdateTime = currentTime; // 更新基準時間
    }
}
//TETRIS邏輯
// random defind
std::mt19937 gen(static_cast<unsigned int>(std::time(nullptr)));
int RandomNumber(int min, int max)
{
    std::uniform_int_distribution<> dist(min, max);
    return dist(gen);
}
// 旋轉矩陣
void rotateList(int ori[4][4], int out[4][4], int n)
{
    // 复制 ori 到 out，避免影响原始数据
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            out[i][j] = ori[i][j]; // 初始化 out 为 ori 的初始值
        }
    }

    // 旋转 n 次
    for (int na = 0; na < n; na++)
    {
        // 先进行转置并反转列
        int temp[4][4]; // 临时数组，用于存储旋转后的结果

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                temp[j][3 - i] = out[i][j]; // 转置并反转列
            }
        }

        // 将临时数组的结果复制到 out
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                out[i][j] = temp[i][j]; // 更新 out 为旋转后的结果
            }
        }
    }
}
// 陣列歸位
void moveShapeToTopLeft(int matrix[4][4])
{
    int rows = 4, cols = 4;

    // 找到非零值的邊界
    int top = rows, bottom = -1, left = cols, right = -1;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (matrix[i][j] != 0)
            {
                if (i < top)
                    top = i; // 更新上邊界
                if (i > bottom)
                    bottom = i; // 更新下邊界
                if (j < left)
                    left = j; // 更新左邊界
                if (j > right)
                    right = j; // 更新右邊界
            }
        }
    }

    // 創建一個新矩陣
    int newMatrix[4][4] = { 0 };

    // 將非零元素移到左上角
    int newRow = 0, newCol = 0;
    for (int i = top; i <= bottom; i++)
    {
        newCol = 0; // 重置列索引
        for (int j = left; j <= right; j++)
        {
            newMatrix[newRow][newCol] = matrix[i][j];
            newCol++;
        }
        newRow++;
    }

    // 將新矩陣的內容覆蓋回原矩陣
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            matrix[i][j] = newMatrix[i][j];
        }
    }
}
//陣列相同
bool arelistSame(int a[4][4], int b[4][4])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (a[i][j] != b[i][j])
            {
                return false; // 如果有任何一個元素不同，返回false
            }
        }
    }
    return true; // 所有元素都相同，返回true
}
// 陣列移動
void ListMove(int matrix[4][4], int direction, int steps)
{
    for (int n = 0; n < steps; n++)
    {
        if (direction == 1)
        {
            for (int i = 0; i < 4; i++)
            {
                if (matrix[i][3] != 0)
                    return;
            }
            for (int i = 0; i < 4; i++)
            {
                for (int ia = 3; ia >= 0; ia--)
                {
                    if (matrix[i][ia] == 1)
                    {
                        matrix[i][ia] = 0;
                        matrix[i][ia + 1] = 1;
                    }
                }
            }
        }
        if (direction == -1)
        {
            for (int i = 0; i < 4; i++)
            {
                if (matrix[i][0] != 0)
                    return;
            }
            for (int i = 0; i < 4; i++)
            {
                for (int ia = 0; ia < 4; ia++)
                {
                    if (matrix[i][ia] == 1)
                    {
                        matrix[i][ia] = 0;
                        matrix[i][ia - 1] = 1;
                    }
                }
            }
        }
    }
}
// runblock匯入tetrisblock
void runBlockReCheckToTetris()
{
    for (int i = 0; i < 20; i++)
    {
        for (int ia = 0; ia < 10; ia++)
        {
            if (TetrisBlock[i][ia] == 1 && checkTetrisBlock[i][ia] == 1 && RunBlock[i][ia] == 1)
            {
                End = true;
            }
            else if (TetrisBlock[i][ia] == 0 && checkTetrisBlock[i][ia] == 0 && RunBlock[i][ia] == 1)
            {
                TetrisBlock[i][ia] = 1;
            }
        }
    }
}
// 隨機方塊選取
void RandomBlock()
{
    int type = RandomBlockSave;
    // runblock初始化
    for (int i = 0; i < 20; i++)
    {
        for (int ia = 0; ia < 10; ia++)
        {
            RunBlock[i][ia] = 0;
        }
    }
    // blockC導入runblock
    for (int i = 0; i < 2; i++)
    {
        for (int ia = 0; ia < 4; ia++)
        {
            RunBlock[i][3 + ia] = blockC[type][i][ia];
        }
    }
    RandomBlockSave = RandomNumber(0, 6);
    runBlockReCheckToTetris();
}
// runblock更新
void RunBlockRe()
{
    for (int i = 0; i < 20; i++)
    {
        for (int ia = 0; ia < 10; ia++)
        {
            if (TetrisBlock[i][ia] == 1 && checkTetrisBlock[i][ia] != 1)
            {
                RunBlock[i][ia] = 1;
            }
            else if (TetrisBlock[i][ia] == 1 && checkTetrisBlock[i][ia] == 1)
            {
                RunBlock[i][ia] = 2;
            }
            else if (TetrisBlock[i][ia] == 0)
            {
                RunBlock[i][ia] = 0;
            }
        }
    }
}
// 轉動
void rotateBlock(int type)
{
    int BlockPOS[4] = { 20, 0, 0, 10 }; // 上、下、右、左初始值
    for (int n = 0; n < 20; n++)
    {
        for (int na = 0; na < 10; na++)
        {
            if (RunBlock[n][na] == 1)
            {
                if (n < BlockPOS[0])
                    BlockPOS[0] = n; // 上
                if (n > BlockPOS[1])
                    BlockPOS[1] = n; // 下
                if (na > BlockPOS[2])
                    BlockPOS[2] = na; // 右
                if (na < BlockPOS[3])
                    BlockPOS[3] = na; // 左
            }
        }
    }
    if (BlockType == 1)
        return;

    // 轉動方向與獲取範圍定義
    int GetBlock[4][4] = { 0 }, rotateBlock[4][4] = { 0 }, Bblock[4][4] = { 0 }, TetrisSave[16][2] = { -1 };
    int rotateType = 0;
    if (type == 2)
    {
        if (BlockPOS[2] == 9)
        {
            rotateType = 2;
        }
        else
        {
            rotateType = 1;
        }
    }
    else if (type == 1)
    {
        if (BlockPOS[3] == 0)
        {
            rotateType = 1;
        }
        else
        {
            rotateType = 2;
        }
    }
    int tetrisSaveCount = 0;
    int inPutTetrisPos[3] = { -1 };
    // 極限位置校準後填入限制表格 -- 並且旋轉表格
    if (rotateType == 0)
        return;
    if (rotateType == 1)
    {
        // 由左至右
        // 基準點匯入
        inPutTetrisPos[0] = BlockPOS[0];
        inPutTetrisPos[1] = BlockPOS[3];
        inPutTetrisPos[2] = 2;
        for (int Ly = 0; Ly <= 3; Ly++)
        {
            for (int Lx = 0; Lx <= 3; Lx++)
            {
                if (BlockPOS[0] + Ly < 20 && BlockPOS[3] + Lx <= 9)
                {
                    if (RunBlock[BlockPOS[0] + Ly][BlockPOS[3] + Lx] == 1)
                    {
                        GetBlock[Ly][Lx] = 1;
                        // tetris 原生方格獲取
                        TetrisSave[tetrisSaveCount][0] = BlockPOS[0] + Ly;
                        TetrisSave[tetrisSaveCount][1] = BlockPOS[3] + Lx;
                        tetrisSaveCount++;
                    }
                    if (RunBlock[BlockPOS[0] + Ly][BlockPOS[3] + Lx] == 2)
                    {
                        Bblock[Ly][Lx] = 1;
                    }
                }
            }
        }
        // 旋轉
        rotateList(GetBlock, rotateBlock, 1);
        // 歸位
        moveShapeToTopLeft(rotateBlock);
    }
    if (rotateType == 2)
    {
        // 邊界偵測

        // 由右至左
        // 基準點匯入
        inPutTetrisPos[0] = BlockPOS[0];
        inPutTetrisPos[1] = BlockPOS[2];
        inPutTetrisPos[2] = 1;
        for (int Ly = 0; Ly <= 3; Ly++)
        {
            for (int Lx = 0; Lx <= 3; Lx++)
            {
                if (BlockPOS[0] + Ly < 20 && BlockPOS[2] - Lx >= 0)
                {
                    if (RunBlock[BlockPOS[0] + Ly][BlockPOS[2] - Lx] == 1)
                    {
                        GetBlock[Ly][3 - Lx] = 1;
                        // tetris 原生方格獲取
                        TetrisSave[tetrisSaveCount][0] = BlockPOS[0] + Ly;
                        TetrisSave[tetrisSaveCount][1] = BlockPOS[2] - Lx;
                        tetrisSaveCount++;
                    }
                    if (RunBlock[BlockPOS[0] + Ly][BlockPOS[2] - Lx] == 2)
                    {
                        Bblock[Ly][3 - Lx] = 1;
                    }
                }
            }
        }
        // 旋轉
        rotateList(GetBlock, rotateBlock, 3);
        // 歸位
        moveShapeToTopLeft(rotateBlock);
        // 校準
        ListMove(rotateBlock, 1, 3);
    }
    // 阻擋方塊核對
    bool canRotate = false;
    for (int n = 0; n < 4; n++)
    {
        if (canRotate == false)
        {
            bool Acheck = true;
            for (int i = 0; i < 4; i++)
            {
                for (int ia = 0; ia < 4; ia++)
                {
                    if (Bblock[i][ia] == 1 && rotateBlock[i][ia] == 1)
                    {
                        Acheck = false;
                    }
                }
            }
            if (Acheck == true)
            {
                canRotate = true;
            }
            else
            {
                if (type == 1)
                    ListMove(rotateBlock, 1, 1);
                if (type == 2)
                    ListMove(rotateBlock, -1, 1);
            }
        }
        else
        {
            break;
        }
    }
    // 核對確認
    if (!canRotate)
        return;
    if (canRotate)
    {
        //   writeFileAsync("consloe.txt", "[TETRIS] rotateBlock " + inPutTetrisPos[2]);
        int DefCheck = 0;
        // 錯誤防護
        if (inPutTetrisPos[2] == 2)
        {
            for (int i = 0; i < 4; i++)
            {
                for (int ia = 0; ia < 4; ia++)
                {
                    // 邊界判斷
                    if (inPutTetrisPos[0] + i < 20 && inPutTetrisPos[1] + ia < 10)
                    {
                        if (rotateBlock[i][ia] == 1)
                        {
                            DefCheck++;
                        }
                    }
                }
            }
        }
        if (inPutTetrisPos[2] == 1)
        {
            for (int i = 0; i < 4; i++)
            {
                for (int ia = 3; ia >= 0; ia--)
                {
                    // 邊界判斷
                    if (inPutTetrisPos[0] + i < 20 && inPutTetrisPos[1] - ia >= 0)
                    {
                        if (rotateBlock[i][3 - ia] == 1)
                        {
                            DefCheck++;
                        }
                    }
                }
            }
        }
        if (DefCheck != tetrisSaveCount)
            return;
        // 原生方塊刪除
        for (int i = 0; i < 16; i++)
        {
            if (TetrisSave[i][0] != -1 && TetrisSave[i][1] != -1)
            {
                TetrisBlock[TetrisSave[i][0]][TetrisSave[i][1]] = 0;
            }
        }
        // 後生方塊填入
        // 基準點核查
        if (inPutTetrisPos[2] == 2)
        {
            for (int i = 0; i < 4; i++)
            {
                for (int ia = 0; ia < 4; ia++)
                {
                    // 邊界判斷
                    if (inPutTetrisPos[0] + i < 20 && inPutTetrisPos[1] + ia < 10)
                    {
                        // 陣列數據判斷
                        if (rotateBlock[i][ia] == 1)
                        {
                            TetrisBlock[inPutTetrisPos[0] + i][inPutTetrisPos[1] + ia] = 1;
                        }
                    }
                }
            }
        }
        if (inPutTetrisPos[2] == 1)
        {
            for (int i = 0; i < 4; i++)
            {
                for (int ia = 3; ia >= 0; ia--)
                {
                    // 邊界判斷
                    if (inPutTetrisPos[0] + i < 20 && inPutTetrisPos[1] - ia >= 0)
                    {
                        // 陣列數據判斷
                        if (rotateBlock[i][3 - ia] == 1)
                        {
                            TetrisBlock[inPutTetrisPos[0] + i][inPutTetrisPos[1] - ia] = 1;
                        }
                    }
                }
            }
        }
    }
}
// 向右移動
void moveRight()
{
    bool canMove = true;
    for (int i = 0; i < 20; i++)
    {
        for (int ia = 9; ia >= 0; ia--)
        {
            if (TetrisBlock[i][ia] == 1 && checkTetrisBlock[i][ia] != 1)
            {
                // 是否為邊界判斷
                if (ia == 9)
                {
                    canMove = false;
                }
                // 是否受到卡頓
                if (TetrisBlock[i][ia] == TetrisBlock[i][ia + 1] && TetrisBlock[i][ia + 1] == checkTetrisBlock[i][ia + 1])
                {
                    canMove = false;
                }
            }
        }
    }
    if (canMove == true)
    {
        //writeFileAsync("consloe.txt", "[TETRIS] moveRightBlock ");
        for (int i = 0; i < 20; i++)
        {
            for (int ia = 8; ia >= 0; ia--)
            {
                if (TetrisBlock[i][ia] == 1 && checkTetrisBlock[i][ia] != 1)
                {
                    // 顯示更新
                    TetrisBlock[i][ia] = 0;
                    TetrisBlock[i][ia + 1] = 1;
                }
            }
        }
    }
}
// 向左移動
void moveLeft()
{
    bool canMove = true;
    for (int i = 0; i < 20; i++)
    {
        for (int ia = 0; ia < 10; ia++)
        {
            if (TetrisBlock[i][ia] == 1 && checkTetrisBlock[i][ia] != 1)
            {
                // 是否為邊界判斷
                if (ia == 0)
                {
                    canMove = false;
                }
                // 是否受到卡頓
                if (TetrisBlock[i][ia] == TetrisBlock[i][ia - 1] && TetrisBlock[i][ia - 1] == checkTetrisBlock[i][ia - 1])
                {
                    canMove = false;
                }
            }
        }
    }
    // 換位
    if (canMove == true)
    {
        //writeFileAsync("consloe.txt", "[TETRIS] moveLeftBlock ");
        for (int i = 0; i < 20; i++)
        {
            for (int ia = 1; ia < 10; ia++)
            {
                if (TetrisBlock[i][ia] == 1 && checkTetrisBlock[i][ia] != 1)
                {
                    TetrisBlock[i][ia] = 0;
                    TetrisBlock[i][ia - 1] = 1;
                }
            }
        }
    }
}
void stringRemove()
{
    int stringAmount = 0;
    // 方塊刪除
    for (int i = 0; i < 20; i++)
    {
        int check = 0;
        for (int ia = 0; ia < 10; ia++)
        {
            if (TetrisBlock[i][ia] == 1)
            {
                check++;
            }
        }
        if (check == 10)
        { // 方塊刪除
            for (int ia = 0; ia < 10; ia++)
            {
                TetrisBlock[i][ia] = 0;
                checkTetrisBlock[i][ia] = 0;
            }
            AllString++;
            stringAmount++;
            removeString++;
            // 方塊總體墜落
            for (int ia = i - 1; ia >= 0; ia--)
            {
                for (int ib = 0; ib < 10; ib++)
                {
                    if (TetrisBlock[ia][ib] == 1 && checkTetrisBlock[ia][ib] == 1)
                    {
                        TetrisBlock[ia][ib] = 0;
                        checkTetrisBlock[ia][ib] = 0;
                        TetrisBlock[ia + 1][ib] = 1;
                        checkTetrisBlock[ia + 1][ib] = 1;
                    }
                }
            }
        }
    }
    if (stringAmount >= 1) {
        int stringScore[4] = { 40 , 100 , 300 , 1200 };
        scores += stringScore[stringAmount - 1] * (level + 1);
        if (removeString >= 10) {
            removeString -= 10;
            level++;
        }
    }
    
}
void checkblockIN()
{
    for (int i = 0; i < 20; i++)
    {
        for (int ia = 0; ia < 10; ia++)
        {
            if (TetrisBlock[i][ia] == 1)
            {
                checkTetrisBlock[i][ia] = 1;
            }
        }
    }
    blockSummon = true;
}
// 向下移動
void movedown()
{
    int downcheck = 0;
    bool breakC = false;
    for (int i = 19; i >= 0; i--)
    {
        for (int ia = 0; ia < 10; ia++)
        {
            if (TetrisBlock[i][ia] == 1 && checkTetrisBlock[i][ia] != 1)
            {
                if (i == 19 || checkTetrisBlock[i + 1][ia] == 1)
                {
                    downcheck = 2;
                    breakC = true;
                }
                else
                {
                    downcheck = 1;
                }
            }
        }
    }
    if (downcheck == 1 && breakC != true)
    {
        //writeFileAsync("consloe.txt", "[TETRIS] moveDwonBlock ");
        for (int i = 19; i >= 0; i--)
        {
            for (int ia = 0; ia < 10; ia++)
            {
                if (TetrisBlock[i][ia] == 1 && checkTetrisBlock[i][ia] != 1 && i != 19)
                {
                    TetrisBlock[i][ia] = 0;
                    TetrisBlock[i + 1][ia] = 1;
                }
            }
        }
    }
    else if (downcheck == 2 || breakC == true)
    {
        if (SdownCheck == 1) {
            SdownCheck = 2;
        }
        checkblockIN();
        stringRemove();
    }
    else if (downcheck == 0)
    {
    }
}