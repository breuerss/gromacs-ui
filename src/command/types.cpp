#include "types.h"


namespace Command {

bool isSet(const Data& data)
{
  return std::visit([] (const auto& data) { return !!data; }, data);
}

}
