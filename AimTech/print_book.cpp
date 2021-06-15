//
// Created by Nikolay Suboch on 15/06/2021.
//

#include "print_book.h"
#include "order_book.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

struct Offset {
  Timestamp timestamp_;
  int input_;
  int snapshot_;
};

void print_book(const std::string& fn_input,
                const std::string& fn_snapshot,
                const std::string& fn_index,
                const Timestamp timestamp)
{
  std::vector<Offset> index;
  std::ifstream indexFile(fn_index);
  while (!indexFile.eof()) {
    Offset offset;
    indexFile >> offset.timestamp_ >> offset.input_ >> offset.snapshot_;
    index.emplace_back(std::move(offset));
  }

  auto it = std::upper_bound(index.begin(), index.end(), Offset{timestamp, 0, 0},
    [] (const Offset& lhs, const Offset& rhs) {
      return (lhs.timestamp_ <= rhs.timestamp_);
  });

  const Offset& offset = *it;
  std::ifstream inputFile(fn_input);
  inputFile.seekg(offset.input_);
  std::ifstream snapshotFile(fn_snapshot);
  snapshotFile.seekg(offset.snapshot_);

  OrderBook orderBook;
  orderBook.DeserializeFrom(snapshotFile);

}
