#include <iostream>
#include "SqliteLazy.h"

#include <sqlite3.h>

#include <catch2/catch_all.hpp>
#include <cstdint>

int callback(void* data, int argc, char** argv, char** col_name) {
    for (int i = 0; i < argc; i++) {
        std::cout << col_name[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    std::cout << std::endl;
    return 0;
}


TEST_CASE( "Factorials are computed", "[Lazy_SQLITE]" ) {

  LazyOrm::SqliteLazy lazyOrm;
  lazyOrm[LazyOrm::SELECT]="student";
  lazyOrm.setProperties({"'name'"});
  std::string sqlString = lazyOrm.queryString();

  std::string aaa="select 1 as 'aa';";

  sqlite3* db;
  char* err_msg = nullptr;
  int rc = sqlite3_open("example.db", &db);
  if (rc != SQLITE_OK) {
      std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
      sqlite3_close(db);
      return;
  }

  rc = sqlite3_exec(db, aaa.c_str(), callback, nullptr, &err_msg);
  if (rc != SQLITE_OK) {
      std::cerr << "Failed to create table: " << err_msg << std::endl;
      sqlite3_free(err_msg);
      sqlite3_close(db);
  }
  std::cout << "Table created successfully" << std::endl;
  sqlite3_close(db);

  std::cout <<"SQLITE \n " << sqlString << std::endl;
}
