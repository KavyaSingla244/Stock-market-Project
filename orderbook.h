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
void fluctuate_prices();
double get_live_price(char * ticker);
void view_market_data();
void print_list_reverse(struct Order *head);
void view_order_book();
void add_order_to_list(struct Order *newOrder);
void save_order_books();
void show_price_chart(char* ticker); 
int is_ticker_valid();
#endif

