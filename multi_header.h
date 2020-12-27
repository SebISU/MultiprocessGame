#ifndef MULTI_HEADER_H
#define MULTI_HEADER_H

#include <stdint.h>
#include <stdio.h>

    enum move_t {DOWN, UP, LEFT, RIGHT, QUIT};

    struct trea_data {  // I can use it to keep spawn and treasure sum info, dropped treasure, (treasure on map can be done on fly)

        uint32_t value;
        uint8_t x;
        uint8_t y;
    };

    struct trea_node {

        struct trea_node * next;
        struct trea_data treasure_info;
    };

    uint32_t add_trea(struct trea_node ** head, uint8_t x, uint8_t y , uint32_t value);
    uint32_t fetch_trea(struct trea_node ** head, uint8_t x, uint8_t y);
    void destroy_trea(struct trea_node ** head);



#endif