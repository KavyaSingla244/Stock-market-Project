#ifndef USER_H
#define USER_H

struct PortfolioItem{
    char ticker[30];
    int quantity;
    double avg_cost;
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
void load_all_data();
struct User* find_user(char *username);
void save_all_data();

#endif