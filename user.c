#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> // Required for boolean type in secure_input logic
#include "user.h"
#include "colors.h"
#include "orderbook.h"
#include "secure_input.h" // New header for secure password functions


struct User all_users[100];
int number_of_users_registered=0;

// Helper function to consume trailing newline/input from previous scanf/input
static void consume_input() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void signup_user(){
    if (number_of_users_registered>=100) {
        printf(RED "Error: User database is full.\n" RESET);
        return ;
    }

    printf(YELLOW "\n--- New User Registration ---\n" RESET);
    
    // 1. Get Username (using friend's original input method)
    printf("Enter the new username: ");
    scanf("%s",all_users[number_of_users_registered].username);
    consume_input(); // Clear input buffer

    // --- SECURE PASSWORD INPUT START ---
    char password_buffer[MAX_PASS_LENGTH];
    char confirm_buffer[MAX_PASS_LENGTH];
    bool password_valid = false;
    
    // Loop until password meets complexity and confirmation requirements
    while (!password_valid) {
        printf("\nPassword Requirements:\n");
        printf(" - Min 8 characters\n");
        printf(" - 1 Upper Case, 1 Lower Case, 1 Digit, 1 Special Character\n");

        // 2. Get Password (with masking)
        printf("Enter new password: ");
        readPassword(password_buffer, MAX_PASS_LENGTH);
        
        // 3. Confirm Password (with masking)
        printf("Confirm password: ");
        readPassword(confirm_buffer, MAX_PASS_LENGTH);

        // 4. Validate complexity
        if (!validatePassword(password_buffer)) {
            printf(RED "\n[ERROR] Password failed complexity check. Please try again.\n" RESET);
            continue; // Go back to start of while loop
        }

        // 5. Check match
        if (strcmp(password_buffer, confirm_buffer) != 0) {
            printf(RED "\n[ERROR] Passwords do not match. Please try again.\n" RESET);
            continue; // Go back to start of while loop
        }
        
        // If all checks pass
        password_valid = true;
    }

    // Copy the valid, confirmed password into the user struct
    strcpy(all_users[number_of_users_registered].password, password_buffer);
    // --- SECURE PASSWORD INPUT END ---


    double starting_cash=0.0;
    const double MIN_DEPOSIT=100.00;
    const double MAX_DEPOSIT=100000.00;
    while(1) {
        printf("Enter your starting cash: $");
        // Using scanf for double as originally done by friend
        if (scanf("%lf", &starting_cash) != 1) {
            printf(RED "Invalid input. Please enter a number.\n" RESET);
            consume_input(); // to clear the garbage from input buffer
            continue;
        }
        consume_input();

        if (starting_cash < MIN_DEPOSIT) {
            printf(RED "Deposit is too low. Minimum is $%.2f\n" RESET ,MIN_DEPOSIT);
        
        } else if (starting_cash > MAX_DEPOSIT) {

            printf(RED "Deposit is too high. Maximum is $%.2f\n" RESET ,MAX_DEPOSIT);
        } else {
            break;
        }
    }

    all_users[number_of_users_registered].cash_balance=starting_cash; // total money the user has in the system.
    all_users[number_of_users_registered].available_cash=starting_cash; // the money they can use to trade (same as starting cash initially).
    all_users[number_of_users_registered].stocks_owned=0;


    printf(GREEN "\nRegistration successful! Welcome,%s!\n" RESET,all_users[number_of_users_registered].username);
    printf(GREEN "You have deposited $%.2f in virtual cash.\n" RESET,all_users[number_of_users_registered].cash_balance);
    struct User *newUser= &all_users[number_of_users_registered]; // creates a pointer to the currently registered user so you can pass it to other functions easily
    number_of_users_registered ++;
    trading_menu(newUser); // This makes the user jump directly to the trading menu after registration.


}



void login_user(){
    char username_input[50];
    char password_input[50];

    while (1){
            printf(YELLOW "\n---- User Login ---\n" RESET);
            printf("Enter username: ");
            scanf("%s",username_input);
            consume_input(); // to clear the garbage for input buffer

            if (strcmp(username_input,"exit")==0){ // if user types exit in the username_input it returns to main menu
                printf("Returning to main menu....\n");
                return;
            }

            printf("Enter password: ");
            // Now using the secure readPassword function for login
            readPassword(password_input, MAX_PASS_LENGTH); 

            // If the user's password from the file is encrypted/hashed,
            // this is where you would encrypt/hash password_input before comparison.
            
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
                            fluctuate_prices(); /* A function call that is executed before the menu is shown.
                            Used to update the simulated market prices, so that user sees the latest data. */
                            printf(CYAN "\n---  %s's Trading Menu ---\n" RESET,currentUser->username);
                            printf("Your cash: $%.2f\n", currentUser->cash_balance);
                            printf("-------------------------\n");
                            printf("1. View Portfolio\n");
                            printf("2. Place Buy Order\n");
                            printf("3. Place Sell Order\n");
                            printf("4. View Live Market Data\n");
                            printf("5. View Orderbook\n");
                            printf("6. View Transaction History\n");
                            printf("7. Settings\n");
                            printf("8. Logout\n");

                            printf("Enter your choice: ");
                            // Cleaned up original input method
                            if (scanf("%d",&choice) != 1) {
                                choice = -1; // Set to invalid choice on scan failure
                                consume_input();
                            } else {
                                consume_input();
                            }

                            switch(choice) {
                                case 1:
                                view_portfolio(currentUser);
                                break;
                                case 2:
                                place_buy_order(currentUser);
                                match_trades();
                                break;
                                case 3:
                                place_sell_order(currentUser);
                                match_trades();
                                break;
                                case 4:
                                view_market_data();
                                break;
                                case 5:
                                view_order_book();
                                break;
                                case 6:
                                view_transaction_history(currentUser);
                                break;
                                case 7:
                                if (settings_menu(currentUser)==1){
                                    return;
                                }
                                break;
                                case 8:
                                printf(GREEN "Logging out....\n" RESET);
                                return;
                                default:
                                printf(RED "Invalid choice.Please try again.\n" RESET);
                                // The input cleaning is now handled above.
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
        printf("%-10s | %-8s | %-12s | %-12s | %-12s\n", /*
            -10s means Left align in 10 spaces basically "-" means left allignment
            and "s" here means that a text is to be printed*/
               "Ticker", "Shares", "Avg. Cost", "Live Price", "Profit/Loss");
        printf("----------------------------------------------------------------------\n");
        /*
----------------------------------------------------------------------
Ticker     | Shares   | Avg. Cost    | Live Price   | Profit/Loss  
----------------------------------------------------------------------
*/
        double total_profit_loss = 0.0;
        
        for (int i = 0; i < currentUser->stocks_owned; i++) {
            char* ticker = currentUser->portfolio[i].ticker;
            int quantity = currentUser->portfolio[i].quantity;
            double avg_cost = currentUser->portfolio[i].avg_cost;
            double live_price = get_live_price(ticker); // gives current stock price
            
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

/* * Deletes the current user's portfolio file and removes them from the array.
 * Returns 1 if deleted (so we can logout), 0 if cancelled.
 */
int delete_account(struct User *currentUser) {
    char confirm;
    printf(RED "\n!!! WARNING: DELETING ACCOUNT !!!\n" RESET);
    printf("This will permanently delete your funds and portfolio.\n");
    printf("Are you sure you want to proceed? (y/n): ");
    
    // Clear input buffer to avoid skipping the scanf
    while(getchar() != '\n'); 
    scanf("%c", &confirm);

    if (confirm != 'y' && confirm != 'Y') {
        printf("Deletion cancelled.\n");
        return 0;
    }

    // 1. Delete the portfolio text file
    char filename[100];
    sprintf(filename, "%s_portfolio.txt", currentUser->username);
    if (remove(filename) == 0) {
        printf("Portfolio file deleted.\n");
    }

    // 2. Find the index of the current user in the array
    int index = -1;
    for (int i = 0; i < number_of_users_registered; i++) {
        if (strcmp(all_users[i].username, currentUser->username) == 0) {
            index = i;
            break;
        }
    }

    // 3. Remove user from array by shifting everyone else left
    if (index != -1) {
        for (int i = index; i < number_of_users_registered - 1; i++) {
            all_users[i] = all_users[i+1];
        }
        number_of_users_registered--; // Decrease total count
    }

    printf(RED "Account deleted successfully.\n" RESET);
    return 1; // Signal that deletion happened
}

void view_transaction_history(struct User *currentUser) {
    char filename[100];
    sprintf(filename, "%s_history.txt", currentUser->username);
    
    FILE *file = fopen(filename, "r");
    
    printf(YELLOW "\n--- Transaction History for %s ---\n" RESET, currentUser->username);
    
    if (file == NULL) {
        printf("No history found.\n");
    } else {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            printf("%s", line);
        }
        fclose(file);
    }
    printf(YELLOW "-----------------------------------\n" RESET);
}

/* * The Settings Menu. Returns 1 if account was deleted, 0 otherwise.
 */
int settings_menu(struct User *currentUser) {
    int choice = 0;
    while (1) {
        printf(CYAN "\n--- Settings ---\n" RESET);
        printf("1. Delete Account\n");
        printf("2. Back to Trading Menu\n");
        printf("----------------\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            // If delete returns 1, we must return 1 to force a logout
            if (delete_account(currentUser)) {
                return 1; 
            }
        } 
        else if (choice == 2) {
            return 0; // Go back
        } 
        else {
            printf(RED "Invalid choice.\n" RESET);
        }
    }
}


void load_all_data() {
    FILE *user_file = fopen("users.txt", "r");
    if (user_file == NULL) {
        printf("No user database found. Starting fresh.\n");
        return;
    }

    char line_buffer[256]; // A buffer to read one line at a time

    // --- Read users.txt ---
    while (fgets(line_buffer, sizeof(line_buffer), user_file) != NULL) { // fgets() reads one line from the file into line_buffer.
         // The loop continues until there are no more lines
        if (number_of_users_registered >= 100) break;
        
        char username[50];
        char password[50];
        double cash;

        // sscanf parses the line buffer. This is much safer than fscanf.
        if (sscanf(line_buffer, "%[^,],%[^,],%lf", username, password, &cash) == 3) {
            /*	sscanf() = “string scanf” — it reads from a string, not directly from input.
		The format "%[^,],%[^,],%lf" means:
	    %[^,] → read all characters until a comma (for username)
		%[^,] → read until next comma (for password)
    	%lf → read a floating-point number (for cash)  */
            struct User *currentUser = &all_users[number_of_users_registered];
            
            strcpy(currentUser->username, username);
            strcpy(currentUser->password, password);
            currentUser->cash_balance = cash;
            currentUser->available_cash = cash;
            currentUser->stocks_owned = 0;

            // --- Read the corresponding portfolio file ---
            char portfolio_filename[100];
            sprintf(portfolio_filename, "%s_portfolio.txt", username);
            // Each user has their own file for stock data.
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
                    if (sscanf(portfolio_line_buffer, "%[^,],%d", ticker, &quantity) == 2) { // == 2 is checking that the line was valid.
                        strcpy(currentUser->portfolio[stock_index].ticker, ticker); // it is a string inside the struct.
                        // strcpy copies the ticker name into that struct field.
                        currentUser->portfolio[stock_index].quantity = quantity;
                        currentUser->portfolio[stock_index].avg_cost = get_live_price(ticker);
                         currentUser->portfolio[stock_index].available_quantity = quantity;
                        stock_index++;
                }
                currentUser->stocks_owned = stock_index;
                fclose(portfolio_file);
            }
            number_of_users_registered++;
        }
    }
    }
    
    fclose(user_file);
    

    /* it reads each saved order, Validates the user,Checks money, Reconstructs an Order struct,
     Adds it into your order list,Deducts (escrows) the money again */
    FILE *buy_file = fopen("buy_orders.txt", "r");
    if (buy_file != NULL) { // If the file doesn’t exist, buy_file = NULL
        char buy_line[256];
        while (fgets(buy_line, sizeof(buy_line), buy_file) != NULL) { // Reads one line at a time into buy_line
            
            char username[50], ticker[10];
            double price;
            int quantity;

            if (sscanf(buy_line, "%[^,],%[^,],%lf,%d", username, ticker, &price, &quantity) == 4) {
                // Find the user who placed this order
                struct User* order_user = find_user(username);
                if (order_user == NULL) continue; // Skip if user doesn't exist

                double total_cost = price * quantity;
                if (total_cost > order_user->available_cash) continue; // Skip invalid order
                // If user doesn’t have enough cash then ignore this saved order
                
                // Creating the order
                struct Order *newOrder = (struct Order*) malloc(sizeof(struct Order));
                strcpy(newOrder->username, username);
                strcpy(newOrder->ticker, ticker);
                newOrder->type = 'B'; // B for Buy
                newOrder->quantity = quantity;
                newOrder->price = price;
                
                // Add to the list
                add_order_to_list(newOrder); 
                
                // Escrow their cash
                order_user->available_cash -= total_cost; // Available cash for buying new stocks is reduced but cash balance is reduced only when trade is executed.
            }
        }
        fclose(buy_file);
    }
    
    // --- Load Sell Orders ---
    FILE *sell_file = fopen("sell_orders.txt", "r");
    if (sell_file != NULL) {
        char sell_line[256];
        while (fgets(sell_line, sizeof(sell_line), sell_file) != NULL) {
            
            char username[50], ticker[10];
            double price;
            int quantity;

            if (sscanf(sell_line, "%[^,],%[^,],%lf,%d", username, ticker, &price, &quantity) == 4) { // sell_line temporarily holds the text of one order
                // (We'd add sell-side escrow here later)
                
                struct Order *newOrder = (struct Order*) malloc(sizeof(struct Order));
                strcpy(newOrder->username, username);
                strcpy(newOrder->ticker, ticker);
                newOrder->type = 'S'; // S for Sell
                newOrder->quantity = quantity;
                newOrder->price = price;
                
                add_order_to_list(newOrder);
            }
        }
        fclose(sell_file);
    }
    

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
            struct User *currentUser = &all_users[i]; // gives the address of the matched user struct.

                        fprintf(user_file, "%s,%s,%.2f\n", // fprintf() works like printf(), but writes to a file instead of the console.
                            currentUser->username,
                            currentUser->password, // NOTE: Password is saved in plaintext. In a real app, this should be a hash.
                            currentUser->cash_balance); 
                            // harit,MyPass123,50000.00
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
        save_order_books();
        printf(GREEN "All data saved successfully.\n" RESET);
    }