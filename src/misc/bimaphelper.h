#include <boost/bimap/bimap.hpp>

template <typename L, typename R>
boost::bimaps::bimap<L, R>
makeBimap(std::initializer_list<typename boost::bimaps::bimap<L, R>::value_type> list)
{
    return boost::bimaps::bimap<L, R>(list.begin(), list.end());
}
