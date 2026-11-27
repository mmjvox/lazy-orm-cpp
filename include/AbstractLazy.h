#ifndef ABSTRACTLAZY_H
#define ABSTRACTLAZY_H

#include "DbVariant.h"
#include "FilteringAbstractLazy.h"
#include "HavingFilter.h"
#include "ParameterizedQuery.h"
#include "WhereFilter.h"

#include <list>
#include <vector>
#include <map>
#include <variant>
#include <string>
#include <initializer_list>


namespace LazyOrm {

enum Query
{
    UNDEFINED = -1,
    INIT      = 0,
    INSERT    = 10,
    SELECT,
    UPDATE,
    DELETE,
    INSERT_UPDATE,
    BULK_INSERT,
    INSERT_IGNORE,
    BULK_UPDATE,
    //TODO: BATCH_INSERT_UPDATE
    SELECT_DISTINCT,
    CONTAINS,
    COUNT,
    RAW_QUERY
};

enum Count{
    COUNT_PROPS=0
};

enum Primary_Key{
    PrimaryKey=0
};

enum Unique_Keys{
    UniqueKeys=0
};

class AbstractLazy
{
public:
  enum class ParameterDialect
  {
      PostgreSQL,
      MariaDB,
      SQLite
  };

protected:

    enum QuoteType
    {
        NoQuote=0,
        SingleQuote=1,
        DoubleQuote,
        BackTick
    };

  Query mQueryType=UNDEFINED;
    // TODO: set backtick for name of tabes in queries
  std::string mTabeName;
  std::map<DbVariant, DbVariant> mProperties;
  std::list<std::map<DbVariant, DbVariant>> mBatchProperties;
  std::string mPrimaryKey;
  std::vector<std::string> mUniqueKeys;
  std::list<DbVariant> mCounts;
  std::string string_join(const std::string &delimiter, const std::vector<std::string> &container, QuoteType quote = NoQuote) const;
  std::string string_join(const std::string &delimiter, const std::vector<DbVariant> &container, QuoteType quote = NoQuote) const;
  bool appendPropAsCount(DbVariant prop);

  virtual std::string insert_query() const = 0;
  virtual std::string select_query() const = 0;
  virtual std::string update_query() const = 0;
  virtual std::string delete_query() const = 0;
  virtual std::string insert_update_query() const = 0;
  virtual std::string bulk_insert_query() const = 0;
  virtual std::string insert_ignore_query() const = 0;
  virtual std::string bulk_update_query() const = 0;
  virtual std::string contains_query() const;
  virtual std::string count_query() const;
  virtual std::string raw_query() const;

  virtual std::list<std::string> count_queries() const;

  virtual FilteringAbstractLazy& getCurrentFilters() = 0;
  virtual void appendFilter(const Filters &filter, LazyOrm::DbVariant dbVariant) = 0;

  void copyStateFrom(const AbstractLazy &other);

  // Bulk UPDATE support.
  // Validates the batch and fills `columns` (ordered column list of the
  // batch rows) and `updateColumns` (columns minus the primary key).
  // Returns false when the input does not satisfy the Bulk UPDATE contract;
  // callers must then produce an empty query, matching the failure
  // convention of bulk_insert_query().
  bool prepareBulkUpdate(std::vector<DbVariant> &columns,
                         std::vector<DbVariant> &updateColumns) const;
  // Name of the temporary staging table, identical for CREATE, INSERT,
  // UPDATE and DROP statements.
  std::string bulkUpdateStagingName() const;
  // Renders the validated target table identifier with dialect quoting
  // per component (e.g. `schema`.`table` or "schema"."table").
  std::string bulkUpdateTargetTable(QuoteType quote) const;
  // Renders the unqualified quoted target table name for column
  // qualification inside SQLite correlated subqueries and EXISTS.
  std::string bulkUpdateTargetRef(QuoteType quote) const;
  // Formats a validated batch cell value for regular SQL Bulk UPDATE:
  // std::monostate becomes SQL NULL, ordinary strings (including "NULL")
  // remain quoted strings, booleans stay true/false, and numeric/blob
  // values keep DbVariant::setQuote() formatting.
  std::string bulkUpdateValue(const DbVariant &value) const;
  // Renders mWhereFilter with every identifier qualified by `alias`
  // (empty alias leaves identifiers unqualified). `quote` selects the
  // identifier quoting style of the dialect. `leading` is prepended when
  // the filter is non-empty (e.g. " WHERE " or " AND ").
  std::string bulkUpdateWhereString(const std::string &alias,
                                    const std::string &leading,
                                    QuoteType quote) const;

  ParameterizedQuery parameterizedQuery(ParameterDialect dialect) const;

  WhereFilter mWhereFilter;
  HavingFilter mHavingFilter;

public:
  const static char* version;
  virtual std::string queryString() const;
  virtual ParameterizedQuery buildParameterized() const;

  void setTabeName(const std::string &name);
  void setQueryType(LazyOrm::Query queryType);
  std::string & operator[](const LazyOrm::Query &queryType);

  void setProperty(const std::string key, const DbVariant value);
  DbVariant & operator[](const std::string &key);

  void setProperties(const std::vector<std::string> &keys);
  AbstractLazy & operator<<(const std::string &key);
  void setProperties(const std::initializer_list<std::pair<DbVariant,DbVariant>> items);
  AbstractLazy & operator<<(const std::pair<DbVariant,DbVariant> &key_value);

  void setProperties(const std::initializer_list<std::map<DbVariant, DbVariant> > list);
  void setProperties(const std::list<std::map<DbVariant, LazyOrm::DbVariant>> &list);

  FilteringAbstractLazy& operator[](const LazyOrm::Filters &filter);
  WhereFilter& operator[](const LazyOrm::NestedWhere &nestedWhere);
  void setNestedWhere(LazyOrm::WhereFilter nestedWhere);
  HavingFilter& operator[](const LazyOrm::Having &nestedHaving);
  void setNestedHaving(LazyOrm::HavingFilter nestedHaving);
//  void operator=(const LazyOrm::FilterVariant &variant);

  void setPrimaryKey(const std::string &primaryKey);
  std::string & operator[](const LazyOrm::Primary_Key &primaryKey);

  void setUniqueKeys(const std::initializer_list<std::string> &uniqueKeys);
  std::vector<std::string> & operator[](const LazyOrm::Unique_Keys &uniqueKeys);

  void setCountType(std::initializer_list<DbVariant> countFields);
  std::list<DbVariant> & operator[](const LazyOrm::Count &count);

  // convert simple SELECT to SELECT DISTINCT
  void enableDistinctSelect(bool distinct=true);

  //getters
  std::string tabeName() const;
  std::map<DbVariant, DbVariant> properties() const;
  std::list<std::map<DbVariant, DbVariant> > batchProperties() const;
  WhereFilter whereFilter() const;
  HavingFilter havingFilter() const;
  Query queryType() const;
  virtual const FilteringAbstractLazy& getFilter() const = 0;
  std::string primaryKey() const;
  std::vector<std::string> uniqueKeys() const;


  static void trim_consecutive_spaces(std::string& str);
  virtual std::string query_with_trim_consecutive_spaces() const;

};
}

#endif // ABSTRACTLAZY_H
