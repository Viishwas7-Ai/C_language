#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT 256
#define MAX_NODES 100
#define MAX_HEAP 100000

typedef struct {
    char name[64];
    char type[32];
    int value;

    int *connections;
    int *weights;

    char **types;
    int *directed;

    int connection_count;
    int capacity;
} Node;

Node nodes[MAX_NODES];
int node_count = 0;

/* Function prototypes */
void create_node(const char *name, int value, const char *type);
void connect_nodes(const char *a, const char *b, const char *type, int weight, int directed);
void show_node(const char *name);
void list_nodes(void);
void list_edges(void);
void graph_stats(void);
void graph_components(void);
void degree_node(const char *name);
void delete_node(const char *name);
void unlink_nodes(const char *a, const char *b);
void neighbors_node(const char *name);
void update_weight(const char *a, const char *b, int new_weight);
void expand_nodes(const char *name, int depth);
void find_value(const char *op, int val);
void find_degree(const char *op, int val);
void find_type(const char *type);
void find_neighbors(const char *name, int max_weight);
void find_path(const char *start, const char *end);
void find_path_length(int max_len);
void find_pattern(const char *a, const char *b, const char *c);
void dijkstra_path(const char *start, const char *end);
void traverse_query(const char *start, const char *end);
void save_graph(const char *filename);
void load_graph(const char *filename);
void cleanup_graph(void);
void free_hash_table(void);
void optimize_memory(void);
void cleanup(void);

// Heap search for faster shortest path finder.

typedef struct {
    int node;
    int dist;
}HeapNode;

HeapNode heap[MAX_HEAP];
int heap_size = 0;

void swap(HeapNode *a,HeapNode *b){
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}
HeapNode heap_pop(void);

void free_hash_table(void);

void heap_push(int node, int dist) {

    heap[heap_size].node = node;
    heap[heap_size].dist = dist;

    int i = heap_size;
    heap_size++;

    while (i > 0) {

        int parent = (i - 1) / 2;

        if (heap[parent].dist <= heap[i].dist)
            break;

        swap(&heap[parent], &heap[i]);
        i = parent;
    }
}

HeapNode heap_pop(){
    HeapNode top = heap[0];
    heap_size--;

    heap[0] = heap[heap_size];

    int i = 0;

    while (1){
        int left = 2*i + 1;
        int right = 2*i + 2;
        int smallest = i;

        if (left < heap_size && heap[left].dist < heap[smallest].dist)
            smallest = left;
        if (right < heap_size && heap[right].dist < heap[smallest].dist)
            smallest = right;
        if (smallest == i)
            break;

        swap(&heap[i],&heap[smallest]);
        i = smallest;
    }
    return top;
}

// Hash for O(1) serch of an object or node name.

#define HASH_SIZE 211

typedef struct HashNode{
    char name[64];
    int index;
    struct HashNode *next;
} HashNode;

HashNode *hash_table[HASH_SIZE];

unsigned int hash(const char *str){
    unsigned int h = 0;
    while(*str){
        h = (h*31) + *str;
        str++;
    }
    return h % HASH_SIZE;
}

void hash_insert(const char *name,int index){
    unsigned int h = hash(name);
    HashNode *newNode = malloc(sizeof(HashNode));

    strcpy(newNode->name,name);
    newNode->index= index;

    newNode->next = hash_table[h];
    hash_table[h] = newNode;
}

int hash_find(const char *name){
    unsigned int h = hash(name);
    HashNode *current = hash_table[h];

    while (current){
        if (strcmp(current->name,name) == 0)
            return current->index;
        current = current->next;
    }
    return -1;
}

/* -------- ENGINE FUNCTIONS (you will implement later) -------- */

void create_node(const char *name,int value,const char *type){

    if (node_count >= MAX_NODES){
        printf("Node limit reached\n");
        return;
    }

    /* Prevent duplicate node names */
    for (int i = 0; i < node_count; i++) {
        if (strcmp(nodes[i].name, name) == 0) {
            printf("Node already exists: %s\n", name);
            return;
        }
    }

    strcpy(nodes[node_count].name, name);
    strcpy(nodes[node_count].type,type);

    nodes[node_count].value = value;
    nodes[node_count].connections = malloc(4 * sizeof(int));
    nodes[node_count].weights = malloc(4 * sizeof(int));

    nodes[node_count].types = malloc(4 * sizeof(char*));
    nodes[node_count].directed = malloc(4 * sizeof(int));

    if (!nodes[node_count].connections || !nodes[node_count].weights ||
        !nodes[node_count].types || !nodes[node_count].directed) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    for(int i=0;i<4;i++){
        nodes[node_count].types[i] = malloc(32);
        if (!nodes[node_count].types[i]) {
            printf("Memory allocation failed\n");
            exit(1);
        }
    }

    nodes[node_count].capacity = 4;
    nodes[node_count].connection_count = 0;

    printf("Node created: %s\n", name);
    
    hash_insert(name, node_count);
    node_count++;
}

void connect_nodes(const char *a,const char *b,const char *type,int weight,int directed) {

    int indexA = hash_find(a);
    int indexB = hash_find(b);

    if (indexA == -1 || indexB == -1) {
        printf("One of the nodes does not exist\n");
        return;
    }

    /* add connection A -> B */
    if (nodes[indexA].connection_count == nodes[indexA].capacity) {

        int old_capacity = nodes[indexA].capacity;
        int new_capacity = nodes[indexA].capacity * 2;

        int *newConnectionsA = realloc(
            nodes[indexA].connections,
            new_capacity * sizeof(int)
        );
        int *newWeightsA = realloc(
            nodes[indexA].weights,
            new_capacity * sizeof(int)
        );
        char **newTypesA = realloc(
            nodes[indexA].types,
            new_capacity * sizeof(char*)
        );
        int *newDirectedA = realloc(
            nodes[indexA].directed,
            new_capacity * sizeof(int)
        );

        if (!newConnectionsA || !newWeightsA || !newTypesA || !newDirectedA) {
            printf("Memory allocation failed\n");
            exit(1);
        }

        nodes[indexA].connections = newConnectionsA;
        nodes[indexA].weights = newWeightsA;
        nodes[indexA].types = newTypesA;
        nodes[indexA].directed = newDirectedA;

        nodes[indexA].capacity = new_capacity;

        for(int i = old_capacity; i < nodes[indexA].capacity; i++){
            nodes[indexA].types[i] = malloc(32);
            if (!nodes[indexA].types[i]) {
                printf("Memory allocation failed\n");
                exit(1);
            }
        }
    }

    int posA = nodes[indexA].connection_count;

    nodes[indexA].connections[posA] = indexB;
    nodes[indexA].weights[posA] = weight;

    strcpy(nodes[indexA].types[posA],type);
    nodes[indexA].directed[posA] = directed;

    nodes[indexA].connection_count++;

    /* add connection B -> A (undirected) */
    if (nodes[indexB].connection_count == nodes[indexB].capacity) {

        int old_capacity = nodes[indexB].capacity;
        int new_capacity = nodes[indexB].capacity * 2;

        int *newConnectionsB = realloc(
            nodes[indexB].connections,
            new_capacity * sizeof(int)
        );
        int *newWeightsB = realloc(
            nodes[indexB].weights,
            new_capacity * sizeof(int)
        );
        char **newTypesB = realloc(
            nodes[indexB].types,
            new_capacity * sizeof(char*)
        );
        int *newDirectedB = realloc(
            nodes[indexB].directed,
            new_capacity * sizeof(int)
        );

        if (!newConnectionsB || !newWeightsB || !newTypesB || !newDirectedB) {
            printf("Memory allocation failed\n");
            exit(1);
        }

        nodes[indexB].connections = newConnectionsB;
        nodes[indexB].weights = newWeightsB;
        nodes[indexB].types = newTypesB;
        nodes[indexB].directed = newDirectedB;

        nodes[indexB].capacity = new_capacity;

        for(int i = old_capacity; i < nodes[indexB].capacity; i++){
            nodes[indexB].types[i] = malloc(32);
            if (!nodes[indexB].types[i]) {
                printf("Memory allocation failed\n");
                exit(1);
            }
        }
    }

    if(!directed){

        int posB = nodes[indexB].connection_count;

        nodes[indexB].connections[posB] = indexA;
        nodes[indexB].weights[posB] = weight;

        strcpy(nodes[indexB].types[posB],type);
        nodes[indexB].directed[posB] = directed;

        nodes[indexB].connection_count++;
    }

    printf("Connected %s <-> %s weight %d\n", a, b, weight);
}

void show_node(const char *name) {

    int index = hash_find(name);

    if (index == -1) {
        printf("Node not found\n");
        return;
    }

    printf("Node: %s\n", nodes[index].name);

    for (int i = 0; i < nodes[index].connection_count; i++) {

        int neighbor = nodes[index].connections[i];
        int weight = nodes[index].weights[i];

        printf(" -> %s [%s] (weight %d)%s\n",
            nodes[neighbor].name,
            nodes[index].types[i],
            weight,
            nodes[index].directed[i] ? " directed" : "");
    }
}

void list_nodes(){
    for(int i = 0; i < node_count;i++){
        printf("%s (type=%s, value=%d):\n", nodes[i].name, nodes[i].type, nodes[i].value);

        for(int j = 0; j < nodes[i].connection_count; j++){
            int neighbor = nodes[i].connections[j];
            int weight = nodes[i].weights[j];
            int directed = nodes[i].directed[j];

            printf("  -> %s (weight %d)%s\n", nodes[neighbor].name, weight, directed ? " [directed]" : "");
        }
        printf("\n");
    }
}

void list_edges(){
    for(int i = 0; i < node_count;i++){
        for(int j = 0;j < nodes[i].connection_count; j++){
            int neighbor = nodes[i].connections[j];
            int weight = nodes[i].weights[j];
            int directed = nodes[i].directed[j];

            if (directed) {
                printf("%s -> %s (%d)\n", nodes[i].name, nodes[neighbor].name, weight);
            } else if (i < neighbor) {
                printf("%s -- %s (%d)\n", nodes[i].name, nodes[neighbor].name, weight);
            }
        }
    }
}

void graph_stats(){

    int edge_count = 0;

    for(int i = 0; i < node_count; i++){
        edge_count += nodes[i].connection_count;
    }

    edge_count = edge_count / 2; // undirected graph

    printf("Nodes: %d\n", node_count);
    printf("Edges: %d\n", edge_count);

    if(node_count > 0){
        double avg_degree = (double)(edge_count * 2) / node_count;
        printf("Average degree: %.2f\n", avg_degree);
    }
}

void graph_components(){
    int visited[MAX_NODES] = {0};
    int stack[MAX_NODES];

    int component = 1;
    
    for(int i = 0; i < node_count; i++){
        if(visited[i])
          continue;
        printf("Component %d:", component++);

        int top = 0;
        stack[top++] = i;
        visited[i] = 1;

        while(top >0){
            int current = stack[--top];
            printf(" %s", nodes[current].name);

            for(int j =0l; j < nodes[current].connection_count;j++){
                int neighbor = nodes[current].connections[j];

                if(!visited[neighbor]){
                    visited[neighbor] = 1;
                    stack[top++] = neighbor;
                }
            }
        }
        printf("\n");
    }
}

void degree_node(const char *name){
    int index = -1;
    for (int i = 0; i < node_count; i++){
        if (strcmp(nodes[i].name,name) == 0){
            index = i;
            break;
        }
    }
    if (index == -1){
        printf("Node not found\n");
        return;
    }
    printf("%s has %d connections\n",nodes[index].name,nodes[index].connection_count);
}

void delete_node(const char *name){

    int index = -1;
    for(int i = 0; i < node_count; i++){
        if(strcmp(nodes[i].name,name) == 0){
            index = i;
            break;
        }
    }
    if (index == -1){
        printf("Node not found\n");
        return;
    }

    /* Free the removed node's adjacency data */
    free(nodes[index].connections);
    free(nodes[index].weights);
    if (nodes[index].types) {
        for (int k = 0; k < nodes[index].capacity; k++) {
            free(nodes[index].types[k]);
        }
        free(nodes[index].types);
    }
    free(nodes[index].directed);

    /* Remove all edges to this node and adjust indices */
    for (int i = 0; i < node_count; i++){
        if (i == index)
            continue;

        int new_count = 0;
        for (int j = 0; j < nodes[i].connection_count; j++){
            int connected_index = nodes[i].connections[j];

            if (connected_index == index)
                continue; /* drop edge to removed node */

            /* shift indices down since we will remove a node from the array */
            if (connected_index > index)
                connected_index--;

            nodes[i].connections[new_count] = connected_index;
            nodes[i].weights[new_count] = nodes[i].weights[j];
            nodes[i].types[new_count] = nodes[i].types[j];
            nodes[i].directed[new_count] = nodes[i].directed[j];
            new_count++;
        }
        nodes[i].connection_count = new_count;
    }

    /* Shift nodes down in the array */
    for (int i = index; i < node_count - 1; i++){
        nodes[i] = nodes[i+1];
    }

    node_count--;

    /* Rebuild hash table to keep indices consistent */
    free_hash_table();
    for (int i = 0; i < node_count; i++) {
        hash_insert(nodes[i].name, i);
    }

    printf("Node %s deleted\n",name);
}

void unlink_nodes(const char *a,const char *b){
    int indexA = hash_find(a);
    int indexB = hash_find(b);

    if(indexA == -1 || indexB == -1){
        printf("Node not found\n");
        return;
    }

    for(int i = 0;i<nodes[indexA].connection_count;i++){
        if(nodes[indexA].connections[i] == indexB){
            for (int j=i;j<nodes[indexA].connection_count-1;j++){
                nodes[indexA].connections[j] = nodes[indexA].connections[j+1];
                nodes[indexA].weights[j] = nodes[indexA].weights[j+1];
                nodes[indexA].types[j] = nodes[indexA].types[j+1];
                nodes[indexA].directed[j] = nodes[indexA].directed[j+1];
            }
            nodes[indexA].connection_count--;
            break;
        }
    }
    for(int i = 0;i<nodes[indexB].connection_count;i++){
        if(nodes[indexB].connections[i] == indexA){
            for (int j=i;j<nodes[indexB].connection_count-1;j++){
                nodes[indexB].connections[j] = nodes[indexB].connections[j+1];
                nodes[indexB].weights[j] = nodes[indexB].weights[j+1];
                nodes[indexB].types[j] = nodes[indexB].types[j+1];
                nodes[indexB].directed[j] = nodes[indexB].directed[j+1];
            }
            nodes[indexB].connection_count--;
            break;
        }
    }
    printf("Connection removed\n");
}

void neighbors_node(const char *name){

    int index = -1;

    for (int i = 0; i < node_count; i++){
        if (strcmp(nodes[i].name,name) == 0){
            index = i;
            break;
        }
    }
    if (index == -1){
        printf("Node not found\n");
        return;
    }
    for (int i = 0; i < nodes[index].connection_count; i++){
        int neighbor = nodes[index].connections[i];
        int weight = nodes[index].weights[i];

        printf("%s (weight %d)\n",nodes[neighbor].name,weight);
    }
}

void update_weight(const char *a,const char *b ,int new_weight){
    int indexA = hash_find(a);
    int indexB = hash_find(b);

    if(indexA == -1 || indexB == -1){
        printf("Node Not Found\n");
        return;
    }

    int found = 0;
    /* update A -> B */
    for(int i = 0;i < nodes[indexA].connection_count;i++){
        if(nodes[indexA].connections[i] == indexB){
            nodes[indexA].weights[i] = new_weight;
            found = 1;
            break;
        }
    }
    /* update B -> A */
    for(int i = 0; i < nodes[indexB].connection_count; i++){
        if(nodes[indexB].connections[i] == indexA){
            nodes[indexB].weights[i] = new_weight;
            found = 1;
            break;
        }
    }

    if(found){
        printf("Weight update: %s <-> %s = %d\n", a,b, new_weight);
    }
    else{
        printf("Edge not found\n");
    }

}

void expand_nodes(const char *name,int depth){
    int start = hash_find(name);

    if( start == -1){
        printf("Node not found\n");
        return;
    }

    int visited[MAX_NODES] = {0};
    int queue[MAX_NODES];
    int level[MAX_NODES];

    int front = 0;
    int back = 0;

    queue[back] = start;
    level[back] = 0;
    back++;

    visited[start] = 1;

    while(front <back){
        int current = queue[front];
        int current_level = level[front];
        front++;

        if(current_level >= depth)
            continue;

        for(int i = 0; i< nodes[current].connection_count;i++){
            int neighbor = nodes[current].connections[i];

            if (!visited[neighbor]){
                visited[neighbor] = 1;
                printf("%s\n",nodes[neighbor].name);
                queue[back] = neighbor;
                level[back] = current_level + 1;
                back++;
            }
        }
    }
}

void find_value(const char *op, int val){
    for (int i = 0;i < node_count; i++){
        int v = nodes[i].value;
        int match = 0;

        if(strcmp(op,">") == 0 && v > val){
            match = 1;
        }
        else if (strcmp(op,"<") == 0 && v < val){
            match = 1;
        }
        else if (strcmp(op,"=") == 0 && v == val){
            match = 1;
        }
        else if (strcmp(op,">=") == 0 && v >= val){
            match = 1;
        }
        else if (strcmp(op,"<=") == 0 && v <= val){
            match = 1;
        }
        if(match){
            printf("%s (%d)\n", nodes[i].name, v);
        }
    }
}

void find_degree(const char *op, int val){

    for(int i = 0; i < node_count; i++){

        int d = nodes[i].connection_count;
        int match = 0;

        if(strcmp(op, ">") == 0 && d > val)
            match = 1;

        else if(strcmp(op, "<") == 0 && d < val)
            match = 1;

        else if(strcmp(op, "=") == 0 && d == val)
            match = 1;

        else if(strcmp(op, ">=") == 0 && d >= val)
            match = 1;

        else if(strcmp(op, "<=") == 0 && d <= val)
            match = 1;

        if(match)
            printf("%s (%d connections)\n", nodes[i].name, d);
    }
}

void find_type(const char *type){
    for(int i = 0; i < node_count;i++){
        if(strcmp(nodes[i].type,type) == 0){
            printf("%s (%s %d)\n",nodes[i].name,nodes[i].type,nodes[i].value);
        }
    }
}

void find_neighbors(const char *name, int max_weight){
    int index = -1;

    for (int i = 0; i < node_count; i++){
        if (strcmp(nodes[i].name,name) == 0){
            index = i;
            break;
        }
    }
    if (index == -1){
        printf("Node not found\n");
        return;
    }

    for (int i = 0; i < nodes[index].connection_count; i++){
        int neighbor = nodes[index].connections[i];
        int weight = nodes[index].weights[i];

        if(weight <= max_weight){
            printf("%s (weight %d)\n", nodes[neighbor].name, weight);
        }
    }
}

void find_path(const char *start ,const char *end){
    int start_index = -1;
    int end_index = -1;

    /* find start and end nodes */
    for(int i = 0; i < node_count; i++){
        if (strcmp(nodes[i].name, start) == 0)
            start_index = i;

        if (strcmp(nodes[i].name, end) == 0)
            end_index = i;
    }
    if (start_index == -1 || end_index == -1) {
        printf("Start or end node not found\n");
        return;
    }
    int visited[MAX_NODES] = {0};
    int queue[MAX_NODES];
    int parent[MAX_NODES];

    for (int i = 0; i < MAX_NODES; i++) {
        parent[i] = -1;
    }

    int front = 0;
    int back = 0;

    queue[back++] = start_index;
    visited[start_index] = 1;
    parent[start_index] = -1;

    while (front < back){
        int current = queue[front++];
        if (current == end_index)
            break;
        for(int i = 0; i < nodes[current].connection_count; i++){
            int neighbor = nodes[current].connections[i];

            if (!visited[neighbor]){
                visited[neighbor] = 1;
                parent[neighbor] = current;
                queue[back++] = neighbor;
            }
        }
    }

    if (!visited[end_index]){
        printf("No path found\n");
        return;
    }
    printf("Path: ");

    int path[MAX_NODES];
    int length = 0;

    int current = end_index;

    while (current != -1) {
        path[length] = current;
        length++;
        current = parent[current];
    }

    for (int i = length - 1; i >= 0; i--) {
        printf("%s", nodes[path[i]].name);

        if (i > 0)
            printf(" -> ");
    }

    printf("\n");
}

void find_path_length(int max_len){

    for(int start = 0; start < node_count; start++){

        int dist[MAX_NODES];
        int visited[MAX_NODES] = {0};

        for(int i = 0; i < node_count; i++)
            dist[i] = 1000000000;

        dist[start] = 0;

        heap_size = 0;
        heap_push(start,0);

        while(heap_size > 0){

            HeapNode hn = heap_pop();
            int current = hn.node;

            if(visited[current])
                continue;

            visited[current] = 1;

            for(int i = 0; i < nodes[current].connection_count; i++){

                int neighbor = nodes[current].connections[i];
                int weight = nodes[current].weights[i];

                if(dist[current] + weight < dist[neighbor]){

                    dist[neighbor] = dist[current] + weight;

                    heap_push(neighbor, dist[neighbor]);
                }
            }
        }

        for(int i = 0; i < node_count; i++){

            if(i != start && dist[i] < max_len)
                printf("%s -> %s (%d)\n",
                    nodes[start].name,
                    nodes[i].name,
                    dist[i]);
        }
    }
}

void find_pattern(const char *a, const char *b,const char *c){
    int ia = hash_find(a);
    int ib = hash_find(b);
    int ic = hash_find(c);

    if(ia == -1 || ib == -1 || ic == -1){
        printf("Node not found\n");
        return;
    }

    int ab = 0 , bc = 0;

    for(int i = 0; i<nodes[ia].connection_count;i++){
        if(nodes[ia].connections[i]==ib)
            ab = 1;
    }
    for(int i = 0; i<nodes[ib].connection_count;i++){
        if(nodes[ib].connections[i] == ic)
            bc=1;
    }

    if(ab && bc)
        printf("Pattern exists\n");
    else
        printf("Pattern not found\n"); 
    
}

// Understand this funtions properly.

void dijkstra_path(const char *start,const char *end){
    int start_index = -1;
    int end_index = -1;

    for (int i = 0;i < node_count;i++){
        if (strcmp(nodes[i].name,start) == 0)
            start_index = i;
        if (strcmp(nodes[i].name,end) == 0)
            end_index = i;
    }
    if (start_index == -1 || end_index == -1){
        printf("Start or end node not found\n");
        return;
    }

    int dist[MAX_NODES];
    int visited[MAX_NODES] = {0};
    int parent[MAX_NODES];

    for (int i = 0; i< MAX_NODES;i++){
        dist[i] = 1000000000;
        parent[i] = -1;
    }

    dist[start_index] = 0;

    heap_size = 0;
    heap_push(start_index, 0);

    while (heap_size > 0) {

        HeapNode hn = heap_pop();
        int current = hn.node;

        if (visited[current])
            continue;

        visited[current] = 1;

        for (int i = 0; i < nodes[current].connection_count; i++) {

            int neighbor = nodes[current].connections[i];
            int weight = nodes[current].weights[i];

            if (dist[current] + weight < dist[neighbor]) {

                dist[neighbor] = dist[current] + weight;
                parent[neighbor] = current;

                heap_push(neighbor, dist[neighbor]);
            }
        }
    }

    if (dist[end_index] == 1000000000){
        printf("No path found\n");
        return;
    }
    printf("Shortest distance: %d\n", dist[end_index]);

    int path[MAX_NODES];
    int length = 0;
    int current = end_index;

    while (current != -1){
        path[length++] = current;
        current = parent[current];
    }

    printf("Path: ");
    for(int i = length - 1; i >=0; i--){
        printf("%s", nodes[path[i]].name);
        if (i > 0)
            printf(" -> ");
    }
    printf("\n");
}

void traverse_query(const char *start,const char *end){
    int start_index = hash_find(start);
    int end_index = hash_find(end);

    if(start_index == -1 || end_index == -1){
        printf("Node not found \n");
        return;
    }

    int visited[MAX_NODES] = {0};
    int parent[MAX_NODES];

    for(int i = 0;i<MAX_NODES;i++)
        parent[i] = -1;

    int queue[MAX_NODES];
    int front = 0;
    int back = 0;

    queue[back++] = start_index;
    visited[start_index] = 1;

    while(front< back){
        int current = queue[front++];

        if(current == end_index)
            break;
        
        for(int i=0;i<nodes[current].connection_count;i++){
            int neighbor = nodes[current].connections[i];
            if(!visited[neighbor]){
                visited[neighbor] = 1;
                parent[neighbor] =  current;
                queue[back++] = neighbor;
            }
        }
    }
    if(!visited[end_index]){
        printf("No path\n");
        return;
    }

    int path[MAX_NODES];
    int length = 0;
    int current = end_index;

    while(current != -1){
        path[length++] = current;
        current = parent[current];
    }
    printf("Traversal: ");
    for(int i = length -1;i>=0;i--){
        printf("%s",nodes[path[i]].name);
        if(i>0)
            printf(" -> ");
    }
    printf("\n");
}

/* -------- COMMAND PARSER -------- */

void parse_command(char *input) {

    char command[32];

    if (sscanf(input, "%31s", command) != 1)
        return;
    /* node <name> */
    if (strcmp(command, "node") == 0) {

        char name[64];
        char type[32];
        int value;

        if (sscanf(input, "node %63s %31s %d", name , type ,&value) == 3) {
            create_node(name, value,type);
        }

    }
    /* link <a> <b> <weight> */
    else if(strcmp(command,"link") ==0){

        char a[64] , b[64],type[32];
        int weight;
        char dir[16];

        int args = sscanf(input,"link %63s %63s %31s %d %15s",a,b,type,&weight,dir);
        if(args>=4){
            int directed = 0;

            if(args==5 && strcmp(dir,"directed") == 0)
                directed = 1;
            connect_nodes(a,b,type,weight,directed);
        }
    }

    /* show <name> */
    else if (strcmp(command, "show") == 0) {

        char name[64];

        if (sscanf(input, "show %63s", name) == 1) {
            show_node(name);
        }

    }
    /* list */
    else if (strcmp(command,"list") == 0){
        list_nodes();
    }
    /* edges */
    else if (strcmp(command,"edges") == 0){
        list_edges();
    }
    else if(strcmp(command,"stats") == 0){
        graph_stats();
    }
    else if(strcmp(command,"help") == 0){
        printf("Commands:\n");
        printf("  node <name> <type> <value>\n");
        printf("  link <a> <b> <type> <weight> [directed]\n");
        printf("  unlink <a> <b>\n");
        printf("  show <name>\n");
        printf("  list\n");
        printf("  edges\n");
        printf("  stats\n");
        printf("  components\n");
        printf("  degree <name>\n");
        printf("  type <type>\n");
        printf("  find value|degree <op> <num>\n");
        printf("  find path length < <num>\n");
        printf("  path <a> <b>\n");
        printf("  dpath <a> <b>\n");
        printf("  pattern <a> <b> <c>\n");
        printf("  neighbors <name>\n");
        printf("  nfind <name> <max_weight>\n");
        printf("  expand <name> <depth>\n");
        printf("  update <a> <b> <weight>\n");
        printf("  delete <name>\n");
        printf("  save <file>\n");
        printf("  load <file>\n");
        printf("  optimize\n");
        printf("  exit | quit\n");
    }
    else if(strcmp(command,"exit") == 0 || strcmp(command,"quit") == 0){
        cleanup();
        exit(0);
    }
    else if(strcmp(command,"optimize") == 0){
        optimize_memory();
    }
    else if(strcmp(command,"components") == 0){
        graph_components();
    }
    /* degree <name> */
    else if (strcmp(command,"degree") == 0){
        char name[64];
        if (sscanf(input,"degree %63s",name) == 1){
            degree_node(name);
        }
    }
    /* type <name.3> */
    else if (strcmp(command,"pattern") == 0){
        char a[64];
        char b[64];
        char c[64];

        if(sscanf(input,"pattern %63s %63s %63s",a , b ,c) == 3){
            find_pattern(a,b,c);
        }
    }
    /* type <type> */
    else if (strcmp(command,"type") == 0){
        char type[32];
        if (sscanf(input, "type %31s", type) == 1) {
            find_type(type);
        }
    }
    /* delete <name> */
    else if (strcmp(command,"delete") == 0){
        char name[64];

        if (sscanf(input,"delete %63s", name) == 1){
            delete_node(name);
        }
    }
    /* delete <name> */
    else if (strcmp(command,"unlink") == 0){
        char a[64], b[64];
        if(sscanf(input,"unlink %63s %63s",a,b) == 2){
            unlink_nodes(a,b);
        }
    }
    /* neighbors <name> */
    else if (strcmp(command,"neighbors") == 0){
        char name[64];

        if(sscanf(input,"neighbors %63s",name) == 1){
            neighbors_node(name);
        }
    }
    /* update <name> */
    else if (strcmp(command,"update") == 0){
        char a[64], b[64];
        int w;
        if(sscanf(input,"update %63s %63s %d",a,b,&w) == 3){
            update_weight(a,b,w);
        }
    }
    /* expand <name> */
    else if (strcmp(command,"expand") == 0){
        char name[64];
        int depth;

        if(sscanf(input,"expand %63s %d",name ,&depth) ==2 ){
            expand_nodes(name,depth);
        }
    }
    /* find <n/v> */
    else if (strcmp(command,"nfind") == 0){
        char name[64];
        int w;
        if (sscanf(input,"nfind %63s %d",name,&w) == 2){
            find_neighbors(name,w);
        }
    }
    else if(strcmp(command,"find") == 0){

        char field[32];
        char op[4];
        int val;

        if(sscanf(input,"find %31s %3s %d",field,op,&val) == 3){

            if(strcmp(field,"value") == 0)
                find_value(op,val);

            else if(strcmp(field,"degree") == 0)
                find_degree(op,val);

            else
                printf("Unknown field\n");
        }

        else if(sscanf(input,"find path length < %d",&val) == 1){

            find_path_length(val);

        }
    }
    else if(strstr(input,"-> * ->") != NULL){
        char a[64], b[64];
        if(sscanf(input,"%63s -> * -> %63s",a,b)==2){
            traverse_query(a,b);
        }
    }
    /* path <direct/shortest> */
    else if (strcmp(command,"path") == 0){
        char a[64] , b[64];
        if (sscanf(input, "path %63s %63s",a,b) == 2){
            find_path(a,b);
        }
    }
    else if (strcmp(command,"dpath") == 0){
        char a[64], b[64];
        if (sscanf(input,"dpath %63s %63s",a,b) == 2){
            dijkstra_path(a,b);
        }
    }
    /* save <name> */
    else if (strcmp(command,"save") == 0){
        char file[128];
        if (sscanf(input,"save %127s",file) == 1){
            save_graph(file);
        }
    }
    /* load <name> */
    else if (strcmp(command,"load") == 0){
        char file[128];
        if (sscanf(input,"load %127s",file) == 1){
            load_graph(file);
        }
    }
    else {
        printf("Unknown command\n");
    }
}

/* -------- MAIN LOOP -------- */
void save_graph(const char *filename){
    FILE *f = fopen(filename, "wb");
    if(!f){
        printf("Cannot open file\n");
        return;
    }

    fwrite(&node_count, sizeof(int), 1, f);

    for(int i = 0; i < node_count; i++){
        fwrite(nodes[i].name, sizeof(char), 64, f);
        fwrite(nodes[i].type, sizeof(char), 32, f);
        fwrite(&nodes[i].value, sizeof(int), 1, f);
        fwrite(&nodes[i].connection_count, sizeof(int), 1, f);

        for(int j = 0; j < nodes[i].connection_count; j++){
            fwrite(&nodes[i].connections[j], sizeof(int), 1, f);
            fwrite(&nodes[i].weights[j], sizeof(int), 1, f);
            fwrite(nodes[i].types[j], sizeof(char), 32, f);
            fwrite(&nodes[i].directed[j], sizeof(int), 1, f);
        }
    }

    fclose(f);
    printf("Graph saved\n");
}

void load_graph(const char *filename){
    FILE *f = fopen(filename, "rb");
    if(!f){
        printf("File not found\n");
        return;
    }

    cleanup();

    fread(&node_count, sizeof(int), 1, f);

    for(int i = 0; i < node_count; i++){
        fread(nodes[i].name, sizeof(char), 64, f);
        fread(nodes[i].type, sizeof(char), 32, f);
        fread(&nodes[i].value, sizeof(int), 1, f);
        fread(&nodes[i].connection_count, sizeof(int), 1, f);

        nodes[i].capacity = nodes[i].connection_count > 4 ? nodes[i].connection_count : 4;
        nodes[i].connections = malloc(nodes[i].capacity * sizeof(int));
        nodes[i].weights = malloc(nodes[i].capacity * sizeof(int));
        nodes[i].types = malloc(nodes[i].capacity * sizeof(char*));
        nodes[i].directed = malloc(nodes[i].capacity * sizeof(int));

        if (!nodes[i].connections || !nodes[i].weights || !nodes[i].types || !nodes[i].directed) {
            printf("Memory allocation failed\n");
            exit(1);
        }

        for(int j = 0; j < nodes[i].capacity; j++){
            nodes[i].types[j] = malloc(32);
            if (!nodes[i].types[j]) {
                printf("Memory allocation failed\n");
                exit(1);
            }
        }

        for(int j = 0; j < nodes[i].connection_count; j++){
            fread(&nodes[i].connections[j], sizeof(int), 1, f);
            fread(&nodes[i].weights[j], sizeof(int), 1, f);
            fread(nodes[i].types[j], sizeof(char), 32, f);
            fread(&nodes[i].directed[j], sizeof(int), 1, f);
        }

        hash_insert(nodes[i].name, i);
    }

    fclose(f);
    printf("Graph loaded\n");
}

void cleanup_graph(void) {
    for (int i = 0; i < node_count; i++) {
        free(nodes[i].connections);
        free(nodes[i].weights);
        if (nodes[i].types) {
            for (int j = 0; j < nodes[i].capacity; j++) {
                free(nodes[i].types[j]);
            }
            free(nodes[i].types);
        }
        free(nodes[i].directed);

        nodes[i].connections = NULL;
        nodes[i].weights = NULL;
        nodes[i].types = NULL;
        nodes[i].directed = NULL;
    }
    node_count = 0;
}

void free_hash_table(void) {
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode *current = hash_table[i];
        while (current) {
            HashNode *temp = current;
            current = current->next;
            free(temp);
        }
        hash_table[i] = NULL;
    }
}

void optimize_memory(){

    for(int i=0;i<node_count;i++){
        int new_size = nodes[i].connection_count;

        nodes[i].connections =
            realloc(nodes[i].connections,
                    new_size * sizeof(int));

        nodes[i].weights =
            realloc(nodes[i].weights,
                    new_size * sizeof(int));

        nodes[i].directed =
            realloc(nodes[i].directed,
                    new_size * sizeof(int));

        nodes[i].types =
            realloc(nodes[i].types,
                    new_size * sizeof(char*));

        /* Resize the strings for types if we reduced capacity */
        nodes[i].capacity = new_size;
    }

    printf("Memory optimized\n");
}

void cleanup(void) {
    cleanup_graph();
    free_hash_table();
}

int main() {

    char input[MAX_INPUT];

    printf("Graph Language Interpreter\n");
    printf("Type 'help' for available commands, or 'exit' to quit.\n\n");

    while (1) {

        printf("> ");

        if (!fgets(input, sizeof(input), stdin))
            break;

        parse_command(input);
    }

    cleanup();
    return 0;
}
