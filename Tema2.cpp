#include "lab_m1/Tema2/Tema2.h"

#include <string>
#include <iostream>

using namespace std;
using namespace m1;

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#define GRID_SIZE 10

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    renderCameraTarget = true;
    cullFace = GL_BACK;
    right = 20.f;
    left = .02f;
    bottom = .02f;
    top = 20.f;
    fov = 40.f;
    camera = new implemented::Camera_hw();
    projectile.x = projectile.y = projectile.z = 0;
    projectile.angle = player.angle = 0.0;
    projectile.lenght = 0.0;
    projectile.shot = false;
    player.x = 0;
    player.y = 0;
    player.z = 0;
    player.rotation = 0.0f;
    player.lives = 0.5f;
    auxPosOfCamera = glm::vec3(0, 0, 0);

    player.position = glm::vec3(0, 0, 0);
    nrOfEnemies = maxScore = 5;
    lives = 0;  
    counter = 0;
    myTime = 0.5f;
    myLife = 0.5f;
    score = 0;
    grid.resize(GRID_SIZE);
    grid_dup.resize(GRID_SIZE + 2);
    for (int i = 0; i < GRID_SIZE; i++) {
        grid[i].resize(GRID_SIZE);
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = 2;
        }
    }
    for (int i = 0; i < GRID_SIZE + 2; i++) {
        grid_dup[i].resize(GRID_SIZE + 2);
        for (int j = 0; j < GRID_SIZE + 2; j++) {
            grid_dup[i][j] = 2;
        }
    }
    srand(time(0));
    startY = endY = 1 + rand() % 9;
    startX = endX = 0;
    visitGrid(startX, startY, grid);
    enemies.resize(5 - nrOfEnemies);
    enemies_dup.resize(5 - nrOfEnemies);

    
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == -1)
                grid[i][j] = 2;
            grid_dup[i + 1][j + 1] = grid[i][j];
        }
    }
    grid_dup[startX][startY + 1] = 0;
    if (endY > endX) {
        while (endY < GRID_SIZE + 2) {
            grid_dup[endX][endY++] = 0;
        }
    }
    else
    {
        while (endX < GRID_SIZE + 2) {
            grid_dup[endX++][endY] = 0;
        }

    }
    //initializez inamicii
    int aux = 0;
    for (int i = 0; i < GRID_SIZE + 2; i++)
    {
        for (int j = 0; j < GRID_SIZE + 2; j++)
        {
            if (grid_dup[i][j] == 1) {
                enemies[aux].x = 7 - i * 1.2f;
                enemies[aux].z = 7 - j * 1.2f;
                enemies_dup[aux].x = 7 - i * 1.2f;
                enemies_dup[aux].z = 7 - j * 1.2f;
                enemies[aux].onScreen = true;
                enemies[aux].moveX = rand() % 2;
                enemies[aux].moveY = rand() % 2;
                aux = aux + 1;
            }
        }
    }

    counter = aux;
    auxPosOfCamera.x = 7 - startX * 1.2f;
    auxPosOfCamera.y = 0.5f;
    auxPosOfCamera.z = 7 - (startY + 1) * 1.2f;
    player.x = auxPosOfCamera.x;
    player.y = auxPosOfCamera.y;
    player.z = auxPosOfCamera.z;
    finalCamera.x = auxPosOfCamera.x + 1.5f;
    finalCamera.y = auxPosOfCamera.y + 1.f;
    finalCamera.z = auxPosOfCamera.z;
    sndFinalCamera.x = auxPosOfCamera.x;
    sndFinalCamera.y = auxPosOfCamera.y + 0.75f;
    sndFinalCamera.z = auxPosOfCamera.z;
    camera->Set(glm::vec3(finalCamera.x, finalCamera.y, finalCamera.z), glm::vec3(sndFinalCamera.x, sndFinalCamera.y, sndFinalCamera.z), glm::vec3(0, 1, 0));
    printf("Hello! Mult succes in Survival Maze!\n");
    printf("2 - perete \n1 - inamic \n0 - liber \nLabirnitul: \n");
    for (int i = 0; i < GRID_SIZE + 2; i++) {
        for (int j = 0; j < GRID_SIZE + 2; j++) {
            printf("%d ", grid_dup[i][j]);
        }
        printf("\n");
    }

    //meshes
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("podea");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("healthbar");
        mesh = CreateMySquare("healthbar", glm::vec3(0, 0, 0), 1, 0.15, glm::vec3(0, 1, 0), true);
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = new Mesh("outline");
        mesh = CreateMySquare("outline", glm::vec3(0, 0, 0), 1, 0.15, glm::vec3(0, 1, 0), false);
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = new Mesh("time");
        mesh = CreateMySquare("time", glm::vec3(0, 0, 0), 1, 0.15, glm::vec3(0, 1, 0), true);
        AddMeshToList(mesh);
    }

    //player shader
    {
        Shader* player_shader = new Shader("PlayerShader");
        player_shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        player_shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        player_shader->CreateAndLink();
        shaders[player_shader->GetName()] = player_shader;
    }
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

}


bool Tema2::isInBounds(int x, int y) 
{
    if(x < 0 || x > GRID_SIZE - 1) return false;
    if(y < 0 || y > GRID_SIZE - 1) return false;
    return true;
}
// -1 - celula nevizitata
// 0 - drum liber 
// 1 - inamic
// 2 - perete
void Tema2::visitGrid(int x, int y, vector<vector<int>> &grid)
{
    grid[x][y] = 0;
    int dirs[4];
    dirs[0] = NORTH;
    dirs[1] = EAST;
    dirs[2] = SOUTH;
    dirs[3] = WEST;
    srand(time(0));

    for (int i = 0; i < 4; i++) {
        int r = rand() % 4;
        int tmp = dirs[r];
        dirs[r] = dirs[i];
        dirs[i] = tmp;
    }

    for (int i = 0; i < 4; i++) {
        int dx = 0;
        int dy = 0;

        switch (dirs[i])
        {
            case NORTH: dy = -1;
                break;
            case SOUTH: dy = 1;
                break;
            case EAST: dx = 1;
                break;
            case WEST: dx = -1;
                break;
        }

        int x2 = x + (dx << 1);
        int y2 = y + (dy << 1);

        if (isInBounds(x2, y2) && isInBounds(x2-dx, y2-dy)) {
            if (grid[x2][y2] == 2) {
                if (nrOfEnemies != 0 && (x2 + y2) % 4 >= 2) {
                    grid[x2 - dx][y2 - dy] = 1;
                    nrOfEnemies -= grid[x2 - dx][y2 - dy];
                }
                else {
                    grid[x2 - dx][y2 - dy] = 0;
                }
                endX = x2 - dx;
                endY = y2 - dy;
                visitGrid(x2, y2, grid);
            }
        }
    }
}

void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

Mesh* Tema2::CreateMySquare(const std::string& name, glm::vec3 leftBottomCorner, float length,
    float width, glm::vec3 color, bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, width, 0), color),
        VertexFormat(corner + glm::vec3(0, width, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}


void Tema2::Update(float deltaTimeSeconds)
{

    player.x = finalCamera.x - 1.5f;
    player.y = finalCamera.y - 1.f;
    player.z = finalCamera.z;

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);  // ca sa putem vedea prin obiecte
    //projectile
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(projectile.x, projectile.y, projectile.z)); 
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        MyRenderSimpleMesh(meshes["sphere"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.058, 0.172, 0.403));

    }

    //podea
     {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f));
        MyRenderSimpleMesh(meshes["podea"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.086, 0.529, 0.654));

     }

    //projectile movement
    if (projectile.shot == true) 
    {
        projectile.x -= deltaTimeSeconds * 59;

        projectile.lenght = sqrt((double)(projectile.x) * (projectile.x));

        for (int i = 0; i < counter; i++) {
            checkProjectileEnemyCollision(i);
        }

        if (projectile.lenght >= 3.5)
            ResetProjectile();
    }
     
    // daca am ramas fara vieti
    if (player.lives - 0.1 * lives <= 0) {
        printf("~~~~~You have lost the game! : (~~~~~\n");
        Exit();
    }

    //healthbar 
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0, 0.65f, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(player.lives -  0.1 * lives));  
        modelMatrix = glm::rotate(modelMatrix, RADIANS(90.f), glm::vec3(0, 1, 0));
        MyRenderSimpleMesh(meshes["healthbar"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.921, 0.901, 0.270));
    }
    //outline healthbar
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0, 0.65f, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(90.f), glm::vec3(0, 1, 0));
        MyRenderSimpleMesh(meshes["outline"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.690, 0.670, 0.090));

    }
    //time 
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0, 0.75f, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(myTime));
        myTime -= deltaTimeSeconds / 100;
        modelMatrix = glm::rotate(modelMatrix, RADIANS(90.f), glm::vec3(0, 1, 0));
        MyRenderSimpleMesh(meshes["time"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.921, 0.901, 0.270));
        if (myTime <= 0) {
            printf("You have no more time to escape. GAME OVER!\n");
            exit(0);
        }
    }
    //outline time
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0, 0.75f, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(90.f), glm::vec3(0, 1, 0));
        MyRenderSimpleMesh(meshes["outline"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.690, 0.670, 0.090));

    }
    //player
    if (renderCameraTarget)
    {
        glm::mat4 modelMatrixTranslateRotate = glm::mat4(1);
        modelMatrixTranslateRotate = glm::translate(modelMatrixTranslateRotate, glm::vec3(player.x, player.y, player.z));
        modelMatrixTranslateRotate = glm::rotate(modelMatrixTranslateRotate, RADIANS(player.rotation), glm::vec3(0, 1, 0));
        //picior drept
        {
            {
                glm::mat4 modelMatrix = modelMatrixTranslateRotate;
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, -0.36f, 0.12f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.207, 0.043, 0.250));
            }
            {
                glm::mat4 modelMatrix = modelMatrixTranslateRotate;
                modelMatrix = glm::translate(modelMatrix,glm::vec3(0.f, -0.16, 0.12f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.207, 0.043, 0.250));
            }
        }
        //picior stang
        {
            {
                glm::mat4 modelMatrix = modelMatrixTranslateRotate;
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, -0.36f, -0.12f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.207, 0.043, 0.250));
            }
            {
                glm::mat4 modelMatrix = modelMatrixTranslateRotate;
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, -0.16, -0.12f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.207, 0.043, 0.250));
            }
        }
        //corp
        {
            {
                glm::mat4 modelMatrix = modelMatrixTranslateRotate;
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.05f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.933, 0.6, 0.627));
            }
            {
                glm::mat4 modelMatrix = modelMatrixTranslateRotate;
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 0.05f, 0.2f)) ;
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.933, 0.6, 0.627));
            }
            {
                glm::mat4 modelMatrix = modelMatrixTranslateRotate;
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 0.05f, -0.2f)) ;
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.933, 0.6, 0.627));
            }

            {
                glm::mat4 modelMatrix = modelMatrixTranslateRotate;
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.22f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.933, 0.6, 0.627));
            }
            {
                glm::mat4 modelMatrix = modelMatrixTranslateRotate;
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 0.22f, 0.2f)) ;
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.933, 0.6, 0.627));
            }
            {
                glm::mat4 modelMatrix = modelMatrixTranslateRotate;
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 0.22f, -0.2f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.933, 0.6, 0.627));
            }
        }
        // cap
        {
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0, 0.43f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.949, 0.866, 0.756));

            }
        }
        //brat drept
        {
            {
                glm::mat4 modelMatrix = modelMatrixTranslateRotate;
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 0.22f, 0.415f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.933, 0.6, 0.627));
            }
            {
                glm::mat4 modelMatrix = modelMatrixTranslateRotate;
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 0.02f, 0.415f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.949, 0.866, 0.756));
            }
        }
        //brat stang
        {
            {
                glm::mat4 modelMatrix = modelMatrixTranslateRotate;
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 0.22f, -0.415f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.933, 0.6, 0.627));
            }
            {
                glm::mat4 modelMatrix = modelMatrixTranslateRotate;
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 0.02f, -0.415f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.949, 0.866, 0.756));
            }
        }
    }

    //labirint & inamici
    {
        int aux = 0;
        for (int i = 0; i < GRID_SIZE + 2; i++) 
        {
            for(int j = 0; j < GRID_SIZE + 2; j++) 
            {
                //construiesc perete
                if (grid_dup[i][j] == 2) {
                    glm::mat4 modelMatrix = glm::mat4(1);
                    modelMatrix = glm::translate(modelMatrix, glm::vec3(7 - i * 1.2f , 0.5f, 7 - j * 1.2f));
                    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.2f));
                    MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.580, 0.709, 0.752));
                }
            }
        }
        //construiesc inamicul
        for (int i = 0; i < counter; i++) {
            if (enemies[i].onScreen == true) {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(enemies[i].x, 0.5f, enemies[i].z));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0, 0.188, 0.247));
            }
        }
    }

    // enemies movement 
    {
        for (int i = 0; i < counter; i++) {
            if (enemies[i].onScreen == true) {
                if (enemies[i].x < enemies_dup[i].x + 0.3f && enemies[i].moveX == 1) {
                    enemies[i].x += deltaTimeSeconds;
                }
                else if (enemies[i].x >= enemies_dup[i].x + 0.3f)
                    enemies[i].moveX = 0;
                if (enemies[i].z < enemies_dup[i].z + 0.3f && enemies[i].moveY == 1) {
                    enemies[i].z += deltaTimeSeconds;
                }
                else if (enemies[i].z >= enemies_dup[i].z + 0.3f)
                    enemies[i].moveY = 0;
                if (enemies[i].x >= enemies_dup[i].x - 0.3f && enemies[i].moveX == 0) {
                    enemies[i].x -= deltaTimeSeconds;
                }
                else if (enemies[i].x < enemies_dup[i].x - 0.3f)
                    enemies[i].moveX = 1;
                if (enemies[i].z >= enemies_dup[i].z - 0.3f && enemies[i].moveY == 0) {
                    enemies[i].z -= deltaTimeSeconds;/**/
                }
                else if (enemies[i].z < enemies_dup[i].z - 0.3f)
                    enemies[i].moveY = 1;
            }
            checkPlayerEnemyCollision(i);
        }
    }
}

bool Tema2::checkProjectileEnemyCollision(int i) {
    if (!enemies[i].onScreen)
        return false;
    float a = 1.f;
    float b = 1.f;

    float x = (player.x + projectile.x + 0.03f * cos(projectile.angle)) - enemies[i].x; // + 2.0f;
    float y = (player.z + projectile.z + 0.06f * sin(projectile.angle)) - enemies[i].z; // + 2.0f;
    if (pow(x / a, 2) + pow(y / b, 2) <= 1 && (projectile.x != player.x || projectile.z != player.z)) {
        enemies[i].onScreen = false;
        projectile.x = 0;
        projectile.y = 0;
        projectile.z = 0;
        projectile.shot = false;
        score += 1;
        printf("~~~~~~Your score is %d! Keep playing!~~~~~~~\n", score);

        return true;
    }
    return false;
}

bool Tema2::checkPlayerEnemyCollision(int i) {
    if (!enemies[i].onScreen)
        return false;
    float a = 1.f;
    float b = 1.f;

    float x = player.x - enemies[i].x; // + 2.0f;
    float y = player.z - enemies[i].z; // + 2.0f;
    if (pow(x / a, 2) + pow(y / b, 2) <= 1) {
        enemies[i].onScreen = false;
        lives++;
        
        printf("~~~~~~Yo have lost a life!~~~~~~~\nMax = 5   Your lives = %f\n", player.lives * 10 - lives);
        return true;
    }
    return false;
}

void Tema2::MyRenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;
    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    int myObjectColor = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(myObjectColor, 1, glm::value_ptr(color));

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema2::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Get shader location for "Time"
    int time_location = glGetUniformLocation(shader->GetProgramID(), "Time");
    glUniform1f(time_location, (float)Engine::GetElapsedTime());

    // Get the transformation matrix when rendering the bar
    int transf_matrix = glGetUniformLocation(shader->program, "TransformationMatrix");
    glUniformMatrix4fv(transf_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

void Tema2::ResetProjectile() {
    projectile.x = 0; 
    projectile.y = 0; 
    projectile.z = 0;
    projectile.angle = 0;
    projectile.lenght = 0;
    projectile.shot = false;
}


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    // move the camera only if MOUSE_RIGHT button is pressed
    float cameraSpeed = 2.0f * deltaTime;
    //TODO de reparat camera
    if (window->KeyHold(GLFW_KEY_W)) {
        player.rotation = 0.0f;
        finalCamera = camera->TranslateForward(cameraSpeed);   // auxPosOfCamera
    }

    if (window->KeyHold(GLFW_KEY_A)) {
        if (player.rotation < 90.0f)
            player.rotation += 1.0f;
        else
            player.rotation = 90.0f;
        finalCamera = camera->TranslateRight(-cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        player.rotation = 0.0f;
        finalCamera = camera->TranslateForward(-cameraSpeed);
    }
    
    if (window->KeyHold(GLFW_KEY_D)) {
        if (player.rotation > -90.0f)
            player.rotation += -1.0f;
        else
            player.rotation = -90.0f;
        finalCamera = camera->TranslateRight(cameraSpeed);
        
    }
    
    //finalCamera.x = auxPosOfCamera.x + 1.5f;
    //finalCamera.y = auxPosOfCamera.y + 1.f;
    //finalCamera.z = auxPosOfCamera.z;
    sndFinalCamera.x = finalCamera.x - 1.5f;
    sndFinalCamera.y = finalCamera.y - 0.25f;
    sndFinalCamera.z = finalCamera.z;
    
    if (window->KeyHold(GLFW_KEY_Q)) {
        if (camera->GetTargetPosition().y > 0.46)
            camera->TranslateUpward(-cameraSpeed);
        else 
            camera->TranslateUpward(cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_E)) {
        camera->TranslateUpward(cameraSpeed);
    }
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT) && window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {

        if (projectile.shot == false && window->KeyHold(GLFW_KEY_SPACE))
            projectile.shot = true;
    }
}


void Tema2::OnKeyPress(int key, int mods)
{

}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.01f;
        float sensivityOY = 0.01f;

        if (window->GetSpecialKeyState() == 0) {
            renderCameraTarget = true;

            camera->RotateThirdPerson_OX(sensivityOX * -deltaY);
            camera->RotateThirdPerson_OY(sensivityOY * -deltaX);
        }

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            renderCameraTarget = false;

            camera->RotateFirstPerson_OX(sensivityOX * -deltaY);
            camera->RotateFirstPerson_OY(sensivityOY * -deltaX);
        }
        renderCameraTarget = true;
    }
    
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if(window->GetSpecialKeyState() & GLFW_MOD_CONTROL)
         camera->Set(glm::vec3(auxPosOfCamera.x, auxPosOfCamera.y + 0.5f, auxPosOfCamera.z), glm::vec3(0, 1, auxPosOfCamera.z), glm::vec3(0, 1, 0));
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
