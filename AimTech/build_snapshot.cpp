//
// Created by Nikolay Suboch on 15/06/2021.
//

#include "build_snapshot.h"
#include "order_book.h"

#include <fstream>


void build_snapshot(const std::string& fn_input, const std::string& fn_snapshot, const std::string& fn_index) {
  const int MAX_STEPS = 2;
  int numSteps = 0;
  OrderBook orderBook;
  std::ifstream inFile(fn_input);
  std::ofstream outFile(fn_snapshot);
  std::ofstream indexFile(fn_index);

  while (!inFile.eof()) {
    int timestamp;
    char operation;
    OrderId orderId;
    Price price;
    int amount;
    inFile >> timestamp >> operation >> orderId;
    if (operation == 'R') {
      orderBook.Delete(orderId);
    } else {
      inFile >> price >> amount;
      if (operation == 'S') {
        orderBook.AddSell(orderId, price, amount);
      } else {
        orderBook.AddBuy(orderId, price, amount);
      }
    }

    ++numSteps;
    if (numSteps % MAX_STEPS == 0) {
      const auto inputPos = inFile.tellg();
      const auto outputPos = outFile.tellp();
      orderBook.SerializeTo(outFile);
      indexFile << timestamp << ' ' << inputPos << ' ' << outputPos << '\n';
    }
  }

  indexFile.close();
  outFile.close();
}
