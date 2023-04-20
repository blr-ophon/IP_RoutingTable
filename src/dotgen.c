#include "dotgen.h"

void Simnet_dotgen(struct Router *routers){
    Agraph_t *graph = agopen("G", Agstrictundirected, NULL);

    Agnode_t *nodes[ROUTER_TOTAL];
    //fill nodes array with router nodes
    char snum[8] = {0};
    for(int i = 0; i < ROUTER_TOTAL; i++){
        sprintf(snum, "%d", i);
        nodes[i] = agnode(graph, snum, 1);   
    }

    //insert edges
    for(int i = 0; i < ROUTER_TOTAL; i ++){
        RouteLink *p;
        for(p = routers[i].neighbors; p != NULL; p = p->next){  
            agedge(graph, nodes[i], nodes[p->node_id], NULL, 1);
        }
    }

    //graph style 
    agattr(graph, AGNODE, "style", "filled");
    agattr(graph, AGNODE, "fillcolor", "#030769");
    agattr(graph, AGNODE, "fontcolor", "#b1acc8");    
    agattr(graph, AGNODE, "shape", "circle");       
    agattr(graph, AGEDGE, "color", "#64d967");       
    agattr(graph, AGNODE, "color", "#b2bfed");       
    agsafeset(graph, "bgcolor", "black", "");      
    agsafeset(graph, "layout", "neato", "");      


    //generate .dot file
    system("mkdir -p ./dot");
    FILE *f = fopen("./dot/graph.dot", "w");    
    agwrite(graph, f);
    fclose(f);
    system("dot -Tpng -o ./dot/graph.png ./dot/graph.dot");  

    agclose(graph);
}

//void dotgen(void) {
//    //cgraph
//    Agraph_t *graph = agopen("G", Agdirected, NULL);
//
//
//    Agnode_t *n1 = agnode(graph, "1", 1);
//    Agnode_t *n2 = agnode(graph, "2", 1);
//    Agnode_t *n3 = agnode(graph, "3", 1);
//    agedge(graph, n1, n2, NULL, 1);
//    agedge(graph, n2, n3, NULL, 1);
//
//    //graph style 
//    agattr(graph, AGNODE, "style", "filled");
//    agattr(graph, AGNODE, "fillcolor", "#030769");
//    agattr(graph, AGNODE, "fontcolor", "#b1acc8");    
//    agattr(graph, AGNODE, "shape", "circle");       
//    agattr(graph, AGEDGE, "color", "#64d967");       
//    agattr(graph, AGNODE, "color", "#b2bfed");       
//    agsafeset(graph, "bgcolor", "black", "");      
//
//
//
//    //generate .dot file
//    FILE *f = fopen("./dot/graph.dot", "w");    
//    agwrite(graph, f);
//    fclose(f);
//    system("dot -Tpng -o ./dot/graph.png ./dot/graph.dot");  
//
//    agclose(graph);
//}

