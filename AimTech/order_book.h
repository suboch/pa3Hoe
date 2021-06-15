//
// Created by Nikolay Suboch on 15/06/2021.
//

#ifndef PROBAH_ORDER_BOOK_H
#define PROBAH_ORDER_BOOK_H

#include <istream>
#include <list>
#include <map>
#include <ostream>
#include <unordered_map>

using OrderId = int;
using Price = int;

class Orders {
public:
  // -> сколько ещё осталось исполнить, 0 <= result <= amount
  int Execute(int amount);

  // добавить в конец очереди заявок
  void Add(OrderId orderId, int amount) {
    auto it = items_.insert(items_.end(), {orderId, amount});
    index_[orderId] = it;
  }

  // удалить заявку из очереди
  void Delete(OrderId orderId) {
    const auto it = index_.find(orderId);
    if (it != index_.end()) {
      items_.erase(it->second);
      index_.erase(it);
    }
  }

  void SerializeTo(std::ostream& ostream) const;
  void DeserializeFrom(std::istream& istream, int numOrders);

  size_t Size() const {
    return items_.size();
  }

  bool IsEmpty() const {
    return items_.empty();
  }


private:
  struct Item {
    OrderId orderId_;
    int amount_;
  };

  using LinkedList = std::list<Item>;

  LinkedList items_;
  std::unordered_map<OrderId, LinkedList::iterator> index_;
};


class OrderBook {
public:
  void AddBuy(OrderId orderId, Price price, int amount);
  void AddSell(OrderId orderId, Price price, int amount);
  void Delete(OrderId orderId);

  void SerializeTo(std::ostream& ostream) const;
  void DeserializeFrom(std::istream& istream);

private:
  using Book = std::map<Price, Orders>;
  using Index = std::unordered_map<OrderId, Book::iterator>;

  Book::iterator GetLowestSell();
  Book::iterator GetHighestBuy();

private:
  Book buys_;
  Book sells_;
  Index indexBuys_;
  Index indexSells_;
};

#endif //PROBAH_ORDER_BOOK_H
