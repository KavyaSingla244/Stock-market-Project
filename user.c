#include <stdio.h>
#include <string.h>
#include "user.h"
#include "colors.h"
#include "orderbook.h"




struct User all_users[100];
int number_of_users_registered=0;
void signup_user(){
    if (number_of_users_registered>=100) {
        printf(RED "Error: User database is full.\n" RESET);
        return ;
    }


    printf(YELLOW "\n--- New User Registration ---\n" RESET);
    printf("Enter the new username: ");
    scanf("%s",all_users[number_of_users_registered].username);
    printf("Enter new password: ");
    scanf("%s",all_users[number_of_users_registered].password);

    double starting_cash=0.0;
    const double MIN_DEPOSIT=100.00;
    const double MAX_DEPOSIT=100000.00;
    while(1) {
        printf("Enter your starting cash: ");
        scanf("%lf", &starting_cash);

        if (starting_cash<MIN_DEPOSIT) {
            printf(RED "Deposit is too low. Minimum is $%.2f\n" RESET ,MIN_DEPOSIT);
        
        }else if (starting_cash>MAX_DEPOSIT) {

             printf(RED "Deposit is too high. Maximum is $%.2f\n" RESET ,MAX_DEPOSIT);
        }else {
            break;
        }
    }

    all_users[number_of_users_registered].cash_balance=starting_cash; //
    all_users[number_of_users_registered].available_cash=starting_cash; //
    all_users[number_of_users_registered].stocks_owned=0;


    printf(GREEN "\nRegistration successfu!Welcome,%s!\n" RESET,all_users[number_of_users_registered].username);
    printf(GREEN "You have deposited $%.2f in virtual cash.\n" RESET,all_users[number_of_users_registered].cash_balance);
    struct User *newUser= &all_users[number_of_users_registered];
    number_of_users_registered ++;
    trading_menu(newUser);

}

void login_user(){
    char username_input[50];
    char password_input[50];

    while (1){
            printf(YELLOW "\n---- User Login ---\n" RESET);
            printf("Enter username: ");
            scanf("%s",username_input);

            if (strcmp(username_input,"exit")==0){
                printf("Returning to main menu....\n");
                return;
            }

            printf("Enter password: ");
            scanf("%s", password_input);

            int user_found=0;

        int i=0;
        while (i<number_of_users_registered) {
            
            if (strcmp(username_input,all_users[i].username)==0){
                if (strcmp(password_input,all_users[i].password)==0) {
                    printf(GREEN "\nLogin successful! Welcome back, %s.\n" RESET,all_users[i].username);
                    user_found=1;
                    trading_menu(&all_users[i]);
                    return;

                    
                    
                }
            }
            i++;
        }
        if (user_found==0){
            printf(RED "\nERROR: INVALID USERNAME OR PASSWORD.\n" RESET);
        }

    }
}



void trading_menu(struct User *currentUser){
                        int choice=0;

                        while(1){
                            fluctuate_prices();
                            printf(CYAN "\n---  %s's Trading Menu ---\n" RESET,currentUser->username);
                            printf("Your cash: $%.2f\n", currentUser->cash_balance);
                            printf("-------------------------\n");
                            printf("1. View Portfolio\n");
                            printf("2. Place Buy Order\n");
                            printf("3. Place Sell Order\n");
                            printf("4. View Live Market Data\n");
                            printf("5. Logout\n");

                            printf("Enter your choice: ");
                            scanf("%d",&choice);

                            switch(choice) {
                                case 1:
                                view_portfolio(currentUser);
                                break;
                                case 2:
                                place_buy_order(currentUser);
                                break;
                                case 3:
                                place_sell_order(currentUser);
                                break;
                                case 4:
                                view_market_data();
                                break;
                                case 5:
                                printf(GREEN "Logging out....\n" RESET);
                                return;
                                default:
                                printf(RED "Invalid choice.Please try again.\n" RESET);
                                while (getchar()!='\n'); // ?
                            }

                        }
                    }

void view_portfolio(struct User *currentUser) {
    printf(YELLOW "\n--- %s's Portfolio ---\n" RESET, currentUser->username);
    printf("Total Cash:   $%.2f\n", currentUser->cash_balance);
    printf("Available Cash: $%.2f\n", currentUser->available_cash);
    
    if (currentUser->stocks_owned == 0) {
        printf("You do not own any stocks yet.\n");
    } else {
        printf("\nYour Stocks:\n");
        printf("----------------------------------------------------------------------\n");
        printf("%-10s | %-8s | %-12s | %-12s | %-12s\n",
               "Ticker", "Shares", "Avg. Cost", "Live Price", "Profit/Loss");
        printf("----------------------------------------------------------------------\n");

        double total_profit_loss = 0.0;
        
        for (int i = 0; i < currentUser->stocks_owned; i++) {
            char* ticker = currentUser->portfolio[i].ticker;
            int quantity = currentUser->portfolio[i].quantity;
            double avg_cost = currentUser->portfolio[i].avg_cost;
            double live_price = get_live_price(ticker); 
            
            double profit_loss = (live_price - avg_cost) * quantity;
            total_profit_loss += profit_loss;

            printf("%-10s | %-8d | $%11.2f | $%11.2f |",
                   ticker, quantity, avg_cost, live_price);
            
                        if (profit_loss >= 0) {
                printf(GREEN " $%11.2f\n" RESET, profit_loss);
            } else {
                
                printf(RED " -$%10.2f\n" RESET, -profit_loss);
            }
        }
        printf("----------------------------------------------------------------------\n");
        printf("Total Portfolio P/L: ");
        
        if (total_profit_loss >= 0) {
            printf(GREEN "$%.2f\n" RESET, total_profit_loss);
        } else {
            printf(RED "-$%.2f\n" RESET, -total_profit_loss);
        }
    }
    printf(YELLOW "----------------------------------------------------------------------\n" RESET);
}



void load_all_data() {
    FILE *user_file = fopen("users.txt", "r");
    if (user_file == NULL) {
        printf("No user database found. Starting fresh.\n");
        return;
    }

    char line_buffer[256]; // A buffer to read one line at a time

    // --- Read users.txt ---
    while (fgets(line_buffer, sizeof(line_buffer), user_file) != NULL) {
        if (number_of_users_registered >= 100) break;
        
        char username[50];
        char password[50];
        double cash;

        // sscanf parses the line buffer. This is much safer than fscanf.
        if (sscanf(line_buffer, "%[^,],%[^,],%lf", username, password, &cash) == 3) {
            
            struct User *currentUser = &all_users[number_of_users_registered];
            
            strcpy(currentUser->username, username);
            strcpy(currentUser->password, password);
            currentUser->cash_balance = cash;
            currentUser->available_cash = cash;
            currentUser->stocks_owned = 0;

            // --- Read the corresponding portfolio file ---
            char portfolio_filename[100];
            sprintf(portfolio_filename, "%s_portfolio.txt", username);
            
            FILE *portfolio_file = fopen(portfolio_filename, "r");
            if (portfolio_file != NULL) {
                char portfolio_line_buffer[100];
                int stock_index = 0;

                // Read the portfolio file line by line
                while (fgets(portfolio_line_buffer, sizeof(portfolio_line_buffer), portfolio_file) != NULL) {
                    if (stock_index >= 30) break;
                    
                    char ticker[30];
                    int quantity;

                    // Parse the portfolio line
                    if (sscanf(portfolio_line_buffer, "%[^,],%d", ticker, &quantity) == 2) {
                        strcpy(currentUser->portfolio[stock_index].ticker, ticker);
                        currentUser->portfolio[stock_index].quantity = quantity;
                        currentUser->portfolio[stock_index].avg_cost = get_live_price(ticker);
                        stock_index++;
                    }
                }
                currentUser->stocks_owned = stock_index;
                fclose(portfolio_file);
            }
            number_of_users_registered++;
        }
    }
    
    fclose(user_file);
}

struct User* find_user(char *username) {
    for (int i = 0; i < number_of_users_registered; i++) {
        if (strcmp(all_users[i].username, username) == 0) {
            
            return &all_users[i];
        }
    }
    return NULL;
}

void save_all_data() {
        
        FILE *user_file = fopen("users.txt", "w");
        if (user_file == NULL) {
            printf(RED "FATAL ERROR: Could not open users.txt for saving.\n" RESET);
            return;
        }

        
        for (int i = 0; i < number_of_users_registered; i++) {
            struct User *currentUser = &all_users[i];

                        fprintf(user_file, "%s,%s,%.2f\n",
                    currentUser->username,
                    currentUser->password,
                    currentUser->cash_balance); 

                        if (currentUser->stocks_owned > 0) {
                char portfolio_filename[100];
                sprintf(portfolio_filename, "%s_portfolio.txt", currentUser->username);

                FILE *portfolio_file = fopen(portfolio_filename, "w");
                if (portfolio_file == NULL) {
                    printf(RED "Error: Could not save portfolio for %s\n" RESET, currentUser->username);
                    continue; 
                }

                            for (int j = 0; j < currentUser->stocks_owned; j++) {
                    fprintf(portfolio_file, "%s,%d\n",
                            currentUser->portfolio[j].ticker,
                            currentUser->portfolio[j].quantity);
                }

                fclose(portfolio_file);
            }
        }

        fclose(user_file);
        printf(GREEN "All data saved successfully.\n" RESET);
    }


