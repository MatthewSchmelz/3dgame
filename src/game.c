#include <SDL.h>            

#include "simple_logger.h"
#include "gfc_input.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"

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
#include "player.h"
#include "world.h"

extern int __DEBUG; 

int main(int argc,char *argv[])
{
    int done = 0;
    int a;
    
    Sprite *mouse = NULL;
    int mousex,mousey;
    //Uint32 then;
    float mouseFrame = 0;
    World *w;
    Entity *agu;
    
    Particle particle[100];
    Matrix4 skyMat;
    Model *sky;
    Entity *player;
    Entity* door;

    //Things added for FNAF Game
    Sprite* UI;
    const Uint8* keys;

    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--debug") == 0)
        {
            __DEBUG = 1;
        }
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

    //How to Summon an Entity
    agu = agumon_new(vector3d(0 ,0,0));
    if (agu)agu->selected = 1;


    //Vector 3d(Front/Back,Right/Left,Height)
    door = door_new(vector3d(-11, 0, 2));
    if (door)door->selected = 1;


    w = world_load("config/cargoship.json");
    w->rotation.z += (1.5708);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    slog_sync();
    gf3d_camera_set_scale(vector3d(1,1,1));
    player = player_new(vector3d(-50,0,0));
    /*
    for (a = 0; a < 1; a++)
    {
        particle[a].position = vector3d((gfc_crandom() * 100),(gfc_crandom() * 100),(gfc_crandom() * 20));
        particle[a].color = gfc_color(.211,.211,.211,1);
        //particle[a].color = gfc_color(gfc_random(),gfc_random(),gfc_random(),1);
        particle[a].size = 0.001 * gfc_random();
    }*/
    a = 0;
    sky = gf3d_model_load("models/sky.model");
    gfc_matrix_identity(skyMat);
    gfc_matrix_scale(skyMat,vector3d(100,100,100));

    //Setting Camera starting position to the console
    gf3d_camera_set_rotation(vector3d(0.785398, 0.785398, 0.785398));
    
    // main game loop

    //Setting resources for the player
    player->location = 1;
    player->sanity = 100;
    player->power = 100;
    player->Lharpoon = 1;
    player->Rharpoon = 1;
    player->time = 300.0;
    slog("gf3d main loop begin");
    while(!done)
    {
        gfc_input_update();
        gf2d_font_update();
        SDL_GetMouseState(&mousex,&mousey);
        
        mouseFrame += 0.01;
        if (mouseFrame >= 16)mouseFrame = 0;
        world_run_updates(w);
        entity_think_all();
        entity_update_all();
        gf3d_camera_update_view();
        gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

        //Setting Camera starting position to the console
        gf3d_camera_set_rotation(vector3d(0.785398, 0.785398, 0.785398));

        gf3d_vgraphics_render_start();

            //3D draws
                gf3d_model_draw_sky(sky,skyMat,gfc_color(1,1,1,1));
                world_draw(w);
            
                entity_draw_all();
                
            /*    for (a = 0; a < 100; a++)
                {
                    //Draws the particles around the Ogumon
                    gf3d_particle_draw(&particle[a]);
                */
            //2D draws, Showing the player their UI
                gf2d_draw_rect_filled(gfc_rect(0 ,0,10000,200),GFC_COLOR_GREY);

                //Example things of what I need for the UI
                //View Examples
                gf2d_font_draw_line_tag("VIEW", FT_H1, gfc_color(1, 1, 1, 1), vector2d(512, 10), 3);
                if (player->location == 1) {
                    gf2d_font_draw_line_tag("CONSOLE", FT_H1, gfc_color(1, 1, 1, 1), vector2d(512, 50), 3);
                    ///Add UI to View the Power
                    char powerString[50]; // Create a character array to store the converted string

                    // Use sprintf to convert player->power to a string
                    sprintf(powerString, "%d", player->power);
                    gf2d_font_draw_line_tag(powerString, FT_H1, gfc_color(1, 1, 1, 1), vector2d(512, 300), 3);
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
                gf2d_font_draw_line_tag("SANITY", FT_H1, gfc_color(1, 1, 1, 1), vector2d(200, 10), 3);
                gf2d_font_draw_line_tag("100", FT_H1, gfc_color(1, 1, 1, 1), vector2d(200, 50), 3); //needs to be updated with a variable
                //Clock
                gf2d_font_draw_line_tag("TIME", FT_H1, gfc_color(1, 1, 1, 1), vector2d(800, 10), 3);               
                
                gf2d_draw_rect(gfc_rect(0 ,0,10000,200),gfc_color8(255,255,255,255)); //Outline
                
                gf2d_sprite_draw(mouse,vector2d(mousex,mousey),vector2d(2,2),vector3d(8,8,0),gfc_color(0.3,.9,1,0.9),(Uint32)mouseFrame);
                

                //ACTUAL GAME
                //Player controls to operate the door, and lights
                keys = SDL_GetKeyboardState(NULL);
                //Opens and closes the door



                if (keys[SDL_SCANCODE_SPACE] && (player->location == 2)) {
                    slog("Reading the Keys");
                    close_door(door);
                }
                else {
                    open_door(door);
                }










        gf3d_vgraphics_render_end();

        if (gfc_input_command_down("exit"))done = 1; // exit condition
    }    
    
    world_delete(w);
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
