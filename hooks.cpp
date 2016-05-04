#include <cstdio>
#include <vector>
#include <algorithm>

using namespace std;

int rsumsreq[] = {45, 44, 4, 48, 7, 14, 47, 43, 33};
int csumsreq[] = {36, 5, 47, 35, 17, 30, 21, 49, 45};

int rsums[9], csums[9];
int dr[] = {1, 1, -1, -1};
int dc[] = {1, -1, -1, 1};
int ar[] = {1, 1, 0, 0};
int ac[] = {1, 0, 0, 1};

vector<int> bitmasks[10][10];
int board[10][10];

inline void printboard(int x, int y) {
	printf("==Board==\n");
	printf("%d %d\n", x, y);
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			printf("%5d ", board[i][j]);
		}
		printf("%2d,%2d", rsums[i], rsumsreq[i]);
		printf("\n");
	}
	for (int i = 0; i < 9; i++)
		printf("%2d,%2d ", csums[i], csumsreq[i]);
	printf("\n");
}

inline void backtrack(int depth, int x, int y) {
	if (depth == 8) {
		board[x][y] = 1;
		rsums[x] += 1;
		csums[y] += 1;
		bool success = true;
		for (int i = 0; i < 9; i++) {
			success &= (rsums[i] == rsumsreq[i]);
			success &= (csums[i] == csumsreq[i]);
		}
		printf("success %d\n", success);
		printboard(x, y);
		rsums[x] -= 1;
		csums[y] -= 1;
		board[x][y] = 0;
		return;
	}

	int val = 9 - depth;
	vector<int> configs(4, 0);
	configs[1] = 1; configs[2] = 2; configs[3] = 3;
	random_shuffle(configs.begin(), configs.end());

	for (int iconfig = 0; iconfig < 4; iconfig++) {
		int config = configs[iconfig];
		int row = x, col = y;
		if (config == 2 || config == 3)
			row += (val - 1);
		if (config == 1 || config == 2)
			col += (val - 1);

		for (int rownum = 1; rownum <= val; rownum++) {
			int colnum = val - rownum;
			for (int i = 0; i < (int)bitmasks[val][rownum].size(); i++) {
				int rowmask = bitmasks[val][rownum][i];
				for (int j = 0; j < (int)bitmasks[val-1][colnum].size(); j++) {
					int colmask = bitmasks[val-1][colnum][j]; // starts at 1 position below

					bool valid = true;
					// validate masks
					for (int j = 0; j < val; j++) {
						if (j < val - 1 && ((colmask >> j) & 1) == 1) {
							valid &= (rsums[row + (1 + j) * dr[config]] + val <= rsumsreq[row + (1 + j) * dr[config]]);
							valid &= (csums[col] + val <= csumsreq[col]);
							rsums[row + (1 + j) * dr[config]] += val;
							csums[col] += val;

							board[row + (1 + j) * dr[config]][col] = val;
						}
						if (((rowmask >> j) & 1) == 1) {
							valid &= (rsums[row] + val <= rsumsreq[row]);
							valid &= (csums[col + j * dc[config]] + val <= csumsreq[col + j * dc[config]]);
							rsums[row] += val;
							csums[col + j * dc[config]] += val;

							board[row][col + j * dc[config]] = val;
						}
					}

					if (rsums[row] != rsumsreq[row])
						valid = false;
					if (csums[col] != csumsreq[col])
						valid = false;

					if (valid) {
						backtrack(depth + 1, x + ar[config], y + ac[config]);
					}

					for (int j = 0; j < val; j++) {
						if (j < val - 1 && ((colmask >> j) & 1) == 1) {
							rsums[row + (1 + j) * dr[config]] -= val;
							csums[col] -= val;

							board[row + (1 + j) * dr[config]][col] = 0;
						}
						if (((rowmask >> j) & 1) == 1) {
							rsums[row] -= val;
							csums[col + j * dc[config]] -= val;

							board[row][col + j * dc[config]] = 0;
						}
					}
				}
			}
		}
	}
}

inline void generate_bitmasks() {
	for (int i = 1; i <= 9; i++) {
		for (int j = 0; j < (1 << i); j++) {
			bitmasks[i][__builtin_popcount(j)].push_back(j);
		}
	}
}

bool rflag[9];
vector<pair<int, int> > columns[9];
vector<pair<int, int> > st;

inline int backtrack(int col) {
	if (col == 9) {
		int prod = 1;
		for (int i = 0; i < static_cast<int>(st.size()); i++) {
			prod *= st[i].second;
			// printf("(%d, %d)->%d ", st[i].first, i, st[i].second);
		} 
		return prod;
	}

	int ret = 0;
	for (int i = 0; i < static_cast<int>(columns[col].size()); i++) {
		int row = columns[col][i].first;
		if (!rflag[row]) {
			rflag[row] = true;
			st.push_back(columns[col][i]);
			ret = max(ret, backtrack(col + 1));
			st.pop_back();
			rflag[row] = false;
		}
	}
	return ret;
}

int main() {
	// generate_bitmasks();
	// backtrack(0, 0, 0);

	int arr[9][9] = {
		{9,0,9,0,0,9,0,9,9},
		{0,0,5,5,5,5,7,8,9},
		{0,0,0,4,0,0,0,0,0},
		{6,5,4,3,3,3,7,8,9},
		{0,0,4,2,1,0,0,0,0},
		{0,0,4,0,2,0,0,8,0},
		{6,0,6,6,6,6,0,8,9},
		{7,0,7,7,0,7,7,8,0},
		{8,0,8,8,0,0,0,0,9}
	};
	
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (arr[i][j]) {
				columns[j].push_back(make_pair(i, arr[i][j]));
			}
		}
	}
	
	int prod = backtrack(0);
	printf("%d\n", prod);
	return 0;
}