#include "multi_header.h"

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

        printf("\n\nProblem with server initialization.\n\n");
        return 1;
    }

    initscr();
    keypad(stdscr, TRUE);


    noecho();

    pthread_t thd_keybinding;
    pthread_t thd_connections;

    if (pthread_create(&thd_keybinding, NULL, server_keybinding, (void*)server)){

        destroy_server(server);
        printf("\n\nProblem with keybinding thread creation.\n\n");
        return 2;
    }

    if (pthread_create(&thd_connections, NULL, handle_connections, (void*)server)){

        destroy_server(server);
        printf("\n\nProblem with connections thread creation.\n\n");
        return 2;
    }

    pthread_mutex_lock(&server->mutex);

    while(1){

        display_server(server);

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

        usleep(600000); // time for move

        pthread_mutex_lock(&server->mutex);

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

            break;
        }

        if (server->key_pressed != 0){

            server->key_pressed = 0;
            sem_post(&server->sem_keybinding);
        }

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

                reset_beasts_info(&server->beasts);
                server->api_conn.api->beasts_in_game = 0;
                update_api_conn(server);
            }
        }

        // manage players
        for (int32_t i = 0; i < PLAYERS_NUM; ++i){

            if (server->players[i].player_number > 0 && server->players[i].client_pid >= 0){

                if (server->api_client[i].api->player_number == 0){

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
                    update_api_conn(server);
                }   
            }
        }

        update_all_api_client(server);

        pthread_mutex_unlock(&server->mutex);

    };

    destroy_server(server);

    printf("\n\nServer closed.\n\n");

    return 0;
}
