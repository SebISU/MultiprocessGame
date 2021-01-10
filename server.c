#include "multi_header.h"

// beast will not go through bushes.

int main(void){

    int8_t game_grid[GAME_HEIGHT][GAME_WIDTH] ={"###################################################",
                                "#   #       #   ~~~~~       #         #       #   #",
                                "# # ### ### ########### ### # ####### ### #####   #",
                                "# #   # # #           # # #   #     #     #   #   #",
                                "# ### # # ###~~~##### # # ##### ############# ### #",
                                "# # #   #           # # # ~~  #       #       # # #",
                                "# # ##### ### ####### # # # ### ### ### ### # # # #",
                                "# #         # #       # # #     #   #   # # #   # #",
                                "# # ####### ### ####### ##### ### ### ### # ### # #",
                                "# # #     #   #   #     #   #   #         # # # # #",
                                "# ### ### ### ### ### ### # ### ########### # # # #",
                                "# #   #       # #   #     # #   # #       # #   # #",
                                "# # ######~## # ### ### ### ### # # ##### # # ### #",
                                "# #    ~#   # #   #   #   #   #   # #     # # #   #",
                                "# # # ~~# ### ### ### ####### ### # ### ### # # ###",
                                "# # #~~ #    ~  #   # #  ~~~  #   #   #     # # # #",
                                "# # #~  ####### # # # # ##~#### ##### ####### # # #",
                                "# # #~      #   # # #   #     #   # #       ~~#   #",
                                "# ######### # ### ####### ####### # ##### # ~~### #",
                                "# #~      # #     #     #       #   #   # # ~~  # #",
                                "# # ##### # ####### # ### ##### ### # # ###~##### #",
                                "#~~~#     #         #  ~~~#~~ #     # #   #~~~~~~ #",
                                "# ### #####################~# ####### ### #~    ~ #",
                                "#   #                 ~~~~~~#~~         #    ~~   #",
                                "###################################################"};

    int8_t * game_rows[GAME_HEIGHT];

    for (int32_t i = 0; i < GAME_HEIGHT; ++i){

        game_rows[i] = game_grid[i];
    }

    struct server_info * server = init_server(game_rows);

    if (server == NULL){

        printf("\n\nProblem with server initialization\n\n");
        return 1;
    }

    initscr();
    keypad(stdscr, TRUE);
    noecho();

    pthread_t thd_keybinding;
    pthread_t thd_connections;

    if (pthread_create(&thd_keybinding, NULL, server_keybinding, (void*)server)){

        printf("\n\nProblem with the keybinding thread creation\n\n");
        destroy_server(server);
        return 2;
    }

    if (pthread_create(&thd_connections, NULL, handle_connections, (void*)server)){

        printf("\n\nProblem with the connections thread creation\n\n");
        destroy_server(server);
        return 2;
    }

    pthread_mutex_lock(&server->mutex);

    while(1){

        system("clear");
        display_server(server);

        printf("\n\nNEXT MOVE\n\n");

        if (server->players[0].player_number > 0 && server->players[0].client_pid >= 0){

            sem_post(server->sem_client_1);
        }

        if (server->players[1].player_number > 0 && server->players[1].client_pid >= 0){

            sem_post(server->sem_client_2);
        }

        if (server->players[2].player_number > 0 && server->players[2].client_pid >= 0){

            sem_post(server->sem_client_3);
        }

        if (server->players[3].player_number > 0 && server->players[3].client_pid >= 0){

            sem_post(server->sem_client_4);
        }

        if (server->beasts.client_number > 0 && server->beasts.beasts_pid >= 0){

            sem_post(server->sem_client_5);
        }

        pthread_mutex_unlock(&server->mutex);

// 1. when 100 rounds per sec. problem with SERVER CONNECTION LOST on the beast side. Server updates
// so often that beast has no enough time to perform a cycle. Server sets 0 to api client_num
// and disconnects beast because beast hasn't set it to 0 (sign that connection exist).
// 2. When 1 round per sec. new beast sometimes gets info that beast already exist, when previous beast
// is already killed because server hasn't had enough time to update beasts. Info about beast already exists
// doesn't work when 100 rounds per sec. and a new "client" wants to be a beast.


        usleep(1000000); // time for move

        printf("\n\nNEXT LOCK\n\n");

        pthread_mutex_lock(&server->mutex);
        printf("\nMUTEX START %lu\n", server->round_number + 1);

        // mutex required to make sure that handle_connections will not set player_number again
        // if after shm_unlink() other programs can still see a shared memory everything is fine
        // if not, children processes won't get info

        if (server->key_pressed == 'q' || server->key_pressed == 'Q'){

            for (int32_t i = 0; i < PLAYERS_NUM + 1; ++i){

                server->api_client[i].api->player_number = 0;
            }

            server->api_conn.api->player_number = 0;

            sem_post(server->sem_client_1);
            sem_post(server->sem_client_2);
            sem_post(server->sem_client_3);
            sem_post(server->sem_client_4);
            sem_post(server->sem_client_5);
            sem_post(server->sem_server);
            // checked, works

            break;
        }

        if (server->key_pressed != 0){

            server->key_pressed = 0;    // this line can be on thread side after sem_wait too
            sem_post(&server->sem_keybinding);
        }

        // printf("%lu CHECK 1 \n", server->round_number);

        // for (int32_t i = 0; i < PLAYERS_NUM; ++i){

        //     printf(" %d PL_NUM: %d PL_PID: %d\n", i, server->players[i].player_number, server->players[i].client_pid);
        //     printf(" %d API_NUM: %d API_PID: %d\n", i, server->api_client[i].api->player_number);
        // }

        server->round_number++;

        // manage beasts
        if (server->beasts.client_number > 0 && server->beasts.beasts_pid >= 0){

            // kill the beast after defined num of rounds
            for (int32_t i = 0; i < BEASTS_MAX_NUM; ++i){

                if (server->round_number - server->beasts.beasts[i].init_round_number > BEAST_NUM_ROUNDS){

                    set_new_character_game_board(server, &server->beasts.beasts[i].position, ' ');
                    server->beasts.beasts[i].in_game = 0;
                }
            }

            if (server->api_client[PLAYERS_NUM].api->player_number == 0){

                for (int32_t i = 0; i < BEASTS_MAX_NUM; ++i){

                    if (server->beasts.beasts[i].in_game == 1){

                        move_beast(server, i + 1);
                    }
                }
            }
            else{

                for (int32_t i = 0; i < BEASTS_MAX_NUM; ++i){

                    if (server->beasts.beasts[i].in_game == 1){

                        set_new_character_game_board(server, &server->beasts.beasts[i].position, ' ');
                    }
                }
               // printf("\n\nHERE\n\n");
                // this info will not be updated in api because update if client_num > 0 and pid >= 0
                reset_beasts_info(&server->beasts);
                server->api_conn.api->beasts_in_game = 0;
                update_api_conn(server);
            }
        }

        // manage players
        for (int32_t i = 0; i < PLAYERS_NUM; ++i){

            if (server->players[i].player_number > 0 && server->players[i].client_pid >= 0){

                if (server->api_client[i].api->player_number == 0){

                    // if connection exists, manage the move
                    move_player(server, i + 1);
                }
                else{

                    if (i == 0){

                        sem_wait(server->sem_client_1);
                    }
                    else if (i == 1){

                        sem_wait(server->sem_client_2);
                    }
                    else if (i == 2){

                        sem_wait(server->sem_client_3);
                    }
                    else{

                        sem_wait(server->sem_client_4);
                    }

                    set_new_character_game_board(server, &server->players[i].curr_position, ' ');
                    reset_player_info(server->players + i);
                    server->api_client[i].api->player_number = 0;
                    // here the same as in multi_header.c:1310?
                    update_api_conn(server);
                }   
            }
        }

        // printf("%lu CHECK 2 \n", server->round_number);

        // for (int32_t i = 0; i < PLAYERS_NUM; ++i){

        //     printf(" %d PL_NUM: %d PL_PID: %d\n", i, server->players[i].player_number, server->players[i].client_pid);
        //     printf(" %d API_NUM: %d\n", i, server->api_client[i].api->player_number);
        // }

        update_all_api_client(server);

        printf("\nMUTEX END %lu\n", server->round_number);
        pthread_mutex_unlock(&server->mutex);

        //sleep(2);
    };

    destroy_server(server);

    printf("\n\nServer closed\n\n");

    return 0;
}
