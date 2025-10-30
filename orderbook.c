#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "orderbook.h"
#include "user.h"
#include "colors.h"



struct Order *buy_order_head=NULL;
struct Order *sell_order_head=NULL;

const char* VALID_STOCKS[]={
    "TATA","RELI","WIPRO","INFOSYS","HDFC","SBI"
};
const int NUM_VALID_STOCKS=6;

double live_tata_price = 500.00;
double live_reli_price = 3100.00;
double live_wipro_price = 450.00;
double live_infy_price = 1520.00;
double live_hdfc_price = 1600.00;
double live_sbi_price = 600.00;


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
    match_trades();
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




 
static void update_portfolio(struct User *user, char *ticker, int quantity, double new_cost_per_share) {
    struct PortfolioItem *item = get_portfolio_item(user, ticker);

    if (item != NULL) {
        

        
        double old_total_cost = item->avg_cost * item->quantity;

        
        double new_total_cost = new_cost_per_share * quantity;

        
        int new_total_quantity = item->quantity + quantity;

        
        item->avg_cost = (old_total_cost + new_total_cost) / new_total_quantity;

        
        item->quantity = new_total_quantity;

    } else {
        
        if (user->stocks_owned < 30) { 
            strcpy(user->portfolio[user->stocks_owned].ticker, ticker);
            user->portfolio[user->stocks_owned].quantity = quantity;
            user->portfolio[user->stocks_owned].avg_cost = new_cost_per_share; 
            user->stocks_owned++;
        }
    }
}


void match_trades() {
    struct Order *buy_prev = NULL;
    struct Order *buy_curr = buy_order_head;

    while (buy_curr != NULL) {
        int trade_made = 0;
        struct Order *sell_prev = NULL;
        struct Order *sell_curr = sell_order_head;

        
        while (sell_curr != NULL) {


            if (strcmp(buy_curr->ticker, sell_curr->ticker) == 0 &&
                buy_curr->price >= sell_curr->price &&
                buy_curr->quantity == sell_curr->quantity) 
            {
                
                printf(MAGENTA "\n--- TRADE EXECUTED ---\n" RESET);
                printf("  Ticker: %s\n", buy_curr->ticker);
                printf("  Shares: %d\n", buy_curr->quantity);
                printf("  Price:  $%.2f\n", sell_curr->price); 
                printf("  Buyer:  %s\n", buy_curr->username);
                printf("  Seller: %s\n", sell_curr->username);

                double trade_cost = sell_curr->price * buy_curr->quantity;

                struct User *buyer = find_user(buy_curr->username);
                struct User *seller = find_user(sell_curr->username);

                if (buyer == NULL || seller == NULL) {
                    printf(RED "FATAL ERROR: Could not find user.\n" RESET);
                    sell_prev = sell_curr;
                    sell_curr = sell_curr->next;
                    continue; 
                }

                
                seller->cash_balance += trade_cost;
                seller->available_cash += trade_cost;
                struct PortfolioItem *sold_item = get_portfolio_item(seller, sell_curr->ticker);
                sold_item->quantity -= sell_curr->quantity;

                
                double buy_cost = buy_curr->price * buy_curr->quantity;
                double refund = buy_cost - trade_cost;

                buyer->cash_balance -= trade_cost;
                buyer->available_cash += refund; 
                update_portfolio(buyer, buy_curr->ticker, buy_curr->quantity,trade_cost);

                

                if (buy_prev == NULL) {
                    buy_order_head = buy_curr->next;
                } else {
                    buy_prev->next = buy_curr->next;
                }

                if (sell_prev == NULL) {
                    sell_order_head = sell_curr->next;
                } else {
                    sell_prev->next = sell_curr->next;
                }

                struct Order *buy_to_free = buy_curr;
                struct Order *sell_to_free = sell_curr;

                buy_curr = buy_curr->next; 

                free(buy_to_free);
                free(sell_to_free);

                trade_made = 1;
                printf(MAGENTA "--- TRADE COMPLETE ---\n" RESET);
                break; 

            } else {
                
                sell_prev = sell_curr;
                sell_curr = sell_curr->next;
            }
        } 

        if (trade_made) {
            continue; 
        } else {
            
            buy_prev = buy_curr;
            buy_curr = buy_curr->next;
        }
    } 
}


void fluctuate_prices() {
    
    double change_direction = ((double)rand() / (RAND_MAX / 2)) - 1.0; 

    double change_percent = change_direction * 0.0005; 

    int stock_to_change = rand() % 6; 

    if (stock_to_change == 0) {
        live_tata_price = live_tata_price * (1.0 + change_percent);
    } else if (stock_to_change == 1) {
        live_reli_price = live_reli_price * (1.0 + change_percent);
    } else if (stock_to_change == 2) {
        live_wipro_price = live_wipro_price * (1.0 + change_percent);
    } else if (stock_to_change == 3) {
        live_infy_price = live_infy_price * (1.0 + change_percent);
    } else if (stock_to_change == 4) {
        live_hdfc_price = live_hdfc_price * (1.0 + change_percent);
    } else if (stock_to_change == 5) {
        live_sbi_price = live_sbi_price * (1.0 + change_percent);
    }
}


double get_live_price(char* ticker) {
    if (strcmp(ticker, "TATA") == 0) return live_tata_price;
    if (strcmp(ticker, "RELI") == 0) return live_reli_price;
    if (strcmp(ticker, "WIPRO") == 0) return live_wipro_price;
    if (strcmp(ticker, "INFY") == 0) return live_infy_price;
    if (strcmp(ticker, "HDFC") == 0) return live_hdfc_price;
    if (strcmp(ticker, "SBI") == 0) return live_sbi_price;
    return 0.0;
}