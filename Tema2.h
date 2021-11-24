#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/lab_camera_hw.h"


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
     public:
         Tema2();
        ~Tema2();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void Tema2::ResetProjectile();
        void Tema2::setPlayerAngle();
        int** Tema2::createMaze(int **grid, int n);
        bool Tema2::checkCell(int** grid, int n, int i, int j, int dir);
        int Tema2::checkFreeDir(int** grid, int i, int j, int n);

     protected:
        implemented::Camera_hw*camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        // TODO(student): If you need any other class variables, define them here.
        bool projection;
        float right;
        float left;
        float bottom;
        float top;
        float fov;

        struct projectile_t {
            int x;
            int y;
            int z;
            float angle;
            float lenght;
            bool shot;
        }projectile;

        struct player_t {
            float angle;
        } player;

        int cursorX, cursorY;

        //int **grid;
    };
}   // namespace m1
