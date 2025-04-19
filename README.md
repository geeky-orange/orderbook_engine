# Orderbook Engine

A high-performance C++ implementation of a limit orderbook matching engine for financial exchanges. This implementation provides a foundation for building trading systems and cryptocurrency exchanges.

## Features

- **Price-Time Priority**: Orders are matched according to price-time priority (FIFO at each price level)
- **Order Types**: Supports GoodTillCancel (GTC) and FillAndKill (FAK) order types
- **Fast Matching Algorithm**: Efficiently matches orders with O(1) lookup by OrderId
- **Memory Efficiency**: Minimizes memory usage through smart pointers and optimized data structures
- **Clean API**: Simple interfaces for adding, canceling, and modifying orders
- **Thread-Safety**: Core functionality designed with concurrency in mind (synchronization to be added as needed)

## Technical Details

### Data Structure

- Bids are stored in a price-ordered map (highest first)
- Asks are stored in a price-ordered map (lowest first)
- Orders are indexed in a hash map for O(1) lookup by ID
- Lists of orders at each price level maintain time priority

### Implementation

The orderbook implementation consists of several key components:

1. **Order**: Represents an individual order with price, quantity, and other attributes
2. **Trade**: Records matching information when two orders are matched
3. **OrderModify**: Handles order modifications
4. **Orderbook**: The main engine that manages the order book and matching logic

## Build Instructions

### Requirements

- C++17 compatible compiler (g++ 7.0+ or equivalent)
- GNU Make

### Compilation

```bash
# Build everything (library, main executable, and test)
make

# Build in debug mode
make debug

# Build only the library
make lib

# Build only the main executable
make main

# Build only the test suite
make test
```

### Running

```bash
# Run the main interactive program
make run

# Run the test suite
make run-test
```

## Usage Example

```cpp
#include "orderbook.h"
#include "Order.h"
#include <memory>

// Create an orderbook
Orderbook orderbook;

// Add a buy order
auto buyOrder = std::make_shared<Order>(
    OrderType::GoodTillCancel,  // Order type
    1,                          // Order ID
    BuyOrSell::Buy,             // Side
    100.5,                      // Price
    10                          // Quantity
);
auto trades = orderbook.AddOrder(buyOrder);

// Add a matching sell order
auto sellOrder = std::make_shared<Order>(
    OrderType::GoodTillCancel,
    2,
    BuyOrSell::Sell,
    100.5,
    5
);
trades = orderbook.AddOrder(sellOrder);

// Process the trades
for (const auto& trade : trades) {
    // Process each trade...
    const auto& bidInfo = trade.GetBidTrade();
    const auto& askInfo = trade.GetAskTrade();
    
    // Trade matched bidInfo.orderid with askInfo.orderid
    // for quantity of bidInfo.quantity at price bidInfo.price
}

// Cancel an order
orderbook.CancelOrder(1);

// Modify an order
OrderModify modOrder(2, BuyOrSell::Sell, 101.0, 7);
trades = orderbook.MatchOrder(modOrder);
```

## API Documentation

### Order Class

```cpp
// Create an order
Order(OrderType ordertype, OrderId orderid, BuyOrSell buyorsell, Price price, Quantity quantity);

// Get order details
OrderId GetOrderId() const;
BuyOrSell GetBuyOrSell() const;
Price GetPrice() const;
OrderType GetOrderType() const;
Quantity GetInitalQuantity() const;
Quantity GetRemainingQuantity() const;
Quantity GetFilledQuantity() const;
bool IsFilled() const;

// Fill an order with a given quantity
void Fill(Quantity quantity);
```

### Orderbook Class

```cpp
// Add a new order to the book
Trades AddOrder(OrderPointer order);

// Cancel an existing order
void CancelOrder(OrderId orderid);

// Modify an existing order
Trades MatchOrder(OrderModify order);

// Get the number of active orders
size_t Size() const;

// Clear all orders
void ClearAll();

// Find an order by ID
OrderPointer FindOrder(OrderId orderid) const;
```

## Interactive Program

The main program provides an interactive shell for testing the orderbook:

```
Available commands:
  buy <price> <quantity> [FAK]     - Place a buy order
  sell <price> <quantity> [FAK]    - Place a sell order
  cancel <orderid>                 - Cancel an order
  modify <orderid> <price> <quantity> - Modify an order
  clear                            - Clear all orders
  quit/exit                        - Exit the program
```

## Testing

The `orderbook_test` program provides comprehensive tests of all orderbook functionality:

- Order class testing
- OrderModify class testing
- Basic orderbook functionality (add, cancel, modify)
- Order matching with various scenarios

## Performance Considerations

- The orderbook is optimized for fast matching and lookups
- For high-frequency trading applications, consider:
  - Using a memory pool allocator for orders
  - Implementing lock-free data structures
  - Adding custom STL allocators

## License

This project is open source and available under the MIT License.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.