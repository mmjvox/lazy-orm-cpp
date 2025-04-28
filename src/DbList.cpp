#include "DbList.h"
#include <sstream>

namespace LazyOrm {

DbList::DbList()
{

}

DbList::DbList(std::initializer_list<DbVariant> input)
{
    setList( {input.begin(), input.end()} );
}

void DbList::setList(std::vector<DbVariant> list)
{
    std::stringstream ss;
    ss<<"[no'](";

    auto it = list.begin();
    if (it != list.end()) {
        ss << it->toString();
        ++it;
    }

    for (; it != list.end(); ++it) {
        ss << "," << it->toString();
    }

    ss<<")";

    emplace<std::string>(ss.str());
}

} // namespace LazyOrm
