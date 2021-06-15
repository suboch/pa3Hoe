#include "order_book.h"

#include <iostream> ///

int Orders::Execute(int amount) {
  while (!items_.empty()) {
    Item &item = *items_.begin();
    if (amount < item.amount_) {
      item.amount_ -= amount;
      return 0;
    }
    amount -= item.amount_;
    index_.erase(item.orderId_);
    items_.erase(items_.begin());
  }
  return amount;
}

void Orders::SerializeTo(std::ostream& os) const {
  for (const Item& node : items_) {
    os << node.orderId_ << ' ' << node.amount_ << '\t';
  }
}

void Orders::DeserializeFrom(std::istream& istream, int numOrders) {
  items_.clear();
  while (numOrders --> 0) {
    Item item;
    istream >> item.orderId_ >> item.amount_;
    items_.push_back(item);
  }

  index_.clear();
  for (auto it = items_.begin(); it != items_.end(); ++it) {
    index_[it->orderId_] = it;
  }
}

// --------------------------------------------------

void OrderBook::AddBuy(OrderId orderId, Price price, int amount) {
  Book::iterator it = GetLowestSell();
  while (it != sells_.end()) {
    const Price lowestPrice = it->first;
    if (price < lowestPrice) {
      break;
    }
    Orders& orders = it->second;
    const int remAmount = orders.Execute(amount);
    if (orders.IsEmpty()) {
      sells_.erase(it);
    }
    if (remAmount == 0) {
      return;
    } else {
      amount = remAmount;
      it = GetLowestSell();
    }
  }
  auto [newIt, _] = buys_.insert({price, Orders()});
  newIt->second.Add(orderId, amount);
  indexBuys_[orderId] = newIt;
}

void OrderBook::AddSell(OrderId orderId, Price price, int amount) {
  Book::iterator it = GetHighestBuy();
  while (it != buys_.end()) {
    const Price highestPrice = it->first;
    if (price > highestPrice) {
      break;
    }
    Orders& orders = it->second;
    const int remAmount = orders.Execute(amount);
    if (orders.IsEmpty()) {
      buys_.erase(it);
    }
    if (remAmount == 0) {
      return;
    } else {
      amount = remAmount;
      it = GetHighestBuy();
    }
  }
  auto [newIt, _] = sells_.insert({price, Orders()});
  newIt->second.Add(orderId, amount);
  indexSells_[orderId] = newIt;
}

void OrderBook::Delete(OrderId orderId) {
  auto deleter = [](Book& book, auto& index, OrderId id) {
    const auto it = index.find(id);
    if (it != index.end()) {
      Book::iterator bookIt = it->second;
      Orders& orders = bookIt->second;
      orders.Delete(id);
      book.erase(bookIt);
      index.erase(it);
    }
  };

  deleter(buys_, indexBuys_, orderId);
  deleter(sells_, indexSells_, orderId);
}

OrderBook::Book::iterator OrderBook::GetLowestSell() {
  return sells_.begin();
}

OrderBook::Book::iterator OrderBook::GetHighestBuy() {
  auto it = buys_.end();
  if (!buys_.empty()) {
    --it;
  }
  return it;
}

void OrderBook::SerializeTo(std::ostream& os) const {
  os << "B\t" << buys_.size() << '\n';
  for (const auto& [price, orders] : buys_) {
    os << price << '\t' << orders.Size() << '\n';
    orders.SerializeTo(os);
    os << '\n';
  }
  os << "S\t" << sells_.size() << '\n';
  for (const auto& [price, orders] : sells_) {
    os << price << '\t' << orders.Size() << '\n';
    orders.SerializeTo(os);
    os << '\n';
  }
  os << "-----\n";
}

void OrderBook::DeserializeFrom(std::istream& istream) {
  buys_.clear();

  char operation;
  istream >> operation;
  assert(operation == 'B');
  int numBuys;
  istream >> numBuys;
  while (numBuys --> 0) {
    Price price;
    int numOrders;
    istream >> price >> numOrders;
    Orders orders;
    orders.DeserializeFrom(istream, numOrders);

    // TODO: Build index
  }

  // TODO: The same for sells
}
