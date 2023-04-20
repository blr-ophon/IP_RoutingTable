#ifndef DOTGEN_H
#define DOTGEN_H

#include <graphviz/gvc.h>
#include <stdio.h>
#include <stdlib.h>
#include "simnet.h"

//receive adjacency list array and creates a dot graph
void Simnet_dotgen(struct Router *routers);

#endif
