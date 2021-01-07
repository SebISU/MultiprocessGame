#include "multi_header.h"

// beast will not go through bushes.

int main(void){

    uint8_t game_grid[25][52] ={"###################################################",
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


    struct server_info * server = init_server(game_grid);

    if (server == NULL){

        printf("\n\nProblem with server initialization\n\n");
        return 1;
    }

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

    while(1){

        system("clear");
        display_server(server);

        // yes, but server sets player_number > 0 when it waits for connection too ...

        if (server->players[0].player_number > 0 && server->players[0].client_pid >= 0){

            sem_post(server->sem_client_1);
        }

        if (server->players[1].player_number > 0 && server->players[0].client_pid >= 0){

            sem_post(server->sem_client_2);
        }

        if (server->players[2].player_number > 0 && server->players[0].client_pid >= 0){

            sem_post(server->sem_client_3);
        }

        if (server->players[3].player_number > 0 && server->players[0].client_pid >= 0){

            sem_post(server->sem_client_4);
        }

        if (server->beasts.client_number > 0 && server->beasts.beasts_pid >= 0){

            sem_post(server->sem_client_5);
        }

        sleep(1); // time for move

        pthread_mutex_lock(&server->mutex);

        // check if everything is fine when server is connecting a new client, mutex?


    // set mutexes, check everything, make file and fix displaying

        // mutex required to make sure that handle_connections will not set player_number again
        // if after shm_unlink() other programs can still see a shared memory everything is fine
        // if not, children processes won't get info
        if (server->key_pressed == 'q' || server->key_pressed == 'Q'){

            for (int32_t i = 0; i < PLAYERS_NUM + 1; ++i){

                server->api_client[i].api->player_number = 0;
            }

            server->api_conn.api->player_number = 0;

            // maybe sleep() to make sure that client, bot, beast received info?

            break;
        }

        if (server->key_pressed != 0){

            server->key_pressed = 0;    // this line can be on thread side after sem_wait too
            sem_post(&server->sem_keybinding);
        }

        server->round_number++;

        // manage beasts
        if (server->beasts.client_number > 0){

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
            }
        }


        // manage players
        for (int32_t i = 0; i < PLAYERS_NUM; ++i){

            // here problem when a free client taken to connections
            // yeah client pid as a additional info
            if (server->players[i].player_number > 0 && server->players[i].client_pid >= 0){

                if (server->api_client[i].api->player_number == 0){

                    // if connection exists, manage the move
                    move_player(server, i + 1);
                }
                else{

                    set_new_character_game_board(server, &server->players[i].curr_position, ' ');
                    reset_player_info(server->players + i);
                }   
            }
        }

        update_all_api_client(server);

        pthread_mutex_unlock(&server->mutex);

    };

    destroy_server(server);

    printf("\n\nServer closed\n\n");

    return 0;
}
