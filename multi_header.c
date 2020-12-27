#include "multi_header.h"
#include <stdio.h>
#include <stdlib.h>


uint32_t add_trea(struct trea_node ** head, uint8_t x, uint8_t y , uint32_t value){
    
    if (head == NULL){

        return 0;
    }

    struct trea_node * temp;

    if (*head == NULL){

        *head = malloc(sizeof(struct trea_node));

        if (*head == NULL){

            return 0;
        }
    }
    else{

        temp = malloc(sizeof(struct trea_node));

        if (temp == NULL){

            return 0;
        }

        temp->next = *head;
        *head = temp;
    }

    (*head)->treasure_info.x = x;
    (*head)->treasure_info.y = y;
    (*head)->treasure_info.value = value;

    return (*head)->treasure_info.value;
}

uint32_t fetch_trea(struct trea_node ** head, uint8_t x, uint8_t y){

    if (head == NULL){

        return 0;
    }

    struct trea_node * temp = *head;
    struct trea_node * prev_temp;
    uint32_t val;

    while(temp != NULL){

        if (temp->treasure_info.x == x && temp->treasure_info.y == y){

            val = temp->treasure_info.value;

            if (*head == temp){

                *head = temp->next;
                free(temp);
            }
            else{

                prev_temp->next = temp->next;
                free(temp);
            }

            return val;
        }

        prev_temp = temp;
        temp = temp->next;
    }

    return 0;
}

void destroy_trea(struct trea_node ** head){

    if (head != NULL){

        struct trea_node * temp;

        while(*head != NULL){

            temp = *head;
            *head = (*head)->next;
            free(temp);
        }
    }
}

