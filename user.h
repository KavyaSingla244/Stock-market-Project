#ifndef USER_H
#define USER_H

struct PortfolioItem{
    char ticker[30];
    int quantity;
};

struct User {
    char username[50];
    char password[50];
    double cash_balance;

    struct PortfolioItem portfolio[30];
    int stocks_owned;
    double available_cash;
};

void signup_user();
void login_user();
void trading_menu(struct User *currentUser);
void view_portfolio(struct User *currentUser);

#endif