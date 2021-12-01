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
    player.lives = 1.0f;
    player.position = glm::vec3(0, 0, 0);
    nrOfEnemies = 5;
    counter = 0;
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
    //printf("%d\n", nrOfEnemies);
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
    //TODO scoate jucatorul din perete
    player.x = 7 - startX * 1.2f;
    player.y = 0.5f;
    player.z = 7 - (startY + 1) * 1.2f;
    // TODO fa camera sa urmareasca cum trebuie jucatorul
    camera->Set(glm::vec3(player.x - 1.0f, player.y + 1.0f, player.z - 1.0f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    /*for (int i = 0; i < GRID_SIZE + 2; i++) {
        for (int j = 0; j < GRID_SIZE + 2; j++) {
            printf("%d ", grid_dup[i][j]);
        }
        printf("\n");
    }
    printf("\nstart %d %d\n", startX, startY);
    printf("\nend %d %d\n", endX, endY);*/
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
        mesh = CreateMySquare("healthbar", glm::vec3(2.2, 1.3, 0), 1, 0.15, glm::vec3(0, 1, 0), true);
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = new Mesh("outline");
        mesh = CreateMySquare("outline", glm::vec3(2.2, 1.3, 0), 1, 0.15, glm::vec3(0, 1, 0), false);
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = new Mesh("time");
        mesh = CreateMySquare("time", glm::vec3(2.2, 0.9, 0), 1, 0.15, glm::vec3(0, 1, 0), true);
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
    // TODO sterge shadersul vertexshader_bar
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
                if (nrOfEnemies != 0) {
                    // TODO fa-i sa apara mai rar
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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);  // ca sa putem vedea prin obiecte
    //projectile
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(projectile.x, projectile.y, projectile.z)); //  +glm::vec3(0.5f, 0, 0));
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
         //TODO aici de facut sa se orientezde in functie de orientarea jucatorului -> de pus conditie
        // in functie de orientare sa se modifice x, z cu + sau -
        projectile.x -= deltaTimeSeconds * 59;

        projectile.lenght = sqrt((double)(projectile.x) * (projectile.x));

        if (projectile.lenght >= 3.5)
            ResetProjectile();
    }

    //healthbar 
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f));  // TODO de facut in functie de player.lives
        modelMatrix = glm::rotate(modelMatrix, RADIANS(90.f), glm::vec3(0, 1, 0));
        MyRenderSimpleMesh(meshes["healthbar"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.921, 0.901, 0.270));
    }
    //outline 
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(90.f), glm::vec3(0, 1, 0));
        MyRenderSimpleMesh(meshes["outline"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.690, 0.670, 0.090));

    }
    //time 
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f-deltaTimeSeconds)); // TODO de facut asta sa scaleze cum trebuie
        modelMatrix = glm::rotate(modelMatrix, RADIANS(90.f), glm::vec3(0, 1, 0));
        MyRenderSimpleMesh(meshes["time"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.921, 0.901, 0.270));

    }
    //player
    if (renderCameraTarget)
    {
        //picior drept
        {
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0.f, -0.36f, 0.12f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                modelMatrix = glm::rotate(modelMatrix, RADIANS(player.rotation), glm::vec3(0, 1, 0));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.207, 0.043, 0.250));
            }
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0.f, -0.16, 0.12f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                modelMatrix = glm::rotate(modelMatrix, RADIANS(player.rotation), glm::vec3(0, 1, 0));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.207, 0.043, 0.250));
            }
        }
        //picior stang
        {
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0.f, -0.36f, -0.12f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                modelMatrix = glm::rotate(modelMatrix, RADIANS(player.rotation), glm::vec3(0, 1, 0));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.207, 0.043, 0.250));
            }
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0.f, -0.16, -0.12f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                modelMatrix = glm::rotate(modelMatrix, RADIANS(player.rotation), glm::vec3(0, 1, 0));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.207, 0.043, 0.250));
            }
        }
        //corp
        {
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0, 0.05f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                modelMatrix = glm::rotate(modelMatrix, RADIANS(player.rotation), glm::vec3(0, 1, 0));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.933, 0.6, 0.627));
            }
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0.f, 0.05f, 0.2f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                modelMatrix = glm::rotate(modelMatrix, RADIANS(player.rotation), glm::vec3(0, 1, 0));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.933, 0.6, 0.627));
            }
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0.f, 0.05f, -0.2f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                modelMatrix = glm::rotate(modelMatrix, RADIANS(player.rotation), glm::vec3(0, 1, 0));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.933, 0.6, 0.627));
            }

            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0, 0.22f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                modelMatrix = glm::rotate(modelMatrix, RADIANS(player.rotation), glm::vec3(0, 1, 0));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.933, 0.6, 0.627));
            }
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0.f, 0.22f, 0.2f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                modelMatrix = glm::rotate(modelMatrix, RADIANS(player.rotation), glm::vec3(0, 1, 0));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.933, 0.6, 0.627));
            }
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0.f, 0.22f, -0.2f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                modelMatrix = glm::rotate(modelMatrix, RADIANS(player.rotation), glm::vec3(0, 1, 0));
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
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0.f, 0.22f, 0.415f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                modelMatrix = glm::rotate(modelMatrix, RADIANS(player.rotation), glm::vec3(0, 1, 0));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.933, 0.6, 0.627));
            }
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0.f, 0.02f, 0.415f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                modelMatrix = glm::rotate(modelMatrix, RADIANS(player.rotation), glm::vec3(0, 1, 0));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.949, 0.866, 0.756));
            }
        }
        //brat stang
        {
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0.f, 0.22f, -0.415f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                modelMatrix = glm::rotate(modelMatrix, RADIANS(player.rotation), glm::vec3(0, 1, 0));
                MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0.933, 0.6, 0.627));
            }
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(player.x, player.y, player.z) + glm::vec3(0.f, 0.02f, -0.415f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                modelMatrix = glm::rotate(modelMatrix, RADIANS(player.rotation), glm::vec3(0, 1, 0));
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
                //construiesc inamicul
                else if (grid_dup[i][j] == 1) {
                    glm::mat4 modelMatrix = glm::mat4(1);
                    enemies[aux].x = 7 - i * 1.2f;
                    enemies[aux].z = 7 - j * 1.2f;
                    enemies_dup[aux].x = 7 - i * 1.2f;
                    enemies_dup[aux].z = 7 - j * 1.2f;
                   
                    modelMatrix = glm::translate(modelMatrix, glm::vec3(enemies[aux].x, 0.5f, enemies[aux].z));
                    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f));
                    aux = aux + 1;
                    MyRenderSimpleMesh(meshes["box"], shaders["PlayerShader"], modelMatrix, glm::vec3(0, 0.188, 0.247));
                }
            }
        }
        counter = aux;
    }

    // enemies movement 
     // TODO make this work, intra dar de ce nu se modifica?!
    {
        for (int i = 0; i < counter; i++) {
            if(i == 0)
                printf("%f %f %f %f\n", enemies[i].x, enemies_dup[i].x, enemies[i].z, enemies_dup[i].x);
            if (enemies[i].x < enemies_dup[i].x + 1.f) {
                enemies[i].x += deltaTimeSeconds * 10;
                if( i == 0)
                    printf("%f aaaaaa\n", enemies[i].x); //aici se schimba dar de ce nu retine ?
              //  printf("intri?");
            }
            else if (enemies[i].x > enemies_dup[i].x - 1.f)
                enemies[i].x -= deltaTimeSeconds * 100;
            if (enemies[i].z < enemies_dup[i].z + 1.f)
                enemies[i].z += deltaTimeSeconds * 100;
            else if (enemies[i].z > enemies_dup[i].z - 1.f)
                enemies[i].z -= deltaTimeSeconds * 100;
        }
    }
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
    // TODO de facut rotiera buna a jucatorului
    if (window->KeyHold(GLFW_KEY_W)) {
        player.rotation = 0.0f;
        player.x -= deltaTime;
        camera->Set(glm::vec3(player.x + 1.0f, player.y + 1.0f, player.z + 1.0f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

        /* //if (camera->GetTargetPosition().y >= 0.45)
            camera->TranslateForward(cameraSpeed);
       else
            camera->TranslateForward(-cameraSpeed);*/

    }

    if (window->KeyHold(GLFW_KEY_A)) {
        player.rotation = 90.0f;
        player.z += deltaTime;
        camera->Set(glm::vec3(player.x + 1.0f, player.y + 1.0f, player.z + 1.0f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

        //camera->TranslateRight(-cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        player.rotation = 0.0f;
        player.x += deltaTime;
        camera->Set(glm::vec3(player.x + 1.0f, player.y + 1.0f, player.z + 1.0f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

     /*   if(camera->GetTargetPosition().y >= 0.45)
            camera->TranslateForward(-cameraSpeed);
        else
            camera->TranslateForward(cameraSpeed);*/
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        player.rotation = -90.0f;
        player.z -= deltaTime;
        camera->Set(glm::vec3(player.x + 1.0f, player.y + 1.0f, player.z + 1.0f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

       // camera->TranslateRight(cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_Q)) {
        if (camera->GetTargetPosition().y > 0.46)
            camera->TranslateUpward(-cameraSpeed);
        else 
            camera->TranslateUpward(cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_E)) {
        camera->TranslateUpward(cameraSpeed);
    }
    if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
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
    // Add mouse move event
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            renderCameraTarget = false;
            camera->Set(glm::vec3(player.x, player.y + 0.5f, player.z), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));  
            // TODO de ce nu te misti complet?
            camera->RotateFirstPerson_OX(sensivityOX * -deltaY);
            camera->RotateFirstPerson_OY(sensivityOY * -deltaX);
        }
        renderCameraTarget = true;   
}
// nefolosita
void Tema2::setPlayerAngle() {

    int ax = camera->GetTargetPosition().x;
    int ay = camera->GetTargetPosition().z;
    ax = cursorX - ax;
    ay = -cursorY + ay;
    glm::vec2 auxvect = glm::normalize(glm::vec2(ax, ay));
    if (auxvect.y < 0) {
        auxvect.x = -auxvect.x;
    }
    if (auxvect.x == 0 && auxvect.y < 0) {
        player.angle = 180.f;
    }
    else if (auxvect.x == 0 && auxvect.y > 0) {
        player.angle = 0.f;
    }
    else
    {
        player.angle = atan(auxvect.x / auxvect.y);
    }
    if (auxvect.y > 0) {
        player.angle = -player.angle + M_PI;
    }
    if (!projectile.shot)
        projectile.angle = player.angle + 3 * M_PI / 2;

}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
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
