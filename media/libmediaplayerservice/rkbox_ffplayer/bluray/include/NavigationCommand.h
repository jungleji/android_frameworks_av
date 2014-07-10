#ifndef _BLURAY_NAVIGATION_COMMAND_H_
#define _BLURAY_NAVIGATION_COMMAND_H_

namespace RKBluray {

class NavigationCommand
{
public:
    unsigned int   ulOpCode;
    unsigned int   ulDstOperand;
    unsigned int   ulSrcOperand;

    NavigationCommand()
    {
        ulOpCode = 0;
        ulDstOperand = 0;
        ulSrcOperand = 0;
    }
};
}


#endif