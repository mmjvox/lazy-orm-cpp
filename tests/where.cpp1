#include <iostream>
//#include "MariadbLazy.h"
#include "MariadbFilteringLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>


std::string where1() {
    LazyOrm::MariadbFilteringLazy filters;
    filters.setFilter({"grade","in", "[1,5,7,9]"});
    filters.setFilter(LazyOrm::GROUPBY , {"group1","group2"});
    filters.setFilter(LazyOrm::ORDERBY , {"num1","num2"});
    filters.setFilter(LazyOrm::LIMIT , {11,23});
    return filters.where_conditions();
}

std::string where2() {
    LazyOrm::MariadbFilteringLazy filters;
    filters.setFilter(LazyOrm::AND , {"grade","in", "[1,5,7,9]"});
    filters.setFilter(LazyOrm::GROUPBY , {"group1","group2"});
    filters.setFilter(LazyOrm::ORDERBY , {"num1","num2"});
    filters.setFilter(LazyOrm::LIMIT , {11,23});
    return filters.where_conditions();
}

std::string where3() {
    LazyOrm::MariadbFilteringLazy filters;
    filters[LazyOrm::WHERE] = {{"grade","in", "[1,5,7,9]"}};
    filters[LazyOrm::GROUPBY] = {{"group1","group2"}};
    filters[LazyOrm::ORDERBY] = {{"num1","num2"}};
    filters[LazyOrm::LIMIT] = "10,10";
    return filters.where_conditions();
}

std::string where4() {

    LazyOrm::WhereFilter p1 = { LazyOrm::AND, {
         {"AGE",12},
         {"SALARY",65000},
         {"hair","pink"},
         {"height","between", "99,198"},
         {"grade","in", "[1,5,7,9]"},
      }
    };

    LazyOrm::MariadbFilteringLazy filters;
    filters.setWhereFilter(p1);
    filters.setFilter(LazyOrm::GROUPBY , {"group1","group2"});
    filters.setFilter(LazyOrm::ORDERBY , {"num1","num2"});
    filters.setFilter(LazyOrm::LIMIT , {"14",26});

return filters.where_conditions();
}


std::string where5() {

    LazyOrm::WhereFilter p1 = { LazyOrm::AND, {
        std::vector<LazyOrm::WhereFilter>{
          { LazyOrm::OR, {
              {"name","like","sa%"},
              {"name","like","%ra"},
            }
          }
        },
        std::vector<LazyOrm::WhereFilter>{
          { LazyOrm::OR, {
              {"age","between","6,13"},
              {"grade","in","[17,18,19,20]"},
            }
          }
        }
      }
    };

    LazyOrm::MariadbFilteringLazy filters;
    filters.setWhereFilter(p1);
    filters.setFilter(LazyOrm::GROUPBY , {"group1","group2"});
    filters.setFilter(LazyOrm::ORDERBY , {"num1","num2"});
    filters.setFilter(LazyOrm::LIMIT , {"14",26});

return filters.where_conditions();
}

std::string where6() {

    LazyOrm::WhereFilter p1 = { LazyOrm::AND, {
        std::vector<LazyOrm::WhereFilter>{
          { LazyOrm::OR, {
              {"name","like","sa%"},
              {"name","like","%ra"},
            }
          }
        },
        std::vector<LazyOrm::WhereFilter>{
          { LazyOrm::OR, {
              {"age","between","6,13"},
              {"grade","in","[17,18,19,20]"},
            }
          }
        }
      }
    };

    LazyOrm::MariadbFilteringLazy filters;
    filters.setFilter({p1});
    filters.setFilter(LazyOrm::GROUPBY , {"group1","group2"});
    filters.setFilter(LazyOrm::ORDERBY , {"num1","num2"});
    filters.setFilter(LazyOrm::LIMIT , {"14",26});

return filters.where_conditions();
}

std::string where7() {

    LazyOrm::WhereFilter whereFilter = {
        LazyOrm::AND, {
            std::vector<LazyOrm::WhereFilter>{
                {
                    LazyOrm::OR, {
                        {"confirmed",false},
                        {"defrayed",false},
                        {"delivered",false},
                    }
                }
            },
            std::vector<LazyOrm::WhereFilter>{
                {
                    LazyOrm::AND, {
                        {"[func]LENGTH(chassis)",">","[no']1"},
                        {"[func]LENGTH(tracker_name)",">","[no']1"},
                        {"[func]SUBSTRING(tracker_phones, 3, 1) REGEXP '^[0-9]+$'"},
                    }
                }
            }
        }
    };

    LazyOrm::MariadbFilteringLazy filters;
    filters.setFilter({whereFilter});
return filters.where_conditions();
}

std::string where8() {

LazyOrm::WhereFilter p0 = { LazyOrm::AND, {

                      {
                        LazyOrm::OR, {"name","like","sa%"}
                      },
                      { LazyOrm::OR, {
                                        {"name","like","sa%"},
                                        {"name","like","%ra"},
                                        }
                      },
                      {
                          { LazyOrm::OR, {
                                            {"name","like","sa%"},
                                            {"name","like","%ra"},
                                            }
                          }
                      },
                      {
                          { LazyOrm::OR, {
                                            {"age","between","6,13"},
                                            {"grade","in","[17,18,19,20]"},
                                            }
                          },
                          { LazyOrm::OR, {
                                            {"age","between","6,13"},
                                            {"grade","in","[17,18,19,20]"},
                                            }
                          }
                      }
                  }
};

    LazyOrm::WhereFilter p1 = { LazyOrm::AND, {
        { LazyOrm::OR, {
                {"name","like","sa%"},
                {"name","like","%ra"},
            }
        },
        { LazyOrm::OR, {
                {"age","between","6,13"},
                {"grade","in","[17,18,19,20]"},
            }
        }
      }
    };

    LazyOrm::WhereFilter p2 = { LazyOrm::AND, {
                      {"name","like","sa%"},
                      {"name","like","sa%"}
                                         }
    };

LazyOrm::WhereFilter p3 =
{
    {
        LazyOrm::WhereFilter{
                                 LazyOrm::AND,
                                 {
                                     {"name","like","sa%"},
                                     {"name","like","sa%"}
                                 }
        },
        LazyOrm::WhereFilter{
                                 LazyOrm::AND,
                                 {
                                     {"name","like","sa%"},
                                     {"name","like","sa%"}
                                 }
        },
        {
            LazyOrm::AND,
            {
                {"name","like","sa%"},
                {"name","like","sa%"}
            }
        }
    }
};


return {};
}

TEST_CASE( "Factorials are computed", "[Lazy_WHERE]" ) {

    std::cout << where7() << std::endl;

    REQUIRE( Catch::trim(where1()) == R"(WHERE `grade` in '[1,5,7,9]' GROUP BY group1,group2 ORDER BY num1,num2 LIMIT 11,23)" );
    REQUIRE( Catch::trim(where2()) == R"(WHERE `grade` in '[1,5,7,9]' GROUP BY group1,group2 ORDER BY num1,num2 LIMIT 11,23)" );
    REQUIRE( Catch::trim(where3()) == R"(WHERE `grade` in '[1,5,7,9]' GROUP BY group1,group2 ORDER BY num1,num2 LIMIT 10,10)" );
    REQUIRE( Catch::trim(where4()) == R"(WHERE `AGE` = '12' AND `SALARY` = '65000' AND `hair` = 'pink' AND `height` between '99,198' AND `grade` in '[1,5,7,9]' GROUP BY group1,group2 ORDER BY num1,num2 LIMIT 14,26)" );
    REQUIRE( Catch::trim(where5()) == R"(WHERE  (`name` like 'sa%' OR `name` like '%ra' ) AND  (`age` between '6,13' OR `grade` in '[17,18,19,20]' ) GROUP BY group1,group2 ORDER BY num1,num2 LIMIT 14,26)" );
}
