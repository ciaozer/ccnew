#ifndef _BUILD_H_
#define _BUILD_H_

#include"data_structure.h"

void settings()
{
    seed = 1;
    cutoff_time = 10;
    p_random_walk = 0.01;
    rdprob = 0.6;
    p_without_hard_conf = 0.2;
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
    neighbor_count = new int[malloc_item];
    neighbor = new int *[malloc_item];

    original_weight = new long long[malloc_element];

    conflict_stack = new int[malloc_item];
    index_in_conflict_stack = new int[malloc_item];
    is_in_conflict_stack = new bool [malloc_item];

    solution_stack = new int[malloc_item];
    index_in_solution_stack = new int[malloc_item];

    element_cover_times = new int[malloc_element];
}

void allocate_memory()
{
	int malloc_var_length = itemnum+10;
    int malloc_edgenum = edgenum+10;
    int malloc_elementnum = elementnum+10;
	
	hard_cscc = new int [malloc_var_length];
	item_conflict_times = new int [malloc_var_length];
	score = new long long [malloc_var_length];
	conf_change = new int [malloc_var_length];
	time_stamp = new long long [malloc_var_length];
	goodvar_stack = new int [malloc_var_length];
	already_in_goodvar_stack = new int [malloc_var_length];
	current_solution = new int [malloc_var_length];
	best_solution = new int [malloc_var_length];
	best_array = new int [malloc_var_length];
	already_in_ccmpvars = new unsigned int [malloc_var_length];
	ccmpvars = new int [malloc_var_length];
	
	sat_count = new int [edgenum+elementnum+10];
	sat_var = new int [edgenum+elementnum+10];
	conflict_edge_stack = new int [malloc_edgenum];
	index_in_conflict_edge_stack = new int [malloc_edgenum];
	uncovered_stack = new int [edgenum+elementnum+10];
	index_in_softunsat_stack = new int [edgenum+elementnum+10];

    edge = new int*[edgenum+10];
}

void free_memory()
{
	for( int i=1; i<=itemnum; i++ )
    {
        delete[] m_item[i];
        delete[] g_item[i];
        delete[] neighbor[i];
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
    delete[] neighbor_count;

    delete[] original_weight;

    delete[] conflict_stack;
    delete[] index_in_conflict_stack;
    delete[] is_in_conflict_stack;

    delete[] solution_stack;
    delete[] index_in_solution_stack;

    delete[] element_cover_times;

    for( int i=0; i<edgenum; i++ )
    {
        delete[] edge[i];
    }
    delete[] edge;
	
	delete [] item_conflict_times;
	delete [] score;
	delete [] conf_change;
	delete [] time_stamp;
	delete [] goodvar_stack;
	delete [] already_in_goodvar_stack;
	delete [] current_solution;
	delete [] best_solution;
	delete [] best_array;
	delete [] already_in_ccmpvars;
	delete [] ccmpvars;
	
	delete [] hard_cscc;
	delete [] sat_count;
	delete [] sat_var;
	delete [] conflict_edge_stack;
	delete [] index_in_conflict_edge_stack;
	delete [] uncovered_stack;
	delete [] index_in_softunsat_stack;
}

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
        g_adj_count[i] = 0;
        g_adj[i] = NULL;
    }
    for( int i=0; i<elementnum; i++ )
    {
        n_ele_count[i] = 0;
        n_ele[i] = NULL;
    }
    edgenum = 0;

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
    best_uncovered_weight = total_weight;

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
        edgenum += g_item_count[i];
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
    
    //对m_item排序，可以用基数排序优化
    for( int i=1; i<=itemnum; i++ )
    {
        sort(m_item[i], m_item[i]+m_item_count[i]);
    }

    //建立n_ele关系
    for( int i=0; i<elementnum; i++ )
    {
        element_cover_times[i] = 0;
        n_ele[i] = new int[n_ele_count[i]+1];
        n_ele_count[i] = 0;  //重新计数
    }

    for( int i=1; i<=itemnum; i++ )
    {
        for( int j=0; j<m_item_count[i]; j++ )
        {
            int cur_ele = m_item[i][j];
            n_ele[cur_ele][n_ele_count[cur_ele]] = i;
            n_ele_count[cur_ele]++;
        }
    }

    edgenum = edgenum / 2;        //会重复计算所以除以2

	allocate_memory();

	int edge_index = 0;
	for( int i=1; i<=itemnum; i++ )
    {
		for( int j=0; j<g_item_count[i]; j++ )
		{
			if( g_item[i][j] > i )
			{
                edge[edge_index] = new int[3];
                edge[edge_index][0] = i;
                edge[edge_index][1] = g_item[i][j];
                g_adj[i][g_adj_count[i]] = edge_index;
                g_adj_count[i]++;
                g_adj[g_item[i][j]][g_adj_count[g_item[i][j]]] = edge_index;
                g_adj_count[g_item[i][j]]++;
				edge_index++;
			}
		}
    }
    
    //建立neighbor关系
    bool is_in_neighbor[itemnum+10];
    for( int i=1; i<=itemnum; i++ )
    {
        is_in_neighbor[i] = 0;
    }
    for( int i=1; i<=itemnum; i++ )
    {
        int temp_neighbor_count = 0;
        int temp_neighbor[itemnum+10];
        is_in_neighbor[i] = 1;  //不能把自己加进来

        //冲突邻居
        for( int j=0; j<g_item_count[i]; j++ )
        {
            int cur_item = g_item[i][j];
            temp_neighbor[temp_neighbor_count] = cur_item;
            temp_neighbor_count++;
            is_in_neighbor[cur_item] = 1;
        }

        //覆盖同一个元素的邻居
        for( int j=0; j<m_item_count[i]; j++ )
        {
            int cur_ele = m_item[i][j];
            for( int k=0; k<n_ele_count[cur_ele]; k++ )
            {
                int another_item = n_ele[cur_ele][k];
                if( is_in_neighbor[another_item] == 0 )
                {
                    is_in_neighbor[another_item] = 1;
                    temp_neighbor[temp_neighbor_count] = another_item;
                    temp_neighbor_count++;
                }
            }
        }

        //根据统计数据分配内存
        is_in_neighbor[i] = 0;
        neighbor[i] = new int[temp_neighbor_count+1];
        neighbor_count[i] = temp_neighbor_count;
        for( int j=0; j<temp_neighbor_count; j++ )
        {
            neighbor[i][j] = temp_neighbor[j];
            is_in_neighbor[temp_neighbor[j]] = 0;   //重置邻居关系
        }
    }

    cout << "finish read file" << endl;
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