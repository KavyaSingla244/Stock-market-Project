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

    all_users[number_of_users_registered].cash_balance=starting_cash;
    all_users[number_of_users_registered].available_cash=starting_cash;
    all_users[number_of_users_registered].stocks_owned=0;


    printf(GREEN "\nRegistration successfu!Welcome,%s!\n" RESET,all_users[number_of_users_registered].username);
    printf(GREEN "You have deposited $%.2f in virtual cash.\n" RESET,all_users[number_of_users_registered].cash_balance);
    number_of_users_registered ++;

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
                            printf(CYAN "\n---  %s's Trading Menu ---\n" RESET,currentUser->username);
                            printf("Your cash: $%.2f\n", currentUser->cash_balance);
                            printf("-------------------------\n");
                            printf("1. View Portfolio\n");
                            printf("2. Place Buy Order\n");
                            printf("3. Place Sell Order\n");
                            printf("4. Logout\n");

                            printf("Enter your choice: ");
                            scanf("%d",&choice);

                            switch(choice) {
                                case 1:
                                view_portfolio(currentUser);
                                break;
                                case 2:
                                place_buy_order(currentUser);
                                case 3:
                                break;
                                case 4:
                                printf(GREEN "Logging out....\n" RESET);
                                return;
                                default:
                                printf(RED "Invalid choice.Please try again.\n" RESET);
                                while (getchar()!='\n');
                            }

                        }
                    }

void view_portfolio(struct User *currentUser) {
     printf(CYAN "\n---  %s's Portfolio ---\n" RESET,currentUser->username);
    printf("Your cash: $%.2f\n", currentUser->cash_balance);
    printf("Available cash: $%.2f\n", currentUser->available_cash);

    if (currentUser->stocks_owned==0){
        printf("You do not own any stocks yet\n");
    }else {
        printf("\nYour stocks: \n");
        for (int i=0;i<currentUser->stocks_owned;i++){
            printf(" %s : %d shares\n",currentUser->portfolio[i].ticket,currentUser->portfolio[i].quantity);
        }
    }
    printf(YELLOW "--------------------\n" RESET);

}
