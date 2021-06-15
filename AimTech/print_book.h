//
// Created by Nikolay Suboch on 15/06/2021.
//

#ifndef PROBAH_PRINT_BOOK_H
#define PROBAH_PRINT_BOOK_H

#include <string>

using Timestamp = int;

void print_book(const std::string& fn_input,
                const std::string& fn_snapshot,
                const std::string& fn_index,
                const Timestamp timestamp);

#endif //PROBAH_PRINT_BOOK_H
