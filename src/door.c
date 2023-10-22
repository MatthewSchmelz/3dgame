
#include "simple_logger.h"

#include "door.h"



//Modify this to actually summon the door, should start in the open position
Entity* door_new(Vector3D position)
{
    Entity* ent = NULL;

    ent = entity_new();
    if (!ent)
    {
        
        return NULL;
    }
    ent->selectedColor = gfc_color(0.1, 1, 0.5, 1);
    ent->color = gfc_color(1, 5, 1, 5);
    ent->model = gf3d_model_load("models/dino.model");
    ent->update = door_update;
    ent->think = door_think;
    vector3d_copy(ent->position, position);
    return ent;
    ent->stage = 0;
    
}

void door_think(Entity* self) {
    if (self->stage == 1) {
        //Drain Power
    
    }
}
//Drain the lad's power
void door_update(Entity* self)
{
    if (!self)
    {
        
        return;
    }


}

void door_check(Entity* self) {
    if (self->stage = 0) {
        close_door(self);
        slog("closed door");
    }
    else {
        open_door(self);
        slog("Opened door");
    }


}

void open_door(Entity* self) {
    slog("Stage is set to 0");
    self->stage = 0;
    self->position = vector3d(-11, 0, 2);
}
//sets the door to the close position, drains power while it is closed
void close_door(Entity* self) {
    slog("Stage is set to 1");
    self->stage = 1;
    self->position = vector3d(-11, 0, 20);
}

/*eol@eof*/
