#ifndef _BLURAY_DISPLAYER_CAPABILITY_
#define _BLURAY_DISPLAYER_CAPABILITY_

extern "C"
{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>

#include <cutils/sockets.h>
}

namespace RKBluray
{

class DisplayCapability
{
};

}
#endif
