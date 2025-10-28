#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "orderbook.h"
#include "user.h"
#include "colors.h"



struct Order *buy_order_head=NULL;
struct Order *sell_order_head=NULL;

const char* VALID_STOCKS[]={
    "TATA","RELI","WIPRO"
};
const int NUM_VALID_STOCKS=3;


int is_ticker_valid(char* ticker_input) {
    for (int i=0;i<NUM_VALID_STOCKS;i++){
        if (strcmp(ticker_input, VALID_STOCKS[i])==0){
            return 1;
        }
    }
    return 0;
}

struct PortfolioItem* get_portfolio_item(struct User *currentUser,char *ticker){
    for(int i=0;i<currentUser->stocks_owned;i++){
        if (strcmp(currentUser->portfolio[i].ticker,ticker)==0){
            return &currentUser->portfolio[i];
        }
    }
    return NULL;
}

static void add_order_to_list(struct Order *newOrder){
    newOrder->next=NULL;
    struct Order **head_ptr;

    if (newOrder->type=='B'){
        head_ptr=&buy_order_head;

    }else {
        head_ptr=&sell_order_head;
    }

    if (*head_ptr==NULL){
        *head_ptr=newOrder;
    }else {
        struct Order *temp=*head_ptr;
        while(temp->next!=NULL){
            temp=temp->next;
        }
        temp->next=newOrder;
    }
}

void place_buy_order(struct User *currentUser){
    char ticket_input[30];
    int quantity_input=0;
    double price_input=0.0;
    double total_cost=0.0;

    printf(YELLOW "n--- Place a Buy Order ---\n" RESET);

    while(1){
        printf("Enter stock ticker: ");
        scanf("%s",ticket_input);
        if (is_ticker_valid(ticket_input)){
            break;
        }
        else {
            printf(RED "ERROR: '%s' is not a valid stock.\n" RESET,ticket_input); 
        }
    }

    printf("Enter desired price: $");
    
    scanf("%lf",&price_input);
    printf("Enter desired Quantity: ");
    scanf("%d",&quantity_input);

    if (price_input<=0||quantity_input<=0){
        printf(RED "Error: Price and quantity must be positive.\n" RESET);
        return;
    }

    total_cost=price_input*quantity_input;
    if (total_cost>currentUser->available_cash){
        printf(RED "INSUFFICIENT FUNDS !!.\n" RESET);
        printf("You have $%.2f available but this order costs $%.2f\n",currentUser->available_cash,total_cost);
        return;
    }

    struct Order *newOrder=(struct Order*) malloc(sizeof(struct Order));
    if (newOrder==NULL){
        printf(RED "FATAL ERROR: OUT OF MEMORY!!\n" RESET);
        return;
    }

    strcpy(newOrder->username,currentUser->username);
    strcpy(newOrder->ticker,ticket_input);
    newOrder->type='B';
    newOrder->quantity=quantity_input;
    newOrder->price=price_input;

    add_order_to_list(newOrder);

    currentUser->available_cash=currentUser->available_cash-total_cost;

    printf(GREEN "\nBuy order for %d shares of %s at %.2f placed successfully.\n" RESET,quantity_input,ticket_input,price_input);
    printf(" A total of $%.2f has been moved from 'Available Cash'.\n",total_cost);
}

void place_sell_order(struct User *currentUser){
    char ticker_input[10];
    int quantity_input=0;
    double price_input=0.0;
    struct PortfolioItem *item_to_sell;
    printf(YELLOW "\n--- Place a Sell Order---\n" RESET);


    while(1){
        printf("Enter stock ticker: ");
        scanf("%s",ticker_input);
        if (is_ticker_valid(ticker_input)){
            break;
        }else {
            printf(RED "ERROR: '%s' is not a valid stock.\n" RESET,ticker_input);
        }
    }


    item_to_sell=get_portfolio_item(currentUser,ticker_input);
    if (item_to_sell==NULL){
        printf(RED "ERROR: YOU DO NOT OWN ANY %s STOCKS.\n" RESET,ticker_input);
        return;
    }
    printf("Enter desired price: $");
    scanf("%lf", &price_input);
    printf("Enter quantity to sell (You own %d): ",item_to_sell->quantity);
    scanf("%d",&quantity_input);

    if (price_input<=0||quantity_input<=0){
        printf(RED "ERROR: Price and quantity must be positive.\n" RESET);
        return;
    }
    if (quantity_input>item_to_sell->quantity){
        printf(RED "ERROR: Insufficient stocks.\n" RESET);
        return;
    }

    struct Order *newOrder=(struct Order*) malloc(sizeof(struct Order));
    if (newOrder==NULL){
        printf(RED "Fatal error: Out of memory!!.\n" RESET);
        return;
    }

    strcpy(newOrder->username,currentUser->username);
    strcpy(newOrder->ticker,ticker_input);
    newOrder->type='S';
    newOrder->quantity=quantity_input;
    newOrder->price=price_input;


    add_order_to_list(newOrder);
    printf(GREEN "\nSell order for %d shares %s at $%.2f placed successfully.\n" RESET,quantity_input,ticker_input,price_input);
}