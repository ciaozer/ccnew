#ifndef _INIT_H_
#define _INIT_H_
#include"data_structure.h"
#include"build.h"
#include"heuristic.h"

void init_tabu()
{
	int rand_num, temp_num;

	for( int i=0; i<=itemnum; i++ )
	{
		W1[i] = (long long) pow(i+1, 1.2);
		W2[i] = (long long) pow(i+1, 1.6);
		W3[i] = (long long) pow(i+1, 2.0);
	}

	for( int i=0; i<=itemnum; i++ )
	{
		while(1)
		{
			rand_num =random()%(itemnum+1);
			if( rand_num != i )
				break;
		}
		temp_num = W1[i];
		W1[i] = W1[rand_num];
		W1[rand_num] = temp_num;

		while(1)
		{
			rand_num =random()%(itemnum+1);
			if( rand_num != i )
				break;
		}
		temp_num = W2[i];
		W2[i] = W2[rand_num];
		W2[rand_num] = temp_num;

		while(1)
		{
			rand_num =random()%(itemnum+1);
			if( rand_num != i )
				break;
		}
		temp_num = W3[i];
		W3[i] = W3[rand_num];
		W3[rand_num] = temp_num;
	}
	hx1 = 0;
	hx2 = 0;
	hx3 = 0;

	memset(H1, 0, sizeof(H1));
	memset(H2, 0, sizeof(H2));
	memset(H3, 0, sizeof(H3));
}

void init_random()
{
	init_tabu();
	solution_stack_count = 0;
	for ( int i=1; i<=itemnum; i++) 
    {
		//if( (random()%1000)/1000.0 < 1.0 )	current_solution[i] = 1;
		//else current_solution[i] = 0;
        current_solution[i] = random()%2;
		time_stamp[i] = 0;
		hard_cscc[i] = 1;		
		conf_change[i] = 1;
		if( current_solution[i] == 1 )
		{
			solution_stack[solution_stack_count] = i;
			index_in_solution_stack[i] = solution_stack_count;
			solution_stack_count++;
			hx1 = (hx1 + W1[i]);
			hx2 = (hx2 + W2[i]);
			hx3 = (hx3 + W3[i]);
		}
	}

    current_uncovered_weight=0;
	conflict_edge_stack_count = 0;
    uncovered_stack_count = 0;
    
	//初始化覆盖次数和冲突次数
	for( int i=0; i<edgenum; i++ )
	{
		edge_node_count[i] = 0;
		
		for( int j=0; j<2; j++ )
		{
			if ( current_solution[edge[i][j]] == 0 )
			{
				edge_node_count[i]++;
				node[i] = edge[i][j];	
			}
		}

		if (edge_node_count[i] == 0) 
        {
            conflict(i);
        }
	}

	for ( int i=0; i<elementnum; i++ ) 
	{
		element_cover_times[i] = 0;
		
		for( int j=0; j<n_ele_count[i]; j++ )
		{
			if ( current_solution[n_ele[i][j]] == 1 )
			{
				element_cover_times[i]++;
				only_cover_item[i] = n_ele[i][j];	
			}
		}

		if (element_cover_times[i] == 0) 
        {
			if( n_ele_count[i] != 0 )
			{
	            uncovered(i);
			}
			else
			{
				current_uncovered_weight += original_weight[i];
			}
        }
	}

	//初始化分数
	for( int i=1; i<=itemnum; i++ )
	{
		item_conflict_times[i] = 0;
		
		for( int j=0; j<g_adj_count[i]; j++ )		//遍历该集合的所有边
		{
			int cur_edge = g_adj[i][j];
			if( edge_node_count[cur_edge] == 0 )	//该边的两个集合都被选入
			{
				item_conflict_times[i]++;
			}
            else if( edge_node_count[cur_edge] == 1 && current_solution[i] == 0 ) 
			{										//邻居被选入，再选自己会导致冲突，用负分表示
				item_conflict_times[i]--;
			}
		}

		score[i] = 0;

		for( int j=0; j<m_item_count[i]; j++ )		//遍历能覆盖的所有元素
		{
			int cur_ele = m_item[i][j];
			if( element_cover_times[cur_ele] == 0 ) 
			{
				score[i] += original_weight[cur_ele];
			}
            else if( element_cover_times[cur_ele] == 1 && current_solution[i] == 1 ) 
			{
				score[i] -= original_weight[cur_ele];
			}
		}
	}
		
	//好变元是冲突，有没有候选
	gooditem_stack_count = 0;
	for( int i=1; i<=itemnum; i++ ) 
	{
		if( item_conflict_times[i] > 0 || ( item_conflict_times[i] == 0 && score[i] > 0 ) )
		{
			is_in_gooditem_stack[i] = 1;
			PUSH(i, gooditem_stack);	
		}
		else
		{
			is_in_gooditem_stack[i] = 0;
		}
	}

    cout << "finish init " << get_runtime() << endl;
}

#endif