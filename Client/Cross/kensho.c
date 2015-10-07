#include<stdio.h>
#include<string.h>

#define n 16
#define len 4

#define rows (n*n*n)
#define cols (n*n*len)
#define nodes (rows*cols)

#define cell_n(r, c) (n*n*3+((r)*n)+(c))
#define row_n(r, x) ((r)*n+(x))
#define col_n(c, x) (n*n+(c)*n+(x))
#define box_n(b, x) (n*n*2+(b)*n+(x))

typedef struct __dancing_links_node {
  int row, col, num;
  struct __dancing_links_node *left, *right, *up, *down;
} dlx_node;

dlx_node head;
dlx_node row[rows+10], col[cols+10];
dlx_node pool[nodes+2000];
int node_count;
int count[cols+10];
int solution[rows+10];
char input[n][n+2];

void dlx_init()
{
  int i;
  memset(count, 0, sizeof(count));
  node_count = 0;
  head.row = head.col = 0;
  head.left = head.right = head.up = head.down = &head;
  for(i = 0; i < cols; ++i) {
    col[i].left = head.left;
    head.left = &col[i];
    col[i].right = &head;
    col[i].num = 0;
    col[i].col = i;
    col[i].up = col[i].down = &col[i];
    col[i].left->right = &col[i];
    col[i].right->left = &col[i];
  }
  for(i = 0; i < rows; ++i) {
    row[i].up = head.up;
    head.up = &row[i];
    row[i].row = i;
    row[i].down = &head;
    row[i].left = row[i].right = &row[i];
    row[i].up->down = &row[i];
    row[i].down->up = &row[i];
  }
}

void dlx_link(int r, int c, int num, int ci)
{
  int ri = r*n*n + c*n + num;
  count[ci]++;
  pool[node_count].row = ri;
  pool[node_count].col = ci;
  pool[node_count].up = &col[ci];
  pool[node_count].down = col[ci].down;
  col[ci].down->up = &pool[node_count];
  col[ci].down = &pool[node_count];
  pool[node_count].left = &row[ri];
  pool[node_count].right = row[ri].right;
  row[ri].right->left = &pool[node_count];
  row[ri].right = &pool[node_count];
  node_count++;
}

void delete(int ci)
{
  dlx_node *p, *q;

  // delete a column
  col[ci].left->right = col[ci].right;
  col[ci].right->left = col[ci].left;

  // delete rows
  for(p = col[ci].down; p != &col[ci]; p = p->down) {
    for(q = p->right; q != p; q = q->right) {
      count[q->col]--;
      q->up->down = q->down;
      q->down->up = q->up;
    }
    p->left->right = p->right;
    p->right->left = p->left;
  }
}

void restore(int ci)
{
  dlx_node *p, *q;

  // restore rows
  for(p = col[ci].down; p != &col[ci]; p = p->down) {
    p->left->right = p;
    p->right->left = p;
    for(q = p->right; q != p; q = q->right) {
      q->up->down = q;
      q->down->up = q;
      count[q->col]++;
    }
  }

  // restore a column
  col[ci].left->right = &col[ci];
  col[ci].right->left = &col[ci];
}

// Knuth's Algorithm X
int solve(int depth)
{
  dlx_node *p, *q;
  int selected_col, min;
  // matrix is empty, the problem is solved
  if(head.right == &head)
    return 1;

  // choose a column
  min = 2*n;
  for(p = head.right; p != &head; p = p->right) {
    if(min > count[p->col]) {
      min = count[p->col];
      selected_col = p->col;
      if(min <= 1) break;
    }
  }
  if(min == 0)
    return 0;

  // choose a row
  delete(selected_col);
  for(p = col[selected_col].down; p != &col[selected_col]; p = p->down) {
    // include choosed row in the partial solution
    solution[depth] = p->row;

    // delete columns
    p->left->right = p;
    for(q = p->right; q != p; q = q->right) {
      delete(q->col);
    }
    p->left->right = p->right;

    // repeat this algorithm recursively
    if(solve(depth + 1))
      return 1;

    // restore columns
    p->right->left = p;
    for(q = p->left; q != p; q = q->left) {
      restore(q->col);
    }
    p->right->left = p->left;
  }
  restore(selected_col);
  return 0;
}

void output()
{
  int i;
  for(i = 0; i < n*n; ++i) {
    int r, c, num;
    r = solution[i] / (n*n);
    c = solution[i] / n % n;
    num = solution[i] % n;
    input[r][c] = num + 'A';
  }
  for(i = 0; i < n; ++i)
    puts(input[i]);
  puts("");
}

int main()
{
  while(~scanf("%s", input[0])) {
    int i, j, k;
    for(i = 1; i < n; ++i)
      scanf("%s", input[i]);
    dlx_init();
    for(i = 0; i < n; ++i) {
      for(j = 0; j < n; ++j) {
	if(input[i][j] == '-') {
	  for(k = 0; k < n; ++k) {
	    dlx_link(i, j, k, cell_n(i, j));
	    dlx_link(i, j, k, row_n(i, k));
	    dlx_link(i, j, k, col_n(j, k));
	    dlx_link(i, j, k, box_n(i/len*len+j/len, k));
	  }
	} else {
	  int x = input[i][j] - 'A';
	  dlx_link(i, j, x, cell_n(i, j));
	  dlx_link(i, j, x, row_n(i, x));
	  dlx_link(i, j, x, col_n(j, x));
	  dlx_link(i, j, x, box_n(i/len*len+j/len, x));
	}
      }
    }
    for(i = 0; i < rows; ++i) {
      row[i].left->right = row[i].right;
      row[i].right->left = row[i].left; 
    }
    solve(0);
    output();
  }
  return 0;
}
