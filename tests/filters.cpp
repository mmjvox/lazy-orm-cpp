#include <iostream>
#include "MariadbFilteringLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>
#include <vector>


std::string filter1() {
    LazyOrm::MariadbFilteringLazy filters;
    filters.setFilter(LazyOrm::GROUPBY , {"group1","group2"});
    filters.setFilter(LazyOrm::ORDERBY , {"num1","num2"});
    filters.setFilter(LazyOrm::LIMIT , {11,23});
    return filters.filter_conditions_with_trim_consecutive_spaces();
}

std::string filter2() {
    LazyOrm::MariadbFilteringLazy filters;
    filters.setFilter(LazyOrm::GROUPBY , {"group1","group2"});
    filters.setFilter(LazyOrm::ORDERBY , {"num1","num2"});
    filters.setFilter(LazyOrm::LIMIT , {11,23});
    return filters.filter_conditions_with_trim_consecutive_spaces();
}

std::string filter3() {
    LazyOrm::MariadbFilteringLazy filters;
    filters[LazyOrm::GROUPBY] = {{"group1","group2"}};
    filters[LazyOrm::ORDERBY] = {{"num1","num2"}};
    filters[LazyOrm::LIMIT] = "10,10";
    return filters.filter_conditions_with_trim_consecutive_spaces();
}

std::string filter4() {

//    LazyOrm::WhereFilter p1 = { LazyOrm::AND, {
//         std::vector<LazyOrm::DbVariant>{"AGE",12},
//         std::vector<LazyOrm::DbVariant>{"SALARY",65000},
//         std::vector<LazyOrm::DbVariant>{"hair","pink"},
//         std::vector<LazyOrm::DbVariant>{"height","between", "99,198"},
//         std::vector<LazyOrm::DbVariant>{"grade","in", "[1,5,7,9]"},
//      }
//    };

    LazyOrm::MariadbFilteringLazy filters;
//    filters.setWhereFilter(p1);
    filters.setFilter(LazyOrm::GROUPBY , {"group1","group2"});
    filters.setFilter(LazyOrm::ORDERBY , {"num1","num2"});
    filters.setFilter(LazyOrm::LIMIT , {"14",26});

return filters.filter_conditions_with_trim_consecutive_spaces();
}


std::string where5() {

//    LazyOrm::WhereFilter p1 = { LazyOrm::AND, {
//        std::vector<LazyOrm::WhereFilter>{
//          { LazyOrm::OR, {
//              std::vector<LazyOrm::DbVariant>{"name","like","sa%"},
//              std::vector<LazyOrm::DbVariant>{"name","like","%ra"},
//            }
//          }
//        },
//        std::vector<LazyOrm::WhereFilter>{
//          { LazyOrm::OR, {
//              std::vector<LazyOrm::DbVariant>{"age","between","6,13"},
//              std::vector<LazyOrm::DbVariant>{"grade","in","[17,18,19,20]"},
//            }
//          }
//        }
//      }
//    };

    LazyOrm::MariadbFilteringLazy filters;
//    filters.setWhereFilter(p1);
    filters.setFilter(LazyOrm::GROUPBY , {"group1","group2"});
    filters.setFilter(LazyOrm::ORDERBY , {"num1","num2"});
    filters.setFilter(LazyOrm::LIMIT , {"14",26});

return filters.filter_conditions_with_trim_consecutive_spaces();
}

std::string where6() {

//    LazyOrm::WhereFilter p1 = { LazyOrm::AND, {
//        std::vector<LazyOrm::WhereFilter>{
//          { LazyOrm::OR, {
//              std::vector<LazyOrm::DbVariant>{"name","like","sa%"},
//              std::vector<LazyOrm::DbVariant>{"name","like","%ra"},
//            }
//          }
//        },
//        std::vector<LazyOrm::WhereFilter>{
//          { LazyOrm::OR, {
//              std::vector<LazyOrm::DbVariant>{"age","between","6,13"},
//              std::vector<LazyOrm::DbVariant>{"grade","in","[17,18,19,20]"},
//            }
//          }
//        }
//      }
//    };

    LazyOrm::MariadbFilteringLazy filters;
//    filters.setFilter({p1});
    filters.setFilter(LazyOrm::GROUPBY , {"group1","group2"});
    filters.setFilter(LazyOrm::ORDERBY , {"num1","num2"});
    filters.setFilter(LazyOrm::LIMIT , {"14",26});

return filters.filter_conditions_with_trim_consecutive_spaces();
}

std::string where7() {

//    LazyOrm::WhereFilter whereFilter = {
//        LazyOrm::AND, {
//            std::vector<LazyOrm::WhereFilter>{
//                {
//                    LazyOrm::OR, {
//                        std::vector<LazyOrm::DbVariant>{"confirmed",false},
//                        std::vector<LazyOrm::DbVariant>{"defrayed",false},
//                        std::vector<LazyOrm::DbVariant>{"delivered",false},
//                    }
//                }
//            },
//            std::vector<LazyOrm::WhereFilter>{
//                {
//                    LazyOrm::AND, {
//                        std::vector<LazyOrm::DbVariant>{"[func]LENGTH(chassis)",">","[no']1"},
//                        std::vector<LazyOrm::DbVariant>{"[func]LENGTH(tracker_name)",">","[no']1"},
//                        std::vector<LazyOrm::DbVariant>{"[func]SUBSTRING(tracker_phones, 3, 1) REGEXP '^[0-9]+$'"},
//                    }
//                }
//            }
//        }
//    };

    LazyOrm::MariadbFilteringLazy filters;
//    filters.setFilter({whereFilter});
return filters.filter_conditions_with_trim_consecutive_spaces();
}

std::string where8() {

//LazyOrm::WhereFilter p0 = { LazyOrm::AND, {

//                      {
//                        LazyOrm::OR, std::vector<LazyOrm::DbVariant>{"name","like","sa%"}
//                      },
//                      { LazyOrm::OR, {
//                                        std::vector<LazyOrm::DbVariant>{"name","like","sa%"},
//                                        std::vector<LazyOrm::DbVariant>{"name","like","%ra"},
//                                        }
//                      },
//                      {
//                          { LazyOrm::OR, {
//                                            std::vector<LazyOrm::DbVariant>{"name","like","sa%"},
//                                            std::vector<LazyOrm::DbVariant>{"name","like","%ra"},
//                                            }
//                          }
//                      },
//                      {
//                          { LazyOrm::OR, {
//                                            std::vector<LazyOrm::DbVariant>{"age","between","6,13"},
//                                            std::vector<LazyOrm::DbVariant>{"grade","in","[17,18,19,20]"},
//                                            }
//                          },
//                          { LazyOrm::OR, {
//                                            std::vector<LazyOrm::DbVariant>{"age","between","6,13"},
//                                            std::vector<LazyOrm::DbVariant>{"grade","in","[17,18,19,20]"},
//                                            }
//                          }
//                      }
//                  }
//};

//    LazyOrm::WhereFilter p1 = { LazyOrm::AND, {
//        { LazyOrm::OR, {
//                std::vector<LazyOrm::DbVariant>{"name","like","sa%"},
//                std::vector<LazyOrm::DbVariant>{"name","like","%ra"},
//            }
//        },
//        { LazyOrm::OR, {
//                std::vector<LazyOrm::DbVariant>{"age","between","6,13"},
//                std::vector<LazyOrm::DbVariant>{"grade","in","[17,18,19,20]"},
//            }
//        }
//      }
//    };

//    LazyOrm::WhereFilter p2 = { LazyOrm::AND, {
//                      std::vector<LazyOrm::DbVariant>{"name","like","sa%"},
//                      std::vector<LazyOrm::DbVariant>{"name","like","sa%"}
//                                         }
//    };

//LazyOrm::WhereFilter p3 =
//{
//    {
//        LazyOrm::WhereFilter{
//                                 LazyOrm::AND,
//                                 {
//                                     std::vector<LazyOrm::DbVariant>{"name","like","sa%"},
//                                     std::vector<LazyOrm::DbVariant>{"name","like","sa%"}
//                                 }
//        },
//        LazyOrm::WhereFilter{
//                                 LazyOrm::AND,
//                                 {
//                                     std::vector<LazyOrm::DbVariant>{"name","like","sa%"},
//                                     std::vector<LazyOrm::DbVariant>{"name","like","sa%"}
//                                 }
//        },
//        {
//            LazyOrm::AND,
//            {
//                std::vector<LazyOrm::DbVariant>{"name","like","sa%"},
//                std::vector<LazyOrm::DbVariant>{"name","like","sa%"}
//            }
//        }
//    }
//};


return {};
}

TEST_CASE( "Factorials are computed", "[Lazy_FILTER]" ) {

    std::cout << where7() << std::endl;

    REQUIRE( Catch::trim(filter1()) == R"(GROUP BY group1,group2 ORDER BY num1,num2 LIMIT 11,23)" );
    REQUIRE( Catch::trim(filter2()) == R"(GROUP BY group1,group2 ORDER BY num1,num2 LIMIT 11,23)" );
    REQUIRE( Catch::trim(filter3()) == R"(GROUP BY group1,group2 ORDER BY num1,num2 LIMIT 10,10)" );
    REQUIRE( Catch::trim(filter4()) == R"(GROUP BY group1,group2 ORDER BY num1,num2 LIMIT 14,26)" );
//    REQUIRE( Catch::trim(where5()) == R"(WHERE  (`name` like 'sa%' OR `name` like '%ra' ) AND  (`age` between '6,13' OR `grade` in '[17,18,19,20]' ) GROUP BY group1,group2 ORDER BY num1,num2 LIMIT 14,26)" );
}
