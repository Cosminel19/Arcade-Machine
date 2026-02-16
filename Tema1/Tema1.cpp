#include "lab_m1/Tema1/Tema1.h"

#include <iostream>
#include <vector>
#include <queue>
#include <math.h>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"

using namespace std;
using namespace m1;


Tema1::Tema1()
{
    isDragging = false;
    draggedBlockX = 0;
    draggedBlockY = 0;
    inNewScene = false;

    gridBlocks.resize(7, vector<bool>(15, false));
    placedBlockPositions.resize(7, vector<glm::vec2>(15, glm::vec2(-1, -1)));

    nrBlocks = 10;
    leftScore = 0;
    rightScore = 0;


    ballSpeed = 700.0f;
    ballRadius = 15.0f;
    ballVX = ballSpeed;
    ballVY = 0;
    ballX = 0;
    ballY = 0;

    gameStarted = false;
    gameOver = false;

    shakeTime = 0;
    shakeStrength = 0;
    shakeMatrix = glm::mat3(1);

    leftPaddleScaleTime = 0;
    rightPaddleScaleTime = 0;
    paddleScaleDuration = 1.0f;
}

Tema1::~Tema1()
{

}

Mesh* Tema1::CreateBallMesh(string name, glm::vec3 color)
{
    vector<VertexFormat> vertices;
    vector<unsigned int> indices;

    float radius = ballRadius;
    int segments = 20;

    vertices.push_back(VertexFormat(glm::vec3(0, 0, 5), color));

    for (int i = 1; i <= segments; i++) {
        float angle = 2 * M_PI * (i - 1) / segments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        vertices.push_back(VertexFormat(glm::vec3(x, y, 5), color));
    }

    for (int i = 1; i <= segments; i++) {
        indices.push_back(0);
        indices.push_back(i);
        if (i != segments) {
            indices.push_back(i + 1);
        }
        else {
            indices.push_back(1);
        }
    }

    Mesh* ball = new Mesh(name);
    ball->InitFromData(vertices, indices);
    AddMeshToList(ball);

    return ball;
}

void Tema1::StartCameraShake(float duration, float strength)
{
    shakeTime = duration;
    shakeStrength = strength;
}

void Tema1::Init()
{
    auto camera = GetSceneCamera();
    GetCameraInput()->SetActive(false);

    glm::ivec2 resolution = window->GetResolution();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();

    {
        vector<VertexFormat> redVertices = {
            VertexFormat(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3(200, 0, 0), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3(200, 200, 0), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3(0, 200, 0), glm::vec3(1, 0, 0))
        };

        vector<unsigned int> redIndices = { 0, 1, 2, 3 };

        Mesh* redOutline = new Mesh("redOutline");
        redOutline->SetDrawMode(GL_LINE_LOOP);
        redOutline->InitFromData(redVertices, redIndices);
        AddMeshToList(redOutline);
    }

    {
        vector<VertexFormat> blueVertices = {
            VertexFormat(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3(250, 0, 0), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3(250, 200, 0), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3(0, 200, 0), glm::vec3(0, 0, 1))
        };

        vector<unsigned int> blueIndices = { 0, 1, 2, 3 };

        Mesh* blueOutline = new Mesh("blueOutline");
        blueOutline->SetDrawMode(GL_LINE_LOOP);
        blueOutline->InitFromData(blueVertices, blueIndices);
        AddMeshToList(blueOutline);
    }

    {
        vector<VertexFormat> greenVertices = {
            VertexFormat(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)),
            VertexFormat(glm::vec3(100, 0, 0), glm::vec3(0, 1, 0)),
            VertexFormat(glm::vec3(100, 100, 0), glm::vec3(0, 1, 0)),
            VertexFormat(glm::vec3(0, 100, 0), glm::vec3(0, 1, 0))
        };

        vector<unsigned int> greenIndices =
        {
            0, 1, 2,
            0, 2, 3
        };

        Mesh* greenSquare = new Mesh("greenSquare");
        greenSquare->InitFromData(greenVertices, greenIndices);
        AddMeshToList(greenSquare);
    }

    {
        vector<VertexFormat> grayVertices = {
            VertexFormat(glm::vec3(0, 0, 2), glm::vec3(0.5f, 0.5f, 0.5f)),
            VertexFormat(glm::vec3(100, 0, 2), glm::vec3(0.5f, 0.5f, 0.5f)),
            VertexFormat(glm::vec3(100, 100, 2), glm::vec3(0.5f, 0.5f, 0.5f)),
            VertexFormat(glm::vec3(0, 100, 2), glm::vec3(0.5f, 0.5f, 0.5f)),
            VertexFormat(glm::vec3(50, 50, 2), glm::vec3(1.0f, 1.0f, 1.0f))
        };

        vector<unsigned int> grayIndices = {
            0, 1, 4,
            1, 2, 4,
            2, 3, 4,
            3, 0, 4
        };

        Mesh* grayBlock = new Mesh("grayBlock");
        grayBlock->InitFromData(grayVertices, grayIndices);
        AddMeshToList(grayBlock);
    }

    {
        vector<VertexFormat> whiteVertices = {
            VertexFormat(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)),
            VertexFormat(glm::vec3(200, 0, 0), glm::vec3(1, 1, 1)),
            VertexFormat(glm::vec3(200, 200, 0), glm::vec3(1, 1, 1)),
            VertexFormat(glm::vec3(0, 200, 0), glm::vec3(1, 1, 1))
        };

        vector<unsigned int> whiteIndices = {0, 1, 2, 3};

        Mesh* whiteOutline = new Mesh("whiteOutline");
        whiteOutline->SetDrawMode(GL_LINE_LOOP);
        whiteOutline->InitFromData(whiteVertices, whiteIndices);
        AddMeshToList(whiteOutline);
    }

    {
        vector<VertexFormat> verts = {
            VertexFormat(glm::vec3(0, 0, 0), glm::vec3(0.58f, 0, 0.82f)),
            VertexFormat(glm::vec3(0, 1, 0), glm::vec3(0.58f, 0, 0.82f))
        };

        vector<unsigned int> inds = {0, 1};

        Mesh* vline = new Mesh("verticalLine");
        vline->SetDrawMode(GL_LINES);
        vline->InitFromData(verts, inds);
        AddMeshToList(vline);
    }

    CreateBallMesh("ballOrange", glm::vec3(1, 0.5f, 0));
    CreateBallMesh("ballRed", glm::vec3(1, 0, 0));
    CreateBallMesh("ballYellow", glm::vec3(1, 1, 0));

    {
        vector<VertexFormat> redV = {
            VertexFormat(glm::vec3(0, 0, 2), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3(100, 0, 2), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3(100, 100, 2), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3(0, 100, 2), glm::vec3(1, 0, 0))
        };

        vector<unsigned int> redIdx = {
            0, 1, 2,
            0, 2, 3
        };

        Mesh* leftBlock = new Mesh("leftPaddleBlock");
        leftBlock->InitFromData(redV, redIdx);
        AddMeshToList(leftBlock);
    }

    {
        vector<VertexFormat> yellowV = {
            VertexFormat(glm::vec3(0, 0, 2), glm::vec3(1, 1, 0)),
            VertexFormat(glm::vec3(100, 0, 2), glm::vec3(1, 1, 0)),
            VertexFormat(glm::vec3(100, 100, 2), glm::vec3(1, 1, 0)),
            VertexFormat(glm::vec3(0, 100, 2), glm::vec3(1, 1, 0))
        };

        vector<unsigned int> yellowIdx = {
            0, 1, 2,
            0, 2, 3
        };

        Mesh* rightBlock = new Mesh("rightPaddleBlock");
        rightBlock->InitFromData(yellowV, yellowIdx);
        AddMeshToList(rightBlock);
    }
}


void Tema1::FrameStart()
{
}


void Tema1::Update(float deltaTimeSeconds)
{
    if (shakeTime > 0)
    {
        float offsetX = ((rand() % 100) / 100.0f - 0.5f) * shakeStrength;
        float offsetY = ((rand() % 100) / 100.0f - 0.5f) * shakeStrength;

        shakeMatrix = transform2D::Translate(offsetX, offsetY);
        shakeTime -= deltaTimeSeconds;
    }

    if (leftPaddleScaleTime > 0) {
        leftPaddleScaleTime -= deltaTimeSeconds;
        if (leftPaddleScaleTime < 0) leftPaddleScaleTime = 0;
    }

    if (rightPaddleScaleTime > 0) {
        rightPaddleScaleTime -= deltaTimeSeconds;
        if (rightPaddleScaleTime < 0) rightPaddleScaleTime = 0;
    }

    glm::ivec2 resolution = window->GetResolution();

    if (!inNewScene) {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, resolution.x, resolution.y);

        DrawLeftPanel();
        DrawMainFrame();
        DrawGridFrame();
        DrawSquares();
        DrawStartButton(nrBlocks);
        DrawGrid();
        DrawGrayBlock();
        DrawPlacedBlocks();

        if (isDragging) {
            DrawDraggedBlock();
        }
    }
    else {
        static bool firstTime = true;
        if (firstTime) {
            ResetBall();
            firstTime = false;
        }

        glClearColor(0, 0, 0.15f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, resolution.x, resolution.y);

        DrawWhiteFrame();
        DrawCenterDashedLine();

        DrawShapeAt(0.2 * resolution.x, leftPaddleY, "leftPaddleBlock");
        DrawShapeAt(0.8 * resolution.x, rightPaddleY, "rightPaddleBlock");

        UpdateBall(deltaTimeSeconds);

        DrawScore();
        DrawBall(ballX, ballY);
    }
}

void Tema1::DrawLeftPanel()
{
    glm::ivec2 resolution = window->GetResolution();

    float totalHeight = (float)resolution.y;
    float totalWidth = resolution.x * 0.20f;
    float sectionHeight = totalHeight / 4.0f;

    for (int i = 0; i < 4; i++) {
        glm::mat3 modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Scale(totalWidth / 200.0f, sectionHeight / 200.0f);
        modelMatrix *= transform2D::Translate(0, i * 200);

        glLineWidth(5.0f);
        RenderMesh2D(meshes["redOutline"], shaders["VertexColor"], modelMatrix);
        glLineWidth(1.0f);
    }
}

void Tema1::DrawMainFrame()
{
    glm::ivec2 resolution = window->GetResolution();

    float totalWidth = resolution.x * 0.80f;
    float totalHeight = (float)resolution.y;

    glm::mat3 modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(resolution.x * 0.20f, 0);
    modelMatrix *= transform2D::Scale(totalWidth / 200.0f, totalHeight / 200.0f);

    glLineWidth(5.0f);
    RenderMesh2D(meshes["redOutline"], shaders["VertexColor"], modelMatrix);
    glLineWidth(1.0f);
}

void Tema1::DrawGridFrame()
{
    glm::ivec2 resolution = window->GetResolution();

    float startX = 0.25f * resolution.x;
    float startY = 0.05f * resolution.y;
    float width = 0.7f * resolution.x;
    float height = 0.7f * resolution.y;

    glm::mat3 modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(startX, startY);
    modelMatrix *= transform2D::Scale(width / 250.0f, height / 200.0f);

    RenderMesh2D(meshes["blueOutline"], shaders["VertexColor"], modelMatrix);
}


void Tema1::DrawSquares()
{
    glm::ivec2 resolution = window->GetResolution();

    float startX = 0.22f * resolution.x;
    float startY = 0.875f * resolution.y;
    float squareSize = 50.0f;

    for (int i = 0; i < nrBlocks; i++) {
        glm::mat3 modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(startX, startY);
        modelMatrix *= transform2D::Scale(squareSize / 100.0f, squareSize / 100.0f);

        RenderMesh2D(meshes["greenSquare"], shaders["VertexColor"], modelMatrix);

        startX += 0.07f * resolution.x;
    }
}

void Tema1::DrawStartButton(int nrBlocks)
{
    glm::ivec2 resolution = window->GetResolution();

    float startX = 0.92f * resolution.x;
    float startY = 0.875f * resolution.y;
    float squareSize = 50.0f;

    bool conex = IsShapeConnected();
    bool vertical = IsVerticalPaddle();

    glm::vec3 color;

    if (nrBlocks == 10 || nrBlocks < 0) {
        color = glm::vec3(1, 0, 0);
    }
    else if (conex && vertical) {
        color = glm::vec3(0, 1, 0);
    }
    else {
        color = glm::vec3(1, 0, 0);
    }

    vector<VertexFormat> startButtonVertices = {
        VertexFormat(glm::vec3(0, 0, 0), color),
        VertexFormat(glm::vec3(100, 0, 0), color),
        VertexFormat(glm::vec3(100, 100, 0), color),
        VertexFormat(glm::vec3(0, 100, 0), color)
    };

    vector<unsigned int> startButtonIndices =
    {
        0, 1, 3,
        0, 2, 3
    };

    Mesh* startButton = new Mesh("startButton");
    startButton->InitFromData(startButtonVertices, startButtonIndices);

    glm::mat3 modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(startX, startY);
    modelMatrix *= transform2D::Scale(squareSize / 100.0f, squareSize / 100.0f);

    RenderMesh2D(startButton, shaders["VertexColor"], modelMatrix);
}

void Tema1::DrawGrid()
{
    glm::ivec2 resolution = window->GetResolution();

    float startX = 0.2625f * resolution.x;
    float startY = 0.075f * resolution.y;
    float width = 0.675f * resolution.x;
    float height = 0.65f * resolution.y;

    float squareSize = 50.0f;

    float cellWidth = width / 15;
    float cellHeight = height / 7;

    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 15; col++) {
            float x = startX + col * cellWidth;
            float y = startY + row * cellHeight;

            float cx = x + cellWidth / 2;
            float cy = y + cellHeight / 2;

            glm::mat3 modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(cx, cy);
            modelMatrix *= transform2D::Scale(squareSize / 150.0f, squareSize / 150.0f);
            modelMatrix *= transform2D::Translate(-50.0f, -50.f);

            RenderMesh2D(meshes["greenSquare"], shaders["VertexColor"], modelMatrix);
        }
    }
}

void Tema1::DrawGrayBlock()
{
    glm::ivec2 resolution = window->GetResolution();

    float blockSize = 100.0f;
    float scaleFactor = 50.0f / 100.0f;

    float panelX = 0.0f;
    float panelY = 0.0f;
    float panelWidth = 0.2f * resolution.x;
    float panelHeight = 0.25f * resolution.y;

    float blockX = panelX + panelWidth / 2.0f;
    float blockY = panelY + panelHeight / 2.0f;

    glm::mat3 modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(blockX - (blockSize * scaleFactor) / 2,
                                          blockY - (blockSize * scaleFactor) / 2);
    modelMatrix *= transform2D::Scale(scaleFactor, scaleFactor);

    RenderMesh2D(meshes["grayBlock"], shaders["VertexColor"], modelMatrix);
}

void Tema1::DrawDraggedBlock()
{
    glm::ivec2 resolution = window->GetResolution();

    float startX = 0.2625f * resolution.x;
    float startY = 0.075f * resolution.y;
    float gridWidth = 0.675f * resolution.x;
    float gridHeight = 0.65f * resolution.y;

    int cols = 15;
    int rows = 7;

    float cellWidth = gridWidth / cols;
    float cellHeight = gridHeight / rows;

    float cellSize = glm::min(cellWidth, cellHeight);
    float meshSize = 100.0f;
    float scale = cellSize / meshSize;

    glm::mat3 modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(draggedBlockX - (meshSize * scale) / 2.0f,
                                          draggedBlockY - (meshSize * scale) / 2.0f);
    modelMatrix *= transform2D::Scale(scale, scale);

    RenderMesh2D(meshes["grayBlock"], shaders["VertexColor"], modelMatrix);
}

void Tema1::DrawPlacedBlocks()
{
    glm::ivec2 resolution = window->GetResolution();

    float startX = 0.2625f * resolution.x;
    float startY = 0.075f * resolution.y;
    float width = 0.675f * resolution.x;
    float height = 0.65f * resolution.y;

    int cols = 15;
    int rows = 7;

    float cellWidth = width / cols;
    float cellHeight = height / rows;

    float meshSize = 100.0f;
    float scaleX = cellWidth / meshSize;
    float scaleY = cellHeight / meshSize;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {

            if (!gridBlocks[row][col]) continue;

            float x = startX + col * cellWidth;
            float y = startY + row * cellHeight;

            float cx = x + cellWidth / 2.0f;
            float cy = y + cellHeight / 2.0f;

            glm::mat3 modelMatrix = glm::mat3(1);

            modelMatrix *= transform2D::Translate(cx, cy);
            modelMatrix *= transform2D::Scale(scaleX, scaleY);
            modelMatrix *= transform2D::Translate(-meshSize / 2.0f, -meshSize / 2.0f);

            RenderMesh2D(meshes["grayBlock"], shaders["VertexColor"], modelMatrix);
        }
    }
}

bool Tema1::IsMouseOverGrayBlock(int mouseX, int mouseY)
{
    glm::ivec2 resolution = window->GetResolution();

    float glMouseY = resolution.y - mouseY;

    float panelWidth = 0.2f * resolution.x;
    float panelHeight = 0.25f * resolution.y;

    float blockX = panelWidth / 2.0f;
    float blockY = panelHeight / 2.0f;
    float blockSize = 50.0f;

    float minX = blockX - blockSize / 2;
    float maxX = blockX + blockSize / 2;
    float minY = blockY - blockSize / 2;
    float maxY = blockY + blockSize / 2;

    return (mouseX >= minX && mouseX <= maxX && glMouseY >= minY && glMouseY <= maxY);
}

void Tema1::SnapToGrid()
{
    glm::ivec2 resolution = window->GetResolution();

    float gridStartX = 0.2625f * resolution.x;
    float gridStartY = 0.075f * resolution.y;
    float gridWidth = 0.675f * resolution.x;
    float gridHeight = 0.65f * resolution.y;

    int cols = 15;
    int rows = 7;

    float cellWidth = gridWidth / cols;
    float cellHeight = gridHeight / rows;

    if (draggedBlockX < gridStartX || draggedBlockX > gridStartX + gridWidth ||
        draggedBlockY < gridStartY || draggedBlockY > gridStartY + gridHeight) {
        isDragging = false;
        return;
    }

    int col = (int)((draggedBlockX - gridStartX) / cellWidth);
    int row = (int)((draggedBlockY - gridStartY) / cellHeight);

    if (col < 0 || col >= cols || row < 0 || row >= rows) {
        isDragging = false;
        return;
    }

    if (gridBlocks[row][col]) {
        isDragging = false;
        return;
    }

    gridBlocks[row][col] = true;

    float centerX = gridStartX + col * cellWidth + cellWidth / 2.0f;
    float centerY = gridStartY + row * cellHeight + cellHeight / 2.0f;

    placedBlockPositions[row][col] = glm::vec2(centerX, centerY);
    nrBlocks--;
    isDragging = false;
}

bool Tema1::IsShapeConnected()
{
    int rows = (int)gridBlocks.size();
    int cols = (int)gridBlocks[0].size();

    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));

    int startRow = -1, startCol = -1;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (gridBlocks[r][c]) {
                startRow = r;
                startCol = c;
                break;
            }
        }
        if (startRow != -1) break;
    }

    if (startRow == -1) return false;

    std::queue<std::pair<int, int>> q;
    q.push(std::make_pair(startRow, startCol));
    visited[startRow][startCol] = true;

    int connectedCount = 1;
    int totalBlocks = 0;
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            if (gridBlocks[r][c]) totalBlocks++;

    const int dr[4] = {1, -1, 0, 0};
    const int dc[4] = {0, 0, 1, -1};

    while (!q.empty()) {
        std::pair<int, int> curr = q.front();
        q.pop();
        int r = curr.first;
        int c = curr.second;

        for (int k = 0; k < 4; k++) {
            int nr = r + dr[k];
            int nc = c + dc[k];

            if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;
            if (!gridBlocks[nr][nc]) continue;
            if (visited[nr][nc]) continue;

            visited[nr][nc] = true;
            q.push(std::make_pair(nr, nc));
            connectedCount++;
        }
    }

    return connectedCount == totalBlocks;
}

bool Tema1::IsVerticalPaddle()
{
    int rows = (int)gridBlocks.size();
    int cols = (int)gridBlocks[0].size();

    int paddleCol = -1;
    int blockCount = 0;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (gridBlocks[r][c]) {
                blockCount++;

                if (paddleCol == -1) {
                    paddleCol = c;
                }
                else if (c != paddleCol) {
                    return false;
                }
            }
        }
    }

    if (blockCount < 2 || blockCount > 3) {
        return false;
    }
    
    return true;
}

bool Tema1::IsMouseOverStartButton(int mouseX, int mouseY)
{
    glm::ivec2 resolution = window->GetResolution();

    int glMouseY = resolution.y - mouseY;

    float startX = 0.92f * resolution.x;
    float startY = 0.875f * resolution.y;
    float squareSize = 50.0f;

    float minX = startX;
    float maxX = startX + squareSize;
    float minY = startY;
    float maxY = startY + squareSize;

    if (mouseX < minX || mouseX > maxX || glMouseY < minY || glMouseY > maxY) {
        return false;
    }

    float relX = mouseX - startX;
    float relY = glMouseY - startY;

    float normX = relX / squareSize;
    float normY = relY / squareSize;

    bool inTri1 = (normX + normY <= 1.0f);
    bool inTri2 = (normY >= normX);

    return inTri1 || inTri2;
}

void Tema1::DrawShapeAt(float offsetX, float offsetY, const string& meshName)
{
    float meshSize = 100.0f;

    glm::ivec2 resolution = window->GetResolution();

    float startX = 0.2625f * resolution.x;
    float startY = 0.075f * resolution.y;
    float cellWidth = (0.675f * resolution.x) / 15.0f;
    float cellHeight = (0.65f * resolution.y) / 7.0f;

    float scaleX = cellWidth / meshSize;
    float scaleY = cellHeight / meshSize;

    float minCX = 1e9, minCY = 1e9;

    for (int r = 0; r < 7; r++) {
        for (int c = 0; c < 15; c++) {

            if (!gridBlocks[r][c]) continue;

            float x = startX + c * cellWidth;
            float y = startY + r * cellHeight;

            float cx = x + cellWidth / 2.0f;
            float cy = y + cellHeight / 2.0f;

            if (cx < minCX) minCX = cx;
            if (cy < minCY) minCY = cy;
        }
    }

    float animScale = 1.0f;

    if (meshName == "leftPaddleBlock" && leftPaddleScaleTime > 0) {
        float progress = leftPaddleScaleTime / paddleScaleDuration;
        animScale = 1.0f + 0.2f * sin(progress * M_PI * 4);
    }

    if (meshName == "rightPaddleBlock" && rightPaddleScaleTime > 0) {
        float progress = rightPaddleScaleTime / paddleScaleDuration;
        animScale = 1.0f + 0.2f * sin(progress * M_PI * 4);
    }

    for (int r = 0; r < 7; r++) {
        for (int c = 0; c < 15; c++) {

            if (!gridBlocks[r][c]) continue;

            float x = startX + c * cellWidth;
            float y = startY + r * cellHeight;

            float cx = x + cellWidth / 2.0f;
            float cy = y + cellHeight / 2.0f;

            float localX = cx - minCX;
            float localY = cy - minCY;

            glm::mat3 modelMatrix = glm::mat3(1);

            modelMatrix *= transform2D::Translate(offsetX + localX, offsetY + localY);
            modelMatrix *= transform2D::Scale(scaleX * animScale, scaleY * animScale);
            modelMatrix *= transform2D::Translate(-meshSize / 2.0f, -meshSize / 2.0f);

            RenderMesh2D(meshes[meshName], shaders["VertexColor"], shakeMatrix * modelMatrix);
        }
    }
}

void Tema1::DrawWhiteFrame()
{
    glm::ivec2 resolution = window->GetResolution();

    float startX = 0.15f * resolution.x;
    float startY = 0.05f * resolution.y;
    float width = 0.7f * resolution.x;
    float height = 0.9f * resolution.y;

    glm::mat3 modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(startX, startY);
    modelMatrix *= transform2D::Scale(width / 200.0f, height / 200.0f);

    glLineWidth(5.0f);
    RenderMesh2D(meshes["whiteOutline"], shaders["VertexColor"], shakeMatrix * modelMatrix);
    glLineWidth(1.0f);
}

void Tema1::DrawCenterDashedLine()
{
    glm::ivec2 resolution = window->GetResolution();

    float startX = 0.15f * resolution.x;
    float startY = 0.05f * resolution.y;
    float width = 0.7f * resolution.x;
    float height = 0.9f * resolution.y;

    float centerX = startX + width / 2.0f;

    float dashLength = 42.0f;
    float gapLength = 20.0f;

    float y = startY;

    while (y <= startY + height)
    {
        float yEnd = y + dashLength;
        if (yEnd > startY + height)
            yEnd = startY + height;

        glm::mat3 modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(centerX, y);
        modelMatrix *= transform2D::Scale(1, yEnd - y);

        glLineWidth(5.0f);
        RenderMesh2D(meshes["verticalLine"], shaders["VertexColor"], shakeMatrix * modelMatrix);
        glLineWidth(1.0f);

        y += dashLength + gapLength;
    }
}

void Tema1::DrawBall(float x, float y)
{
    float size = 30.0f;

    glm::mat3 modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(x, y);
    modelMatrix *= transform2D::Scale(size / 25.0f, size / 25.0f);
    
    RenderMesh2D(meshes["ballOrange"], shaders["VertexColor"], shakeMatrix * modelMatrix);
}

void Tema1::DrawScore()
{
    glm::ivec2 resolution = window->GetResolution();

    float size = 30.0f;

    float lX = 0.075 * resolution.x;
    float rX = 0.925 * resolution.x;
    float lY = 0.09 * resolution.y;
    float rY = 0.09 * resolution.y;

    for (int i = 0; i < rightScore; i++) {
        glm::mat3 modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(lX, lY);
        modelMatrix *= transform2D::Scale(size / 20.0f, size / 20.0f);

        RenderMesh2D(meshes["ballYellow"], shaders["VertexColor"], shakeMatrix * modelMatrix);

        lY += 0.21 * resolution.y;
    }

    for (int i = 0; i < leftScore; i++) {
        glm::mat3 modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(rX, rY);
        modelMatrix *= transform2D::Scale(size / 20.0f, size / 20.0f);

        RenderMesh2D(meshes["ballRed"], shaders["VertexColor"], shakeMatrix * modelMatrix);

        rY += 0.21 * resolution.y;
    }
}

void Tema1::ResetBall()
{
    glm::ivec2 resolution = window->GetResolution();

    ballX = 0.5f * resolution.x;
    ballY = 0.5f * resolution.y;
}

bool Tema1::CheckPaddleCollision(float paddleX, float paddleY, float paddleWidth, float paddleHeight)
{
    bool collisionX = ballX + ballRadius >= paddleX &&
        ballX - ballRadius <= paddleX + paddleWidth;
    bool collisionY = ballY + ballRadius >= paddleY &&
        ballY - ballRadius <= paddleY + paddleHeight;

    return collisionX && collisionY;
}

void Tema1::UpdateBall(float deltaTime)
{
    if (!gameStarted)
        return;

    glm::ivec2 resolution = window->GetResolution();

    float frameStartX = 0.15f * resolution.x;
    float frameEndX = 0.85f * resolution.x;
    float frameStartY = 0.05f * resolution.y;
    float frameEndY = 0.95f * resolution.y;

    ballX += ballVX * deltaTime;
    ballY += ballVY * deltaTime;

    if (ballY - ballRadius <= frameStartY) {
        ballY = frameStartY + ballRadius;
        ballVY = -ballVY;
    }
    if (ballY + ballRadius >= frameEndY) {
        ballY = frameEndY - ballRadius;
        ballVY = -ballVY;
    }

    if (ballX - ballRadius <= frameStartX) {
        rightScore++;
        StartCameraShake(0.15f, 6.0f);

        ResetBall();

        float angle = (rand() % 120 - 60) * M_PI / 180.0f;
        ballVX = abs(ballSpeed) * cos(angle);
        ballVY = ballSpeed * sin(angle);

        return;
    }

    if (ballX + ballRadius >= frameEndX) {
        leftScore++;
        StartCameraShake(0.15f, 6.0f);

        ResetBall();

        float angle = (rand() % 120 - 60) * M_PI / 180.0f;
        ballVX = -abs(ballSpeed) * cos(angle);
        ballVY = ballSpeed * sin(angle);

        return;
    }

    if (leftScore == 5 || rightScore == 5)
    {
        gameOver = true;
        gameStarted = false;
        StartCameraShake(0.5f, 25.0f);

        ResetBall();
        ballVX = 0;
        ballVY = 0;

        return;
    }

    if (gameOver)
        return;

    float cellWidth = (0.675f * resolution.x) / 15.0f;
    float cellHeight = (0.65f * resolution.y) / 7.0f;

    int minRow = 7, maxRow = -1, minCol = 15, maxCol = -1;

    for (int r = 0; r < 7; r++) {
        for (int c = 0; c < 15; c++) {
            if (gridBlocks[r][c]) {
                if (r < minRow) minRow = r;
                if (r > maxRow) maxRow = r;
                if (c < minCol) minCol = c;
                if (c > maxCol) maxCol = c;
            }
        }
    }

    float paddleWidth = (maxCol - minCol + 1) * cellWidth;
    float paddleHeight = (maxRow - minRow + 1) * cellHeight;

    float leftPaddleX = 0.2f * resolution.x - paddleWidth / 2.0f;

    if (CheckPaddleCollision(leftPaddleX, leftPaddleY, paddleWidth, paddleHeight) && ballVX < 0) {
        float relativeY = (ballY - (leftPaddleY + paddleHeight / 2.0f)) / (paddleHeight / 2.0f);
        float angle = relativeY * (M_PI / 3.0f);

        ballVX = ballSpeed * cos(angle);
        ballVY = ballSpeed * sin(angle);
        ballX = leftPaddleX + paddleWidth + ballRadius;

        leftPaddleScaleTime = paddleScaleDuration;
    }

    float rightPaddleX = 0.8f * resolution.x - paddleWidth / 2.0f;

    if (CheckPaddleCollision(rightPaddleX, rightPaddleY, paddleWidth, paddleHeight) && ballVX > 0) {
        float relativeY = (ballY - (rightPaddleY + paddleHeight / 2.0f)) / (paddleHeight / 2.0f);
        float angle = relativeY * (M_PI / 3.0f);

        ballVX = -ballSpeed * cos(angle);
        ballVY = ballSpeed * sin(angle);
        ballX = rightPaddleX - ballRadius;

        rightPaddleScaleTime = paddleScaleDuration;
    }
}

void Tema1::ResetGame()
{
    glm::ivec2 resolution = window->GetResolution();

    leftScore = 0;
    rightScore = 0;

    ResetBall();
    gameStarted = false;
    gameOver = false;

    float offsetY = 53.0f;

    leftPaddleY = 0.5f * resolution.y - offsetY;
    rightPaddleY = 0.5f * resolution.y - offsetY;
}

void Tema1::FrameEnd()
{

}


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    float speed = 700.0f;
    glm::ivec2 resolution = window->GetResolution();

    float cellHeight = (0.65f * resolution.y) / 7.0f;
    int minRow = 7, maxRow = -1;
    for (int r = 0; r < 7; r++) {
        for (int c = 0; c < 15; c++) {
            if (gridBlocks[r][c]) {
                if (r < minRow) minRow = r;
                if (r > maxRow) maxRow = r;
            }
        }
    }
    float paddleHeight = (maxRow - minRow + 1) * cellHeight;

    float frameStartY = 0.05f * resolution.y;
    float frameEndY = 0.95f * resolution.y;

    float startY = frameStartY + cellHeight * 0.54f;
    float finalY = frameEndY - paddleHeight + cellHeight * 0.46f;

    if (inNewScene && window->KeyHold(GLFW_KEY_W)) {
        leftPaddleY += speed * deltaTime;
        if (leftPaddleY >= finalY) {
            leftPaddleY = finalY;
        }
    }

    if (inNewScene && window->KeyHold(GLFW_KEY_S)) {
        leftPaddleY -= speed * deltaTime;
        if (leftPaddleY <= startY) {
            leftPaddleY = startY;
        }
    }

    if (inNewScene && window->KeyHold(GLFW_KEY_UP)) {
        rightPaddleY += speed * deltaTime;
        if (rightPaddleY >= finalY) {
            rightPaddleY = finalY;
        }
    }

    if (inNewScene && window->KeyHold(GLFW_KEY_DOWN)) {
        rightPaddleY -= speed * deltaTime;
        if (rightPaddleY <= startY) {
            rightPaddleY = startY;
        }
    }
}


void Tema1::OnKeyPress(int key, int mods)
{

    if (key == GLFW_KEY_B) {
        inNewScene = false;
        ResetGame();
    }

    if (key == GLFW_KEY_P)
    {
        if (!gameOver && !gameStarted)
        {
            gameStarted = true;

            float angle = (rand() % 120 - 60) * M_PI / 180.0f;
            ballVX = ballSpeed * cos(angle);
            ballVY = ballSpeed * sin(angle);

            if (rand() % 2 == 0)
                ballVX = -ballVX;
        }
    }

    if (key == GLFW_KEY_R)
    {
        ResetGame();
    }
}


void Tema1::OnKeyRelease(int key, int mods)
{
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    if (isDragging) {
        glm::ivec2 resolution = window->GetResolution();
        draggedBlockX = mouseX;
        draggedBlockY = resolution.y - mouseY;
    }
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    glm::ivec2 resolution = window->GetResolution();
    int glMouseY = resolution.y - mouseY;

    if (button == 1) {
        if (IsMouseOverGrayBlock(mouseX, mouseY)) {
            isDragging = true;
            draggedBlockX = mouseX;
            draggedBlockY = glMouseY;
        }

        if (IsMouseOverStartButton(mouseX, mouseY)) {
            if (IsShapeConnected() && IsVerticalPaddle() && nrBlocks >= 0) {
                ResetGame();
                inNewScene = true;
            }
        }
    }
    else if (button == 2) {
        float gridStartX = 0.2625f * resolution.x;
        float gridStartY = 0.075f * resolution.y;
        float gridWidth = 0.675f * resolution.x;
        float gridHeight = 0.65f * resolution.y;

        int cols = 15;
        int rows = 7;
        float cellWidth = gridWidth / cols;
        float cellHeight = gridHeight / rows;

        if (mouseX >= gridStartX && mouseX <= gridStartX + gridWidth &&
            glMouseY >= gridStartY && glMouseY <= gridStartY + gridHeight) {

            int col = (int)((mouseX - gridStartX) / cellWidth);
            int row = (int)((glMouseY - gridStartY) / cellHeight);

            if (gridBlocks[row][col]) {
                gridBlocks[row][col] = false;
                placedBlockPositions[row][col] = glm::vec2(-1, -1);
                nrBlocks++;
            }
        }
    }
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    if (button == 1 && isDragging) {
        glm::ivec2 resolution = window->GetResolution();
        draggedBlockX = mouseX;
        draggedBlockY = resolution.y - mouseY;
        SnapToGrid();
    }
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
    auto camera = GetSceneCamera();

    camera->SetOrthographic(0, (float)width, 0, (float)height, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();

    glViewport(0, 0, width, height);
}