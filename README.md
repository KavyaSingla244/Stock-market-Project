# Stock Market Simulator (in C)

A high-performance, persistent stock market simulation engine built in C. This project simulates a realistic "Order Book" exchange where trades are executed based on supply and demand matching, rather than a simple fixed-price shop model.

It features a hybrid system combining a live price ticker for market analysis with a strict order-matching engine for trade execution.

---

## Key Features

### 1. The Core Trading Engine
* **Order Book Model:** Uses linked lists to manage a live book of Buy (Bid) and Sell (Ask) orders.
* **Matching Algorithm:** Trades only occur when a buyer's bid meets or exceeds a seller's ask price.
* **Escrow System:** Implements "Sell-Side Stock Escrow" and "Buy-Side Cash Escrow" to prevent users from spending money or selling stocks they don't have.

### 2. Financial Analytics
* **Live Price Ticker:** A background engine that simulates realistic price fluctuations with volatility and bias (Bullish/Bearish trends).
* **Real-Time P/L:** Users can view their portfolio to see live Profit/Loss calculations based on their weighted average cost.
* **Visual Charts:** Integrated with **GNUplot** to generate real-time pop-up price charts from within the terminal.

### 3. Persistence & System Design
* **Full Persistence:** All users, portfolios, cash, and *pending order books* are saved to text files on exit and reloaded on startup.
* **Professional UI:** Features a color-coded "Dashboard" interface with a clean layout and clear screen logic.
* **Memory Safe:** rigorously tested with Valgrind to ensure zero memory leaks.

---

## Technical Stack
* **Language:** C (Standard C99)
* **Build System:** GNU Make
* **Version Control:** Git & GitHub (with Branch Protection & PR workflow)
* **Tools Used:**
    * `Gnuplot` (for graphing)
    * `Valgrind` (for memory verification)

---

## How to Run

### Prerequisites
You need `gcc`, `make`, and `gnuplot` installed.
```bash
sudo apt update
sudo apt install build-essential gnuplot

## How to Compile and Run

This project is built using 'make'.

1. **Clone the repository:**
   git clone https://github.com/KavyaSingla244/Stock-market-Project.git
     cd stock-market-Project

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

---


