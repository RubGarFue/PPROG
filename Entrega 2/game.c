/**
 * @brief Implements the game interface and all associated calls
   for each command
 *
 * @file game.c
 * @author Group 1
 * @version 1.0
 * @date 04-10-2019
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"

#define N_CALLBACK 9

/**
 * @brief Game
 *
 * This structure defines a game
 */
struct _Game {
  Player* player;                       /*!< Game player */
  Object* objects[MAX_OBJECTS + 1];     /*!< Game bject */
  Space* spaces[MAX_SPACES + 1];        /*!<Spaces in which the game takes place */
  Die* die;                             /*!<Game die*/
  T_Command last_cmd;                   /*!<Last command executed in the game*/
  STATUS status;                        /*!<Status of last command*/
};

/**
 * @brief Defines the type of function for calls
 */
typedef void (*callback_fn)(Game* game);

/**
 * @brief It is executed if the command is unknown (unknown)
 * @param game that is running.
 */
void game_callback_unknown(Game* game);
/**
 * @brief It is executed if the command is to exit (exit)
 * @param game that is running.
 */
void game_callback_exit(Game* game);
/**
 * @brief  It is executed if the command is to move along (next)
 * @param game that is running.
 */
void game_callback_next(Game* game);
/**
 * @briefIt is executed if the command is to go back(back)
 * @param game that is running.
 */
void game_callback_back(Game* game);
/**
 * @brief It is executed if the command is to take an object(take)
 * @param game that is running.
 */
void game_callback_take(Game* game);
/**
 * @brief  It is executed if the command is to drop an object(drop)
 * @param game that is running.
 */
void game_callback_drop(Game* game);
/**
 * @brief It is executed if the command is to roll the die (roll)
 * @param game that is running.
 */
void game_callback_roll(Game* game);

/**
 * @brief It is executed if the command is to move to the left (left)
 * @param game that is running.
 */
void game_callback_left(Game* game);
/**
 * @brief It is executed if the command is to move to the right (right)
 * @param game that is running.
 */
void game_callback_right(Game* game);


/**
 * @brief Call list for each game command
 */
 static callback_fn game_callback_fn_list[N_CALLBACK]={
   game_callback_unknown,      /*!< Call if the command is unknown(unknown) */
   game_callback_exit,         /*!< Call if the command is to exit (exit) */
   game_callback_next,         /*!< Call if the command is to move along(next) */
   game_callback_back,         /*!< Call if the command is to move back (back) */
   game_callback_take,         /*!< Call if the command is to take an object (take) */
   game_callback_drop,         /*!< Call if the command is to drop an object (drop) */
   game_callback_roll,         /*!< Call if the command is to roll th die (roll) */
   game_callback_left,         /*!< Call if the command is to move to the left (left) */
   game_callback_right         /*!< Call if the command is to move to the right (right) */
 };

/**
   Game interface implementation
*/

Game* game_create() {
  int i;
  Game* game=NULL;
  game=(Game*)malloc(sizeof(Game));
  if (game==NULL)
    return NULL;

  for (i = 0; i < MAX_SPACES; i++) {
    game->spaces[i] = NULL;
  }
  for (i = 0; i < MAX_OBJECTS; i++) {
    game->objects[i] = NULL;
  }

  game->player = NULL;
  game->die=die_create(0, MIN_DIE, MAX_DIE);
  if (game->die==NULL) {
    game_destroy(game);
    return NULL;
  }
  game->last_cmd = NO_CMD;

  return game;
}

STATUS game_destroy(Game* game) {
  int i = 0;

  for (i = 0; (i < MAX_SPACES) && (game->spaces[i] != NULL); i++) {
    space_destroy(game->spaces[i]);
  }
  for(i = 0; (i < MAX_OBJECTS) && (game->objects[i] != NULL); i++) {
    object_destroy(game->objects[i]);
  }

  if (player_destroy(game->player)==ERROR)
    return ERROR;
  if (die_destroy(game->die)==ERROR)
    return ERROR;
  free(game);

  return OK;
}

STATUS game_add_space(Game* game, Space* space) {
  int i = 0;

  if (space == NULL) {
    return ERROR;
  }

  while ( (i < MAX_SPACES) && (game->spaces[i] != NULL)) {
    i++;
  }

  if (i >= MAX_SPACES) {
    return ERROR;
  }

  game->spaces[i] = space;

  return OK;
}

Id game_get_space_id_at(Game* game, int index) {

  if (index < 0 || index >= MAX_SPACES) {
    return NO_ID;
  }

  return space_get_id(game->spaces[index]);
}

Space* game_get_space(Game* game, Id id) {
  int i = 0;

  if (id == NO_ID) {
    return NULL;
  }

  for (i = 0; i < MAX_SPACES && game->spaces[i] != NULL; i++) {
    if (id == space_get_id(game->spaces[i])){
      return game->spaces[i];
    }
  }

  return NULL;
}

STATUS game_set_player(Game* game, Player* player) {

  if (game==NULL || player==NULL) {
    return ERROR;
  }

  game->player = player;

  return OK;
}

STATUS game_set_object(Game* game, Object* object) {
  int i = 0;

  if (game==NULL || object == NULL) {
    return ERROR;
  }

  while ( (i < MAX_OBJECTS) && (game->objects[i] != NULL)) {
    i++;
  }

  if (i >= MAX_OBJECTS) {
    return ERROR;
  }

  game->objects[i] = object;

  return OK;
}

Player* game_get_player(Game* game) {
  return game->player;
}

Object* game_get_object(Game* game, Id id) {
  int i = 0;

  if (id == NO_ID) {
    return NULL;
  }

  for (i = 0; i < MAX_OBJECTS && game->objects[i] != NULL; i++) {
    if (id == object_get_id(game->objects[i])) {
      return game->objects[i];
    }
  }

  return NULL;
}

Object* game_get_object_by_index(Game* game, int index) {
  if (!game || index<0 || index>MAX_OBJECTS) {
    return NULL;
  }

  return game->objects[index];
}

Object* game_get_object_by_name(Game* game, char* name) {
  int i = 0;
  if (!game || !name) {
    return NULL;
  }

  for (i = 0; i < MAX_OBJECTS && game->objects[i] != NULL; i++) {
    if (strcmp(object_get_name(game->objects[i]), name)==0) {
      return game->objects[i];
    }
  }

  return NULL;
}

Id game_get_object_location(Game* game, Id id) {
  int i=0, j=0;
  if (!game || !(game->objects) || !(game->spaces))
    return NO_ID;
  for (i=0; game->spaces[i]!=NULL && i<=MAX_SPACES; i++) {
    for (j=0; space_get_object(game->spaces[i], j)!=NO_ID && j<MAX_ID; j++) {
      if (space_get_object(game->spaces[i], j)==id) {
        return game_get_space_id_at(game, i);
      }
    }
  }
  if (game->spaces[i]==NULL)
    return NO_ID;
  return game_get_space_id_at(game, i);
}

int game_get_number_of_objects(Game* game) {
  int i=0;

  if(!game)
    return -1;

  while(game->objects[i]!=NULL) {
    i++;
  }

  return i;
}

STATUS game_get_status(Game* game) {
  if (!game)
    return ERROR;
  return game->status;
}

STATUS game_update(Game* game, T_Command cmd) {
  game->last_cmd = cmd;
  (*game_callback_fn_list[cmd])(game);
  return OK;
}

T_Command game_get_last_command(Game* game) {
  return game->last_cmd;
}

void game_print_data(Game* game) {
  int i = 0;

  printf("\n\n-------------\n\n");

  printf("=> Spaces: \n");
  for (i = 0; i < MAX_SPACES && game->spaces[i] != NULL; i++) {
    space_print(game->spaces[i]);
  }

  printf("=> Object location: \n");
  for (i = 0; i < MAX_OBJECTS && game->objects[i] != NULL; i++) {
    object_print(game->objects[i]);
  }
  object_print(game->objects[i]);
  printf("=> Player location: \n");
  player_print(game->player);
  printf("prompt:> ");
}

BOOL game_is_over(Game* game) {
  return FALSE;
}

int game_get_die_last_value(Game* game){
  if(!game|| !game->die){
    return -1;
  }
  return die_last_value(game->die);
}

/**
   Callbacks implementation for each action
*/

void game_callback_unknown(Game* game) {
}

void game_callback_exit(Game* game) {
}

void game_callback_next(Game* game) {
  int i = 0;
  Id current_id = NO_ID;
  Id space_id = NO_ID;

  space_id = player_get_location(game->player);
  if (space_get_south(game_get_space(game, space_id))==NO_ID) {
    game->status = ERROR;
    return;
  }

  if (space_id == NO_ID) {
    game->status = ERROR;
    return;
  }

  for (i = 0; i < MAX_SPACES && game->spaces[i] != NULL; i++) {
    current_id = space_get_id(game->spaces[i]);
    if (current_id == space_id) {
      current_id = space_get_south(game->spaces[i]);
      if (current_id != NO_ID) {
        player_set_location(game->player, current_id);
      }
      game->status = OK;
      return;
    }
  }
}

void game_callback_back(Game* game) {
  int i = 0;
  Id current_id = NO_ID;
  Id space_id = NO_ID;

  space_id = player_get_location(game->player);
  if (space_get_north(game_get_space(game, space_id))==NO_ID) {
    game->status = ERROR;
    return;
  }

  if (NO_ID == space_id) {
    game->status = ERROR;
    return;
  }

  for (i = 0; i < MAX_SPACES && game->spaces[i] != NULL; i++) {
    current_id = space_get_id(game->spaces[i]);
    if (current_id == space_id) {
      current_id = space_get_north(game->spaces[i]);
      if (current_id != NO_ID) {
        player_set_location(game->player, current_id);
      }
      game->status = OK;
      return;
    }
  }
}

void game_callback_take(Game* game) {
  Id object_id;
  char input[CMD_LENGTH]="";

  scanf("%s", input);
  object_id=object_get_id(game_get_object_by_name(game, input));

  if(!game || !(game->player) || !(game->spaces) || object_id==NO_ID || player_get_object(game_get_player(game))!=NO_ID) {
    game->status = ERROR;
    return;
 }

  if(player_get_location(game->player)!=game_get_object_location(game, object_id)) {
    game->status = ERROR;
    return;
  }

  if (player_set_object(game->player, object_id)==ERROR) {
    game->status = ERROR;
    return;
  }


  if (space_del_object(game_get_space(game, game_get_object_location(game, object_id)), object_id)==ERROR) {
    game->status = ERROR;
    return;
  }

  game->status = OK;
  return;
}

void game_callback_drop(Game* game) {
  Id object_id;
  if(!game || !(game->player) || !(game->spaces)) {
    game->status = ERROR;
    return;
  }

  object_id=player_get_object(game->player);

  if (space_set_object(game_get_space(game, player_get_location(game->player)), object_id)==ERROR) {
    game->status = ERROR;
    return;
  }


  if (player_set_object(game->player, NO_ID)==ERROR) {
    game->status = ERROR;
    return;
  }

  game->status = OK;
  return;
}

void game_callback_roll(Game* game) {
  if(!game || !(game->die)) {
    game->status = ERROR;
    return;
  }

  if(die_roll(game->die)==ERROR) {
    game->status = ERROR;
    return;
  }


  if(die_print(game->die)==ERROR) {
    game->status = ERROR;
    return;
  }

  game->status = OK;
  return;
}

void game_callback_left(Game* game) {
  int i = 0;
  Id current_id = NO_ID;
  Id space_id = NO_ID;

  space_id = player_get_location(game->player);

  if (NO_ID == space_id) {
    game->status = ERROR;
    return;
  }

  for (i = 0; i < MAX_SPACES && game->spaces[i] != NULL; i++) {
    current_id = space_get_id(game->spaces[i]);
    if (current_id == space_id) {
      current_id = space_get_west(game->spaces[i]);
      if (current_id != NO_ID) {
  player_set_location(game->player, current_id);
      }
      game->status = OK;
      return;
    }
  }
}



void game_callback_right(Game* game) {
  int i = 0;
  Id current_id = NO_ID;
  Id space_id = NO_ID;

  space_id = player_get_location(game->player);

  if (NO_ID == space_id) {
    game->status = ERROR;
    return;
  }

  for (i = 0; i < MAX_SPACES && game->spaces[i] != NULL; i++) {
    current_id = space_get_id(game->spaces[i]);
    if (current_id == space_id) {
      current_id = space_get_east(game->spaces[i]);
      if (current_id != NO_ID) {
	player_set_location(game->player, current_id);
      }
      game->status = OK;
      return;
    }
  }
}
