
#include "simple_logger.h"
#include "agumon.h"


void agumon_update(Entity *self);

void agumon_think(Entity *self);

Entity *agumon_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no agumon for you!");
        return NULL;
    }
    ent->selectedColor = gfc_color(0.1,1,0.1,1);
    ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/dino.model");
    ent->think = agumon_think;
    ent->update = agumon_update;
    vector3d_copy(ent->position,position);
    return ent;
    ent->aggression = 0;
}

void agumon_update(Entity *self)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);
    self->rotation.z += 0.01;
}

void agumon_think(Entity* self)
{
    /*if (!self)return;
    // slog("Thinking");
    //Every think we increase the agression by 1, stacking indefinately. We then add a Check 
    //Against a threat variable(Or just hard code a threat). If it rolls higher than the threat, we advance the stage
    //of the Entity by one. If the stage of the shambler hits 5, it kills the player if the door is open.
    self->aggression++;
    if ((self->aggression + (gfc_crandom() * 100)) > 200) {
        entity_increase_stage(self);
    }
    //check if it hit stage 5, if it did, check the door, if it is open, kill the player, if it is not
    //reduce stage.
    if (self->stage >= 5 ) {
        //check door
    }
    */
}

/*eol@eof*/
