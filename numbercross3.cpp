#include <cstdio>
#include <utility>
#include <map>

using namespace std;

int rowneed[32], colneed[32];
map<int, int> rowmap[32], colmap[32];
int rowsums[32], colsums[32];
int rowmask[15], colmask[15];
bool rowneg[15], colneg[15];
bool blocked[15][15];
int grid[15][15];
int cntrow[32], cntcol[32];
int n = 10;
pair<int, int> tmp;
map<int, int> rows[32], cols[32];

pair<int, int> next(int row, int col) {
    if (col == n - 1)
        return make_pair(row + 1, 0);
    return make_pair(row, col + 1);
}

inline bool validate(int row, int col) {
    int auxrow = rows[row][col];
    int auxcol = cols[row][col];
    int rowsum = rowsums[auxrow];
    int colsum = colsums[auxcol];
    if (rowsum - 9 > rowneed[rowmap[row][col]] || rowsum + cntrow[auxrow] * 9 < rowneed[rowmap[row][col]]) {
        return false;
    }
    if (colsum - 9 > colneed[colmap[col][row]] || colsum + cntcol[auxcol] * 9 < colneed[colmap[col][row]]) {
        return false;
    }
    if (col == n - 1 || blocked[row][col+1]) {
        if (rowsum != rowneed[rowmap[row][col]])
            return false;
    }
    if (row == n - 1 || blocked[row+1][col]) {
        if (colsum != colneed[colmap[col][row]])
            return false;
    }
    return true;
}

inline void printBoard() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%2d ", grid[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

inline void backtrack(int row, int col) {
    // printBoard();
    if (row == n) {
        printBoard();
        return;
    }
    tmp = next(row, col);
    if (blocked[row][col]) {
        backtrack(tmp.first, tmp.second);
        return;
    }
    for (int i = 1; i <= 9; i++) {
        if ((((rowmask[row] >> i) & 1) == 0) &&
            (((colmask[col] >> i) & 1) == 0)) {
            bool canneg = (!rowneg[row]) && (!colneg[col]);
            int auxrow = rows[row][col];
            int auxcol = cols[row][col];
            // Try positive digit 
            grid[row][col] = i;
            tmp = next(row, col);
            rowsums[auxrow] += i;
            colsums[auxcol] += i;
            cntrow[auxrow]--;
            cntcol[auxcol]--;
            rowmask[row] ^= (1 << i);
            colmask[col] ^= (1 << i);
            if (validate(row, col)) {
                backtrack(tmp.first, tmp.second);
            }
            rowmask[row] ^= (1 << i);
            colmask[col] ^= (1 << i);
            rowsums[auxrow] -= i;
            colsums[auxcol] -= i;
            cntrow[auxrow]++;
            cntcol[auxcol]++;
            grid[row][col] = 0;
            // Try negative digit
            if (canneg) {
                grid[row][col] = -i;
                tmp = next(row, col);
                rowneg[row] = true;
                colneg[col] = true;
                rowmask[row] ^= (1 << i);
                colmask[col] ^= (1 << i);
                rowsums[auxrow] -= i;
                colsums[auxcol] -= i;
                cntrow[auxrow]--;
                cntcol[auxcol]--;
                if (validate(row, col)) {
                    backtrack(tmp.first, tmp.second);
                }
                rowneg[row] = false;
                colneg[col] = false;
                rowmask[row] ^= (1 << i);
                colmask[col] ^= (1 << i);
                rowsums[auxrow] += i;
                colsums[auxcol] += i;
                cntrow[auxrow]++;
                cntcol[auxcol]++;
                grid[row][col] = 0;
            }
        }
    }
}

int main() {
    blocked[0][3] = blocked[0][4] = blocked[0][5] = true;
    blocked[1][3] = true;
    blocked[2][3] = blocked[2][n-1] = true;
    blocked[3][n-1] = true;
    blocked[4][4] = blocked[4][7] = blocked[4][8] = blocked[4][9] = true;
    blocked[5][0] = blocked[5][1] = blocked[5][2] = blocked[5][5] = true;
    blocked[6][0] = true;
    blocked[7][0] = blocked[7][6] = true;
    blocked[8][6] = true;
    blocked[9][4] = blocked[9][5] = blocked[9][6] = true;

    rowmap[0][2] = 1;
    rowmap[0][n-1] = 4;
    rowmap[1][2] = 8;
    rowmap[1][n-1] = 9;
    rowmap[2][2] = 11;
    rowmap[2][n-2] = 12;
    rowmap[3][n-2] = 13;
    rowmap[4][3] = 15;
    rowmap[4][n-4] = 16;
    rowmap[5][4] = 17;
    rowmap[5][n-1] = 19;
    rowmap[6][n-1] = 23;
    rowmap[7][5] = 26;
    rowmap[7][n-1] = 27;
    rowmap[8][5] = 28;
    rowmap[8][n-1] = 29;
    rowmap[9][3] = 30;
    rowmap[9][n-1] = 31;

    colmap[0][4] = 1;
    colmap[0][n-1] = 28;
    colmap[1][4] = 2;
    colmap[1][n-1] = 23;
    colmap[2][4] = 3;
    colmap[2][n-1] = 24;
    colmap[3][n-1] = 14;
    colmap[4][3] = 9;
    colmap[4][n-2] = 18;
    colmap[5][4] = 10;
    colmap[5][n-2] = 25;
    colmap[6][n-4] = 4;
    colmap[7][3] = 5;
    colmap[7][n-1] = 20;
    colmap[8][3] = 6;
    colmap[8][n-1] = 21;
    colmap[9][1] = 7;
    colmap[9][n-1] = 22;

    rowneed[1] = 24;
    rowneed[4] = 14;
    rowneed[8] = 10;
    rowneed[9] = 25;
    rowneed[11] = 0;
    rowneed[12] = 19;
    rowneed[13] = 41;
    rowneed[15] = 23;
    rowneed[16] = 6;
    rowneed[17] = -8;
    rowneed[19] = 14;
    rowneed[23] = 39;
    rowneed[26] = 15;
    rowneed[27] = 17;
    rowneed[28] = 17;
    rowneed[29] = 22;
    rowneed[30] = 8;
    rowneed[31] = 10;

    colneed[1] = 17;
    colneed[2] = 6;
    colneed[3] = 35;
    colneed[4] = 32;
    colneed[5] = 13;
    colneed[6] = 30;
    colneed[7] = 8;
    colneed[9] = 8;
    colneed[10] = 19;
    colneed[14] = 30;
    colneed[18] = 4;
    colneed[20] = 22;
    colneed[21] = 9;
    colneed[22] = 30;
    colneed[23] = 21;
    colneed[24] = 4;
    colneed[25] = 7;
    colneed[28] = 1;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (rowmap[i][j]) {
                int temp = rowmap[i][j];
                for (int k = j; k >= 0 && !blocked[i][k]; k--) {
                    rows[i][k] = temp;
                    rowmap[i][k] = temp;
                    cntrow[temp]++;
                }
            }
            if (colmap[i][j]) {
                int temp = colmap[i][j];
                for (int k = j; k >= 0 && !blocked[k][i]; k--) {
                    cols[k][i] = temp;
                    colmap[i][k] = temp;
                    cntcol[temp]++;
                }
            }
        }
    }
    backtrack(0, 0);
    return 0;
}
