/**
 * @brief It implements the game interface and all the associated callbacks
 * for each command
 *
 * @file game.c
 * @author Profesores PPROG
 * @version 1.0
 * @date 13-01-2015
 * @copyright GNU Public License
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"

#define N_CALLBACK 7

/**
   Define the function type for the callbacks
*/
typedef void (*callback_fn)(Game* game);

/**
   List of callbacks for each command in the game
*/
void game_callback_unknown(Game* game);
void game_callback_exit(Game* game);
void game_callback_next(Game* game);
void game_callback_back(Game* game);
void game_callback_turnon(Game* game);
void game_callback_turnoff(Game* game);
void game_callback_look(Game* game);

static callback_fn game_callback_fn_list[N_CALLBACK]={
  game_callback_unknown,
  game_callback_exit,
  game_callback_next,
  game_callback_back,
  game_callback_turnon,
  game_callback_turnoff,
  game_callback_look};

/**
   Private functions
*/

STATUS game_load_spaces(Game* game, char* filename);
STATUS game_add_space(Game* game, Space* space);
Id     game_get_space_id_at(Game* game, int position);
STATUS game_set_player_location(Game* game, Id id);
STATUS game_set_object_location(Game* game, Id id);

/**
   Game interface implementation
*/

STATUS game_create(Game* game) {
  int i;

  for (i = 0; i < MAX_SPACES; i++) {
    game->spaces[i] = NULL;
  }

  game->player_location = NO_ID;
  game->object_location = NO_ID;
  game->last_cmd = NO_CMD;
  game->description[0]='\0';

  return OK;
}

STATUS game_create_from_file(Game* game, char* filename) {

  if (game_create(game) == ERROR)
    return ERROR;

  if (game_load_spaces(game, filename) == ERROR)
    return ERROR;

  game_set_player_location(game, game_get_space_id_at(game, 0));
  game_set_object_location(game, game_get_space_id_at(game, 0));

  return OK;
}

STATUS game_destroy(Game* game) {
  int i = 0;

  for (i = 0; (i < MAX_SPACES) && (game->spaces[i] != NULL); i++) {
    space_destroy(game->spaces[i]);
  }

  return OK;
}

STATUS game_add_space(Game* game, Space* space) {
  int i = 0;

  if (space == NULL) {
    return ERROR;
  }

  while ( (i < MAX_SPACES) && (game->spaces[i] != NULL)){
    i++;
  }

  if (i >= MAX_SPACES) {
    return ERROR;
  }

  game->spaces[i] = space;

  return OK;
}

Id game_get_space_id_at(Game* game, int position) {

  if (position < 0 || position >= MAX_SPACES) {
    return NO_ID;
  }

  return space_get_id(game->spaces[position]);
}

Space* game_get_space(Game* game, Id id){
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

STATUS game_set_player_location(Game* game, Id id) {

  if (id == NO_ID) {
    return ERROR;
  }

  game->player_location = id;

  return OK;
}

STATUS game_set_object_location(Game* game, Id id) {

  if (id == NO_ID) {
    return ERROR;
  }

  game->object_location = id;

  return OK;
}

STATUS game_set_description(Game* game, const char* description) {
  if (!game || !description)
    return ERROR;
  strncpy(game->description, description, WORD_SIZE);
  return OK;
}

const char* game_get_description(Game* game) {
  if (!game)
    return "\0";
  return game->description;
}

Id game_get_player_location(Game* game) {
  return game->player_location;
}

Id game_get_object_location(Game* game) {
  return game->object_location;
}

STATUS game_update(Game* game, T_Command cmd) {
  game->description[0]='\0';
  game->last_cmd = cmd;
  (*game_callback_fn_list[cmd])(game);
  return OK;
}

T_Command game_get_last_command(Game* game){
  return game->last_cmd;
}

void game_print_data(Game* game) {
  int i = 0;

  printf("\n\n-------------\n\n");

  printf("=> Spaces: \n");
  for (i = 0; i < MAX_SPACES && game->spaces[i] != NULL; i++) {
    space_print(game->spaces[i]);
  }

  printf("=> Object location: %d\n", (int) game->object_location);
  printf("=> Player location: %d\n", (int) game->player_location);
  printf("prompt:> ");
}

BOOL game_is_over(Game* game) {
  return FALSE;
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

  space_id = game_get_player_location(game);
  if (space_id == NO_ID) {
    return;
  }

  for (i = 0; i < MAX_SPACES && game->spaces[i] != NULL; i++) {
    current_id = space_get_id(game->spaces[i]);
    if (current_id == space_id) {
      current_id = space_get_south(game->spaces[i]);
      if (current_id != NO_ID) {
	game_set_player_location(game, current_id);
      }
      return;
    }
  }
}

void game_callback_back(Game* game) {
  int i = 0;
  Id current_id = NO_ID;
  Id space_id = NO_ID;

  space_id = game_get_player_location(game);

  if (NO_ID == space_id) {
    return;
  }

  for (i = 0; i < MAX_SPACES && game->spaces[i] != NULL; i++) {
    current_id = space_get_id(game->spaces[i]);
    if (current_id == space_id) {
      current_id = space_get_north(game->spaces[i]);
      if (current_id != NO_ID) {
	game_set_player_location(game, current_id);
      }
      return;
    }
  }
}

void game_callback_turnon(Game* game) {
  Id space_id = NO_ID;

  space_id = game_get_player_location(game);

  if (space_id == NO_ID)
    return;

  space_set_illuminated(game_get_space(game, space_id), TRUE);
  return;
}

void game_callback_turnoff(Game* game) {
  Id space_id = NO_ID;

  space_id = game_get_player_location(game);

  if (space_id == NO_ID)
    return;

  space_set_illuminated(game_get_space(game, space_id), FALSE);
  return;
}

void game_callback_look(Game* game) {
  char input[WORD_SIZE]="";
  int act_id = NO_ID;
  Space* next = NULL;
  Space* act = NULL;
  Space* back = NULL;

  scanf("%s", input);
  printf("%s", input);
  act_id = game_get_player_location(game);

  if (act_id == NO_ID)
    return;

  next = game_get_space(game, space_get_south(game_get_space(game, act_id)));
  act = game_get_space(game, act_id);
  back = game_get_space(game, space_get_north(game_get_space(game, act_id)));

  if (strcmp(input, "back")==0 || strcmp(input, "b")==0) {
    if (!back)
      return;
    if(space_get_illuminated(back)==FALSE) {
      game_set_description(game, "Space is not illuminated");
      return;
    }
    game_set_description(game, space_get_name(back));
  }

  if (strcmp(input, "here")==0 || strcmp(input, "h")==0) {
    if (!act)
      return;
    if(space_get_illuminated(act)==FALSE) {
      game_set_description(game, "Space is not illuminated");
      return;
    }
    game_set_description(game, space_get_name(act));
  }

  if (strcmp(input, "next")==0 || strcmp(input, "n")==0) {
    if (!next)
      return;
    if(space_get_illuminated(next)==FALSE) {
      game_set_description(game, "Space is not illuminated");
      return;
    }
    game_set_description(game, space_get_name(next));
  }

  return;
}

STATUS game_load_spaces(Game* game, char* filename) {
  FILE* file = NULL;
  char line[WORD_SIZE] = "";
  char name[WORD_SIZE] = "";
  char* toks = NULL;
  Id id = NO_ID, north = NO_ID, east = NO_ID, south = NO_ID, west = NO_ID;
  Space* space = NULL;
  STATUS status = OK;
  BOOL illuminated = FALSE;

  if (!filename) {
    return ERROR;
  }

  file = fopen(filename, "r");
  if (file == NULL) {
    return ERROR;
  }

  while (fgets(line, WORD_SIZE, file)) {
    if (strncmp("#s:", line, 3) == 0) {
      toks = strtok(line + 3, "|");
      id = atol(toks);
      toks = strtok(NULL, "|");
      strcpy(name, toks);
      toks = strtok(NULL, "|");
      north = atol(toks);
      toks = strtok(NULL, "|");
      east = atol(toks);
      toks = strtok(NULL, "|");
      south = atol(toks);
      toks = strtok(NULL, "|");
      west = atol(toks);
      toks = strtok(NULL, "|");
      illuminated = atol(toks);
#ifdef DEBUG
      printf("Leido: %ld|%s|%ld|%ld|%ld|%ld|%ld\n", id, name, north, east, south, west, illuminated);
#endif
      space = space_create(id);
      if (space != NULL) {
	space_set_name(space, name);
	space_set_north(space, north);
	space_set_east(space, east);
	space_set_south(space, south);
	space_set_west(space, west);
  space_set_illuminated(space, illuminated);
	game_add_space(game, space);
      }
    }
  }

  if (ferror(file)) {
    status = ERROR;
  }

  fclose(file);

  return status;
}
