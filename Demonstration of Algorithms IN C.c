#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


void print_separator(const char *title) {
    printf("\n");
    printf("============================================================\n");
    printf("  %s\n", title);
    printf("============================================================\n");
}

void print_array(const char *label, int arr[], int n) {
    printf("%s: [", label);
    for (int i = 0; i < n; i++)
        printf("%d%s", arr[i], i < n - 1 ? ", " : "");
    printf("]\n");
}

int compare_int(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

/* ============================================================
 *  1. GREEDY ALGORITHMS
 * ============================================================ */

void activity_selection(int start[], int finish[], int n) {
    print_separator("GREEDY Activity Selection");

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (finish[j] > finish[j + 1]) {
                int tmp = finish[j]; finish[j] = finish[j+1]; finish[j+1] = tmp;
                    tmp = start[j];  start[j]  = start[j+1];  start[j+1]  = tmp;
            }
        }
    }

    printf("Activities (start, finish):\n");
    for (int i = 0; i < n; i++)
        printf("  A%d: (%d, %d)\n", i+1, start[i], finish[i]);

    printf("\nSelected activities:\n");
    int last = 0;
    printf("  A1 (finish=%d)\n", finish[0]);
    for (int i = 1; i < n; i++) {
        if (start[i] >= finish[last]) {
            printf("  A%d (start=%d, finish=%d)\n", i+1, start[i], finish[i]);
            last = i;
        }
    }
}


typedef struct { int weight; int value; } Item;

int cmp_ratio(const void *a, const void *b) {
    double ra = (double)((Item *)a)->value / ((Item *)a)->weight;
    double rb = (double)((Item *)b)->value / ((Item *)b)->weight;
    return (rb > ra) - (rb < ra);  
}

void fractional_knapsack(Item items[], int n, int capacity) {
    print_separator("GREEDY Fractional Knapsack");

    qsort(items, n, sizeof(Item), cmp_ratio);

    printf("Items sorted by value/weight ratio:\n");
    printf("  %-8s %-8s %-8s %s\n", "Weight", "Value", "Ratio", "");
    for (int i = 0; i < n; i++)
        printf("  %-8d %-8d %.2f\n", items[i].weight, items[i].value,
               (double)items[i].value / items[i].weight);

    double total = 0.0;
    int remaining = capacity;

    printf("\nPacking knapsack (capacity=%d):\n", capacity);
    for (int i = 0; i < n && remaining > 0; i++) {
        if (items[i].weight <= remaining) {
            total += items[i].value;
            remaining -= items[i].weight;
            printf("  Take full item: weight=%d, value=%d\n",
                   items[i].weight, items[i].value);
        } else {
            double frac = (double)remaining / items[i].weight;
            total += frac * items[i].value;
            printf("  Take %.2f of item: weight=%d, value=%.2f\n",
                   frac, items[i].weight, frac * items[i].value);
            remaining = 0;
        }
    }
    printf("  Maximum value: %.2f\n", total);
}

/* ============================================================
 *  2. DYNAMIC PROGRAMMING
 * ============================================================ */

/* --- 2a. 0/1 Knapsack (bottom-up tabulation) --- */
void knapsack_01(int weights[], int values[], int n, int W) {
    print_separator("DYNAMIC PROGRAMMING 0/1 Knapsack");

    int dp[n + 1][W + 1];

    for (int i = 0; i <= n; i++) {
        for (int w = 0; w <= W; w++) {
            if (i == 0 || w == 0)
                dp[i][w] = 0;
            else if (weights[i-1] <= w)
                dp[i][w] = (values[i-1] + dp[i-1][w - weights[i-1]] > dp[i-1][w])
                            ? values[i-1] + dp[i-1][w - weights[i-1]]
                            : dp[i-1][w];
            else
                dp[i][w] = dp[i-1][w];
        }
    }

    printf("Items:\n");
    for (int i = 0; i < n; i++)
        printf("  Item %d: weight=%d, value=%d\n", i+1, weights[i], values[i]);
    printf("Capacity: %d\n", W);
    printf("Maximum value: %d\n", dp[n][W]);

    printf("Selected items: ");
    int w = W;
    for (int i = n; i > 0; i--) {
        if (dp[i][w] != dp[i-1][w]) {
            printf("Item%d ", i);
            w -= weights[i-1];
        }
    }
    printf("\n");
}

/* --- 2b. Longest Common Subsequence --- */
void lcs(char *X, char *Y) {
    print_separator("DYNAMIC PROGRAMMING Longest Common Subsequence");

    int m = strlen(X), n = strlen(Y);
    int dp[m + 1][n + 1];

    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            if (i == 0 || j == 0)
                dp[i][j] = 0;
            else if (X[i-1] == Y[j-1])
                dp[i][j] = dp[i-1][j-1] + 1;
            else
                dp[i][j] = dp[i-1][j] > dp[i][j-1] ? dp[i-1][j] : dp[i][j-1];
        }
    }

    /* Reconstruct LCS string */
    int len = dp[m][n];
    char lcs_str[len + 1];
    lcs_str[len] = '\0';
    int i = m, j = n;
    while (i > 0 && j > 0) {
        if (X[i-1] == Y[j-1]) {
            lcs_str[--len] = X[i-1];
            i--; j--;
        } else if (dp[i-1][j] > dp[i][j-1])
            i--;
        else
            j--;
    }

    printf("  X = \"%s\"\n", X);
    printf("  Y = \"%s\"\n", Y);
    printf("  LCS = \"%s\" (length %d)\n", lcs_str, dp[m][n]);
}

/* ============================================================
 *  3. DIVIDE AND CONQUER
 * ============================================================ */

/* --- 3a. Merge Sort --- */
void merge(int arr[], int left, int mid, int right) {
    int n1 = mid - left + 1, n2 = right - mid;
    int L[n1], R[n2];

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2)
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void merge_sort(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(arr, left, mid);
        merge_sort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void demo_merge_sort(void) {
    print_separator("DIVIDE & CONQUER Merge Sort");
    int arr[] = {38, 27, 43, 3, 9, 82, 10};
    int n = sizeof(arr) / sizeof(arr[0]);
    print_array("Before", arr, n);
    merge_sort(arr, 0, n - 1);
    print_array("After ", arr, n);
    printf("Time Complexity: O(n log n) | Space: O(n)\n");
}

/* --- 3b. Binary Search --- */
int binary_search(int arr[], int n, int target) {
    int low = 0, high = n - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid] == target) return mid;
        else if (arr[mid] < target) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

void demo_binary_search(void) {
    print_separator("DIVIDE & CONQUER Binary Search");
    int arr[] = {2, 5, 8, 12, 16, 23, 38, 56, 72, 91};
    int n = sizeof(arr) / sizeof(arr[0]);
    int target = 23;
    print_array("Array", arr, n);
    int idx = binary_search(arr, n, target);
    if (idx != -1)
        printf("Found %d at index %d\n", target, idx);
    else
        printf("%d not found\n", target);
}

/* ============================================================
 *  4. BACKTRACKING
 * ============================================================ */

/* --- 4a. N-Queens Problem --- */
#define QUEENS 6

int queens_board[QUEENS][QUEENS];
int queens_solutions = 0;

int is_safe_queen(int row, int col) {
    for (int i = 0; i < row; i++)
        if (queens_board[i][col]) return 0;
    for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--)
        if (queens_board[i][j]) return 0;
    for (int i = row - 1, j = col + 1; i >= 0 && j < QUEENS; i--, j++)
        if (queens_board[i][j]) return 0;
    return 1;
}

void solve_queens(int row) {
    if (row == QUEENS) {
        queens_solutions++;
        if (queens_solutions <= 2) {  
            printf("\n  Solution %d:\n", queens_solutions);
            for (int i = 0; i < QUEENS; i++) {
                printf("  ");
                for (int j = 0; j < QUEENS; j++)
                    printf("%s ", queens_board[i][j] ? "Q" : ".");
                printf("\n");
            }
        }
        return;
    }
    for (int col = 0; col < QUEENS; col++) {
        if (is_safe_queen(row, col)) {
            queens_board[row][col] = 1;
            solve_queens(row + 1);
            queens_board[row][col] = 0;  
        }
    }
}

void demo_n_queens(void) {
    print_separator("BACKTRACKING N-Queens (N=6)");
    memset(queens_board, 0, sizeof(queens_board));
    queens_solutions = 0;
    solve_queens(0);
    printf("\n  Total solutions for %d-Queens: %d\n", QUEENS, queens_solutions);
}

/* --- 4b. Subset Sum --- */
int subset_target;
int subset_arr[10];
int subset_n;
int subset_found = 0;

void find_subset(int idx, int current, int chosen[], int chosen_n) {
    if (current == subset_target) {
        if (!subset_found) {
            printf("  Subset found: {");
            for (int i = 0; i < chosen_n; i++)
                printf("%d%s", chosen[i], i < chosen_n - 1 ? ", " : "");
            printf("}\n");
        }
        subset_found++;
        return;
    }
    if (idx == subset_n || current > subset_target) return;

    chosen[chosen_n] = subset_arr[idx];
    find_subset(idx + 1, current + subset_arr[idx], chosen, chosen_n + 1);
    find_subset(idx + 1, current, chosen, chosen_n);
}

void demo_subset_sum(void) {
    print_separator("BACKTRACKING Subset Sum");
    int arr[] = {3, 7, 1, 8, 2, 6};
    int n = 6, target = 14;
    subset_n = n; subset_target = target;
    memcpy(subset_arr, arr, n * sizeof(int));
    subset_found = 0;

    print_array("Set   ", arr, n);
    printf("Target: %d\n", target);

    int chosen[10];
    find_subset(0, 0, chosen, 0);
    printf("  Total subsets found: %d\n", subset_found);
}

/* ============================================================
 *  5. GRAPH ALGORITHMS
 * ============================================================ */

#define VERTICES 6
#define INF INT_MAX

/* Adjacency matrix representation */
int graph[VERTICES][VERTICES] = {
    /*  0   1   2   3   4   5  */
    {   0,  4,  0,  0,  0,  8  },  
    {   4,  0,  8,  0,  0, 11  },  
    {   0,  8,  0,  7,  0,  0  },  
    {   0,  0,  7,  0,  9,  0  },  
    {   0,  0,  0,  9,  0, 10  },  
    {   8, 11,  0,  0, 10,  0  },  
};

/* --- 5a. BFS --- */
void bfs(int start) {
    print_separator("GRAPH Breadth-First Search (BFS)");
    int visited[VERTICES] = {0};
    int queue[VERTICES], front = 0, rear = 0;

    visited[start] = 1;
    queue[rear++] = start;

    printf("BFS traversal from vertex %d: ", start);
    while (front < rear) {
        int v = queue[front++];
        printf("%d ", v);
        for (int i = 0; i < VERTICES; i++) {
            if (graph[v][i] && !visited[i]) {
                visited[i] = 1;
                queue[rear++] = i;
            }
        }
    }
    printf("\n");
}

/* --- 5b. DFS --- */
int dfs_visited[VERTICES];

void dfs_recursive(int v) {
    dfs_visited[v] = 1;
    printf("%d ", v);
    for (int i = 0; i < VERTICES; i++)
        if (graph[v][i] && !dfs_visited[i])
            dfs_recursive(i);
}

void dfs(int start) {
    print_separator("GRAPH Depth-First Search (DFS)");
    memset(dfs_visited, 0, sizeof(dfs_visited));
    printf("DFS traversal from vertex %d: ", start);
    dfs_recursive(start);
    printf("\n");
}

/* --- 5c. Dijkstra's Shortest Path --- */
int min_distance(int dist[], int visited[], int n) {
    int min = INF, min_idx = -1;
    for (int v = 0; v < n; v++)
        if (!visited[v] && dist[v] <= min) {
            min = dist[v]; min_idx = v;
        }
    return min_idx;
}

void dijkstra(int src) {
    print_separator("GRAPH Dijkstra's Shortest Path");

    int dist[VERTICES], visited[VERTICES], prev[VERTICES];
    for (int i = 0; i < VERTICES; i++) {
        dist[i] = INF; visited[i] = 0; prev[i] = -1;
    }
    dist[src] = 0;

    for (int count = 0; count < VERTICES - 1; count++) {
        int u = min_distance(dist, visited, VERTICES);
        visited[u] = 1;
        for (int v = 0; v < VERTICES; v++) {
            if (!visited[v] && graph[u][v] && dist[u] != INF
                && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                prev[v] = u;
            }
        }
    }

    printf("Shortest distances from vertex %d:\n\n", src);
    printf("  %-10s %-10s %s\n", "Dest", "Distance", "Path");
    printf("  %-10s %-10s %s\n", "----", "--------", "----");

    for (int v = 0; v < VERTICES; v++) {
        if (v == src) continue;

        /* Reconstruct path */
        int path[VERTICES], path_len = 0, cur = v;
        while (cur != -1) { path[path_len++] = cur; cur = prev[cur]; }

        printf("  %-10d %-10d ", v, dist[v]);
        for (int i = path_len - 1; i >= 0; i--)
            printf("%d%s", path[i], i > 0 ? " -> " : "");
        printf("\n");
    }
}

/* ============================================================
 *  MAIN — Run all demonstrations
 * ============================================================ */

int main(void) {
    printf("\n");
    printf("************************************************************\n");
    printf("*          Demonstration of ALGORITHMS IN C       *\n");
    printf("************************************************************\n");

    /* ---------- 1. GREEDY ---------- */
    int start[]  = {1, 3, 0, 5, 8, 5};
    int finish[] = {2, 4, 6, 7, 9, 9};
    activity_selection(start, finish, 6);

    Item items[] = {{10, 60}, {20, 100}, {30, 120}};
    fractional_knapsack(items, 3, 50);

    /* ---------- 2. DYNAMIC PROGRAMMING ---------- */
    int weights[] = {2, 3, 4, 5};
    int values[]  = {3, 4, 5, 6};
    knapsack_01(weights, values, 4, 8);

    lcs("ABCBDAB", "BDCABA");

    /* ---------- 3. DIVIDE & CONQUER ---------- */
    demo_merge_sort();
    demo_binary_search();

    /* ---------- 4. BACKTRACKING ---------- */
    demo_n_queens();
    demo_subset_sum();

    /* ---------- 5. GRAPH ALGORITHMS ---------- */
    bfs(0);
    dfs(0);
    dijkstra(0);

    printf("\n************************************************************\n");
    printf("*                    All algorithms done!                  *\n");
    printf("************************************************************\n\n");

    return 0;
}
