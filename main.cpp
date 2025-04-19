#include <iostream>
#include <memory>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>

#include "Order.h"
#include "OrderModify.h"
#include "Trade.h"
#include "Orderbook.h"

using namespace std;

// Helper to print a line of dashes
void printDivider() {
    cout << "---------------------------------------------" << endl;
}

// Helper to print the current state of the orderbook
void printOrderBookSummary(const Orderbook& orderbook) {
    cout << "Current orderbook size: " << orderbook.Size() << " orders" << endl;
}

// Helper to print trade details
void printTrade(const Trade& trade) {
    const auto& bidTrade = trade.GetBidTrade();
    const auto& askTrade = trade.GetAskTrade();
    
    cout << "TRADE EXECUTED: " << endl;
    cout << "  Bid Order ID: " << bidTrade.orderid 
         << ", Price: " << fixed << setprecision(2) << bidTrade.price
         << ", Quantity: " << bidTrade.quantity << endl;
    cout << "  Ask Order ID: " << askTrade.orderid 
         << ", Price: " << fixed << setprecision(2) << askTrade.price
         << ", Quantity: " << askTrade.quantity << endl;
}

// Process a simple CLI command
bool processCommand(const string& cmd, Orderbook& orderbook) {
    static OrderId nextOrderId = 1;
    stringstream ss(cmd);
    string action;
    ss >> action;
    
    if (action == "quit" || action == "exit") {
        return false;
    } 
    else if (action == "help") {
        cout << "Available commands:" << endl;
        cout << "  buy <price> <quantity> [FAK]     - Place a buy order" << endl;
        cout << "  sell <price> <quantity> [FAK]    - Place a sell order" << endl;
        cout << "  cancel <orderid>                 - Cancel an order" << endl;
        cout << "  modify <orderid> <price> <quantity> - Modify an order" << endl;
        cout << "  clear                           - Clear all orders" << endl;
        cout << "  quit/exit                       - Exit the program" << endl;
    } 
    else if (action == "buy" || action == "sell") {
        Price price;
        Quantity quantity;
        string orderTypeStr;
        BuyOrSell side = (action == "buy") ? BuyOrSell::Buy : BuyOrSell::Sell;
        OrderType orderType = OrderType::GoodTillCancel;
        
        if (!(ss >> price >> quantity)) {
            cout << "Error: Invalid price or quantity" << endl;
            return true;
        }
        
        // Check for optional FAK parameter
        ss >> orderTypeStr;
        if (orderTypeStr == "FAK") {
            orderType = OrderType::FillAndKill;
        }
        
        // Create and add the order
        OrderId orderId = nextOrderId++;
        cout << "Creating " << (side == BuyOrSell::Buy ? "Buy" : "Sell") 
             << " order ID: " << orderId 
             << ", Price: " << price 
             << ", Quantity: " << quantity 
             << ", Type: " << (orderType == OrderType::GoodTillCancel ? "GTC" : "FAK") << endl;
        
        auto order = make_shared<Order>(orderType, orderId, side, price, quantity);
        auto trades = orderbook.AddOrder(order);
        
        if (!trades.empty()) {
            cout << "Generated " << trades.size() << " trade(s):" << endl;
            for (const auto& trade : trades) {
                printTrade(trade);
                printDivider();
            }
        }
    } 
    else if (action == "cancel") {
        OrderId orderId;
        if (!(ss >> orderId)) {
            cout << "Error: Invalid order ID" << endl;
            return true;
        }
        
        cout << "Canceling order ID: " << orderId << endl;
        orderbook.CancelOrder(orderId);
    } 
    else if (action == "modify") {
        OrderId orderId;
        Price price;
        Quantity quantity;
        
        if (!(ss >> orderId >> price >> quantity)) {
            cout << "Error: Invalid parameters for modify" << endl;
            return true;
        }
        
        // Assumes we're modifying an order with the same buy/sell side
        // In a real application, you might want to lookup the original order first
        cout << "Finding order ID: " << orderId << " to modify" << endl;
        
        auto it = orderbook.FindOrder(orderId);
        if (!it) {
            cout << "Error: Order ID " << orderId << " not found" << endl;
            return true;
        }
        
        auto side = it->GetBuyOrSell();
        OrderModify modOrder(orderId, side, price, quantity);
        
        cout << "Modifying order ID: " << orderId 
             << ", New Price: " << price 
             << ", New Quantity: " << quantity << endl;
        
        auto trades = orderbook.MatchOrder(modOrder);
        
        if (!trades.empty()) {
            cout << "Generated " << trades.size() << " trade(s):" << endl;
            for (const auto& trade : trades) {
                printTrade(trade);
                printDivider();
            }
        }
    } 
    else if (action == "clear") {
        cout << "Clearing all orders" << endl;
        orderbook.ClearAll();
    } 
    else {
        cout << "Unknown command. Type 'help' for available commands." << endl;
    }
    
    return true;
}

int main() {
    try {
        cout << "\n===== ORDERBOOK ENGINE =====\n" << endl;
        cout << "Welcome to the Orderbook Engine" << endl;
        cout << "Type 'help' for available commands or 'quit' to exit" << endl;
        printDivider();
        
        Orderbook orderbook;
        string command;
        
        while (true) {
            printOrderBookSummary(orderbook);
            cout << "\nEnter command: ";
            getline(cin, command);
            
            if (command.empty()) {
                continue;
            }
            
            printDivider();
            
            if (!processCommand(command, orderbook)) {
                break; // Exit the loop if command returns false
            }
            
            printDivider();
        }
        
        cout << "\nExiting Orderbook Engine. Goodbye!" << endl;
        return 0;
    }
    catch (const exception& e) {
        cerr << "ERROR: " << e.what() << endl;
        return 1;
    }
    catch (...) {
        cerr << "UNKNOWN ERROR OCCURRED" << endl;
        return 1;
    }
} 