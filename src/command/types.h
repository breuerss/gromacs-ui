#ifndef COMMAND_TYPE_H
#define COMMAND_TYPE_H

#include <variant>
#include "fileobject.h"
#include "../config/configuration.h"

namespace Command {

typedef std::variant<
  FileObject::Pointer,
  Config::Configuration::Pointer
  > Data;

bool isSet(const Data& data);

template<typename ContainedType>
bool isSet(const Data& data)
{
  return std::holds_alternative<ContainedType>(data) &&
    std::get<ContainedType>(data);
}

}

#endif
