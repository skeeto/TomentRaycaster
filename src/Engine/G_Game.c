#include <math.h>

#include "I_InputHandling.h"
#include "G_Game.h"
#include "P_Physics.h"
#include "M_Map.h"
#include "G_Pathfinding.h"
#include "G_AI.h"
#include "G_MainMenu.h"

// Game Timer
Timer* gameTimer;

// Current Game Time
double curTime = 0;

// Game Time at last tick
double oldTime = 0;

// Doors
int doorstateLevel0[MAP_HEIGHT][MAP_WIDTH];       // State of the door (open, closed, opening, closing)
int doorstateLevel1[MAP_HEIGHT][MAP_WIDTH];       // State of the door (open, closed, opening, closing)
int doorstateLevel2[MAP_HEIGHT][MAP_WIDTH];       // State of the door (open, closed, opening, closing)

float doorpositionsLevel0[MAP_HEIGHT][MAP_WIDTH]; // Timer holding the position of the door
float doorpositionsLevel1[MAP_HEIGHT][MAP_WIDTH]; // Timer holding the position of the door
float doorpositionsLevel2[MAP_HEIGHT][MAP_WIDTH]; // Timer holding the position of the door

// Projectiles travelling in the world
projectileNode_t* projectilesHead = NULL;

// Projectiles that hit something and are exploding
projectileNode_t* explodingProjectilesHead = NULL;

//-------------------------------------
// Initialize game related stuff 
//-------------------------------------
void G_InitGame(void)
{
    // Initialize game
    if(gameTimer == NULL)
        gameTimer = U_TimerCreateNew();

    gameTimer->Init(gameTimer);

    // Initialize Doors //
    memset(doorstateLevel0, 0, MAP_HEIGHT*MAP_WIDTH*sizeof(int));
    memset(doorstateLevel1, 0, MAP_HEIGHT*MAP_WIDTH*sizeof(int));
    memset(doorstateLevel2, 0, MAP_HEIGHT*MAP_WIDTH*sizeof(int));

    // All doors start closed
    for(int y = 0; y < MAP_HEIGHT; y++)
        for(int x = 0; x < MAP_WIDTH; x++)
        {
            doorpositionsLevel0[y][x] = DOOR_FULLY_CLOSED;
            doorpositionsLevel1[y][x] = DOOR_FULLY_CLOSED;
            doorpositionsLevel2[y][x] = DOOR_FULLY_CLOSED;
        }

    // Empty projectile list
    if(projectilesHead != NULL)
    {
        projectileNode_t* current = projectilesHead;

        while(current != NULL)
        {
            projectileNode_t* tmp = current;
            current = current->next;
            free(tmp);
        }
        
        projectilesHead = NULL;
    }

    G_ChangeMap("lvl1");
    
    gameTimer->Start(gameTimer);
}

//-------------------------------------
// Game Tick 
//-------------------------------------
void G_GameLoop(void)
{

    switch(application.gamestate)
    {
        case GSTATE_MENU:
            G_StateMenuLoop();
            break;

        case GSTATE_GAME:
            G_StateGameLoop();
            break;

        default:
            break;
    }
}


void G_StateGameLoop(void)
{
    curTime = gameTimer->GetTicks(gameTimer);

    P_PhysicsTick();

    // Handle input
    I_HandleInputGame();

    // If the game state was changed by the game input handler prevent this loop from going forward
    if(application.gamestate != GSTATE_GAME)
        return;

    // Do stuff
    G_PlayerTick();
    G_UpdateDoors();
    G_AIUpdate();
    G_UpdateProjectiles();
    P_PhysicsEndTick();

    // Render
    // Creates the frame
    R_ComposeFrame();

    // Displays it on the screen
    R_FinishUpdate();

    oldTime = curTime;
}

void G_StateMenuLoop(void)
{
    // Keep physic ticking even in menus, because game can be paused
    P_PhysicsTick();

    // Handles input
    I_HandleInputMenu();

    // If the game state was changed by the menu input handler prevent this loop from going forward
    // Not doing that could cause the program to freeze at the R_ComposeFrame() call, because in this loop we would try to render as if we were in the game state without having things initialized/updated
    if(application.gamestate != GSTATE_MENU)
        return;

    P_PhysicsEndTick();

    // Clears current render
    SDL_FillRect(win_surface, NULL, r_blankColor);

    // Creates the frame
    R_ComposeFrame();

    // Displays it on the screen
    R_FinishUpdate();
}

//-------------------------------------
// Update Doors by moving them in base of their timer
//-------------------------------------
void G_UpdateDoors(void)
{
    for(int y = 0; y < MAP_HEIGHT; y++)
        for(int x = 0; x < MAP_WIDTH; x++)
            {
                // LEVEL 0 
                // If the door is closed or open, continue
                if(doorstateLevel0[y][x] == DState_Closed || doorstateLevel0[y][x] == DState_Open)
                {
                    // continue
                }
                else
                {
                    // Open the door
                    if(doorstateLevel0[y][x] == DState_Opening)
                    {
                        if(doorpositionsLevel0[y][x] > DOOR_FULLY_OPENED &&
                            doorpositionsLevel0[y][x] - DOOR_OPEN_SPEED * deltaTime > DOOR_FULLY_OPENED) // check if a step is too big
                            doorpositionsLevel0[y][x] -= DOOR_OPEN_SPEED * deltaTime;
                        else
                        {
                            // Door opened
                            doorpositionsLevel0[y][x] = DOOR_FULLY_OPENED;
                            doorstateLevel0[y][x] = DState_Open;

                            // Update collision map
                            currentMap.collisionMapLevel0[y][x] = 0;
                        }
                    }
                    else if(doorstateLevel0[y][x] == DState_Closing)
                    {
                        if(doorpositionsLevel0[y][x] < DOOR_FULLY_CLOSED &&
                            doorpositionsLevel0[y][x] + DOOR_CLOSE_SPEED * deltaTime < DOOR_FULLY_CLOSED) // check if step is too big
                            doorpositionsLevel0[y][x] += DOOR_CLOSE_SPEED * deltaTime;
                        else
                        {
                            // Door closed
                            doorpositionsLevel0[y][x] = DOOR_FULLY_CLOSED;
                            doorstateLevel0[y][x] = DState_Closed;

                            // Update collision map
                            currentMap.collisionMapLevel0[y][x] = 1;
                        }
                    }
                }

                // LEVEL 1
                // If the door is closed or open, continue
                if(doorstateLevel1[y][x] == DState_Closed || doorstateLevel1[y][x] == DState_Open)
                {
                    // continue
                }
                else
                {
                    // Open the door
                    if(doorstateLevel1[y][x] == DState_Opening)
                    {
                        if(doorpositionsLevel1[y][x] > DOOR_FULLY_OPENED &&
                            doorpositionsLevel1[y][x] - DOOR_OPEN_SPEED * deltaTime > DOOR_FULLY_OPENED) // check if a step is too big
                            doorpositionsLevel1[y][x] -= DOOR_OPEN_SPEED * deltaTime;
                        else
                        {
                            // Door opened
                            doorpositionsLevel1[y][x] = DOOR_FULLY_OPENED;
                            doorstateLevel1[y][x] = DState_Open;

                            // Update collision map
                            currentMap.collisionMapLevel1[y][x] = 0;
                        }
                    }
                    else if(doorstateLevel1[y][x] == DState_Closing)
                    {
                        if(doorpositionsLevel1[y][x] < DOOR_FULLY_CLOSED &&
                            doorpositionsLevel1[y][x] + DOOR_CLOSE_SPEED * deltaTime < DOOR_FULLY_CLOSED) // check if step is too big
                            doorpositionsLevel1[y][x] += DOOR_CLOSE_SPEED * deltaTime;
                        else
                        {
                            // Door closed
                            doorpositionsLevel1[y][x] = DOOR_FULLY_CLOSED;
                            doorstateLevel1[y][x] = DState_Closed;

                            // Update collision map
                            currentMap.collisionMapLevel1[y][x] = 1;
                        }
                    }
                }

                // LEVEL 2
                // If the door is closed or open, continue
                if(doorstateLevel2[y][x] == DState_Closed || doorstateLevel2[y][x] == DState_Open)
                {
                    // continue
                }
                else
                {
                    // Open the door
                    if(doorstateLevel2[y][x] == DState_Opening)
                    {
                        if(doorpositionsLevel2[y][x] > DOOR_FULLY_OPENED &&
                            doorpositionsLevel2[y][x] - DOOR_OPEN_SPEED * deltaTime > DOOR_FULLY_OPENED) // check if a step is too big
                            doorpositionsLevel2[y][x] -= DOOR_OPEN_SPEED * deltaTime;
                        else
                        {
                            // Door opened
                            doorpositionsLevel2[y][x] = DOOR_FULLY_OPENED;
                            doorstateLevel2[y][x] = DState_Open;

                            // Update collision map
                            currentMap.collisionMapLevel2[y][x] = 0;
                        }
                    }
                    else if(doorstateLevel2[y][x] == DState_Closing)
                    {
                        if(doorpositionsLevel2[y][x] < DOOR_FULLY_CLOSED &&
                            doorpositionsLevel2[y][x] + DOOR_CLOSE_SPEED * deltaTime < DOOR_FULLY_CLOSED) // check if step is too big
                            doorpositionsLevel2[y][x] += DOOR_CLOSE_SPEED * deltaTime;
                        else
                        {
                            // Door closed
                            doorpositionsLevel2[y][x] = DOOR_FULLY_CLOSED;
                            doorstateLevel2[y][x] = DState_Closed;

                            // Update collision map
                            currentMap.collisionMapLevel2[y][x] = 1;
                        }
                    }
                }
            }
}

void G_ChangeMap(char* mapID)
{
    // Check if game has ended
    if(strcmp("END_GAME", mapID) == 0)
    {
        player.hasBeenInitialized = false;
        G_SetMenu(&EndGameMenu);
        A_ChangeState(GSTATE_MENU);
    }
    else
    {
        M_LoadMapAsCurrent(mapID);
        G_InitPlayer();
    }
}

void G_UpdateProjectiles(void)
{   
    projectileNode_t* cur = projectilesHead;
    int i = 0;
    while(cur != NULL)
    {
        // Update base info needed for rendering
        cur->this.base.centeredPos.x = cur->this.base.pos.x;
        cur->this.base.centeredPos.y = cur->this.base.pos.y;

        cur->this.base.pSpacePos.x = cur->this.base.centeredPos.x - player.centeredPos.x;
        cur->this.base.pSpacePos.y = cur->this.base.centeredPos.y - player.centeredPos.y;

        cur->this.base.gridPos.x = cur->this.base.centeredPos.x / TILE_SIZE;
        cur->this.base.gridPos.y = cur->this.base.centeredPos.y / TILE_SIZE;

        cur->this.base.dist = sqrt(cur->this.base.pSpacePos.x*cur->this.base.pSpacePos.x + cur->this.base.pSpacePos.y*cur->this.base.pSpacePos.y);

        // If the projectile hasnt hit anything, check for hit
        if(!cur->this.isBeingDestroyed)
        {
            // Update pos
            cur->this.base.pos.x += cos(cur->this.base.angle) * cur->this.speed * deltaTime;
            cur->this.base.pos.y += sin(cur->this.base.angle) * cur->this.speed * deltaTime;

             // Determine Projectile's level
             // half a tile is added to the base.z because the collision with the bottom part should always be a bit higer than normal (otherwise the projectile hits a wall with the transparent part of the sprite)
            cur->this.base.level = (int)floor((cur->this.base.z+(HALF_TILE_SIZE)) / TILE_SIZE);
            cur->this.base.level = SDL_clamp(cur->this.base.level, 0, MAX_N_LEVELS-1);

            if(cur->this.verticalMovementDelta > 0 || cur->this.verticalMovementDelta < 0)
                cur->this.base.z += cur->this.verticalMovementDelta * deltaTime;

            // Check if projectile is not out of map
            bool insideMap = cur->this.base.gridPos.x >= 0 && cur->this.base.gridPos.y >= 0 && cur->this.base.gridPos.x < MAP_WIDTH && cur->this.base.gridPos.y < MAP_HEIGHT && cur->this.base.z > -TILE_SIZE && cur->this.base.z < TILE_SIZE*(MAX_N_LEVELS);

            // Destroy condition
            if(G_CheckCollisionMap(cur->this.base.level, cur->this.base.gridPos.y, cur->this.base.gridPos.x) == 1 || !insideMap)
            {
                cur->this.isBeingDestroyed = true;
                G_AIPlayAnimationOnce(&cur->this, ANIM_DIE);
                return;
            }

            // AI hit
            dynamicSprite_t* sprite = NULL;
            if((sprite = G_GetFromDynamicSpriteMap(cur->this.base.level, cur->this.base.gridPos.y, cur->this.base.gridPos.x)) != NULL &&
                cur->this.aiOwner != sprite && sprite->canBeHit)
            {
                float damage = 0.0f;

                // Damage sprite
                switch(cur->this.base.spriteID)
                {
                    case S_Fireball1:
                        damage = 55.65f;
                        break;

                    case S_IceDart1:
                        damage = 15.0f;
                        break;

                    default:
                        damage = 0.0f;
                        break;
                }

                if(cur->this.isOfPlayer)
                {
                    player.crosshairHit = true;
                    player.crosshairTimer->Start(player.crosshairTimer);
                }

                G_AITakeDamage(sprite, damage);

                cur->this.isBeingDestroyed = true;
                G_AIPlayAnimationOnce(&cur->this, ANIM_DIE);
                return;
            }

            // Player hit
            // Add distance checking for being more precise
            float playerDist = sqrt(cur->this.base.pSpacePos.x*cur->this.base.pSpacePos.x + cur->this.base.pSpacePos.y*cur->this.base.pSpacePos.y);
            if(!cur->this.isOfPlayer && cur->this.base.level == player.level && cur->this.base.gridPos.x == player.gridPosition.x && cur->this.base.gridPos.y == player.gridPosition.y && playerDist < TILE_SIZE-12)
            {
                SDL_Log("%f\n", playerDist);
                float damage = 0.0f;

                // Damage sprite
                switch(cur->this.base.spriteID)
                {
                    case S_Fireball1:
                        damage = 55.65f;
                        break;

                    case S_IceDart1:
                        damage = 15.0f;
                        break;

                    default:
                        damage = 0.0f;
                        break;
                }

                G_PlayerTakeDamage(damage);

                cur->this.isBeingDestroyed = true;
                G_AIPlayAnimationOnce(&cur->this, ANIM_DIE);
                return;
            }
        }
        // The projectile has hit, wait for the death animation to play
        else
        {
            if(cur->this.animFrame >= tomentdatapack.sprites[cur->this.base.spriteID]->animations->animDieSheetLength-1)
            {
                // Destroy this
                if(projectilesHead == cur)
                    projectilesHead = cur->next;

                if(cur->next != NULL)
                    cur->next->previous = cur->previous;

                if(cur->previous != NULL)
                    cur->previous->next = cur->next;

                projectileNode_t* dead = cur;
                cur = cur->next;
                free(dead->this.animTimer);
                free(dead);
                continue;
            }
        }

        // Select Animation & Play it
        switch(cur->this.state)
        {
            case DS_STATE_IDLE:
                cur->this.curAnim = tomentdatapack.sprites[cur->this.base.spriteID]->animations->animIdle;
                cur->this.curAnimLength = tomentdatapack.sprites[cur->this.base.spriteID]->animations->animIdleSheetLength;
                break;

            case DS_STATE_DEAD:
                cur->this.curAnim = tomentdatapack.sprites[cur->this.base.spriteID]->animations->animDie;
                cur->this.curAnimLength = tomentdatapack.sprites[cur->this.base.spriteID]->animations->animDieSheetLength;
                break;

            case DS_STATE_ATTACKING:
                cur->this.curAnim = tomentdatapack.sprites[cur->this.base.spriteID]->animations->animAttack;
                cur->this.curAnimLength = tomentdatapack.sprites[cur->this.base.spriteID]->animations->animAttackSheetLength;
                break;

            default:
                cur->this.curAnim = tomentdatapack.sprites[cur->this.base.spriteID]->animations->animIdle;
                cur->this.curAnimLength = tomentdatapack.sprites[cur->this.base.spriteID]->animations->animIdleSheetLength;
                break;
        }

        if(cur->this.animPlay)
        {
            if(cur->this.animPlayOnce)
            {
                if(cur->this.curAnimLength > 0)
                    cur->this.animFrame = ((int)floor(cur->this.animTimer->GetTicks(cur->this.animTimer) / cur->this.animSpeed) % cur->this.curAnimLength);

                // Prevent loop
                if(cur->this.animFrame >= cur->this.curAnimLength-1)
                {
                    cur->this.animPlay = false;

                    // Go back to idle if it was attacking
                    if(cur->this.state == DS_STATE_ATTACKING)
                    {
                        G_AIPlayAnimationLoop(&cur->this, ANIM_IDLE);
                    }
                }
            }
            else
            {
                // Allow loop
                if(cur->this.curAnimLength > 0)
                    cur->this.animFrame = ((int)floor(cur->this.animTimer->GetTicks(cur->this.animTimer) / cur->this.animSpeed) % cur->this.curAnimLength);
            }
        }

        i++;
        cur = cur->next;
    }
}


void G_SpawnProjectile(int id, float angle, int level, float posx, float posy, float posz, float verticalAngle, bool isOfPlayer, dynamicSprite_t* aiOwner)
{
    // Allocate a node
    projectileNode_t* newNode = (projectileNode_t*)malloc(sizeof(projectileNode_t));

    // Set initial data like pos, dir and speed
    newNode->this.type = DS_TYPE_PROJECTILE;
    newNode->this.curAnim = NULL;
    newNode->this.curAnimLength = 0;
    newNode->this.animPlay = true;
    newNode->this.state = DS_STATE_NULL;
    newNode->this.animTimer = U_TimerCreateNew();
    newNode->this.isBeingDestroyed = false;

    newNode->this.base.spriteID = id;
    newNode->this.base.sheetLength = tomentdatapack.spritesSheetsLenghtTable[id];
    newNode->this.speed = 500.0f;

    newNode->this.base.active = true;
    newNode->this.base.angle = angle;
    newNode->this.base.level = level;

    newNode->this.base.pos.x = posx;
    newNode->this.base.pos.y = posy;
    newNode->this.base.z = posz;

    // Vertical angle is how much the entity that launches the projectile is looking up/down
    newNode->this.verticalMovementDelta = verticalAngle;

    newNode->this.base.gridPos.x = posx / TILE_SIZE;
    newNode->this.base.gridPos.y = posy / TILE_SIZE;

    newNode->this.base.centeredPos.x = newNode->this.base.pos.x + (HALF_TILE_SIZE);
    newNode->this.base.centeredPos.y = newNode->this.base.pos.y + (HALF_TILE_SIZE);
    
    newNode->this.base.pSpacePos.x = newNode->this.base.centeredPos.x - player.centeredPos.x;
    newNode->this.base.pSpacePos.y = newNode->this.base.centeredPos.y - player.centeredPos.y;

    newNode->this.isOfPlayer = isOfPlayer;
    newNode->this.aiOwner = aiOwner;

    newNode->next = NULL;
    if(projectilesHead == NULL)
    {
        projectilesHead = newNode;
        projectilesHead->next = NULL;
        projectilesHead->previous = NULL;
    }
    else
    {
        projectileNode_t* current = projectilesHead;

        while(current->next != NULL)
            current = current->next;

        // Now we can add
        current->next = newNode;
        current->next->next = NULL;
        newNode->previous = current;
    }

    // Play idle anim
    G_AIPlayAnimationLoop(&newNode->this, ANIM_IDLE);
}
