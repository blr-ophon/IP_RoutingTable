#include "dotgen.h"

void Simnet_dotgen(struct Router *routers){
    Agraph_t *graph = agopen("G", Agstrictundirected, NULL);

    Agnode_t *nodes[ROUTER_TOTAL];
    //fill nodes array with router nodes
    for(int i = 0; i < ROUTER_TOTAL; i++){
        char snum[8] = {0};
        sprintf(snum, "%d", i);
        nodes[i] = agnode(graph, snum, 1);   

        //Address label on nodes
        char addr_str[INET_ADDRSTRLEN] = {0};
        struct in_addr iaddr;
        iaddr.s_addr = htonl(routers[i].ipv4);
        inet_ntop(AF_INET, &iaddr, addr_str, INET_ADDRSTRLEN);
        agsafeset(nodes[i], "xlabel", addr_str , "");
    }

    //insert edges
    for(int i = 0; i < ROUTER_TOTAL; i ++){
        RouteLink *p;
        for(p = routers[i].neighbors; p != NULL; p = p->next){  
            char snum[8] = {0};
            sprintf(snum, "%lu", p->weight);
            Agedge_t *e = agedge(graph, nodes[i], nodes[p->node_id], snum, 1);
            agsafeset(e, "label", snum, "");
        }
    }

    //graph style 
    agattr(graph, AGNODE, "style", "filled");
    agattr(graph, AGNODE, "fillcolor", "#030769");
    agattr(graph, AGNODE, "fontcolor", "#b1acc8");    
    agattr(graph, AGEDGE, "fontcolor", "#b1acc8");    
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
