# Stock-Market-Project


# C Stock Market Simulator 
This is a comprehensive multi-user stock market simulator built entirely in C.It emulates a real-world stock exchange by using persistent "Order Book" model.
This program features a hybrid system:
a "live price ticker" provides dynamic market data for analysis while a powerful "matching engine" executes trades only when buyer' bid meets a seller's ask. All user data,portfolios, and pending market orders are saved to '.txt' files,making the entire market persistent between sessions.

# Core Features

### User and Data Management 
* **Persistent Users:** Full user registration and login system.All user data is saved to 'users.txt'.
* **Persistent Portfolios:** Each User's cash balance and stock holdings are saved to their own '[username]_portfolio.txt' file.
* **Persistent Order Book:** The *entire* state of market( all pending buy orders and sell orders) is saved to 'buy_orders.txt' and 'sell_orders.txt' when the program quits and is reloaded on startp.

### Market and Trading Engine
* **Live Price Ticker:** A "live" price for each stock is simulated, which constantly fluctuates with a "biased" random tick.
* **Order Book Matching:** A true-to-life matching engine scans  the order book and executes trades when a buyer's price is greater than or equal to a seller's price.
* **Cash Escrow:** When user places a "buy" order, that cash is moved from "Available cash" to "escrow", preventing over-spending.
* **Place buy/sell Orders:** Users can place "limit orders" for any stock available in market.

### User Interface and Analysis
* **Color-coded UI:** Uses ANSI color codes for a clean,readable interface(e.g., profits are green,loses are red).
* **View Market Data:** A "dashboard" that shows the current "live price" of all available stocks,helping users decide what to trade.
* **View Order Book(Market Depth):** A screen that shows *all* pending "Buy Orders"(Bids) and "Sell Orders"(Asks) for a specific stock, just like a real trading platform.
* **Real-Time P/L:** The "View Portfolio" screen calculates and displays your real-time Profit/loss by comparing your 'avg_cost' (what you paid) to the current 'live_price'.

---

## Technical Concepts Implemented 

This project was built from scratch in C, focusing on robust,modular design and memory management.

* **Data Structures:** Struct's use for all core data(User,PortfolioItem,Order).
* **Dynamic Memory:** 'malloc()' and 'free()' are used to create and destroy 'struct Order' objects as they are placed and filled.
* **Linked Lists:** The entire buy and sell order book is managed using two separate singly-linked lists.
* **File I/O(Persistence):** 'fopen','fclose','fgets', and 'sscanf' are used to load and save all market and user data from/to '.txt' files.
* **Modular Programming:** The project is split into logical components('users.c','orderbook.c','main.c') with corresponding '.h' header files to manage complexity.
* **Build Management:** A 'Makefile' is used to compile the entire multi-file project.
* **Randomization:** 'rand()' and 'srand(time(NULL))' are used to power the live price fluctuation engine.

---

## How to Compile and Run

This project is buit using 'make'.

1. **Clone the respiratory:**
   git clone [https://github.com/KavyaSingla244/Stock-market-Project.git](https://github.com/KavyaSingla244/Stock-market-Project.git)
     cd Stock-market-Project

2. **Compile the program:**
    make

3. **Run the simulator:**
   ./stock_simulator

4. **Clean up compiled files:**
   make clean

---

## Authors

This project was built by:
* **KavyaSingla244**
* **Harit-Goyal-17**
* **Rishi Cherukuri**


