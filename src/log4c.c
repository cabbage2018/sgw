#include <stdlib.h>
#include <stdio.h>
#include "log4c.h"

int outputDebugString() {

	DEBUG("%s entry to a function()", ">>");

	ERROR("%s exit from a function()", "<<");

	FATAL("%s capture memory", "..");

	DISABLED("SHOULD be OK with only one argument?");
}
