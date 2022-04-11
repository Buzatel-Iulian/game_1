/*******************************************************************************************
*
*   raylib [core] example - 3d camera first person
*
*   This example has been created using raylib 1.3 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2015 Ramon Santamaria (@raysan5)
*
********************************************************************************************/
#include <initializer_list>
#include "raylib.h"
#include "raymath.h"

#define MAX_COLUMNS 20

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif



int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera first person");

    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = { 0 };
    camera.position = (Vector3){ 4.0f, 2.0f, 4.0f };
    camera.target = (Vector3){ 0.0f, 1.8f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    //camera.type = CAMERA_ORTHOGRAPHIC;
    camera.type = CAMERA_PERSPECTIVE;
    //camera.projection = CAMERA_PERSPECTIVE;

    Shader shader = LoadShader(TextFormat("resources/shaders/glsl%i/base_lighting.vs", GLSL_VERSION),
                               TextFormat("resources/shaders/glsl%i/fog.fs", GLSL_VERSION));
    // Get some required shader loactions
    shader.locs[LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    // NOTE: "matModel" location name is automatically assigned on shader loading, 
    // no need to get the location again if using that uniform name
    shader.locs[LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    float arr[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    //auto value = (float[4])arr;
    const void* value ;
    
    // Ambient light level (some basic lighting)
    int ambientLoc = GetShaderLocation(shader, "ambient");
    //SetShaderValue(shader, ambientLoc, (float[4]){ 0.1f, 0.1f, 0.1f, 1.0f }, UNIFORM_VEC4);
    SetShaderValue(shader, ambientLoc, arr, UNIFORM_VEC4);
    //float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
    //SetShaderValue(shader, shader.locs[LOC_VECTOR_VIEW], cameraPos, UNIFORM_VEC3);

    Light another = CreateLight(LIGHT_POINT, (Vector3){ 20, 20, -2 }, Vector3Zero(), WHITE, shader);

    float fogDensity = 0.015f;
    int fogDensityLoc = GetShaderLocation(shader, "fogDensity");
    SetShaderValue(shader, fogDensityLoc, &fogDensity, UNIFORM_FLOAT);

    Model tower = LoadModel("resources/models/obj/turret.obj");                 // Load OBJ model
    Texture2D tower_skin = LoadTexture("resources/models/obj/turret_diffuse.png"); // Load model texture
    Texture2D texture = LoadTexture("resources/models/obj/turret_diffuse.png");
    //tower.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;            // Set model diffuse texture
    tower.materials[0].maps[MAP_DIFFUSE].texture = tower_skin;            // Set model diffuse texture
    tower.materials[0].shader = shader;

    Vector3 towerPos = { 0.0f, 0.0f, 0.0f };                        // Set model position

    // Generates some random columns
    float heights[MAX_COLUMNS] = { 0 };
    Vector3 positions[MAX_COLUMNS] = { 0 };
    Color colors[MAX_COLUMNS] = { 0 };
    Model COLUMNS[MAX_COLUMNS] = {0};

    for (int i = 0; i < MAX_COLUMNS; i++)
    {
        heights[i] = (float)GetRandomValue(1, 12);
        positions[i] = (Vector3){ (float)GetRandomValue(-15, 15), heights[i]/2.0f, (float)GetRandomValue(-15, 15) };
        //colors[i] = (Color){ GetRandomValue(20, 255), GetRandomValue(10, 55), 30, 255 };
	    COLUMNS[i] = LoadModelFromMesh(GenMeshCube(1.0f, heights[i], 1.0f));
	    COLUMNS[i].materials[0].shader = shader;
        COLUMNS[i].materials[0].maps[MAP_DIFFUSE].texture = texture;
    }

    SetCameraMode(camera, CAMERA_FIRST_PERSON); // Set a first person camera mode

    SetTargetFPS(60);                           // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())                // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera);                  // Update camera
        //----------------------------------------------------------------------------------
        // Update the shader with the camera view vector (points towards { 0.0f, 0.0f, 0.0f })
        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(shader, shader.locs[LOC_VECTOR_VIEW], cameraPos, UNIFORM_VEC3);
        //----------------------------------------------------------------------------------


        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

	    	// Draw the tower
                // WARNING: If scale is different than 1.0f,
                // not considered by GetRayCollisionModel()
                DrawModel(tower, towerPos, 1.0f, WHITE);

                DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 32.0f, 32.0f }, LIGHTGRAY); // Draw ground
                //DrawCube((Vector3){ -16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, BLUE);     // Draw a blue wall
                //DrawCube((Vector3){ 16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, LIME);      // Draw a green wall
                //DrawCube((Vector3){ 0.0f, 2.5f, 16.0f }, 32.0f, 5.0f, 1.0f, GOLD);      // Draw a yellow wall

                // Draw some cubes around
                for (int i = 0; i < MAX_COLUMNS; i++)
                {
                    //DrawCube(positions[i], 2.0f, heights[i], 2.0f, colors[i]);
                    //DrawCubeWires(positions[i], 2.0f, heights[i], 2.0f, MAROON);
                    DrawModel(COLUMNS[i], positions[i], 1.0f, WHITE);
                }

            EndMode3D();

            //DrawRectangle( 10, 10, 220, 70, Fade(SKYBLUE, 0.5f));
            //DrawRectangleLines( 10, 10, 220, 70, BLUE);

            //DrawText("First person camera default controls:", 20, 20, 10, BLACK);
            //DrawText("- Move with keys: W, A, S, D", 40, 40, 10, DARKGRAY);
            //DrawText("- Mouse move to look around", 40, 60, 10, DARKGRAY);

            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    UnloadModel(tower);         // Unload model
    UnloadTexture(texture);     // Unload texture
    UnloadTexture(tower_skin);
    UnloadShader(shader);   // Unload shader
    for (int i = 0; i < MAX_COLUMNS; i++)
                {
                    UnloadModel(COLUMNS[i]);
                }
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
