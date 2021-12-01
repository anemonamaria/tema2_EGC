#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/lab_camera_hw.h"
#include <vector>
using namespace std;

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

        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;

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
        Mesh* Tema2::CreateMySquare(const std::string& name, glm::vec3 leftBottomCorner, float length,
            float width, glm::vec3 color, bool fill);
        void Tema2::MyRenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);
        bool Tema2::isInBounds(int x, int y);
        void Tema2::visitGrid(int x, int y, vector<vector<int>> &grid);



    protected:
        implemented::Camera_hw* camera;
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
            int x;
            int y;
            int z;
            glm::vec3 position;
            float rotation;
            float lives;
        } player;

        struct enemy_t {
            float x;
            float y;
            float z;
        } enemy;

        int cursorX, cursorY;

        vector<vector<int>> grid;
        vector<vector<int>> grid_dup;
        vector<enemy_t> enemies;
        vector<enemy_t> enemies_dup;


        glm::vec3 lightPosition;
        unsigned int materialShininess;
        float materialKd;
        float materialKs;

        int startX;
        int startY;
        int nrOfEnemies;
        int endX;
        int endY;
        int counter;
    };
};   // namespace m1
