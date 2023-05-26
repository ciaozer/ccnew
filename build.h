#ifndef _BUILD_H_
#define _BUILD_H_

#include"data_structure.h"

void settings()
{
    seed = 1;
    cutoff_time = 10;
    pacprob = 0.01;
    rdprob = 0.6;
    prob = 0.2;
}

void w_allocate_memory()
{
    int malloc_item = itemnum + 10;
    int malloc_element = elementnum + 10;

    m_item_count = new int[malloc_item];
    m_item = new int *[malloc_item];
    g_item_count = new int[malloc_item];
    g_item = new int *[malloc_item];
    n_ele_count = new int[malloc_element];
    n_ele = new int *[malloc_element];
    g_adj_count = new int[malloc_item];
    g_adj = new int *[malloc_item]; 

    original_weight = new long long[malloc_element];

    conflict_stack = new int[malloc_item];
    index_in_conflict_stack = new int[malloc_item];
    is_in_conflict_stack = new bool [malloc_item];

    solution_stack = new int[malloc_item];
    index_in_solution_stack = new int[malloc_item];

    element_cover_times = new int[malloc_element];
}

void w_free_memory()
{
    for( int i=1; i<=itemnum; i++ )
    {
        delete[] m_item[i];
        delete[] g_item[i];
        //delete[] neighbor[i];
        delete[] g_adj[i];
    }

    for( int i=0; i<elementnum; i++ )
    {
        delete[] n_ele[i];
    }

    delete[] m_item;
    delete[] m_item_count;
    delete[] g_item;
    delete[] g_item_count;
    delete[] n_ele;
    delete[] n_ele_count;
    delete[] g_adj;
    delete[] g_adj_count;

    delete[] original_weight;

    delete[] conflict_stack;
    delete[] index_in_conflict_stack;
    delete[] is_in_conflict_stack;

    delete[] solution_stack;
    delete[] index_in_solution_stack;

    delete[] element_cover_times;
}

void allocate_memory();

void build_neighbor_relation();

void build(char *filename)
{
    ifstream infile;
    infile.open(filename, ios::in);
    if( !infile.is_open() )
    {
        cout << "read file " << filename << "error" << endl;
        exit(-1);
    }

    infile >> itemnum >> elementnum;

    w_allocate_memory();

    for( int i=1; i<=itemnum; i++ )
    {
        m_item_count[i] = 0;
        m_item[i] = NULL;
        g_item_count[i] = 0;
        g_item[i] = NULL;
    }
    for( int i=0; i<elementnum; i++ )
    {
        n_ele_count[i] = 0;
        n_ele[i] = NULL;
    }

    //给初始权重赋值
    total_weight = 0; 
    current_uncovered_weight = 0;
    for( int i=0; i<elementnum; i++ )
    {
        infile >> original_weight[i];
        total_weight += original_weight[i];
    }
    cout << total_weight << endl;

    current_uncovered_weight = total_weight;

    for( int i=1; i<=itemnum; i++ )
    {
        int item_index;
        infile >> item_index;

        //读入该集合覆盖的元素
        infile >> m_item_count[i];
        m_item[i] = new int[m_item_count[i]+1];

        for( int ele=0; ele<m_item_count[i]; ele++ )
        {
            int cur_element;
            infile >> cur_element;
            n_ele_count[cur_element]++;  //该元素被覆盖次数加一
            m_item[i][ele] = cur_element;
        }

        //读入该集合冲突的集合
        infile >> g_item_count[i];
        g_item[i] = new int[g_item_count[i]+1];
        g_adj[i] = new int[g_item_count[i]+1];

        for( int it=0; it<g_item_count[i]; it++ )
        {
            int cur_item;
            infile >> cur_item;
            g_item[i][it] = cur_item;
        }
    }

    infile.close();

    //建立n_ele关系
    for( int i=0; i<elementnum; i++ )
    {
        element_cover_times[i] = 0;
        n_ele[i] = new int[n_ele_count[i]+1];
        n_ele_count[i] = 0;  //重新计数
    }

    for( int it=1; it<=itemnum; it++ )
    {
        for( int ele=0; ele<m_item_count[it]; ele++ )
        {
            int cur_ele = m_item[it][ele];
            n_ele[cur_ele][n_ele_count[cur_ele]] = it;
            n_ele_count[cur_ele]++;
        }
    }
    cout << "finish read file" << endl;

	top_clause_weight = total_weight + 1;

	edgenum = 0;
	for ( int i=1; i<=itemnum; i++ )
    {
        edgenum += g_item_count[i];
    }
    edgenum = edgenum / 2;        //会重复计算所以除以2

	num_clauses = edgenum+elementnum;

	allocate_memory();
	for (int v=1; v<=itemnum; ++v)
		var_lit_count[v] = 0;
    
    num_hclauses = edgenum;
	num_sclauses = elementnum;

	int c=0;
    for( int i=1; i<=itemnum; i++ )
    {
        g_adj_count[i] = 0;
    }
	for (int i = 1; i <=itemnum; i++)
    {
		for( int j=0; j<g_item_count[i]; j++ )
		{
			if( g_item[i][j] > i )
			{
				clause_lit[c] = new lit[3];
				clause_lit_count[c] = 2;
				clause_lit[c][0].clause_num = c;
				clause_lit[c][1].clause_num = c;
				clause_lit[c][0].var_num = i;
				clause_lit[c][1].var_num = g_item[i][j];
				clause_lit[c][0].sense = 0;
				clause_lit[c][1].sense = 0;
				clause_lit[c][2].var_num=0; 
            	clause_lit[c][2].clause_num = -1;
				org_clause_weight[c] = top_clause_weight;
                edge[c] = new int[3];
                edge[c][0] = i;
                edge[c][1] = g_item[i][j];
                g_adj[i][g_adj_count[i]] = c;
                g_adj_count[i]++;
                g_adj[g_item[i][j]][g_adj_count[g_item[i][j]]] = c;
                g_adj_count[g_item[i][j]]++;
				c++;
			}
		}
    }
	for( int i=0; i<elementnum; i++ )
	{
		clause_lit_count[c] = n_ele_count[i];
		clause_lit[c] = new lit[clause_lit_count[c]+1];
		int j;
		for( j=0; j<clause_lit_count[c]; j++ )
		{
			clause_lit[c][j].clause_num = c;
			clause_lit[c][j].var_num = n_ele[i][j];
			clause_lit[c][j].sense = 1;
		}
		clause_lit[c][j].var_num=0; 
        clause_lit[c][j].clause_num = -1;
		org_clause_weight[c] = original_weight[i];
		c++;
	}

	for( int i=0; i<=itemnum; i++ )
	{
		var_lit[i] = new lit[g_item_count[i]+m_item_count[i]+1];
		var_lit_count[i] = 0;
	}

	for (int c = 0; c < num_clauses; ++c) 
	{
		for(int i=0; i<clause_lit_count[c]; ++i)
		{
			int v = clause_lit[c][i].var_num;
			var_lit[v][var_lit_count[v]] = clause_lit[c][i];
			++var_lit_count[v];
		}
	}

	for (int v=1; v<=itemnum; ++v)
		var_lit[v][var_lit_count[v]].clause_num=-1;

	build_neighbor_relation();
    
    best_uncovered_weight=total_weight;

}

void allocate_memory()
{
	int malloc_var_length = itemnum+10;
	int malloc_clause_length = num_clauses+10;
	
	var_lit = new lit* [malloc_var_length];
	var_lit_count = new int [malloc_var_length];
	hard_cscc = new int [malloc_var_length];
	item_conflict_times = new long long [malloc_var_length];
	score = new long long [malloc_var_length];
	conf_change = new int [malloc_var_length];
	var_neighbor = new int* [malloc_var_length];
	var_neighbor_count = new int [malloc_var_length];
	time_stamp = new long long [malloc_var_length];
	neighbor_flag = new int [malloc_var_length];
	temp_neighbor = new int [malloc_var_length];
	goodvar_stack = new int [malloc_var_length];
	already_in_goodvar_stack = new int [malloc_var_length];
	current_solution = new int [malloc_var_length];
	best_solution = new int [malloc_var_length];
	best_array = new int [malloc_var_length];
	already_in_ccmpvars = new unsigned int [malloc_var_length];
	ccmpvars = new int [malloc_var_length];
	
	clause_lit = new lit* [malloc_clause_length];
	clause_lit_count = new int [malloc_clause_length];
	org_clause_weight = new long long [malloc_clause_length];
	sat_count = new int [malloc_clause_length];
	sat_var = new int [malloc_clause_length];
	conflict_edge_stack = new int [malloc_clause_length];
	index_in_conflict_edge_stack = new int [malloc_clause_length];
	uncovered_stack = new int [malloc_clause_length];
	index_in_softunsat_stack = new int [malloc_clause_length];

    edge = new int*[edgenum+10];
}

void free_memory()
{
	int i;
	
	for(i=1; i<=itemnum; ++i)
	{
		delete[] var_lit[i];
		delete[] var_neighbor[i];
	}

    for( int i=0; i<edgenum; i++ )
    {
        delete[] edge[i];
    }
    delete[] edge;
	
	delete [] var_lit;
	delete [] var_lit_count;
	delete [] item_conflict_times;
	delete [] score;
	delete [] conf_change;
	delete [] var_neighbor;
	delete [] var_neighbor_count;
	delete [] time_stamp;
	delete [] neighbor_flag;
	delete [] temp_neighbor;
	delete [] goodvar_stack;
	delete [] already_in_goodvar_stack;
	delete [] current_solution;
	delete [] best_solution;
	delete [] best_array;
	delete [] already_in_ccmpvars;
	delete [] ccmpvars;
	
	for (i = 0; i < num_clauses; i++) 
	{
		delete[] clause_lit[i];
	}
	
	delete [] clause_lit;
	delete [] clause_lit_count;
	delete [] hard_cscc;
	delete [] org_clause_weight;
	delete [] sat_count;
	delete [] sat_var;
	delete [] conflict_edge_stack;
	delete [] index_in_conflict_edge_stack;
	delete [] uncovered_stack;
	delete [] index_in_softunsat_stack;
}

void build_neighbor_relation()
{
	int		i,j,count;
	int 	v,c;

	for(v=1; v<=itemnum; ++v)
	{
		neighbor_flag[v] = 0;
	}
	neighbor_flag[0] = 0;

	for(v=1; v<=itemnum; ++v)
	{
		var_neighbor_count[v] = 0;
		neighbor_flag[v] = 1;  //自己不是自己的邻居
		temp_neighbor_count = 0;
		for(i=0; i<var_lit_count[v]; ++i)
		{
			c = var_lit[v][i].clause_num;
			for(j=0; j<clause_lit_count[c]; ++j)
			{
				if(neighbor_flag[clause_lit[c][j].var_num]==0) //没有把这个变元当作自己的邻居
				{
					var_neighbor_count[v]++;
					neighbor_flag[clause_lit[c][j].var_num] = 1;
					temp_neighbor[temp_neighbor_count++] = clause_lit[c][j].var_num;
				}
			}
			
		}

		neighbor_flag[v] = 0;//重置邻居关系
 
		var_neighbor[v] = new int[var_neighbor_count[v]+1];

		count = 0;
		for(i=0; i<temp_neighbor_count; i++)
		{
			var_neighbor[v][count++] = temp_neighbor[i];
			neighbor_flag[temp_neighbor[i]] = 0; //重置邻居关系
		}
		var_neighbor[v][count]=0;
		var_neighbor_count[v] = count;
	}
}

double get_runtime()
{
    times(&stop);
    return (double)(stop.tms_utime - start.tms_utime +stop.tms_stime - start.tms_stime) / sysconf(_SC_CLK_TCK);
}

void print_best_solution()
{	
    for (int i=1; i<=itemnum; i++)
    {
        if( best_solution[i] == 1 )
        {
            cout << i << " ";
        }
    }
    cout << endl;
}

int verify_sol()
{        
	int c,j,flag;
    long long verify_unsat_weight=0;
    
	for (c = 0; c<num_clauses; ++c) 
	{
        flag = 0;
        for(j=0; j<clause_lit_count[c]; ++j) 
            if (best_solution[clause_lit[c][j].var_num] == clause_lit[c][j].sense) {flag = 1; break;}
        
        if(flag ==0)
        {
            if(org_clause_weight[c]==top_clause_weight)//verify hard clauses
            {
                printf("c Error: hard clause %d is not satisfied\n", c);

                printf("c ");
                for(j=0; j<clause_lit_count[c]; ++j)
                {
                    if(clause_lit[c][j].sense==0) printf("-");
                    printf("%d ", clause_lit[c][j].var_num);
                }
                printf("\n");
                
                printf("c ");
                for(j=0; j<clause_lit_count[c]; ++j)
                	printf("%d ", best_solution[clause_lit[c][j].var_num]);
                printf("\n");
                fflush(stdout);
                return 0;
            }
            else
            {
                verify_unsat_weight+=org_clause_weight[c];
            }
        }
    }
    
    if(verify_unsat_weight==best_uncovered_weight)  return 1;
    else{
    	printf("c Error: find opt=%lld, but verified opt=%lld", best_uncovered_weight, verify_unsat_weight);
    	fflush(stdout);
    }
    
    return 0;
}

int check_answer()
{//0表示不正确，1表示正确
    int check_cover_times[elementnum+10];
    int check_uncovered_weight = 0;
    memset(check_cover_times, 0, sizeof(check_cover_times));

    //检验是否有冲突
    for( int i=1; i<=itemnum; i++ )
    {
        if( best_solution[i] == 1 )
        {//只处理选中该集合的情况，没选中不会有冲突
            for( int j=0; j<g_item_count[i]; j++ )
            {
                if( best_solution[g_item[i][j]] == 1 )
                {
                    cout << i << " conflict with " << g_item[i][j] << endl;
                    return 0;
                }
            }

            //重新计算元素被覆盖次数
            for( int j=0; j<m_item_count[i]; j++ )
            {
                check_cover_times[m_item[i][j]]++;
            }
        }
    }

    //检查未覆盖权重是否正确
    for( int i=0; i<elementnum; i++ )
    {
        if( check_cover_times[i] == 0 )
        {
            check_uncovered_weight += original_weight[i];
        }
    }

    if( check_uncovered_weight != current_uncovered_weight )
    {
        cout << "real: " << check_uncovered_weight << endl;
        cout << "count: " << current_uncovered_weight << endl;
        return 0;
    }

    return 1;
}

#endif