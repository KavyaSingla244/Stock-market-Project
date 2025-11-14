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
double live_infosys_price = 1520.00;
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

 void add_order_to_list(struct Order *newOrder){
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
        // --- User already owns this stock, calculate new average ---
        
        // 1. Calculate the total cost of old shares
        double old_total_cost = item->avg_cost * item->quantity;
        
        // 2. Calculate the total cost of new shares
        double new_total_cost = new_cost_per_share * quantity;
        
        // 3. Calculate the new total quantity
        int new_total_quantity = item->quantity + quantity;
        
        // 4. Calculate the new weighted average cost
        if (new_total_quantity > 0) {
            item->avg_cost = (old_total_cost + new_total_cost) / new_total_quantity;
        } else {
            item->avg_cost = 0.0;
        }
        
        // 5. Add the new quantity
        item->quantity = new_total_quantity;

    } else {
        // --- This is the first time user is buying this stock ---
        if (user->stocks_owned < 30) { // Check if portfolio is full
            strcpy(user->portfolio[user->stocks_owned].ticker, ticker);
            user->portfolio[user->stocks_owned].quantity = quantity;
            user->portfolio[user->stocks_owned].avg_cost = new_cost_per_share;
            user->stocks_owned++;
        }
    }
}


void match_trades() {
    
    // We use a label to restart the entire search after a trade
restart_matching:
    ; // A 'goto' must jump to a statement, and an empty ; is a statement

    struct Order *buy_prev = NULL;
    struct Order *buy_curr = buy_order_head;
    
    // Loop 1: Iterate through all BUY orders
    while (buy_curr != NULL) {
        
        struct Order *sell_prev = NULL;
        struct Order *sell_curr = sell_order_head;

        // Loop 2: For each BUY order, iterate through all SELL orders
        while (sell_curr != NULL) {
            
            // Check for a match
            if (strcmp(buy_curr->ticker, sell_curr->ticker) == 0 &&
                buy_curr->price >= sell_curr->price &&
                buy_curr->quantity == sell_curr->quantity) 
            {
                // --- MATCH FOUND! ---
                printf(MAGENTA "\n--- TRADE EXECUTED ---\n" RESET);
                printf("  Ticker: %s\n", buy_curr->ticker);
                printf("  Shares: %d\n", buy_curr->quantity);
                printf("  Price:  $%.2f\n", sell_curr->price); 
                printf("  Buyer:  %s\n", buy_curr->username);
                printf("  Seller: %s\n", sell_curr->username);

                double trade_cost = sell_curr->price * buy_curr->quantity;
                
                struct User *buyer = find_user(buy_curr->username);
                struct User *seller = find_user(sell_curr->username);

                // --- Update Seller ---
                struct PortfolioItem *sold_item = get_portfolio_item(seller, sell_curr->ticker);
                sold_item->quantity -= sell_curr->quantity;
                seller->cash_balance += trade_cost;
                seller->available_cash += trade_cost;
                
                // --- Update Buyer ---
                double buy_cost_escrowed = buy_curr->price * buy_curr->quantity;
                double refund = buy_cost_escrowed - trade_cost;
                
                buyer->cash_balance -= trade_cost;
                buyer->available_cash += refund; // Give back difference
                update_portfolio(buyer, buy_curr->ticker, buy_curr->quantity, sell_curr->price);
                
                // --- Remove Orders & Free Memory ---
                
                // 1. Unlink the 'buy' node from its list
                if (buy_prev == NULL) {
                    buy_order_head = buy_curr->next;
                } else {
                    buy_prev->next = buy_curr->next;
                }
                
                // 2. Unlink the 'sell' node from its list
                if (sell_prev == NULL) {
                    sell_order_head = sell_curr->next;
                } else {
                    sell_prev->next = sell_curr->next;
                }
                
                // 3. Free the memory
                free(buy_curr);
                free(sell_curr);
                
                printf(MAGENTA "--- TRADE COMPLETE ---\n" RESET);

                // 4. A trade happened, so the lists are changed.
                // We must restart the *entire* matching process.
                goto restart_matching; 

            } else {
                // No match, move to next sell order
                sell_prev = sell_curr;
                sell_curr = sell_curr->next;
            }
        } // End of sell-loop

        // No trade was found for this buy order, move to next buy order
        buy_prev = buy_curr;
        buy_curr = buy_curr->next;
        
    } // End of buy-loop
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
        live_infosys_price = live_infosys_price * (1.0 + change_percent);
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
    if (strcmp(ticker, "INFOSYS") == 0) return live_infosys_price;
    if (strcmp(ticker, "HDFC") == 0) return live_hdfc_price;
    if (strcmp(ticker, "SBI") == 0) return live_sbi_price;
    return 0.0;
}


void view_market_data() {
    printf(YELLOW "\n--- Live Market Prices ---\n" RESET);

        for (int i = 0; i < NUM_VALID_STOCKS; i++) {
        const char* ticker = VALID_STOCKS[i];
        double price = get_live_price((char*)ticker);

                printf("  %-10s: $%.2f\n", ticker, price);
    }

    printf(YELLOW "---------------------------\n" RESET);

}

/*
 * This is a recursive "helper" function
 * to print the buy-order list from
 * lowest price to highest price.
*/
void print_list_reverse(struct Order *head) {
    if (head == NULL) {
        return;
    }
    print_list_reverse(head->next);

    // Print after the recursive call
    printf("  %-10s | %-12.2f | %-10d\n",
           head->username, head->price, head->quantity);
}


void view_order_book() {
    char ticker_input[10];

    printf(YELLOW "\n--- View Full Order Book ---\n" RESET);
    printf("Enter stock ticker (TATA, RELI, etc.): ");
    scanf("%s", ticker_input);

    if (is_ticker_valid(ticker_input) == 0) {
        printf(RED "Error: '%s' is not a valid stock.\n" RESET, ticker_input);
        return;
    }

    printf(CYAN "\n--- Order Book for %s ---\n" RESET, ticker_input);

    // --- Print Sell Orders (Top Down) ---
    printf(RED "--- SELL ORDERS (ASKS) ---\n" RESET);
    printf("  User       | Price        | Quantity  \n");
    printf("  --------------------------------------\n");

    struct Order *temp = sell_order_head;
    int orders_found = 0;
    while (temp != NULL) {
        if (strcmp(temp->ticker, ticker_input) == 0) {
            printf("  %-10s | $%-11.2f | %-10d\n",
                   temp->username, temp->price, temp->quantity);
            orders_found = 1;
        }
        temp = temp->next;
    }
    if (orders_found == 0) {
        printf("  (No sell orders)\n");
    }

    // --- Print Buy Orders (Bottom Up) ---
    printf(GREEN "\n--- BUY ORDERS (BIDS) ---\n" RESET);
    printf("  User       | Price        | Quantity  \n");
    printf("  --------------------------------------\n");

    temp = buy_order_head;
    orders_found = 0;
    while (temp != NULL) {
        if (strcmp(temp->ticker, ticker_input) == 0) {
            // We will print the buy list "normally" for simplicity.
            // For a true "bottom up" print, we'd use the recursive helper.
            printf("  %-10s | $%-11.2f | %-10d\n",
                   temp->username, temp->price, temp->quantity);
            orders_found = 1;
        }
        temp = temp->next;
    }
    if (orders_found == 0) {
        printf("  (No buy orders)\n");
    }

    printf(YELLOW "---------------------------\n" RESET);
}



void save_order_books() {
    // --- 1. Save Buy Orders ---
    FILE *buy_file = fopen("buy_orders.txt", "w");
    if (buy_file == NULL) {
        printf(RED "Error: Could not save buy_orders.txt\n" RESET);
        return;
    }

    struct Order *temp = buy_order_head;
    while (temp != NULL) {
        
        fprintf(buy_file, "%s,%s,%.2f,%d\n",
                temp->username,
                temp->ticker,
                temp->price,
                temp->quantity);
        temp = temp->next;
    }
    fclose(buy_file);

    
    FILE *sell_file = fopen("sell_orders.txt", "w");
    if (sell_file == NULL) {
        printf(RED "Error: Could not save sell_orders.txt\n" RESET);
        return;
    }

    temp = sell_order_head;
    while (temp != NULL) {
    
        fprintf(sell_file, "%s,%s,%.2f,%d\n",
                temp->username,
                temp->ticker,
                temp->price,
                temp->quantity);
        temp = temp->next;
    }
    fclose(sell_file);
}