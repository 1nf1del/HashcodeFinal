#include <iostream>
#include <queue>
#include <vector>
#include <cassert>

using namespace std;

const int N = 1000;
int n, m, radius, cost_edge, cost_router, budget, start_i, start_j;
char board[N][N];

struct coord
{
    int i, j;
} ;

// ----------

// ----------
// backbone

struct edge
{
	int from, to;
	int len;
} ;

bool operator<(edge a, edge b)
{
	// > for priority_queue as min-heap
	return a.len == b.len ? a.from == b.from ? a.to < b.to : a.from < b.from : a.len > b.len;
}

int dist(coord a, coord b)
{
	return max(abs(a.i - b.i), abs(a.j - b.j));
}

int sgn(int x) { return x ? (x > 0 ? 1 : -1) : 0; }

bool in_mst[N];
bool backbone[N][N];
vector<coord> make_backbone(vector<coord> routers)
{
    routers.push_back({start_i, start_j});
	swap(routers[0], routers[routers.size() - 1]);

    priority_queue<edge> edges;
	for(int i = 0; i < routers.size(); i++)
		in_mst[i] = false;

	for(int i = 0; i < n; i++)
		for(int j = 0; j < m; j++)
			backbone[i][j] = false;

	in_mst[0] = true;
	for(int i = 1; i < routers.size(); i++)
		edges.push({0, i, dist(routers[0], routers[i])});

	vector<coord> res;
	for(int iter = 1; iter < routers.size(); iter++)
	{
		while(in_mst[edges.top().to])
			edges.pop();

		edge curr = edges.top();
		edges.pop();

//		printf("(%d,%d) -> (%d,%d)\n", routers[curr.from].i, routers[curr.from].j, routers[curr.to].i, routers[curr.to].j);

		in_mst[curr.to] = true;
		for(int i = 0; i < routers.size(); i++)
			if(!in_mst[i])
				edges.push({curr.to, i, dist(routers[curr.to], routers[i])});

		// draw edge
		int ii = routers[curr.from].i, jj = routers[curr.from].j;
		while(ii != routers[curr.to].i || jj != routers[curr.to].j)
		{
//			printf("%d %d\n", ii, jj);
		    ii += sgn(routers[curr.to].i - ii);
		    jj += sgn(routers[curr.to].j - jj);
			res.push_back({ii, jj});
		}
		res.push_back({ii, jj});
	}

	vector<coord> real_res;
	for(coord i : res)
	{
		if(!backbone[i.i][i.j])
			real_res.push_back(i);
		backbone[i.i][i.j] = true;
	}

	return real_res;
}

// ----------

int value(vector<coord> routers)
{
    bool mark[n][m];
    for (int i=0;i<n;i++)
    {
        for (int j=0;j<m;j++)
        {
            mark[i][j] = false;
        }
    }

    int ret = 0;

    for (int i=0;i<routers.size();i++)
    {
        // check if outta bounds
        if (routers[i].i < 0 || routers[i].i > n - 1 || routers[i].j < 0 || routers[i].j > m - 1) assert(false);
        // check if wallhacked
        if (board[routers[i].i][routers[i].j] == '#') assert(false);
        // otherwise, expand up

        int ly = -1;
        int ry = m;

        for (int dx=0;dx>=-radius;dx--)
        {
            int xt = routers[i].i + dx;
            if (xt < 0) break;
            if (board[xt][routers[i].j] == '#') break;
            
            // top-left
            for (int dy=0;dy>=-radius;dy--)
            {
                int yt = routers[i].j + dy;
                if (yt <= ly) break;
                if (board[xt][yt] == '#') 
                {
                    ly = max(ly, yt);
                    break; // no need to go on
                }
                if (!mark[xt][yt])
                {
                    mark[xt][yt] = true;
                    if (board[xt][yt] == '.') ret++;
                }
            }

            // top-right
            for (int dy=0;dy<=radius;dy++)
            {
                int yt = routers[i].j + dy;
                if (yt >= ry) break;
                if (board[xt][yt] == '#') 
                {
                    ry = min(ry, yt);
                    break;
                }
                if (!mark[xt][yt])
                {
                    mark[xt][yt] = true;
                    if (board[xt][yt] == '.') ret++;
                }
            }
        }

        ly = -1;
        ry = m;

        // now expand down
        for (int dx=0;dx<=radius;dx++)
        {
            int xt = routers[i].i + dx;
            if (xt > n - 1) break;
            if (board[xt][routers[i].j] == '#') break;
            
            // bottom-left
            for (int dy=0;dy>=-radius;dy--)
            {
                int yt = routers[i].j + dy;
                if (yt <= ly) break;
                if (board[xt][yt] == '#') 
                {
                    ly = max(ly, yt);
                    break; // no need to go on
                }
                if (!mark[xt][yt])
                {
                    mark[xt][yt] = true;
                    if (board[xt][yt] == '.') ret++;
                }
            }

            // top-right
            for (int dy=0;dy<=radius;dy++)
            {
                int yt = routers[i].j + dy;
                if (yt >= ry) break;
                if (board[xt][yt] == '#') 
                {
                    ry = min(ry, yt);
                    break;
                }
                if (!mark[xt][yt])
                {
                    mark[xt][yt] = true;
                    if (board[xt][yt] == '.') ret++;
                }
            }
        }
    }
    return ret;
}

vector<coord> coord_value(vector<coord> routers)
{
    bool mark[n][m];
    for (int i=0;i<n;i++)
    {
        for (int j=0;j<m;j++)
        {
            mark[i][j] = false;
        }
    }

    vector<coord> ret = {};

    for (int i=0;i<routers.size();i++)
    {
        // check if outta bounds
        if (routers[i].i < 0 || routers[i].i > n - 1 || routers[i].j < 0 || routers[i].j > m - 1) assert(false);
        // check if wallhacked
        if (board[routers[i].i][routers[i].j] == '#') assert(false);
        // otherwise, expand up

        int ly = -1;
        int ry = m;

        for (int dx=0;dx>=-radius;dx--)
        {
            int xt = routers[i].i + dx;
            if (xt < 0) break;
            if (board[xt][routers[i].j] == '#') break;
            
            // top-left
            for (int dy=0;dy>=-radius;dy--)
            {
                int yt = routers[i].j + dy;
                if (yt <= ly) break;
                if (board[xt][yt] == '#') 
                {
                    ly = max(ly, yt);
                    break; // no need to go on
                }
                if (!mark[xt][yt])
                {
                    mark[xt][yt] = true;
                    if (board[xt][yt] == '.') ret.push_back({xt, yt});
                }
            }

            // top-right
            for (int dy=0;dy<=radius;dy++)
            {
                int yt = routers[i].j + dy;
                if (yt >= ry) break;
                if (board[xt][yt] == '#') 
                {
                    ry = min(ry, yt);
                    break;
                }
                if (!mark[xt][yt])
                {
                    mark[xt][yt] = true;
                    if (board[xt][yt] == '.') ret.push_back({xt, yt});
                }
            }
        }
        
        ly = -1;
        ry = m;

        // now expand down
        for (int dx=0;dx<=radius;dx++)
        {
            int xt = routers[i].i + dx;
            if (xt > n - 1) break;
            if (board[xt][routers[i].j] == '#') break;
            
            // bottom-left
            for (int dy=0;dy>=-radius;dy--)
            {
                int yt = routers[i].j + dy;
                if (yt <= ly) break;
                if (board[xt][yt] == '#') 
                {
                    ly = max(ly, yt);
                    break; // no need to go on
                }
                if (!mark[xt][yt])
                {
                    mark[xt][yt] = true;
                    if (board[xt][yt] == '.') ret.push_back({xt, yt});
                }
            }

            // top-right
            for (int dy=0;dy<=radius;dy++)
            {
                int yt = routers[i].j + dy;
                if (yt >= ry) break;
                if (board[xt][yt] == '#') 
                {
                    ry = min(ry, yt);
                    break;
                }
                if (!mark[xt][yt])
                {
                    mark[xt][yt] = true;
                    if (board[xt][yt] == '.') ret.push_back({xt, yt});
                }
            }
        }
    }
    return ret;
}

// -----------
// output

void write_output(string filename, vector<coord> routers, vector<coord> backbone)
{
    FILE *f = fopen(filename.c_str(), "w");
    if(!f)
    {
		fprintf(stderr, "Opening file %s for writing failed!\n", filename.c_str());
		return;
    }

    fprintf(f, "%d\n", backbone.size());
	for(coord i : backbone)
		fprintf(f, "%d %d\n", i.i, i.j);

	fprintf(f, "%d\n", routers.size());
	for(coord i : routers)
		fprintf(f, "%d %d\n", i.i, i.j);

	fclose(f);
}

// ----------

vector<coord> make_initial(int num_routers)
{
	vector<coord> res;
	for(int i = 0; i < num_routers; i++)
	{
		coord curr;
		do
		{
			curr.i = rand() % n;
			curr.j = rand() % m;
		} while(board[curr.i][curr.j] != '.');

		res.push_back(curr);
	}

	return res;
}

int coverage[N][N];

void mod_coverage(coord router, int diff, int &score)
{
	for(coord tile : coord_value({router}))
	{
		if(coverage[tile.i][tile.j] == 0) score++;
		coverage[tile.i][tile.j] += diff;
		if(coverage[tile.i][tile.j] == 0) score--;
	}
}

void perturb(coord &router)
{
	int diff = radius / 2;
	int ii, jj;
	for(int i = 0; i < 10 && i && board[ii][jj] != '.'; i++)
	{
	    ii = min(n - 1, max(0, router.i + rand() % (2 * diff + 1) - diff));
		jj = min(m - 1, max(0, router.j + rand() % (2 * diff + 1) - diff));
	}

	if(board[ii][jj] == '.')
	{
		router.i = ii;
		router.j = jj;
	}
}

vector<coord> solve()
{
	//vector<coord> res = make_initial(budget / (cost_router + max(n, m)));
	vector<coord> res = make_initial(825);
	for(coord router : res)
		for(coord tile : coord_value({router}))
			coverage[tile.i][tile.j]++;


	int score = 0;
	for(int i = 0; i < n; i++)
		for(int j = 0; j < m; j++)
			score += coverage[i][j] > 0;

	clock_t start = clock();
	for(int iter = 0; clock() - start < 30 * CLOCKS_PER_SEC; iter++)
	{
		if(iter % 10000 == 9999) fprintf(stderr, "%d (%5d msec)\n", iter + 1, (clock() - start) * 1000 / CLOCKS_PER_SEC);
		int curr = rand() % res.size();
		coord old = res[curr];

		int new_score = score;
		
		mod_coverage(res[curr], -1, new_score);
		perturb(res[curr]);
		mod_coverage(res[curr], +1, new_score);

		if(new_score < score)
		{
			mod_coverage(res[curr], -1, new_score);
			res[curr] = old;
			mod_coverage(res[curr], +1, new_score);
		}
		else
			score = new_score;
	}

	return res;
}

// ----------

int main()
{
    scanf("%d %d %d", &n, &m, &radius);
    scanf("%d %d %d", &cost_edge, &cost_router, &budget);
    scanf("%d %d", &start_i, &start_j);

    for(int i = 0; i < n; i++)
		for(int j = 0; j < m; j++)
			scanf(" %c", &board[i][j]);

	auto routers = solve();
	write_output("out/opera.out", routers, make_backbone(routers));
	
    return 0;
}
