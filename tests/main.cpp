#include <iostream>
#include "MariadbLazy.h"


int main()
{
  std::cout << "Hello Lazy!" << std::endl;

  LazyOrm::MariadbLazy lazyOrm("student", LazyOrm::INSERT);
  lazyOrm["name"]="ania";
  lazyOrm["age"]= 6;
  lazyOrm["hair"]="pink";
  lazyOrm["cute"]=true;
  std::cout << lazyOrm.queryString() << std::endl;


  LazyOrm::MariadbLazy lazyOrm2("student", LazyOrm::INSERT);
  lazyOrm2["first_name"]="ali";
  lazyOrm2["last_name"]= "yavari";
  lazyOrm2["gender"]="male";
  lazyOrm2["name_father"]="reza";
  std::cout << lazyOrm2.queryString() << std::endl;

  return 0;
}
