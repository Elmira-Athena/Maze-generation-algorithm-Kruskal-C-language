#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Định nghĩa kích thước mê cung
#define WIDTH 43
#define HEIGHT 43

// Cấu trúc cho một ô trong mê cung
typedef struct {
    int row;
    int col;
} Cell;

// Cấu trúc cho một cạnh (tường)
typedef struct {
    Cell c1;
    Cell c2;
} Edge;

// Hàm tạo một mê cung rỗng
void initializeMaze(char maze[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i % 2 == 0 || j % 2 == 0) {
                maze[i][j] = '#'; // Tường
            } else {
                maze[i][j] = ' '; // Đường đi
            }
        }
    }
}

// Hàm in mê cung
void printMaze(char maze[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            printf("%c ", maze[i][j]);
        }
        printf("\n");
    }
}

// Cấu trúc cho tập hợp rời rạc (Disjoint Set Union)
typedef struct {
    int parent[HEIGHT * WIDTH / 2]; // Số lượng ô là (HEIGHT-1)/2 * (WIDTH-1)/2
} DisjointSet;

// Hàm khởi tạo tập hợp rời rạc
void ds_init(DisjointSet *ds, int numCells) {
    for (int i = 0; i < numCells; i++) {
        ds->parent[i] = i;
    }
}

// Hàm tìm đại diện của tập hợp
int ds_find(DisjointSet *ds, int i) {
    if (ds->parent[i] == i) {
        return i;
    }
    return ds->parent[i] = ds_find(ds, ds->parent[i]); // Path compression
}

// Hàm hợp nhất hai tập hợp
void ds_union(DisjointSet *ds, int i, int j) {
    int root_i = ds_find(ds, i);
    int root_j = ds_find(ds, j);
    if (root_i != root_j) {
        ds->parent[root_i] = root_j;
    }
}

int main() {
    char maze[HEIGHT][WIDTH];
    initializeMaze(maze);

    // Số lượng ô (không tính tường)
    int numCells = (HEIGHT - 1) / 2 * (WIDTH - 1) / 2;
    DisjointSet ds;
    ds_init(&ds, numCells);

    // Tạo danh sách các cạnh (tường giữa các ô)
    Edge edges[2 * numCells - (HEIGHT - 1) / 2 - (WIDTH - 1) / 2]; // Ước tính số cạnh
    int edgeCount = 0;

    for (int i = 1; i < HEIGHT - 1; i += 2) {
        for (int j = 1; j < WIDTH - 1; j += 2) {
            // Thêm cạnh phía đông
            if (j < WIDTH - 2) {
                edges[edgeCount].c1 = (Cell){i, j};
                edges[edgeCount].c2 = (Cell){i, j + 2};
                edgeCount++;
            }
            // Thêm cạnh phía nam
            if (i < HEIGHT - 2) {
                edges[edgeCount].c1 = (Cell){i, j};
                edges[edgeCount].c2 = (Cell){i + 2, j};
                edgeCount++;
            }
        }
    }

    // Trộn ngẫu nhiên danh sách các cạnh
    srand(time(NULL));
    for (int i = edgeCount - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Edge temp = edges[i];
        edges[i] = edges[j];
        edges[j] = temp;
    }

    // Thuật toán Kruskal
    int cellsMerged = 0;
    while (cellsMerged < numCells - 1 && edgeCount > 0) {
        Edge currentEdge = edges[edgeCount - 1];
        edgeCount--;

        // Tính chỉ số của hai ô liên quan đến cạnh
        int cell1_index = ((currentEdge.c1.row - 1) / 2) * ((WIDTH - 1) / 2) + (currentEdge.c1.col - 1) / 2;
        int cell2_index = ((currentEdge.c2.row - 1) / 2) * ((WIDTH - 1) / 2) + (currentEdge.c2.col - 1) / 2;

        // Tìm tập hợp của hai ô
        int set1 = ds_find(&ds, cell1_index);
        int set2 = ds_find(&ds, cell2_index);

        // Nếu hai ô thuộc các tập hợp khác nhau, phá bỏ bức tường và hợp nhất các tập hợp
        if (set1 != set2) {
            // Phá bỏ bức tường
            int row_wall = (currentEdge.c1.row + currentEdge.c2.row) / 2;
            int col_wall = (currentEdge.c1.col + currentEdge.c2.col) / 2;
            maze[row_wall][col_wall] = ' ';

            ds_union(&ds, set1, set2);
            cellsMerged++;
        }
    }

    // In mê cung hoàn hảo
    printMaze(maze);

    return 0;
}
