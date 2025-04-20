#include "Game.h"
/*
    Credits Damian Kêpczyk 2025
    ---------------------------
    Minecraft clone made using OpenGL (glad) and GLFW.
    External dependencies:
        - OpenSimplexNoise
        - GLM
        - premake (for project files)
        - STB
    ---------------------------
    Started January 2025
    Finished ?
*/
int main(int argc, char** argv) 
{
    Game game; // Create the game object
    game.GameLoop(); // Processing the entire game
    game.Terminate(); // Turn the game off
    return 0;
}
