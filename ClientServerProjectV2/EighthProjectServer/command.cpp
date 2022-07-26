#include "command.h"

Command::Command() {}

std::string Command::getCommandName()
{
    return this->commandName;
}

void Command::setCommandName(const std::string & commandName)
{
    this->commandName = commandName;
}
