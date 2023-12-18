#include "simple_logger.h"
#include "gfc_types.h"

#include "gf3d_camera.h"
#include "player.h"

static int thirdPersonMode = 0;
void player_think(Entity *self);
void player_update(Entity *self);
/*
Entity *player_new(Vector3D position)
{
    Entity *ent = NULL;

    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }

    ent->model = gf3d_model_load("models/dino.model");
    ent->think = player_think;
    ent->update = player_update;
    vector3d_copy(ent->position,position);
    ent->rotation.x = -GFC_PI;
    ent->rotation.z = -GFC_HALF_PI;
    ent->hidden = 1;
    return ent;
}



Model * gf3d_model_load(const char * filename)
{
    SJson *json,*config;
    Model *model;
    if (!filename)return NULL;
    json = sj_load(filename);
    if (!json)return NULL;
    config = sj_object_get_value(json,"model");
    if (!config)
    {
        slog("file %s contains no model object",filename);
        sj_free(json);
        return NULL;
    }
    model = gf3d_model_load_from_config(config);
    sj_free(json);
    return model;
}



Model * gf3d_model_load_from_config(SJson *json)
{
    const char *model;
    const char *texture;
    if (!json)return NULL;
    model = sj_get_string_value(sj_object_get_value(json,"model"));
    texture = sj_get_string_value(sj_object_get_value(json,"texture"));
    return gf3d_model_load_full(model,texture);
}
*/
Entity *player_new(Vector3D position, const char* filename)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }
    
    ent->model = gf3d_model_load("models/dino.model");
    ent->think = player_think;
    ent->update = player_update;
    vector3d_copy(ent->position,position);
    ent->rotation.x = -GFC_PI;
    ent->rotation.z = -GFC_HALF_PI;
    ent->hidden = 1;

    // Ensuring the config file works
    SJson* json, * config;
    if (!filename)return NULL;
    json = sj_load(filename);
    if (!json)return NULL;
    config = sj_object_get_value(json, "player");
    if (!config)
    {
        slog("file %s contains no player object", filename);
        sj_free(json);
        return NULL;
    }
    //taking the stats from said config file
  
    if (!sj_object_get_value_as_int(config, "sanity", &ent->sanity)) {
        slog("Sanity not found");
    }
    if (!sj_object_get_value_as_int(config, "power", &ent->power)) {
        slog("power not found");
    }
    if (!sj_object_get_value_as_int(config, "Lharpoon", &ent->Lharpoon)) {
        slog("LH not found");
    }
    if (!sj_object_get_value_as_int(config, "Rharpoon", &ent->Rharpoon)) {
        slog("RH not found");
    }
    if (!sj_object_get_value_as_int(config, "time", &ent->time)) {
        slog("Time not found");
    }
   


    return ent;
}


void player_think(Entity *self)
{
    Vector3D forward = {0};
    Vector3D right = {0};
    Vector2D w,mouse;
    int mx,my;
    SDL_GetRelativeMouseState(&mx,&my);
    const Uint8 * keys;
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

    mouse.x = mx;
    mouse.y = my;
    w = vector2d_from_angle(self->rotation.z);
    forward.x = w.x;
    forward.y = w.y;
    w = vector2d_from_angle(self->rotation.z - GFC_HALF_PI);
    right.x = w.x;
    right.y = w.y;
    if (keys[SDL_SCANCODE_W])
    {   
        vector3d_add(self->position,self->position,forward);
    }
    if (keys[SDL_SCANCODE_S])
    {
        vector3d_add(self->position,self->position,-forward);        
    }
    if (keys[SDL_SCANCODE_D])
    {
        vector3d_add(self->position,self->position,right);
    }
    if (keys[SDL_SCANCODE_A])    
    {
        vector3d_add(self->position,self->position,-right);
    }
    if (keys[SDL_SCANCODE_SPACE])self->position.z += 1;
    if (keys[SDL_SCANCODE_Z])self->position.z -= 1;
    
    if (keys[SDL_SCANCODE_UP])self->rotation.x -= 0.0050;
    if (keys[SDL_SCANCODE_DOWN])self->rotation.x += 0.0050;
    if (keys[SDL_SCANCODE_RIGHT])self->rotation.z -= 0.0050;
    if (keys[SDL_SCANCODE_LEFT])self->rotation.z += 0.0050;
    
    if (mouse.x != 0)self->rotation.z -= (mouse.x * 0.001);
    if (mouse.y != 0)self->rotation.x += (mouse.y * 0.001);

    if (keys[SDL_SCANCODE_F3])
    {
        thirdPersonMode = !thirdPersonMode;
        self->hidden = !self->hidden;
    }
}

void player_update(Entity *self)
{
    Vector3D forward = {0};
    Vector3D position;
    Vector3D rotation;
    Vector2D w;
    
    if (!self)return;
    
    vector3d_copy(position,self->position);
    vector3d_copy(rotation,self->rotation);
    if (thirdPersonMode)
    {
        position.z += 100;
        rotation.x += M_PI*0.125;
        w = vector2d_from_angle(self->rotation.z);
        forward.x = w.x * 100;
        forward.y = w.y * 100;
        vector3d_add(position,position,-forward);
    }
    gf3d_camera_set_position(position);
    gf3d_camera_set_rotation(rotation);
}

/*eol@eof*/
