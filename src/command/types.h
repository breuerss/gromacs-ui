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


}

#endif
