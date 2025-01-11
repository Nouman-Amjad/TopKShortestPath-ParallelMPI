/*
Muhammad Nouman Amjad (21I-0853)
    Moeez Muslim (21I-0490)
           E
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <omp.h>
#include <mpi.h>
#include <time.h>

#define V 100

int minDistance(int dist[], bool sptSet[]) {
    int min = INT_MAX, min_index;

    for (int v = 0; v < V; v++) {
        if (!sptSet[v] && dist[v] <= min) {
            min = dist[v];
            min_index = v;
        }
    }

    return min_index;
}

int* findPath(int parent[], int dest, int* pathLength) {
    int* path = (int*)malloc(V * sizeof(int));
    int index = 0;
    int current = dest;

    while (current != -1) {
        path[index++] = current;
        current = parent[current];
    }

    *pathLength = index;
    return path;
}

int dijkstra(int graph[V][V], int src, int dest, int** path, int* pathLength) {
    int dist[V];
    bool sptSet[V];
    int parent[V];

    for (int i = 0; i < V; i++) {
        dist[i] = INT_MAX;
        sptSet[i] = false;
        parent[i] = -1;
    }

    dist[src] = 0;

    for (int count = 0; count < V - 1; count++) {
        int u = minDistance(dist, sptSet);

        sptSet[u] = true;

        for (int v = 0; v < V; v++) {
            if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }
    *path = findPath(parent, dest, pathLength);
    return dist[dest];
}

int main(int argc, char** argv) {

    clock_t st, et;

    int graph[V][V] = {0};

    FILE *file = fopen("email.txt", "r");
    if (file == NULL) {
        printf("Error: Unable to open file\n");
        exit(1);
    }

    int start = 0;
    int end = start + V;
    printf("%d,%d \n", start, end);
    int r, c; 
    while (fscanf(file, "%d\t%d\n", &r, &c) != EOF) {
        if ((r >= start && r < end) && (c >= start && c < end)) {
            graph[r - start][c - start] = 1;
        }
        if(r-1 == end)
        break;
    }

    fclose(file);

    bool directed = false;
    int k = 3;
    int pathnumber = 1;
    int dest = V - 1; 
    int* path;
    int pathLength;
    int cost;
    int new_path;

    st = clock();
    cost = dijkstra(graph, 0, dest, &path, &pathLength);

    printf("\n%d Shortest Path:\n", pathnumber++);
    for (int i = pathLength - 1; i >= 0; i--)
        printf("%d ", path[i]);
    printf("\nCost : %d\n\n\n", cost);

    for (int p = 1; p<k; p++)
    {
        int min_cost[pathLength];
        int pl[pathLength];
        int* newpath[pathLength];
        for(int i = 0; i<pathLength-1; i++){
            int g[V][V]= {0};
            copy(graph, g);
            if (directed)
                g[path[i]][path[i + 1]] = 0;
            else
                g[path[i]][path[i + 1]] = g[path[i + 1]][path[i]] = 0;
            min_cost[i] = dijkstra(g, 0, dest, &newpath[i], &pl[i]);

        }
        int newcost = 123123123;
        int rank = -1;
        for(int i = 0; i<pathLength-1; i++){
            if(newcost > min_cost[i]){
                newcost = min_cost[i];
                rank = i;
            }
        }
        if (rank != -1) 
        {
            printf("Minimum cost of the path from Source to Destination: %d,%d\n", rank, cost);
            printf("Processes with minimum cost: %d\n", rank);
        }
        else
            break;
        /*for (int y = 0; y<pathLength-1; y++){
            for (int i = pl[y] - 1; i >= 0; i--)
                printf("%d ", newpath[rank][i]);
            printf("\n");
        }*/
        graph[path[rank]][path[rank + 1]] = graph[path[rank + 1]][path[rank]] = 0;
        pathLength = pl[rank];
        cost = newcost;
        printf("\n%d Shortest Path:\n", pathnumber++);
        for (int i = pathLength - 1; i >= 0; i--){
            path[i] = newpath[rank][i];
            printf("%d ", path[i]);
        }
        printf("\nCost : %d\n\n\n", cost);
	
        if(pathLength == 1)
        {
            printf("\nNo more Shortest Paths\n\n");

            break;
        }
        
    }
    et = clock();
    double ts = ((double)(et - st)) / CLOCKS_PER_SEC;
    printf("\nTime Taken by Serial Execution: %f s\n\n", ts);

    free(path); 

    return 0;
}

