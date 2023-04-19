#include "net.h"
#include <graphviz/gvc.h>
#include <stdio.h>
#include <stdlib.h>

//weighted graph structs. Objective is:
//calculate fastest route between two nodes
//fill each node routing table  with proper gateways


int main(void) {
    //cgraph
    Agraph_t *graph = agopen("G", Agdirected, NULL);


    Agnode_t *n1 = agnode(graph, "1", 1);
    Agnode_t *n2 = agnode(graph, "2", 1);
    Agnode_t *n3 = agnode(graph, "3", 1);
    agedge(graph, n1, n2, NULL, 1);
    agedge(graph, n2, n3, NULL, 1);

    //graph style 
    agattr(graph, AGNODE, "style", "filled");
    agattr(graph, AGNODE, "fillcolor", "#030769");
    agattr(graph, AGNODE, "fontcolor", "#b1acc8");    
    agattr(graph, AGNODE, "shape", "circle");       
    agattr(graph, AGEDGE, "color", "#64d967");       
    agattr(graph, AGNODE, "color", "#b2bfed");       
    agsafeset(graph, "bgcolor", "black", "");      



    //generate .dot file
    FILE *f = fopen("graph.dot", "w");    
    agwrite(graph, f);
    fclose(f);
    system("dot -Tpng -o graph.png graph.dot");  

    agclose(graph);
    return 0;
}

