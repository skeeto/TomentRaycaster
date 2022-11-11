// Unity build
//   $ cc -o Toment toment.c -lm $(sdl2-config --cflags --libs)
// Consider using -fwhole-program. For the Windows icon, also link in:
//   $ windres -o icon.o src/toment.rc
#if _WIN32
#  define SDLMAIN_DECLSPEC __declspec(dllexport)
#endif
#include "src/Engine/A_Application.c"
#include "src/Engine/D_AssetsManager.c"
#include "src/Engine/D_ObjectsCallbacks.c"
#include "src/Engine/G_AI.c"
#include "src/Engine/G_AIBehaviour.c"
#include "src/Engine/G_Game.c"
#include "src/Engine/G_MainMenu.c"
#include "src/Engine/G_Pathfinding.c"
#include "src/Engine/G_Player.c"
#include "src/Engine/I_InputHandling.c"
#include "src/Engine/M_Map.c"
#include "src/Engine/P_Physics.c"
#include "src/Engine/R_Rendering.c"
#include "src/Engine/T_TextRendering.c"
#include "src/Engine/U_Timer.c"
#include "src/Engine/U_Utilities.c"
#include "src/main.c"
