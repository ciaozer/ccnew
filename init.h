#ifndef _INIT_H_
#define _INIT_H_
#include"data_structure.h"
#include"build.h"
#include"heuristic.h"

void init()
{
	for ( int i=1; i<=itemnum; i++) 
    {
        current_solution[i] = random()%2;
		time_stamp[i] = 0;
		hard_cscc[i] = 1;		
		conf_change[i] = 1;

        is_in_conflict_stack[i] = 0;

	}

    current_uncovered_weight=0;
	conflict_edge_stack_count = 0;
    uncovered_stack_count = 0;
    conflict_stack_count = 0;
    solution_stack_count = 0;
    
	/* figure out sat_count, and init unsat_stack */
	for( int i=0; i<edgenum; i++ )
	{
		sat_count[i] = 0;
		
		for( int j=0; j<2; j++ )
		{
			if ( current_solution[edge[i][j]] == 0 )
			{
				sat_count[i]++;
				sat_var[i] = edge[i][j];	
			}
		}

		if (sat_count[i] == 0) 
        {
            unsat(i);
        }
	}
	for ( int i=0; i<elementnum; i++ ) 
	{
		sat_count[i+edgenum] = 0;
		
		for( int j=0; j<n_ele_count[i]; j++ )
		{
			if ( current_solution[n_ele[i][j]] == 1 )
			{
				sat_count[i+edgenum]++;
				sat_var[i+edgenum] = n_ele[i][j];	
			}
		}

		if (sat_count[i+edgenum] == 0) 
        {
            unsat(i);
        }
	}

	/*figure out score*/
	int tmp_item_conflict_times[itemnum+10];
	int tmp_score[itemnum+10];
	for( int i=1; i<=itemnum; i++ )
	{
		item_conflict_times[i] = 0;
		score[i] = 0;
		for( int j=0; j<g_adj_count[i]; j++ )
		{
			int cur_edge = g_adj[i][j];
			if (sat_count[cur_edge]==0) item_conflict_times[i]++;
            else if (sat_count[cur_edge]==1 && current_solution[i] == 0) item_conflict_times[i]--;
		}
		for( int j=0; j<m_item_count[i]; j++ )
		{
			int cur_ele = m_item[i][j];
			if (sat_count[cur_ele+edgenum]==0) score[i]+=original_weight[cur_ele];
            else if (sat_count[cur_ele+edgenum]==1 && current_solution[i] == 1) score[i]-=original_weight[cur_ele];
		}
	}
		
	//好变元是冲突，有没有候选
	goodvar_stack_count = 0;
	for (int i=1; i<=itemnum; i++) 
	{
		if( item_conflict_times[i]>0 )
		{
			already_in_goodvar_stack[i] = 1;
			push(i,goodvar_stack);	
		}
		else
			already_in_goodvar_stack[i] = 0;
	}
    cout << "finish init" << endl;
}

#endif