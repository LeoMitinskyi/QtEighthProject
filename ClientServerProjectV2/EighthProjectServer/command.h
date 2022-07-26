#ifndef COMMAND_H
#define COMMAND_H

#include <iostream>

class Command
{
public:
    Command();

private:
    std::string commandName;

public:
    virtual std::string getCommandName();
    virtual void setCommandName(const std::string&);
    // virtual void execute();
};

#endif // COMMAND_H
