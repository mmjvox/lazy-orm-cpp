#include "DbList.h"
#include <sstream>

namespace LazyOrm {

DbList::DbList()
{

}

DbList::DbList(std::initializer_list<DbVariant> input)
{
    std::stringstream ss;
    ss<<"[no'](";

    auto it = input.begin();
    if (it != input.end()) {
        ss << it->toString();
        ++it;
    }

    for (; it != input.end(); ++it) {
        ss << "," << it->toString();
    }

    ss<<")";

    emplace<std::string>(ss.str());
}

} // namespace LazyOrm
