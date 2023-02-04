#include <iostream>
#include "MariadbLazy.h"


int main()
{
  std::cout << "Hello Lazy!" << std::endl;

  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::INSERT]="student";
  lazyOrm["name"]="anya";
  lazyOrm["age"]= 6;
  lazyOrm["hair"]="pink";
  lazyOrm["cute"]=true;
  std::cout << lazyOrm.queryString() << std::endl;

  LazyOrm::MariadbLazy lazyOrm1;
  lazyOrm1[LazyOrm::INSERT]="student";
  lazyOrm1.setProperties({{"name","ahmad"},{"age",27},{"cute",false}});
  std::cout << lazyOrm1.queryString() << std::endl;

  LazyOrm::MariadbLazy lazyOrm2;
  lazyOrm2[LazyOrm::SELECT]="student";
  lazyOrm2.setProperties({"name","age","hair","*"});
  std::cout << lazyOrm2.queryString() << std::endl;

  LazyOrm::MariadbLazy lazyOrm3;
  lazyOrm3[LazyOrm::SELECT]="student";
  lazyOrm3<<"name"<<"age"<<"hair"<<"*";
  std::cout << lazyOrm3.queryString() << std::endl;

  LazyOrm::MariadbLazy lazyOrm4;
  lazyOrm4[LazyOrm::INSERT]="student";
  lazyOrm4<<LazyOrm::pair{"name","ahmad"};
  lazyOrm4<<LazyOrm::pair{"age",23}<<LazyOrm::pair{"hair","black"};
  lazyOrm4<<LazyOrm::pair{"cute",false};
  std::cout << lazyOrm4.queryString() << std::endl;


  LazyOrm::MariadbLazy lazyOrm200("student", LazyOrm::INSERT);
  lazyOrm200["first_name"]="ali";
  lazyOrm200["last_name"]= "yavari";
  lazyOrm200["gender"]="male";
  lazyOrm200["name_father"]="reza";
  std::cout << lazyOrm200.queryString() << std::endl;

  return 0;
}
