#include "msg.pb.h"
#include <iostream>
int main() {
  Book book;
  book.set_name("CPP programing");
  book.set_pages(100);
  book.set_price(200);
  std::string bookstr;
  book.SerializeToString(&bookstr);
  std::cout << "serialize str is " << bookstr << std::endl;
  Book book2;
  book2.ParseFromString(bookstr);
  std::cout << "book2 name is " << book2.name() << " price is " << book2.price()
            << " pages is " << book2.pages() << std::endl;
  getchar();
}