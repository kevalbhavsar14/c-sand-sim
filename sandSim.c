#include <stdio.h>
#include <stdbool.h>

#include "raylib.h"

#define WIDTH 200
#define HEIGHT 100

#define BACKGROUND_COLOR CLITERAL(Color) {0x18, 0x18, 0x18, 0xFF}

#define DIR_DOWN        CLITERAL(Vector2Int) { 0,  1}
#define DIR_UP          CLITERAL(Vector2Int) { 0, -1}
#define DIR_LEFT        CLITERAL(Vector2Int) {-1,  0}
#define DIR_RIGHT       CLITERAL(Vector2Int) { 1,  0}
#define DIR_UP_LEFT     CLITERAL(Vector2Int) {-1, -1}
#define DIR_UP_RIGHT    CLITERAL(Vector2Int) { 1, -1}
#define DIR_DOWN_LEFT   CLITERAL(Vector2Int) {-1,  1}
#define DIR_DOWN_RIGHT  CLITERAL(Vector2Int) { 1,  1}

typedef struct Vector2Int
{
    int x, y;
} Vector2Int;

typedef enum CellType
{
    CELLTYPE_AIR,
    CELLTYPE_SOLID,
    CELLTYPE_SAND,
    CELLTYPE_WATER,
    CELLTYPE_COUNT
}
CellType;

typedef struct CellUpdateRule
{
    Vector2Int* dirs;
    int dirsCount;
    CellType* displacables;
    int displacablesCount;
    Vector2Int* displaceDirs;
    int displaceDirsCount;
} 
CellUpdateRule;

Color cellColors[] = {
    /* CELLTYPE_AIR   */ CLITERAL(Color) {0x00, 0x00, 0x00, 0xFF},
    /* CELLTYPE_SOLID */ CLITERAL(Color) {0x64, 0x64, 0x64, 0xFF},
    /* CELLTYPE_SAND  */ CLITERAL(Color) {0xFF, 0xFF, 0x00, 0xFF},
    /* CELLTYPE_WATER */ CLITERAL(Color) {0x00, 0x00, 0xFF, 0xFF},
};

Vector2Int sandDirs[] = {DIR_DOWN, DIR_DOWN_LEFT, DIR_DOWN_RIGHT};
Vector2Int waterDirs[] = {DIR_DOWN, DIR_DOWN_LEFT, DIR_DOWN_RIGHT, DIR_LEFT, DIR_RIGHT};

Vector2Int sandDisplaceDirs[] = {DIR_LEFT, DIR_RIGHT, DIR_UP_LEFT, DIR_UP_RIGHT, DIR_UP};
Vector2Int waterDisplaceDirs[] = {DIR_UP_LEFT, DIR_UP_RIGHT, DIR_UP};

CellType defaultDisplacables[] = {CELLTYPE_AIR};
CellType sandDisplacables[] = {CELLTYPE_AIR, CELLTYPE_WATER};

const CellUpdateRule cellRules[] = {
    /* CELLTYPE_AIR   */ {NULL, 0, NULL, 0, NULL, 0},
    /* CELLTYPE_SOLID */ {NULL, 0, NULL, 0, NULL, 0},
    /* CELLTYPE_SAND  */ {sandDirs, 3, sandDisplacables, 2, sandDisplaceDirs, 5},
    /* CELLTYPE_WATER */ {waterDirs, 5, defaultDisplacables, 2, NULL, 0},
};

CellType cells[HEIGHT][WIDTH] = {0};

void updateCells();

int main(void)
{
    int screenWidth = 800;
    int screenHeight = 450;

    const int cellSize = screenWidth / WIDTH;

    InitWindow(screenWidth, screenHeight, "Sand sim");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    bool playing = true;

    int brushSize = 1;
    int brushSizeMin = 1;
    int brushSizeMax = 10;
    CellType brush = CELLTYPE_SAND;

    int brushSelectorSize = 30;
    int brushSelectorPadding = 10;
    int brushSizeSliderWidth = 100;
    int brushSizeSliderHeight = 10;
    int brushSizeKnobRadius = 15;
    int brushSizeSliderPadding = 20;

    bool movingSlider = false;
    char textBuffer[256];

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();
        Vector2 mousePosition = GetMousePosition();
        if (IsKeyPressed(KEY_SPACE)) playing = !playing;
        if (!playing && IsKeyPressed(KEY_ENTER)) updateCells();

        for (int i = 0; i < CELLTYPE_COUNT; i++)
        {
            Rectangle rect = {
                i * (brushSelectorSize + brushSelectorPadding) + brushSelectorPadding, 
                screenHeight - (brushSelectorSize + brushSelectorPadding),
                brushSelectorSize,
                brushSelectorSize,
            };
            if (IsKeyPressed(KEY_ONE + i) || 
                (CheckCollisionPointRec(mousePosition, rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)))
            {
                brush = i;
            }
        }
        

        int brushX = mousePosition.x / cellSize;
        int brushY = mousePosition.y / cellSize;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && 
            0 <= brushX && brushX < WIDTH && 
            0 <= brushY && brushY < HEIGHT)
        {
            for (int dy = 0; dy < brushSize; dy++)
            {
                for (int dx = 0; dx < brushSize; dx++)
                {
                    int x = dx + brushX - brushSize / 2;
                    int y = dy + brushY - brushSize / 2;
                    if (0 <= x && x < WIDTH && 0 <= y && y < HEIGHT)
                        cells[y][x] = brush;
                }
            }
        } 
        // Draw
        BeginDrawing();
        {
            ClearBackground(BACKGROUND_COLOR);
            
            // Cells
            for (int y = 0; y < HEIGHT; y++)
            {
                for (int x = 0; x < WIDTH; x++)
                {
                    Color color = cellColors[cells[y][x]];
                    DrawRectangle(x * cellSize, y * cellSize, cellSize, cellSize, color);
                }
                
            }
            
            // ----- GUI -----
            // brush
            if (0 <= brushX && brushX < WIDTH && 
                0 <= brushY && brushY < HEIGHT)
            {
                DrawRectangleLines((brushX - brushSize / 2) * cellSize, 
                    (brushY - brushSize / 2) * cellSize, 
                    brushSize * cellSize, 
                    brushSize * cellSize,
                    WHITE);
            }

            // brush selector
            for (int i = 0; i < CELLTYPE_COUNT; i++)
            {
                Rectangle rect = {
                    i * (brushSelectorSize + brushSelectorPadding) + brushSelectorPadding, 
                    screenHeight - (brushSelectorSize + brushSelectorPadding),
                    brushSelectorSize,
                    brushSelectorSize,
                };
                DrawRectangleRec(rect, cellColors[i]);
                if (brush == (CellType) i) DrawRectangleLinesEx(rect, 2, WHITE);
            }

            // brush size slider
            int sliderX = screenWidth - brushSizeSliderWidth - brushSizeSliderPadding;
            int sliderY = screenHeight - brushSizeSliderHeight - brushSizeSliderPadding;
            Vector2 knobCenter = {
                (brushSize - brushSizeMin) / (float)(brushSizeMax - brushSizeMin) * brushSizeSliderWidth + sliderX,
                sliderY + brushSizeKnobRadius / 2
            };
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) 
                && CheckCollisionPointCircle(mousePosition, knobCenter, brushSizeKnobRadius)) 
                movingSlider = true;
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) movingSlider = false;

            if (movingSlider)
            {
                brushSize = (mousePosition.x - sliderX) / (float)brushSizeSliderWidth * (brushSizeMax - brushSizeMin) + brushSizeMin;
                if (brushSize < brushSizeMin) brushSize = brushSizeMin;
                if (brushSize > brushSizeMax) brushSize = brushSizeMax;
            }

            DrawRectangle(sliderX,
                sliderY, 
                brushSizeSliderWidth,
                brushSizeSliderHeight,
                GRAY);
            
            DrawCircle((brushSize - brushSizeMin) / (float)(brushSizeMax - brushSizeMin) * brushSizeSliderWidth + sliderX,
                sliderY + brushSizeKnobRadius / 2,
                brushSizeKnobRadius,
                WHITE);

            sprintf(textBuffer, "%d", brushSize);
            DrawText(textBuffer, sliderX - 30, sliderY, 15, WHITE);

            // Controls
            DrawText(playing ? "[space] Pause" : "[space] Play", 10, 10, 20, GRAY);
            DrawText("[enter] Update", 10, 30, 20, GRAY);
        }
        EndDrawing();
        if (playing)
            updateCells();
    }

    CloseWindow();
    return 0;
}

void updateCells()
{
    for (int y = HEIGHT - 1; y >= 0; y--)
    {
        for (int x = WIDTH - 1; x >= 0; x--)
        {
            for (int i = 0; i < cellRules[cells[y][x]].dirsCount; i++)
            {
                Vector2Int dir = cellRules[cells[y][x]].dirs[i];
                if (0 > x + dir.x || x + dir.x >= WIDTH || 
                    0 > y + dir.y || y + dir.y >= HEIGHT)
                {
                    continue;   
                }
                bool canMove = false;
                for (int j = 0; j < cellRules[cells[y][x]].displacablesCount; j++)
                {
                    if (cells[y + dir.y][x + dir.x] == cellRules[cells[y][x]].displacables[j])
                    {
                        canMove = true;
                        break;
                    }
                }
                if (!canMove) continue;
                CellType temp = cells[y + dir.y][x + dir.x];
                cells[y + dir.y][x + dir.x] = cells[y][x];
                cells[y][x] = CELLTYPE_AIR;
                for (int j = 0; j < cellRules[cells[y + dir.y][x + dir.x]].displaceDirsCount; j++)
                {
                    Vector2Int displaceDir = cellRules[cells[y + dir.y][x + dir.x]].displaceDirs[j];
                    if (cells[y + dir.y + displaceDir.y][x + dir.x + displaceDir.x] == CELLTYPE_AIR)
                    {
                        cells[y + dir.y + displaceDir.y][x + dir.x + displaceDir.x] = temp;
                        break;
                    }
                }
                break;
            }
        }
        
    }
}
