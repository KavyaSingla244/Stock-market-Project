#ifndef ORDERBOOK_H
#define ORDERBOOK_H
#include "user.h"

struct Order

{
    char username[50];
    char ticker[30];
    char type;
    int quantity;
    double price;


    struct Order *next;
};
void place_buy_order(struct User *currentUser);
void place_sell_order(struct User *currentUser);
void match_trades();
#endif

