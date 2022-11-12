#include <string.h>
#include "M_Map.h"
#include "G_Game.h"
#include "G_AI.h"

// Reads a line at at ime from a buffer
typedef struct lineReader_s {
    char* buf;
    char* end;
    char* ptr;
} lineReader_t;

map_t currentMap;

static char* I_ReadLine(char *, size_t, lineReader_t *);
static void I_LoadWallMapFromFile(wallObject_t map[MAP_HEIGHT][MAP_WIDTH], lineReader_t *lr);
static void I_LoadMapFromFile(int map[MAP_HEIGHT][MAP_WIDTH], lineReader_t* lr);
static void I_LoadIntFromFile(lineReader_t* lr, int* toLoad);
static void I_LoadBoolFromFile(lineReader_t* lr, bool* toLoad);
static void I_LoadFloatFromFile(lineReader_t* lr, float* toLoad);

// -------------------------------
// Loads the map from the file named mapID
// -------------------------------
void M_LoadMapAsCurrent(char* mapID)
{
      SDL_RWops* fp;
      lineReader_t lr;
      char filepath[MAX_FILEPATH_L] = "Data/maps/";

      // Find path
      strcat(filepath, mapID);
      strcat(filepath, ".tmap");

      SDL_Log("Loading map... %s\n", filepath);

      // Open file
      fp = SDL_RWFromFile(filepath, "r");
      if(fp == NULL)
      {
            SDL_Log("Map %s is not present in the Data folder. Aborting. \n", mapID);
            return;
      }
      size_t size = SDL_RWsize(fp);
      lr.buf = lr.ptr = calloc(size, 1);
      lr.end = lr.buf + SDL_RWread(fp, lr.buf, 1, size);
      SDL_RWclose(fp);

      // Load Map
      char curLine[MAX_STRL_R];   // Current line we're reading
      char* str;                  // Used to strchr
      int indx;                   // Index of the =
      int i;                      // Index for writing in new string

      // --------------------
      // Read ID
      // --------------------
      I_ReadLine(curLine, MAX_STRL_R, &lr);

      // Find index for reading
      str = strchr(curLine, '=');
      indx = (int)(str - curLine) + 1;

      // Init index for writing
      i = 0;
      
      // Write
      while(curLine[indx] != ';' && curLine[indx] != '\n' && curLine[indx] != 0)
      {
            currentMap.id[i] = curLine[indx];
            i++;
            indx++;
      }
      currentMap.id[i] = '\0';

      // --------------------
      // Read MapName
      // --------------------
      I_ReadLine(curLine, MAX_STRL_R, &lr);

      // Find index for reading
      str = strchr(curLine, '=');
      indx = (int)(str - curLine) + 1;

      // Init index for writing
      i = 0;

      // Write
      while(curLine[indx] != ';' && curLine[indx] != '\n' && curLine[indx] != 0)
      {
            currentMap.name[i] = curLine[indx];
            i++;
            indx++;
      }
      currentMap.name[i] = '\0';

      
      I_LoadIntFromFile(&lr, &currentMap.playerStartingLevel);

      // Load Starting player pos
      I_LoadIntFromFile(&lr, &currentMap.playerStartingGridX);
      I_LoadIntFromFile(&lr, &currentMap.playerStartingGridY);
      I_LoadFloatFromFile(&lr, &currentMap.playerStartingRot);

      // Load Wall Map
      I_LoadWallMapFromFile(currentMap.level0, &lr);
      I_LoadWallMapFromFile(currentMap.level1, &lr);
      I_LoadWallMapFromFile(currentMap.level2, &lr);

      // Load Floor Map
      I_LoadMapFromFile(currentMap.floorMap, &lr);

      // Load Ceiling Map
      I_LoadMapFromFile(currentMap.ceilingMap, &lr);
      
      // Load Sprites Map
      I_LoadMapFromFile(currentMap.spritesMapLevel0, &lr);
      I_LoadMapFromFile(currentMap.spritesMapLevel1, &lr);
      I_LoadMapFromFile(currentMap.spritesMapLevel2, &lr);

      // Read Wall Lighting
      I_LoadFloatFromFile(&lr, &currentMap.wallLight);

      // Read Ceiling Lighting
      I_LoadFloatFromFile(&lr, &currentMap.floorLight);

      // Read SkyID
      I_LoadIntFromFile(&lr, &currentMap.skyID);

      // Read hasAbsCeiling
      I_LoadBoolFromFile(&lr, &currentMap.hasAbsCeiling);

      // Read absCeilingLevel
      I_LoadIntFromFile(&lr, &currentMap.absCeilingLevel);
      
      free(lr.buf);

      // Load the TMap
      M_LoadObjectTMap();

      // Load the Collision Map
      M_LoadCollisionMaps();

      SDL_Log("Map loaded successfully! ID: %s\n", currentMap.id);
}

// -------------------------------
// Loads the object map
// -------------------------------
void M_LoadObjectTMap(void)
{
      for(int i = 0; i < allDynamicSpritesLength; i++)
            if(allDynamicSprites[i])
                  free(allDynamicSprites[i]);

      // Clear maps
      for(int y = 0; y < MAP_HEIGHT; y++)
            for(int x = 0; x < MAP_WIDTH; x++)
                  {
                        currentMap.dynamicSpritesLevel0[y][x] = NULL;
                        currentMap.dynamicSpritesLevel1[y][x] = NULL;
                        currentMap.dynamicSpritesLevel2[y][x] = NULL;
                        currentMap.deadDynamicSpritesLevel0[y][x] = NULL;
                        currentMap.deadDynamicSpritesLevel1[y][x] = NULL;
                        currentMap.deadDynamicSpritesLevel2[y][x] = NULL;
                  }

      allDynamicSpritesLength = 0;
      
      for(int y = 0; y < MAP_HEIGHT; y++)
            for(int x = 0; x < MAP_WIDTH; x++)
                  {     
                        // LEVEL 0 
                        // Initialize
                        currentMap.objectTMapLevel0[y][x] = ObjT_Empty;

                        // Check if it's a wall
                        int wallID = currentMap.level0[y][x].assetID;
                        if(wallID > 0)
                        {
                              currentMap.objectTMapLevel0[y][x] = ObjT_Wall;

                              // Check if it is a door
                              if(U_GetBit(&tomentdatapack.walls[wallID]->flags, 2) == 1)
                              {
                                    currentMap.objectTMapLevel0[y][x] = ObjT_Door;
                              }

                              if (U_GetBit(&tomentdatapack.walls[wallID]->flags, 3) == 1)
                              {
                                    currentMap.objectTMapLevel0[y][x] = ObjT_Trigger;
                              }
                        }

                        // Check if it's a sprite (overrides doors, but spirtes should never be placed on top of walls)
                        int spriteID = currentMap.spritesMapLevel0[y][x];
                        if(spriteID > 0)
                        {
                              currentMap.objectTMapLevel0[y][x] = ObjT_Sprite;

                              // Check if this is a dynamic sprite
                              if(U_GetBit(&tomentdatapack.sprites[spriteID]->flags, 2) == 1)
                              {
                                    // Construct the dynamic sprite in base of its id
                                    currentMap.dynamicSpritesLevel0[y][x] = (dynamicSprite_t*)malloc(sizeof(dynamicSprite_t));
                                    dynamicSprite_t* cur = currentMap.dynamicSpritesLevel0[y][x];

                                    G_AIInitialize(cur, 0, spriteID, x, y);

                                    // The rest is calculated at runtime
                              }
                        }


                        // LEVEL 1
                        // Initialize
                        currentMap.objectTMapLevel1[y][x] = ObjT_Empty;

                        // Check if it's a wall
                        wallID = currentMap.level1[y][x].assetID;
                        if(wallID > 0)
                        {
                              currentMap.objectTMapLevel1[y][x] = ObjT_Wall;

                              // Check if it is a door
                              if(U_GetBit(&tomentdatapack.walls[wallID]->flags, 2) == 1)
                              {
                                    currentMap.objectTMapLevel1[y][x] = ObjT_Door;
                              }
                              if (U_GetBit(&tomentdatapack.walls[wallID]->flags, 3) == 1)
                              {
                                    currentMap.objectTMapLevel1[y][x] = ObjT_Trigger;
                              }
                        }

                        // Check if it's a sprite (overrides doors, but spirtes should never be placed on top of walls)
                        spriteID = currentMap.spritesMapLevel1[y][x];
                        if(spriteID > 0)
                        {
                              currentMap.objectTMapLevel1[y][x] = ObjT_Sprite;

                              // Check if this is a dynamic sprite
                              if(U_GetBit(&tomentdatapack.sprites[spriteID]->flags, 2) == 1)
                              {
                                    currentMap.dynamicSpritesLevel1[y][x] = (dynamicSprite_t*)malloc(sizeof(dynamicSprite_t));
                                    dynamicSprite_t* cur = currentMap.dynamicSpritesLevel1[y][x];
                                    
                                    G_AIInitialize(cur, 1, spriteID, x, y);

                                    // The rest is calculated at runtime
                              }
                        }


                        // LEVEL 2
                        // Initialize
                        currentMap.objectTMapLevel2[y][x] = ObjT_Empty;

                        // Check if it's a wall
                        wallID = currentMap.level2[y][x].assetID;
                        if(wallID > 0)
                        {
                              currentMap.objectTMapLevel2[y][x] = ObjT_Wall;

                              // Check if it is a door
                              if(U_GetBit(&tomentdatapack.walls[wallID]->flags, 2) == 1)
                              {
                                    currentMap.objectTMapLevel2[y][x] = ObjT_Door;
                              }
                              if (U_GetBit(&tomentdatapack.walls[wallID]->flags, 3) == 1)
                              {
                                    currentMap.objectTMapLevel2[y][x] = ObjT_Trigger;
                              }
                        }

                        // Check if it's a sprite (overrides doors, but spirtes should never be placed on top of walls)
                        spriteID = currentMap.spritesMapLevel2[y][x];
                        if(spriteID > 0)
                        {
                              currentMap.objectTMapLevel2[y][x] = ObjT_Sprite;

                              // Check if this is a dynamic sprite
                              if(U_GetBit(&tomentdatapack.sprites[spriteID]->flags, 2) == 1)
                              {
                                    currentMap.dynamicSpritesLevel2[y][x] = (dynamicSprite_t*)malloc(sizeof(dynamicSprite_t));
                                    dynamicSprite_t* cur = currentMap.dynamicSpritesLevel2[y][x];
                                    
                                    G_AIInitialize(cur, 2, spriteID, x, y);

                                    // The rest is calculated at runtime
                              }
                        }
                  }
}

// -------------------------------
// Loads the collision map
//
// Collision:     1 = For everything
//                2 = For player but not projectiles
// -------------------------------
void M_LoadCollisionMaps(void)
{
      // Load Level 0
      for(int y = 0; y < MAP_HEIGHT; y++)
            for(int x = 0; x < MAP_WIDTH; x++)
                  {
                        // LEVEL 0
                        // Initialize
                        currentMap.collisionMapLevel0[y][x] = 0;

                        // Check if it's a wall
                        int wallID = currentMap.level0[y][x].assetID;
                        if(wallID > 0)
                        {
                              // If this is an invisible wall, projectile should be able to go throught it
                              if(U_GetBit(&tomentdatapack.walls[wallID]->flags, 4))
                              {
                                    currentMap.collisionMapLevel0[y][x] = 2;
                              }
                              else
                                    currentMap.collisionMapLevel0[y][x] = 1;
                        }

                        // Check if it's a sprite (overrides doors, but spirtes should never be placed on top of walls)
                        int spriteID = currentMap.spritesMapLevel0[y][x];
                        if(spriteID > 0 && U_GetBit(&tomentdatapack.sprites[spriteID]->flags, 0) == 1 &&
                              U_GetBit(&tomentdatapack.sprites[spriteID]->flags, 2) == 0)
                              currentMap.collisionMapLevel0[y][x] = 1;


                        // LEVEL 1
                        // Initialize
                        currentMap.collisionMapLevel1[y][x] = 0;

                        // Check if it's a wall
                        wallID = currentMap.level1[y][x].assetID;
                        if(wallID > 0)
                        {
                              // If this is an invisible wall, projectile should be able to go throught it
                              if(U_GetBit(&tomentdatapack.walls[wallID]->flags, 4))
                              {
                                    currentMap.collisionMapLevel1[y][x] = 2;
                              }
                              else
                              currentMap.collisionMapLevel1[y][x] = 1;
                        }

                        // Check if it's a sprite (overrides doors, but spirtes should never be placed on top of walls)
                        spriteID = currentMap.spritesMapLevel1[y][x];
                        if(spriteID > 0 && U_GetBit(&tomentdatapack.sprites[spriteID]->flags, 0) == 1 &&
                              U_GetBit(&tomentdatapack.sprites[spriteID]->flags, 2) == 0)
                              currentMap.collisionMapLevel1[y][x] = 1;

                        // LEVEL 2
                        // Initialize
                        currentMap.collisionMapLevel2[y][x] = 0;

                        // Check if it's a wall
                        wallID = currentMap.level2[y][x].assetID;
                        if(wallID > 0)
                        {
                              // If this is an invisible wall, projectile should be able to go throught it
                              if(U_GetBit(&tomentdatapack.walls[wallID]->flags, 4))
                              {
                                    currentMap.collisionMapLevel2[y][x] = 2;
                              }
                              else
                                    currentMap.collisionMapLevel2[y][x] = 1;
                        }

                        // Check if it's a sprite (overrides doors, but spirtes should never be placed on top of walls)
                        spriteID = currentMap.spritesMapLevel2[y][x];
                        if(spriteID > 0 && U_GetBit(&tomentdatapack.sprites[spriteID]->flags, 0) == 1 &&
                              U_GetBit(&tomentdatapack.sprites[spriteID]->flags, 2) == 0)
                              currentMap.collisionMapLevel2[y][x] = 1;
                  }
}


static char *I_ReadLine(char *line, size_t length, lineReader_t *lr)
{
    SDL_assert(length);

    char *end = memchr(lr->ptr, '\n', lr->end - lr->ptr);
    if (end) {
        end++;
    } else {
        end = lr->end;
    }

    size_t lineLength = end - lr->ptr;
    size_t copyLength = lineLength > length-1 ? length-1 : lineLength;
    memcpy(line, lr->ptr, copyLength);
    line[copyLength] = 0;
    lr->ptr = end;
    return copyLength ? line : NULL;
}

static void I_LoadWallMapFromFile(wallObject_t map[MAP_HEIGHT][MAP_WIDTH], lineReader_t *lr)
{
      // Load Map
      char curLine[MAX_STRL_R];   // Current line we're reading
      char* str;                  // Used to strchr
      int indx;                   // Index of the =

      I_ReadLine(curLine, MAX_STRL_R, lr); // Layout =
      I_ReadLine(curLine, MAX_STRL_R, lr); // [ start of map
      I_ReadLine(curLine, MAX_STRL_R, lr); // First Row

      // Find the first row
      str = strchr(curLine, '{');
      indx = (int)(str - curLine) + 1; // '('

      bool mapDone = false;
      int column = 0;
      int row = 0;

      while(!mapDone)
      {
            // Read columns
            while(curLine[indx] != '}')
            {
                  indx++;
                  unsigned sum = 0;
                  
                  // Read Asset ID
                  do
                  {
                        sum *= 10;
                        sum += curLine[indx] - '0';
                        map[column][row].assetID = sum; // Set int value
                        indx++;  
                  } while(curLine[indx] != ',' && curLine[indx] != ')');
                  indx++;
                  
                  // Read Top Texture
                  sum = 0;
                  do
                  {
                        sum *= 10;
                        sum += curLine[indx] - '0';
                        map[column][row].texturesArray[TEXTURE_ARRAY_TOP] = sum; // Set int value
                        indx++;  
                  } while(curLine[indx] != ',' && curLine[indx] != ')');
                  indx++;

                  // Read Bottom Texture
                  sum = 0;
                  do
                  {
                        sum *= 10;
                        sum += curLine[indx] - '0';
                        map[column][row].texturesArray[TEXTURE_ARRAY_BOTTOM] = sum; // Set int value
                        indx++;  
                  } while(curLine[indx] != ',' && curLine[indx] != ')');
                  indx++;

                  // Read Left Texture
                  sum = 0;
                  do
                  {
                        sum *= 10;
                        sum += curLine[indx] - '0';
                        map[column][row].texturesArray[TEXTURE_ARRAY_LEFT] = sum; // Set int value
                        indx++;  
                  } while(curLine[indx] != ',' && curLine[indx] != ')');
                  indx++;

                  // Read Right Texture
                  sum = 0;
                  do
                  {
                        sum *= 10;
                        sum += curLine[indx] - '0';
                        map[column][row].texturesArray[TEXTURE_ARRAY_RIGHT] = sum; // Set int value
                        indx++;  
                  } while(curLine[indx] != ',' && curLine[indx] != ')');
                  indx++;

                  // Read Forward Texture
                  sum = 0;
                  do
                  {
                        sum *= 10;
                        sum += curLine[indx] - '0';
                        map[column][row].texturesArray[TEXTURE_ARRAY_UP] = sum; // Set int value
                        indx++;  
                  } while(curLine[indx] != ',' && curLine[indx] != ')');
                  indx++;

                  // Read Back Texture
                  sum = 0;
                  do
                  {
                        sum *= 10;
                        sum += curLine[indx] - '0';
                        map[column][row].texturesArray[TEXTURE_ARRAY_DOWN] = sum; // Set int value
                        indx++;  
                  } while(curLine[indx] != ',' && curLine[indx] != ')');
                  indx++;

                  // Read Data
                  int dataIndex = 0;
                  indx++;
                  while(curLine[indx] != '"')
                  {
                        map[column][row].data[dataIndex] = curLine[indx];
                        dataIndex++;
                        indx++;
                  }
                  map[column][row].data[dataIndex] = '\0';
                  indx++;
                  indx++;

                  // If next is comma, continue and get next number
                  if(curLine[indx] == ',')
                  {
                        indx++;
                        row++;
                  }
                  //SDL_Log("%c!\n", curLine[indx]);
            }

            // Row end, check if there's a next row or if it is finished
            if(curLine[indx + 1] == ',')
            {
                  // There is a next column
                  column++;
                  indx = 1; // Move at the start of the next column
                  row = 0;
                  I_ReadLine(curLine, MAX_STRL_R, lr); // Get next line
                  continue;
            }
            else if(curLine[indx + 1] == ']')
            {
                  // Map has finished loading
                  mapDone = true;
                  break;
            }
      }

      SDL_Log("%s\n", map[0][0].data);
}

static void I_LoadMapFromFile(int map[MAP_HEIGHT][MAP_WIDTH], lineReader_t* lr)
{
      // Load Map
      char curLine[MAX_STRL_R];   // Current line we're reading
      char* str;                  // Used to strchr
      int indx;                   // Index of the =

      I_ReadLine(curLine, MAX_STRLEN, lr); // Layout =
      I_ReadLine(curLine, MAX_STRLEN, lr); // [ start of map
      I_ReadLine(curLine, MAX_STRLEN, lr); // First Row

      // Find the first row
      str = strchr(curLine, '{');
      indx = (int)(str - curLine) + 1;

      bool mapDone = false;
      int column = 0;
      int row = 0;

      while(!mapDone)
      {
            // Read columns
            while(curLine[indx] != '}')
            {
                  unsigned sum = 0;

                  do
                  {
                        sum *= 10;
                        sum += curLine[indx] - '0';
                        map[column][row] = sum; // Set int value
                        indx++;  
                  } while(curLine[indx] != ',' && curLine[indx] != '}');


                  // If next is comma, continue and get next number
                  if(curLine[indx] == ',')
                  {
                        indx++;
                        row++;
                  }

                  //SDL_Log("%c!\n", curLine[indx]);
            }

            // Row end, check if there's a next row or if it is finished
            if(curLine[indx + 1] == ',')
            {
                  // There is a next column
                  column++;
                  indx = 1; // Move at the start of the next column
                  row = 0;
                  I_ReadLine(curLine, MAX_STRLEN, lr); // Get next line
                  continue;
            }
            else if(curLine[indx + 1] == ']')
            {
                  // Map has finished loading
                  mapDone = true;
                  break;
            }
      }
}

static void I_LoadIntFromFile(lineReader_t* lr, int* toLoad)
{
      // Load Map
      char curLine[MAX_STRL_R];   // Current line we're reading
      char* str;                  // Used to strchr
      int indx;                   // Index of the =
      int i;                      // Index for writing in new string

      // --------------------
      // Read SkyID
      // --------------------
      I_ReadLine(curLine, MAX_STRLEN, lr); // Layout =

      // Find index for reading
      str = strchr(curLine, '=');
      indx = (int)(str - curLine) + 1;

      // Init index for writing
      i = 0;
      
      char tempStr[256];
      // Write
      while(curLine[indx] != ';' && curLine[indx] != '\n' && curLine[indx] != 0)
      {
            tempStr[i] = curLine[indx];
            i++;
            indx++;
      }
      tempStr[i] = '\0';

      // Convert to float
      *toLoad = atoi(tempStr);
}

static void I_LoadBoolFromFile(lineReader_t* lr, bool* toLoad)
{
      // Load Map
      char curLine[MAX_STRL_R];   // Current line we're reading
      char* str;                  // Used to strchr
      int indx;                   // Index of the =
      int i;                      // Index for writing in new string

      // --------------------
      // Read SkyID
      // --------------------
      I_ReadLine(curLine, MAX_STRLEN, lr); // Layout =

      // Find index for reading
      str = strchr(curLine, '=');
      indx = (int)(str - curLine) + 1;

      // Init index for writing
      i = 0;
      
      char tempStr[256];
      // Write
      while(curLine[indx] != ';' && curLine[indx] != '\n' && curLine[indx] != 0)
      {
            tempStr[i] = curLine[indx];
            i++;
            indx++;
      }
      tempStr[i] = '\0';

      // Convert to float
      *toLoad = atoi(tempStr);
}

static void I_LoadFloatFromFile(lineReader_t* lr, float* toLoad)
{
      // Load Map
      char curLine[MAX_STRL_R];   // Current line we're reading
      char* str;                  // Used to strchr
      int indx;                   // Index of the =
      int i;                      // Index for writing in new string

      // --------------------
      // Read SkyID
      // --------------------
      I_ReadLine(curLine, MAX_STRLEN, lr); // Layout =

      // Find index for reading
      str = strchr(curLine, '=');
      indx = (int)(str - curLine) + 1;

      // Init index for writing
      i = 0;
      
      char tempStr[256];
      // Write
      while(curLine[indx] != ';' && curLine[indx] != '\n' && curLine[indx] != 0)
      {
            tempStr[i] = curLine[indx];
            i++;
            indx++;
      }
      tempStr[i] = '\0';

      // Convert to float
      *toLoad = atof(tempStr);
}
