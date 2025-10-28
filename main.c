#include <stdio.h> 
#include "colors.h"
#include "user.h"
#include "orderbook.h"

int main() {
    int choice =0;
    load_all_data();

    while(1){
    
        printf("\n--- Stock Market Simulator ---\n");
        printf("1. Register New User\n");
        printf("2. Login\n");
        printf("3. Quit\n");
        printf("---------------------------------");
        printf("Enter your choice: ");

        scanf("%d",&choice);

        switch (choice){
            case 1:
            signup_user();
            break;
            case 2:
            login_user();
            break;

            case 3:


            printf("Exiting.Goodbye!\n");
            return 0;

            default:
            printf("Invalid choice.Please try again.\n");

            while (getchar()!='\n');

        }


    }
}