#include "multi_header.h"

// beast will not go through bushes.

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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

    //pthread_mutex_init(&mutex, NULL); pthread mutex initializer instead

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

        display_server(server);

        if (server->players[0].player_number > 0){

            sem_post(server->sem_client_1);
        }

        if (server->players[1].player_number > 0){

            sem_post(server->sem_client_2);
        }

        if (server->players[2].player_number > 0){

            sem_post(server->sem_client_3);
        }

        if (server->players[3].player_number > 0){

            sem_post(server->sem_client_4);
        }

        if (server->beasts.client_number > 0){

            sem_post(server->sem_client_5);
        }

        sleep(1); // time for move

        // check if everything is fine when server is connecting a new client, mutex?


        // close the server. Before update funcs so not to unnecessarily update funcs calls
        if (server->key_pressed == 'q' || server->key_pressed == 'Q'){

            for (int32_t i = 0; i < PLAYERS_NUM + 1; ++i){

                server->api_client[i].api->player_number = 0;
            }

            // maybe sleep() to make sure that client, bot, beast received info?

            break;
        }

        server->round_number++;

        // kill beast after defined num of rounds
        for (int32_t i = 0; i < BEASTS_MAX_NUM; ++i){

            if (server->round_number - server->beasts.beasts[i].init_round_number > BEAST_NUM_ROUNDS){

                set_new_character_game_board(server, &server->beasts.beasts[i].position, ' ');
                server->beasts.beasts[i].in_game = 0;
            }
        }


        // manage beasts
        if (server->beasts.client_number > 0){

            if (server->api_client[PLAYERS_NUM].api->player_number == 0){

                // if connected

                for (int32_t i = 0; i < BEASTS_MAX_NUM; ++i){

                    if (server->beasts.beasts[i].in_game == 1){

                        move_beast(server, i + 1);
                    }
                }

            }
            else{

                server->beasts.client_number = 0;
            }
        }


        // manage players
        for (int32_t i = 0; i < PLAYERS_NUM; ++i){

            if (server->players[i].player_number > 0){
                if (server->api_client[i].api->player_number == 0){

                    // if connection exists, manage the move
                    move_player(server, i + 1);
                }
                else{

                    server->players[i].player_number = 0;
                }   
            }
        }

        update_all_api_client(server);

        if (server->key_pressed != 0){ // where is the best place for this?
                                       // can reset value before thread will manage the keybinding
                                       // every if st in thread inside mutex? easy ... 
                                       // just after if st closing server? and mutex
            server->key_pressed = 0;    // this line can be on thread side after sem_wait too
            sem_post(&server->sem_keybinding);
        }

    };

    destroy_server(server);

    printf("\n\nServer closed\n\n");

    return 0;
}
