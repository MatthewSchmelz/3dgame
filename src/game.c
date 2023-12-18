#include <SDL.h>

#include "simple_logger.h"
#include "gfc_input.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_audio.h"


#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_particle.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_draw.h"


#include "entity.h"
#include "agumon.h"
#include "door.h"
#include "shambler.h"
#include "watcher.h"
#include "drainer.h"
#include "harpoon.h"
#include "player.h"
#include "world.h"
#include "AIDirector.h"


extern int __DEBUG; 


// Declare global variables for entities
Entity* shambler = NULL;
Entity* door = NULL;
Entity* drainer = NULL;
Entity* watcher1 = NULL;
Entity* watcher2 = NULL;
Entity* player = NULL;
Entity* harpoonL = NULL;
Entity* harpoonR = NULL;

int cargo = 0;
int arcane = 0;
int madman = 0;

int doorstate =0;

int nights = 6;
int currentNight = 2;

int sanity;
int time;
int Lharpoon;
int Rharpoon;
int power;
int jumpscareover =0;


SDL_TimerID timerTime;
SDL_TimerID timerPower;
SDL_TimerID timerMonster;
SDL_TimerID timerNight;
SDL_TimerID timerAnim;
SDL_TimerID timerJump;

int menu = 0;
int splash = 1;
int custom = 0;

int speedy = 0;
int electronics = 0;
int agro = 0;
int endless = 0;
int blind = 0;


Uint32 callback_jump(Uint32 interval, void* param) {
    jumpscareover = 1;
}

Uint32 callback_anim(Uint32 interval, void* param) {
    watcher_anim(watcher1);
    watcher_anim(watcher2);
    return interval;
}


Uint32 callback_night(Uint32 interval, void* param) {
    if (player == NULL) {
        slog("Invalid Player");
        return 0;
    }
    //We need to reset all entities and then display the current night.
    //resetting player
    player->location = 1;
    player->speed = 15;
    player->drainers = 0;
    player->rightlight = 0;
    player->leftlight = 0;
    player->Rharpoon = Rharpoon;
    player->Lharpoon = Lharpoon;
    player->power = power;
    player->time = time;
    player->sanity = sanity;

    player->position.z = 15;
    player->position.x = 0;
    player->position.y = 7;
    player->rotation.x = -GFC_PI + 1.0472;
    player->rotation.z = -GFC_HALF_PI;

    //resetting entities
    watcher1->stage = 0;
    watcher2->stage = 0;
    shambler->stage = 0;
    drainer->stage = 0;

    return 0;
}


Uint32 callback_time(Uint32 interval, void* param) {
    

    if (player == NULL) {
        slog("Invalid Player");
        return 0;
    }

   

    // Ensure player->time doesn't go below zero
    if (player->time > 0) {
        player->time--;
        if (madman == 1) {
            player->time--;
            player->sanity--;
        }
    }

    if (cargo > 0) {
        cargo--;
    }



    return interval; // Re-add the timer
}

Uint32 callback_power(Uint32 interval, void* param) {

    if (player == NULL) {
        slog("Invalid Player");
        return 0;
    }

   
    // Ensure player->time doesn't go below zero
    if (player->power > 0) {
        player->power = player->power - (1 + player->drainers);
    }
    if (player->speed > 0) {
        if (arcane ==1) {
            player->sanity = player->sanity - 2;
        }
        else {
            player->speed--;
        }
       
    }



    return interval; // Re-add the timer
}


Uint32 callback_monster(Uint32 interval, void* param) {

    //Call the AI director to assess the situation
    DirectorChangeState(player, watcher1, watcher2, shambler, drainer);
    //slog("Monster check");
    entity_think_all();

    if (player->speed == 0) {
        watcher1->stage = 5;
        watcher2->stage = 5;
    }

    //if shambler is stage 5, check the door

    if (shambler->stage == 5) {
        if (door->stage == 0) {
           // slog("You died");

        } else {
            shambler->stage = 0;
            //slog("shambler returned to sender");
        }
    }
    if (drainer->stage == 5) {
        //slog("Draining Power");
        if (player->power > 10) {
            player->power = player->power - 3;
        }
        
    }

   // slog("Watcher 1 Status: %d", watcher1->stage);
    //slog("Watcher 2 Status: %d", watcher2->stage);
    //slog("Drainer Status: %d", drainer->stage);
    //slog("Shambler Status: %d", shambler->stage);
    if (watcher1 != NULL && watcher1->stage == 5) {
       // slog("Watcher1 Reducing Sanity");
        if (player->sanity > 0) {

            player->sanity = player->sanity - 5;
        }
    }
    if (watcher2 != NULL && watcher2->stage == 5) {
       // slog("Watcher2 Reducing Sanity");
        if (player->sanity > 0) {
            player->sanity = player->sanity - 5;
        }
    }
    return interval; // Re-add the timer
}

void start() {
    menu = true;
    player->location = 1;
    player->position.z = 15;
    player->position.x = 0;
    player->position.y = 7;
    player->rotation.x = -GFC_PI + 1.0472;
    player->rotation.z = -GFC_HALF_PI;
    if (!endless) {
        timerTime = SDL_AddTimer(1000, callback_time, NULL);
    }

    timerPower = SDL_AddTimer(5000, callback_power, NULL);
    if (speedy) {
        timerMonster = SDL_AddTimer(1500, callback_monster, NULL);
    }
    else {
        timerMonster = SDL_AddTimer(3000, callback_monster, NULL);
    }
    timerAnim = SDL_AddTimer(500, callback_anim, NULL);

    //Starting Resouces
    /*sanity = player->sanity;
    time = player->time;
    Lharpoon = player->Lharpoon;
    Rharpoon = player->Rharpoon;
    power = player->power;*/
    player->sanity = sanity;
    player->time = time;
    player->Lharpoon = Lharpoon;
    player->Rharpoon = Rharpoon;
    player->power = power;
    player->speed = 15;
}

int main(int argc,char *argv[])
{
    int done = 0;
    int a;

    Sprite *mouse = NULL;
    int mousex,mousey;
    //Uint32 then;
    float mouseFrame = 0;
    World *w;
    //Entity *shambler;
    //Entity* watcher1;
    //Entity* watcher2;
    //Entity* drainer;
    
    Particle particle[100];
    Matrix4 skyMat;
    Model *sky;
    //Entity *player;
    //Entity* door;

  //Booleans
    int rightlight = 0;
    int leftlight = 0;
    //Things added for FNAF Game
    Sprite* UI;
    const Uint8* keys;
    
    Mix_Music* music;
    Sound* jumpscare;
    Sound* doorSound;
    Sound* lightSound;

    int x;
    //SDL Window and initalization
  
 
    
  //Initializing Audio
/* 3 Max sounds, 1 channels, 1 channel groups, 1 music track, mp3 support, ogg enabled
*/
    gfc_audio_init(3,1,1,1,true,true);
    //Initalizing the music, a pointer to where the music is located
    //music = gfc_sound_load_music("audio/music.mp3");
    //play the music
    //gfc_sound_play(&music, -1, 1.0, 1, 1);
    music = Mix_LoadMUS("audio/music.mp3");
    if (!music) {
        printf("Mix_LoadMUS(\"music.mp3\"): %s\n", Mix_GetError());
        // this might be a critical error...
    }


    // play music forever
    // Mix_Music *music; // I assume this has been loaded already
    if (Mix_PlayMusic(music, -1) == -1) {
        printf("Mix_PlayMusic: %s\n", Mix_GetError());
        // well, there's no music, but most games don't break without music...
    }

    jumpscare = gfc_sound_load("audio/jumpscare.mp3", 1, 1);
    if (!jumpscare) {
        slog("Cant retrieve jumpscare audio");
    }
    doorSound = gfc_sound_load("audio/door.mp3", 1, 1);
    if (!doorSound) {
        slog("Cant retrieve door audio");
    }
    lightSound = gfc_sound_load("audio/light.mp3", 1, 1);
    if (!lightSound) {
        slog("Cant retrieve light audio");
    }

    int location;
    SJson*  temp;
   // SJson* temporary;
    SJson* locations = sj_object_new();;


    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--editor") == 0)
        {
            __DEBUG = 1;
            slog("Welcome to the Editor");
            slog("WARNING: This editor is meant for minute changes, to ensure a model isn't clipping into the floor for example.");
            slog("Changing the values too much may result in the game being unplayable and require a reset.");
            //sj_save(SJson *json,const char *filename);
            //sj_object_insert(SJson *object,const char *key,SJson *value);
            //SJson *sj_object_new();
            slog("Watcher 1 location, Normal is (0, 360, -120) ");

            scanf("%d", &location);
            temp = sj_new_int(location);
            sj_object_insert(locations,"Watcher1x", temp);
            scanf("%d", &location);
            temp = sj_new_int(location);
            sj_object_insert(locations, "Watcher1y", temp);
            scanf("%d", &location);
            temp = sj_new_int(location);
            sj_object_insert(locations, "Watcher1z", temp);


            slog("Watcher 2 location, Normal is (0, -360, -120) ");

            scanf("%d", &location);
            temp = sj_new_int(location);
            sj_object_insert(locations, "Watcher2x", temp);
            scanf("%d", &location);
            temp = sj_new_int(location);
            sj_object_insert(locations, "Watcher2y", temp);
            scanf("%d", &location);
            temp = sj_new_int(location);
            sj_object_insert(locations, "Watcher2z", temp);
            slog("Drainer location, Normal is (350, 0, -60) ");

            scanf("%d", &location);
            temp = sj_new_int(location);
            sj_object_insert(locations, "Drainerx", temp);
            scanf("%d", &location);
            temp = sj_new_int(location);
            sj_object_insert(locations, "Drainery", temp);
            scanf("%d", &location);
            temp = sj_new_int(location);
            sj_object_insert(locations, "Drainerz", temp);
            slog("Shambler location, Normal is (-110 ,0,0) ");

            scanf("%d", &location);
            temp = sj_new_int(location);
            sj_object_insert(locations, "Shamblerx", temp);
            scanf("%d", &location);
            temp = sj_new_int(location);
            sj_object_insert(locations, "Shamblery", temp);
            scanf("%d", &location);
            temp = sj_new_int(location);
            sj_object_insert(locations, "Shamblerz", temp);

            sj_save(locations, "stats/locations.json");

            //Load from Json

           /*
            SJson* json, * config;
            json = sj_load("stats/locations.json");
            config = sj_object_get_value(json, "Watcher1x");
            sj_object_get_value_as_int(config, "Watcher1x", location);
            slog( "%d", location);
           */

            
            scanf("%d", &location);

        }
    }
    
    SJson* json, * config;
    float w1x,w1y,w1z;
    float w2x, w2y, w2z;
    float sx, sy, sz;
    float dx, dy, dz;
    json = sj_load("stats/locations.json");
    //Watcher1
    if (!sj_object_get_value_as_float(json, "Watcher1y", &w1y)) {
        slog("Cannot find value");
    }
    if (!sj_object_get_value_as_float(json, "Watcher1x", &w1x)) {
        slog("Cannot find value");
    }
    if (!sj_object_get_value_as_float(json, "Watcher1z", &w1z)) {
        slog("Cannot find value");
    }
    //Watcher2
    if (!sj_object_get_value_as_float(json, "Watcher2y", &w2y)) {
        slog("Cannot find value");
    }
    if (!sj_object_get_value_as_float(json, "Watcher2x", &w2x)) {
        slog("Cannot find value");
    }
    if (!sj_object_get_value_as_float(json, "Watcher2z", &w2z)) {
        slog("Cannot find value");
    }
    //Shambler
    if (!sj_object_get_value_as_float(json, "Shamblerx", &sx)) {
        slog("Cannot find value");
    }
    if (!sj_object_get_value_as_float(json, "Shamblery", &sy)) {
        slog("Cannot find value");
    }
    if (!sj_object_get_value_as_float(json, "Shamblerz", &sz)) {
        slog("Cannot find value");
    }
    //Drainer
    if (!sj_object_get_value_as_float(json, "Drainery", &dy)) {
        slog("Cannot find value");
    }
    if (!sj_object_get_value_as_float(json, "Drainerx", &dx)) {
        slog("Cannot find value");
    }
    if (!sj_object_get_value_as_float(json, "Drainerz", &dz)) {
        slog("Cannot find value");
    }
    

    init_logger("gf3d.log",0);    
    gfc_input_init("config/input.cfg");
    slog("gf3d begin");
    gf3d_vgraphics_init("config/setup.cfg");
    gf2d_font_init("config/font.cfg");
    gf2d_draw_manager_init(1000);
    
    slog_sync();
    
    entity_system_init(1024);
    
    mouse = gf2d_sprite_load("images/pointer.png",32,32, 16);

    //Vector 3d(Front/Back,Right/Left,Height)
    //How to Summon an Entity
    //shambler = shambler_new(vector3d(-110 ,0,0));
    shambler = shambler_new(vector3d(sx, sy, sz),agro);
    //if (shambler)shambler->selected = 1;
    shambler->position.z + 5;

    //watcher1 = watcher_new(vector3d(0, 360, -120));
    watcher1 = watcher_new(vector3d(w1x, w1y, w1z),agro);
    //if (watcher1)watcher1->selected = 1;


    //watcher2 = watcher_new(vector3d(0, -360, -120));
    watcher2 = watcher_new(vector3d(w2x, w2y, w2z),agro);
    //if (watcher2)watcher2->selected = 1;
    watcher2->rotation.z -= (3.1416);


    //drainer = drainer_new(vector3d(350, 0, -60));
    drainer = drainer_new(vector3d(dx, dy, dz),agro);
    //if (drainer)drainer->selected = 1;
    drainer->rotation.z -= (1.5708);



    player = player_new(vector3d(0,7,15),"stats/player.json");
    player->rotation.x = -GFC_PI + 1.0472;
    player->rotation.z = -GFC_HALF_PI;

    door = door_new(vector3d(-13, 5, 300));
    //if (door)door->selected = 1;
    door->stage = 0;

    //End Entities

    //Spheres for collision
    Sphere watcher1S = gfc_sphere(0, -360, 0, 50);

    Sphere watcher2S = gfc_sphere(0, 360, 0, 50);

    w = world_load("config/cargoship.json");
    w->rotation.z += (1.5708);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    slog_sync();
    gf3d_camera_set_scale(vector3d(1,1,1));
  
    
    //Set up the lights for later use
    particle[0].position = vector3d(0, 50, 25);
    particle[0].color = gfc_color(1, 1, .7, 1);
    //particle[a].color = gfc_color(gfc_random(),gfc_random(),gfc_random(),1);
    particle[0].size = 70;

    particle[1].position = vector3d(0, -30, 25);
    particle[1].color = gfc_color(1, 1, .7, 1);
    //particle[a].color = gfc_color(gfc_random(),gfc_random(),gfc_random(),1);
    particle[1].size = 70;

    
    a = 0;
    sky = gf3d_model_load("models/sky.model");
    gfc_matrix_identity(skyMat);
    gfc_matrix_scale(skyMat,vector3d(100,100,100));

    //Setting Camera starting position to the console
    gf3d_camera_set_rotation(vector3d(0.785398, 0.785398, 0.785398));
    
    // main game loop

    //Setting resources for the player
    player->location = 1;
    player->speed = 15;
    player->drainers = 0;
    player->rightlight = 0;
    player->leftlight = 0;
    //Grab the rest of the resources for variables
    sanity = player->sanity;
    time = player->time;
    Lharpoon = player->Lharpoon;
    Rharpoon = player->Rharpoon;
    power = player->power;





    








    slog("gf3d main loop begin");
    while (!done)
    {
        //SDL_INIT_Video
        jump:

        gfc_input_update();
        gf2d_font_update();
        SDL_GetMouseState(&mousex, &mousey);

        mouseFrame += 0.01;
        if (mouseFrame >= 16)mouseFrame = 0;
        world_run_updates(w);
    ///    entity_think_all();
        entity_update_all();
        player_think(player);
        player_update(player);
        gf3d_camera_update_view();
        gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

        //Setting Camera starting position to the console
        gf3d_camera_set_rotation(vector3d(0.785398, 0.785398, 0.785398));

        gf3d_vgraphics_render_start();

        //3D draws
        gf3d_model_draw_sky(sky, skyMat, gfc_color(1, 1, 1, 1));
        world_draw(w);

        entity_draw_all();

        if (!menu) {
            // Setting the player in the starting position for the menu
            player->position.z = -30;
            player->position.x = 250;
            player->position.y = 7;
            player->rotation.x = GFC_PI;
            player->rotation.z = GFC_HALF_PI;
            if (splash) {
                gf2d_font_draw_line_tag("Press Space to Start", FT_H1, gfc_color(1, 1, 1, 1), vector2d(300, 590), 3);
                gf2d_font_draw_line_tag("Press E for Custom Night", FT_H1, gfc_color(1, 1, 1, 1), vector2d(300, 640), 3);
                if (gfc_input_command_pressed("defense")) {
                    start();
                }
                if (gfc_input_command_pressed("activate")) {
                    splash = 0;
                    custom = 1;
                }
            }
            if (custom) {
                gf2d_font_draw_line_tag("Press Space to Start", FT_H1, gfc_color(1, 1, 1, 1), vector2d(300, 590), 3);
                if (!speedy) {
                    gf2d_font_draw_line_tag("Press 1 for Speedy", FT_H1, gfc_color(1, 1, 1, 1), vector2d(300, 90), 3);
                }
                else {
                    gf2d_font_draw_line_tag("Speedy Activated", FT_H1, gfc_color(1, 1, 1, 1), vector2d(300, 90), 3);
                }
                
                if (gfc_input_command_pressed("nightlight")) {
                    if (speedy) {
                        speedy = 0;
                    }
                    else {
                        speedy = 1;
                    }
                }

                if (!electronics) {
                    gf2d_font_draw_line_tag("Press 2 for Faulty Electronics", FT_H1, gfc_color(1, 1, 1, 1), vector2d(300, 190), 3);
                }
                else {
                    gf2d_font_draw_line_tag("Faulty Electronics Activated", FT_H1, gfc_color(1, 1, 1, 1), vector2d(300, 190), 3);
                }

                if (gfc_input_command_pressed("energy")) {
                    if (electronics) {
                        electronics = 0;
                    }
                    else {
                        electronics = 1;
                    }
                }

                if (!agro) {
                    gf2d_font_draw_line_tag("Press 3 for Aggressive Haunting", FT_H1, gfc_color(1, 1, 1, 1), vector2d(300, 290), 3);
                }
                else {
                    gf2d_font_draw_line_tag("Aggressive Haunting Activated", FT_H1, gfc_color(1, 1, 1, 1), vector2d(300, 290), 3);
                }

                if (gfc_input_command_pressed("cargo")) {
                    if (agro) {
                        agro = 0;
                    }
                    else {
                        agro = 1;
                    }
                }

                if (!endless) {
                    gf2d_font_draw_line_tag("Press 4 for Endless", FT_H1, gfc_color(1, 1, 1, 1), vector2d(300, 390), 3);
                }
                else {
                    gf2d_font_draw_line_tag("Endless Activated", FT_H1, gfc_color(1, 1, 1, 1), vector2d(300, 390), 3);
                }

                if (gfc_input_command_pressed("madman")) {
                    if (endless) {
                        endless = 0;
                    }
                    else {
                        endless = 1;
                    }
                }

                if (!blind) {
                    gf2d_font_draw_line_tag("Press 5 for Blind Mode", FT_H1, gfc_color(1, 1, 1, 1), vector2d(300, 490), 3);
                }
                else {
                    gf2d_font_draw_line_tag("Blind Activated", FT_H1, gfc_color(1, 1, 1, 1), vector2d(300, 490), 3);
                }

                if (gfc_input_command_pressed("arcane")) {
                    if (blind) {
                        blind = 0;
                    }
                    else {
                        blind = 1;
                    }
                }
               
             
            
            
            
                if (gfc_input_command_pressed("defense")) {
                    start();
                }
            }

            

            gf3d_vgraphics_render_end();
            //Game not started,
            continue;
        }

        if (blind) {
            gf2d_draw_rect_filled(gfc_rect(0, 0, 10000, 10000), GFC_COLOR_BLACK);
        }

        //2D draws, Showing the player their UI
        gf2d_draw_rect_filled(gfc_rect(0, 0, 10000, 200), GFC_COLOR_DARKGREY);

        //Example things of what I need for the UI
        //View Examples
        gf2d_font_draw_line_tag("VIEW", FT_H1, gfc_color(1, 1, 1, 1), vector2d(512, 10), 3);
        if (player->location == 1) {
            gf2d_font_draw_line_tag("CONSOLE", FT_H1, gfc_color(1, 1, 1, 1), vector2d(512, 50), 3);
            ///Add UI to View the Power
            char powerString[50]; // Create a character array to store the converted string

            // Use sprintf to convert player->power to a string
            sprintf(powerString, "%d", player->power);
            gf2d_font_draw_line_tag(powerString, FT_H1, GFC_COLOR_GREEN, vector2d(512, 300), 3);

            ///Add UI to View the Speed
            char SpeedString[50]; // Create a character array to store the converted string

            // Use sprintf to convert player->power to a string
            sprintf(SpeedString, "%d", player->speed);
            gf2d_font_draw_line_tag(SpeedString, FT_H1, GFC_COLOR_GREEN, vector2d(512, 350), 3);

        }
        else if (player->location == 2) {
            gf2d_font_draw_line_tag("BOW", FT_H1, gfc_color(1, 1, 1, 1), vector2d(512, 50), 3);
        }
        else if (player->location == 3) {
            gf2d_font_draw_line_tag("HALLWAY", FT_H1, gfc_color(1, 1, 1, 1), vector2d(512, 50), 3);
        }
        else if (player->location == 4) {
            gf2d_font_draw_line_tag("PORT", FT_H1, gfc_color(1, 1, 1, 1), vector2d(512, 50), 3);
        }
        else if (player->location == 5) {
            gf2d_font_draw_line_tag("STARBOARD", FT_H1, gfc_color(1, 1, 1, 1), vector2d(512, 50), 3);
        }
        //Update based on camera view
       //Sanity Meter
        char SanityString[50];
        sprintf(SanityString, "%d", player->sanity);
        gf2d_font_draw_line_tag("Sanity", FT_H1, gfc_color(1, 1, 1, 1), vector2d(200, 10), 3);
        gf2d_font_draw_line_tag(SanityString, FT_H1, gfc_color(1, 1, 1, 1), vector2d(200, 50), 3); //needs to be updated with a variable
        //Clock
        gf2d_font_draw_line_tag("TIME", FT_H1, gfc_color(1, 1, 1, 1), vector2d(800, 10), 3);
        char timeString[50]; // Create a character array to store the converted string

        // Use sprintf to convert player->power to a string
        sprintf(timeString, "%d", player->time);
        gf2d_font_draw_line_tag(timeString, FT_H1, gfc_color(1, 1, 1, 1), vector2d(820, 50), 3);

        gf2d_draw_rect(gfc_rect(0, 0, 10000, 200), gfc_color8(255, 255, 255, 255)); //Outline

        if (player->time <=0 || player->sanity <=0 || shambler->stage ==5) {
            
            if (player->time <= 0) {
                gf2d_draw_rect_filled(gfc_rect(0, 0, 10000, 10000), GFC_COLOR_BLACK);
                    char NightString[50];
                    sprintf(NightString, "%d", currentNight);
                    gf2d_font_draw_line_tag("FINISHED SHIFT", FT_H1, GFC_COLOR_WHITE, vector2d(312, 200), 3);
                    gf2d_font_draw_line_tag("NEXT SHIFT IS ", FT_H1, GFC_COLOR_WHITE, vector2d(312, 250), 3);
                    gf2d_font_draw_line_tag("UNKNOWN", FT_H1, GFC_COLOR_CYAN, vector2d(430, 300), 3);
                    //Code for having multiple nights
                    timerNight = SDL_AddTimer(3000, callback_night, NULL);
            }
            if (player->sanity <= 0 && !(player->time <= 0)) {
                gf2d_draw_rect_filled(gfc_rect(0, 0, 10000, 10000), GFC_COLOR_BLACK);
                gf2d_font_draw_line_tag("MIND BROKEN", FT_H1, GFC_COLOR_CYAN, vector2d(312, 200), 3);
            }
            if (shambler->stage == 5 && door->stage ==0 && !(player->time <= 0)) {
                gfc_sound_play(jumpscare, 1, 1, 1, 1);
                //Jumpscare code goes here
                //put player in jumpscare position
                timerJump = SDL_AddTimer(3000, callback_jump, NULL);
                player->location = 3;
                player->position.z = 15;
                player->position.x = 20;
                player->position.y = 5;
                player->rotation.x = -GFC_PI;
                player->rotation.z = -GFC_HALF_PI + 3.14159;


                if (jumpscareover) {
                    gf2d_draw_rect_filled(gfc_rect(0, 0, 10000, 10000), GFC_COLOR_BLACK);
                    gf2d_font_draw_line_tag("LOST AT SEA", FT_H1, GFC_COLOR_RED, vector2d(312, 200), 3);
                }

               
                    
                
            }
            
        }

        //gf2d_sprite_draw(mouse, vector2d(mousex, mousey), vector2d(2, 2), vector3d(8, 8, 0), gfc_color(0.3, .9, 1, 0.9), (Uint32)mouseFrame);


        //ACTUAL GAME
        //Player controls to operate the door, and lights


            /* Poll for events. SDL_PollEvent() returns 0 when there are no  */
            /* more events on the event queue, our while loop will exit when */
            /* that occurs.                                                  */
        if (gfc_input_command_pressed("defense") && cargo == 0 && player->power > 0) {
            slog("we pressed space, activating current defense");
            //check for faulty electronics
            if (electronics) {
                if (gfc_crandom() > 0) {
                    player->power  = player->power - 5;
                }
            }
            if (player->location == 3) {
                slog("activating doors");
                //close or open doors
                gfc_sound_play(doorSound, 1, 1, 1, 1);
                if(doorstate != 0){
                    player->drainers--;
                    doorstate = 0;
                }
                else {
                    player->drainers++;
                    doorstate = 1;
                }
                door_check(door);


            }
            else if (player->location == 2) {
                slog("Shocking Power dude");
                player->power = player->power -5;
                //Shock the power eater
                if (drainer->stage == 5) {
                    drainer->stage = 0;
                }
            }
            else if (player->location == 5) {
                slog("left light");
                gfc_sound_play(lightSound, 1, 1, 1, 1);
                //left light
                if (player->leftlight == 0) {
                    slog("turn on");
                    player->leftlight = 1;
                    player->drainers++;
                }
                else if (player->leftlight == 1) {
                    slog("Turn off");
                    player->leftlight = 0;
                    player->drainers--;
                }

            }
            else if (player->location == 4) {
                slog("turn on right light");
                gfc_sound_play(lightSound, 1, 1, 1, 1);
                //right light
                if (player->rightlight == 0) {
                    slog("turn on");
                    player->rightlight = 1;
                    player->drainers++;
                }
                else if (player->rightlight == 1) {
                    slog("Turn off");
                    player->rightlight = 0;
                    player->drainers--;
                }

            }
            else if (player->location == 1) {
                slog("Activating all defenses");
                gfc_sound_play(lightSound, 1, 1, 1, 1);
                gfc_sound_play(doorSound, 1, 1, 1, 1);
                rightlight = 1;
                leftlight = 1;
                close_door(door);
                player->drainers = player->drainers + 3;

            }
                       
        }
        //Player presses e at the console
        if (gfc_input_command_held("activate") && cargo ==0) {
            if (player->location == 1) {
             if (player->speed < 15) {
                player->speed = player->speed++;
             }
            }
            //Right Side
            else if (player->location ==4) {
                slog("Activating Right Side");
                if (player->Rharpoon >0) {
                    player->Rharpoon = 0;
                    harpoonR = harpoon_new(vector3d(0, -50, 30));
                    harpoonR->velocity = vector3d(0, -1, 0);
                    //watcher2->stage = 0; //Swap this to watcher1 later
                }
            }
            //Left Side
            else if(player->location ==5) {
                slog("Activating Left Side");
                if (player->Lharpoon > 0) {
                    player->Lharpoon = 0;
                    harpoonL = harpoon_new(vector3d(0, 50, 30));
                    harpoonL->velocity = vector3d(0, 1, 0);
                   // watcher1->stage = 0;
                }
            }
           
        }

                //Check to see the light states, draw them if theyre supposed to be on
                if (player->leftlight == 1) {
                    gf3d_particle_draw(&particle[0]);
                }
                if (player->rightlight == 1) {
                    gf3d_particle_draw(&particle[1]);
                }
                
      //if speed has dropped down to 0, Set watchers to stage 5
       if (player->speed == 0) {
           watcher1->stage = 5;
           watcher2->stage = 5;
       }
       //If lights are on, and watchers aren't stage 5, reset them
       if (player->leftlight == 1 && watcher1->stage != 5) {
           watcher1->stage = 0;
       }
       if (player->rightlight == 1 && watcher2->stage != 5) {
           watcher2->stage = 0;
       }

       //Basic collision for harpoons hitting watchers
       if (harpoonR) {
        if (gfc_point_in_sphere(harpoonR->position, watcher1S)) {
           slog("Collision Detected");
           if (watcher2->stage == 5) {
               watcher2->stage = 0;
               entity_free(harpoonR);
           }
         }
       }

       if (harpoonL) {
           if (gfc_point_in_sphere(harpoonL->position, watcher2S)) {
               slog("Collision Detected");
               if (watcher1->stage == 5) {
                   watcher1->stage = 0;
                   entity_free(harpoonL);
               }
           }
       }
       




       //Code for the Abilities
       if (gfc_input_command_pressed("nightlight") && cargo == 0) {
           slog("1 works");
           player->power = player->power - 10;
           player->sanity =player->sanity + 40;
       }
       if (gfc_input_command_pressed("energy") && cargo == 0) {
           player->power = player->power + 15;
           player-> sanity = player->sanity - 40;
           slog("2 works");
       }
       if (gfc_input_command_pressed("cargo") && cargo == 0) {
           slog("3 works");
           cargo = 5;
           player->Lharpoon = 1;
           player->Rharpoon = 1;
       }
       if (gfc_input_command_pressed("madman") && cargo == 0) {
           slog("4 works");
           if (madman == 0) {
               madman = 1;
           }
           else {
               madman = 0;
           }
       }
       if (gfc_input_command_pressed("arcane") && cargo == 0) {
           slog("5 works");
           if (arcane == 0) {
               arcane = 1;
           }
           else {
               arcane = 0;
           }
       }


        gf3d_vgraphics_render_end();

        //multiple nights here




        if (gfc_input_command_down("cancel")) {
            slog("Back to Menu");
            menu = 0;
            SDL_RemoveTimer(timerTime);
            SDL_RemoveTimer(timerPower);
            SDL_RemoveTimer(timerMonster);
            SDL_RemoveTimer(timerNight);
            SDL_RemoveTimer(timerAnim);
            watcher1->stage = 0;
            watcher2->stage = 0;
            shambler->stage = 0;
            drainer->stage = 0;
        } // exit condition


        if (gfc_input_command_down("exit")) {
            slog("Exiting");
            done = 1;
        } // exit condition
    }

   SDL_RemoveTimer(timerTime);
   SDL_RemoveTimer(timerPower);
   SDL_RemoveTimer(timerMonster);
   SDL_RemoveTimer(timerNight);
   SDL_RemoveTimer(timerAnim);
    
    world_delete(w);
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
