#include "MariadbFilteringLazy.h"

namespace LazyOrm {
MariadbFilteringLazy::MariadbFilteringLazy()
{

}

std::string MariadbFilteringLazy::where_conditions()
{
    return "WHERE ... mariadb... ";
}
}
