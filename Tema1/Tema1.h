#pragma once

#include <vector>
using namespace std;
#include "components/simple_scene.h"

namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
     public:
        Tema1();
        ~Tema1();

        void Init() override;

    private:

        int nrBlocks;
        bool isDragging;
        float draggedBlockX;
        float draggedBlockY;
        vector<vector<bool>> gridBlocks;
        vector<vector<glm::vec2>> placedBlockPositions;
        bool inNewScene;

        int leftScore;
        int rightScore;

        float leftPaddleY;
        float rightPaddleY;

        float ballX, ballY;
        float ballVX, ballVY;
        float ballSpeed;
        float ballRadius;
        bool gameStarted;
        bool gameOver;

        float shakeTime;
        float shakeStrength;
        glm::mat3 shakeMatrix;

        float leftPaddleScaleTime;
        float rightPaddleScaleTime;
        float paddleScaleDuration;

        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        Mesh* Tema1::CreateBallMesh(string name, glm::vec3 color);
        void StartCameraShake(float duration, float strength);

        void DrawLeftPanel();
        void DrawMainFrame();
        void DrawGridFrame();
        void DrawSquares();
        void DrawStartButton(int nrBlocks);
        void DrawGrid();
        void DrawGrayBlock();

        void DrawDraggedBlock();
        void DrawPlacedBlocks();
        bool IsMouseOverGrayBlock(int mouseX, int mouseY);
        void SnapToGrid();
        bool IsShapeConnected();
        bool IsVerticalPaddle();
        bool IsMouseOverStartButton(int mouseX, int mouseY);

        void DrawShapeAt(float offsetX, float offsetY, const string &meshName);
        void DrawWhiteFrame();
        void DrawCenterDashedLine();
        void DrawBall(float x, float y);
        void DrawScore();

        void ResetBall();
        bool CheckPaddleCollision(float paddleX, float paddleY, float paddleWidth, float paddleHeight);
        void UpdateBall(float deltaTime);
        void ResetGame();

    };
}
