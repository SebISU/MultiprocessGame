#include "multi_header.h"

void * bot_keybinding(void * handle){

    struct client_mv_handle * mv_handle = (struct client_mv_handle *)handle;

    while(1){

        //improve this, now it does not work
        mv_handle->key_pressed = 0;//getch();

        // without this should work too  ??
        if (mv_handle->key_pressed == 'q' || mv_handle->key_pressed == 'Q'){

            return NULL;
        }

        sem_wait(&mv_handle->intern_sem);
    }

    return NULL;
}

uint32_t get_closest_beast(const struct api_inner_t * player, struct pos_t * cl_beast){

    if (cl_beast == NULL || player == NULL){

        return 1;
    }

    struct pos_t position;

    for (int8_t i = 0; i < 5; ++i){
        for (int8_t j = 0; j < 5; ++j){

            if (player->game_grid[i][j] == '*'){

                position.x = j;
                position.y = i;

                if (can_chase(player, &position)){

                    shortest_maze_distance(cl_beast, &position);
                }
            }
        }
    }

    return 0;
}

//brute force yeah
bool can_chase(const struct api_inner_t * client, const struct pos_t * pos_enemy){

    if (client == NULL || pos_enemy == NULL){

        return false;
    }

    if ((pos_enemy->x == 2 && (pos_enemy->y == 1 || pos_enemy->y == 3)) ||
    (pos_enemy->y == 2 && (pos_enemy->x == 1 || pos_enemy->x == 3))){

        return true;
    }
    else if ((pos_enemy->x == 2 && pos_enemy->y == 0 && client->game_grid[1][2] == ' ')
    || (pos_enemy->x == 2 && pos_enemy->y == 4 && client->game_grid[3][2] == ' ')
    || (pos_enemy->x == 0 && pos_enemy->y == 2 && client->game_grid[2][1] == ' ')
    || (pos_enemy->x == 4 && pos_enemy->y == 2 && client->game_grid[2][3] == ' ')){

        return true;
    }
    else if ((pos_enemy->x == 1 && pos_enemy->y == 1 &&
    (client->game_grid[1][2] == ' ' || client->game_grid[2][1] == ' '))
    || (pos_enemy->x == 3 && pos_enemy->y == 1 &&
    (client->game_grid[1][2] == ' ' || client->game_grid[2][3] == ' '))
    || (pos_enemy->x == 1 && pos_enemy->y == 3 &&
    (client->game_grid[3][2] == ' ' || client->game_grid[2][1] == ' '))
    || (pos_enemy->x == 3 && pos_enemy->y == 3 &&
    (client->game_grid[3][2] == ' ' || client->game_grid[2][3] == ' '))){

        return true;
    }
    else if (client->game_grid[1][1] == ' '){

        if (pos_enemy->x == 1 && pos_enemy->y == 0 && client->game_grid[1][2] == ' '){

            return true;
        }
        else if (pos_enemy->x == 0 && pos_enemy->y == 1 && client->game_grid[2][1] == ' '){

            return true;
        }
        else if (pos_enemy->x == 0 && pos_enemy->y == 0 && (
        (client->game_grid[1][0] == ' ' || client->game_grid[0][1] == ' ')
        && (client->game_grid[1][2] == ' ' || client->game_grid[2][1] == ' '))){

            return true;
        }
    }
    else if (client->game_grid[1][3] == ' '){

        if (pos_enemy->x == 3 && pos_enemy->y == 0 && client->game_grid[1][2] == ' '){

            return true;
        }
        else if (pos_enemy->x == 4 && pos_enemy->y == 1 && client->game_grid[2][3] == ' '){

            return true;
        }
        else if (pos_enemy->x == 4 && pos_enemy->y == 0 && (
        (client->game_grid[1][4] == ' ' || client->game_grid[0][3] == ' ')
        && (client->game_grid[1][2] == ' ' || client->game_grid[2][3] == ' '))){

            return true;
        }
    }
    else if (client->game_grid[3][3] == ' '){

        if (pos_enemy->x == 3 && pos_enemy->y == 4 && client->game_grid[3][2] == ' '){

            return true;
        }
        else if (pos_enemy->x == 4 && pos_enemy->y == 3 && client->game_grid[2][3] == ' '){

            return true;
        }
        else if (pos_enemy->x == 4 && pos_enemy->y == 4 && (
        (client->game_grid[3][4] == ' ' || client->game_grid[4][3] == ' ')
        && (client->game_grid[3][2] == ' ' || client->game_grid[2][3] == ' '))){

            return true;
        }
    }
    else if (client->game_grid[3][1] == ' '){

        if (pos_enemy->x == 1 && pos_enemy->y == 4 && client->game_grid[3][2] == ' '){

            return true;
        }
        else if (pos_enemy->x == 0 && pos_enemy->y == 3 && client->game_grid[2][1] == ' '){

            return true;
        }
        else if (pos_enemy->x == 0 && pos_enemy->y == 4 && (
        (client->game_grid[3][0] == ' ' || client->game_grid[4][1] == ' ')
        && (client->game_grid[3][2] == ' ' || client->game_grid[2][1] == ' '))){

            return true;
        }
    }

    return false;
}

uint32_t shortest_maze_distance(struct pos_t * cl_enemy, struct pos_t * pos_enemy){

    if (cl_enemy == NULL || pos_enemy == NULL){

        return 3;
    }

    int32_t maze_dist_cl_beast = abs(cl_enemy->x - CLIENT_POS_X) + 
    abs(cl_enemy->y - CLIENT_POS_Y);

    int32_t maze_dist_pos_beast = abs(pos_enemy->x - CLIENT_POS_X) + 
    abs(pos_enemy->y - CLIENT_POS_Y);

    if (maze_dist_cl_beast > maze_dist_pos_beast){

        cl_enemy->x = pos_enemy->x;
        cl_enemy->y = pos_enemy->y;

        return 2;
    }
    else if (maze_dist_cl_beast < maze_dist_pos_beast){

        return 1;
    }

    return 0;
}

enum move_t run_away_next_move(const struct api_inner_t * player,const struct pos_t * beast){

    if (player == NULL || beast == NULL){

        return QUIT;
    }

    if (beast->x >= 5 && beast->y >= 5){

        return rand() % 5;
    }

    if (beast->x > CLIENT_POS_X && beast->y > CLIENT_POS_Y){

        if (player->game_grid[2][1] == ' '){

            return LEFT;
        }
        else if (player->game_grid[1][2] == ' '){

            return UP;
        }
        else if (player->game_grid[2][3] == ' '){

            return RIGHT;
        }
        else{

            return DOWN;
        }
    }
    else if (beast->x == CLIENT_POS_X && beast->y > CLIENT_POS_Y){

        if (player->game_grid[1][2] == ' '){

            return UP;
        }
        else if (player->game_grid[2][3] == ' '){

            return RIGHT;
        }
        else if (player->game_grid[2][1] == ' '){

            return LEFT;
        }
        else{

            return DOWN;
        }
    }
    else if (beast->x < CLIENT_POS_X && beast->y > CLIENT_POS_Y){

        if (player->game_grid[2][3] == ' '){

            return RIGHT;
        }
        else if (player->game_grid[1][2] == ' '){

            return UP;
        }
        else if (player->game_grid[2][1] == ' '){

            return LEFT;
        }
        else{

            return DOWN;
        }
    }
    else if (beast->x < CLIENT_POS_X && beast->y == CLIENT_POS_Y){

        if (player->game_grid[2][3] == ' '){

            return RIGHT;
        }
        else if (player->game_grid[3][2] == ' '){

            return DOWN;
        }
        else if (player->game_grid[1][2] == ' '){

            return UP;
        }
        else{

            return LEFT;
        }
    }
    else if (beast->x < CLIENT_POS_X && beast->y < CLIENT_POS_Y){

        if (player->game_grid[2][3] == ' '){

            return RIGHT;
        }
        else if (player->game_grid[3][2] == ' '){

            return DOWN;
        }
        else if (player->game_grid[2][1] == ' '){

            return LEFT;
        }
        else{

            return UP;
        }
    }
    else if (beast->x == CLIENT_POS_X && beast->y < CLIENT_POS_Y){

        if (player->game_grid[3][2] == ' '){

            return DOWN;
        }
        else if (player->game_grid[2][1] == ' '){

            return LEFT;
        }
        else if (player->game_grid[2][3] == ' '){

            return RIGHT;
        }
        else{

            return UP;
        }
    }
    else if (beast->x > CLIENT_POS_X && beast->y < CLIENT_POS_Y){

        if (player->game_grid[2][1] == ' '){

            return LEFT;
        }
        else if (player->game_grid[3][2] == ' '){

            return DOWN;
        }
        else if (player->game_grid[2][3] == ' '){

            return RIGHT;
        }
        else{

            return UP;
        }
    }
    else{

        if (player->game_grid[2][1] == ' '){

            return LEFT;
        }
        else if (player->game_grid[3][2] == ' '){

            return DOWN;
        }
        else if (player->game_grid[1][2] == ' '){

            return UP;
        }
        else{

            return RIGHT;
        }
    }

    return NO_MOVE;
}
