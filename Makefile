CC      = cc
WINDRES = windres
CFLAGS  = -Wall -Wextra -Wno-unused-parameter -g3 $$(sdl2-config --cflags)
LDFLAGS =
LDLIBS  = $$(sdl2-config --static-libs)

obj = \
 src/Engine/A_Application.o \
 src/Engine/D_AssetsManager.o \
 src/Engine/D_ObjectsCallbacks.o \
 src/Engine/G_AI.o \
 src/Engine/G_AIBehaviour.o \
 src/Engine/G_Game.o \
 src/Engine/G_MainMenu.o \
 src/Engine/G_Pathfinding.o \
 src/Engine/G_Player.o \
 src/Engine/I_InputHandling.o \
 src/Engine/M_Map.o \
 src/Engine/P_Physics.o \
 src/Engine/R_Rendering.o \
 src/Engine/T_TextRendering.o \
 src/Engine/U_Timer.o \
 src/Engine/U_Utilities.o \
 src/main.o \
 src/icon.o

all: Toment.exe

clean:
	rm -f Toment.exe $(obj)

Toment.exe: $(obj)
	$(CC) $(LDFLAGS) -o $@ $(obj) $(LDLIBS)

src/icon.o: src/toment.ico
	$(WINDRES) -o $@ src/toment.rc

src/Engine/A_Application.o: src/Engine/A_Application.c \
 src/Engine/A_Application.h src/Engine/R_Rendering.h src/Engine/M_Map.h \
 src/Engine/D_AssetsManager.h src/Engine/U_DataTypes.h \
 src/Engine/U_Timer.h src/Engine/U_Utilities.h
src/Engine/D_AssetsManager.o: src/Engine/D_AssetsManager.c \
 src/Engine/R_Rendering.h src/Engine/M_Map.h src/Engine/A_Application.h \
 src/Engine/D_AssetsManager.h src/Engine/U_DataTypes.h \
 src/Engine/U_Timer.h src/Engine/U_Utilities.h \
 src/Engine/D_ObjectsCallbacks.h
src/Engine/D_ObjectsCallbacks.o: src/Engine/D_ObjectsCallbacks.c \
 src/Engine/G_Game.h src/Engine/G_Player.h src/Engine/A_Application.h \
 src/Engine/U_DataTypes.h src/Engine/U_Timer.h \
 src/Engine/D_AssetsManager.h src/Engine/R_Rendering.h src/Engine/M_Map.h \
 src/Engine/U_Utilities.h src/Engine/D_ObjectsCallbacks.h
src/Engine/G_AI.o: src/Engine/G_AI.c src/Engine/G_AI.h \
 src/Engine/D_AssetsManager.h src/Engine/U_DataTypes.h \
 src/Engine/U_Timer.h src/Engine/G_Player.h src/Engine/A_Application.h \
 src/Engine/G_Game.h src/Engine/R_Rendering.h src/Engine/M_Map.h \
 src/Engine/U_Utilities.h src/Engine/G_Pathfinding.h \
 src/Engine/P_Physics.h src/Engine/G_AIBehaviour.h
src/Engine/G_AIBehaviour.o: src/Engine/G_AIBehaviour.c \
 src/Engine/G_AIBehaviour.h src/Engine/D_AssetsManager.h \
 src/Engine/U_DataTypes.h src/Engine/U_Timer.h src/Engine/G_AI.h \
 src/Engine/G_Player.h src/Engine/A_Application.h src/Engine/G_Game.h \
 src/Engine/R_Rendering.h src/Engine/M_Map.h src/Engine/U_Utilities.h \
 src/Engine/G_Pathfinding.h src/Engine/P_Physics.h
src/Engine/G_Game.o: src/Engine/G_Game.c src/Engine/I_InputHandling.h \
 src/Engine/A_Application.h src/Engine/G_Player.h src/Engine/G_Game.h \
 src/Engine/R_Rendering.h src/Engine/M_Map.h src/Engine/D_AssetsManager.h \
 src/Engine/U_DataTypes.h src/Engine/U_Timer.h src/Engine/U_Utilities.h \
 src/Engine/G_MainMenu.h src/Engine/P_Physics.h \
 src/Engine/G_Pathfinding.h src/Engine/G_AI.h
src/Engine/G_MainMenu.o: src/Engine/G_MainMenu.c src/Engine/G_MainMenu.h \
 src/Engine/G_Game.h src/Engine/G_Player.h src/Engine/A_Application.h \
 src/Engine/U_DataTypes.h src/Engine/U_Timer.h \
 src/Engine/D_AssetsManager.h src/Engine/R_Rendering.h src/Engine/M_Map.h \
 src/Engine/U_Utilities.h src/Engine/T_TextRendering.h
src/Engine/G_Pathfinding.o: src/Engine/G_Pathfinding.c \
 src/Engine/G_Pathfinding.h src/Engine/M_Map.h src/Engine/A_Application.h \
 src/Engine/D_AssetsManager.h src/Engine/U_DataTypes.h \
 src/Engine/U_Timer.h src/Engine/U_Utilities.h src/Engine/R_Rendering.h \
 src/Engine/G_Player.h src/Engine/G_Game.h
src/Engine/G_Player.o: src/Engine/G_Player.c src/Engine/G_Player.h \
 src/Engine/A_Application.h src/Engine/G_Game.h src/Engine/R_Rendering.h \
 src/Engine/M_Map.h src/Engine/D_AssetsManager.h src/Engine/U_DataTypes.h \
 src/Engine/U_Timer.h src/Engine/U_Utilities.h \
 src/Engine/I_InputHandling.h src/Engine/G_MainMenu.h \
 src/Engine/P_Physics.h src/Engine/G_Pathfinding.h \
 src/Engine/D_ObjectsCallbacks.h src/Engine/G_AI.h \
 src/Engine/T_TextRendering.h
src/Engine/I_InputHandling.o: src/Engine/I_InputHandling.c \
 src/Engine/I_InputHandling.h src/Engine/A_Application.h \
 src/Engine/G_Player.h src/Engine/G_Game.h src/Engine/R_Rendering.h \
 src/Engine/M_Map.h src/Engine/D_AssetsManager.h src/Engine/U_DataTypes.h \
 src/Engine/U_Timer.h src/Engine/U_Utilities.h src/Engine/G_MainMenu.h
src/Engine/M_Map.o: src/Engine/M_Map.c src/Engine/M_Map.h \
 src/Engine/A_Application.h src/Engine/D_AssetsManager.h \
 src/Engine/U_DataTypes.h src/Engine/U_Timer.h src/Engine/U_Utilities.h \
 src/Engine/G_Game.h src/Engine/G_Player.h src/Engine/R_Rendering.h \
 src/Engine/G_AI.h
src/Engine/P_Physics.o: src/Engine/P_Physics.c src/Engine/P_Physics.h \
 src/Engine/U_Timer.h src/Engine/U_DataTypes.h
src/Engine/R_Rendering.o: src/Engine/R_Rendering.c \
 src/Engine/R_Rendering.h src/Engine/M_Map.h src/Engine/A_Application.h \
 src/Engine/D_AssetsManager.h src/Engine/U_DataTypes.h \
 src/Engine/U_Timer.h src/Engine/U_Utilities.h src/Engine/G_Player.h \
 src/Engine/G_Game.h src/Engine/I_InputHandling.h src/Engine/G_MainMenu.h \
 src/Engine/T_TextRendering.h src/Engine/G_Pathfinding.h \
 src/Engine/G_AI.h
src/Engine/T_TextRendering.o: src/Engine/T_TextRendering.c \
 src/Engine/T_TextRendering.h src/Engine/D_AssetsManager.h \
 src/Engine/U_DataTypes.h src/Engine/U_Timer.h src/Engine/R_Rendering.h \
 src/Engine/M_Map.h src/Engine/A_Application.h src/Engine/U_Utilities.h
src/Engine/U_Timer.o: src/Engine/U_Timer.c src/Engine/U_Timer.h
src/Engine/U_Utilities.o: src/Engine/U_Utilities.c \
 src/Engine/U_Utilities.h src/Engine/U_DataTypes.h src/Engine/U_Timer.h
src/main.o: src/main.c src/Engine/A_Application.h \
 src/Engine/I_InputHandling.h src/Engine/A_Application.h \
 src/Engine/G_Player.h src/Engine/G_Game.h src/Engine/R_Rendering.h \
 src/Engine/M_Map.h src/Engine/D_AssetsManager.h src/Engine/U_DataTypes.h \
 src/Engine/U_Timer.h src/Engine/U_Utilities.h src/Engine/G_MainMenu.h \
 src/Engine/M_Map.h src/Engine/G_Player.h src/Engine/G_Game.h \
 src/Engine/D_AssetsManager.h
