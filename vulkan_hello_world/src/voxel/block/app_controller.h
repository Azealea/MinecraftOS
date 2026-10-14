#pragma once

typedef struct Block Block;
typedef struct Chunk Chunk;

void app_controller_on_place(Block* b, Chunk* c);
void app_controller_on_break(Block* b, Chunk* c);
