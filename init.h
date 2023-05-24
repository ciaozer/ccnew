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
	for ( int i=0; i<num_clauses; i++ ) 
	{
		sat_count[i] = 0;
		
		for( int j=0; j<clause_lit_count[i]; j++ )
		{
			if (current_solution[clause_lit[i][j].var_num] == clause_lit[i][j].sense)
			{
				sat_count[i]++;
				sat_var[i] = clause_lit[i][j].var_num;	
			}
		}

		if (sat_count[i] == 0) 
        {
            unsat(i);
        }
	}

	/*figure out score*/
	for ( int v=1; v<=itemnum; v++) 
	{
		item_conflict_times[v]=score[v]=0;
		for(int i=0; i<var_lit_count[v]; ++i)
		{
			int c = var_lit[v][i].clause_num;
            if (org_clause_weight[c]==top_clause_weight)
            {
                if (sat_count[c]==0) item_conflict_times[v]++;
                else if (sat_count[c]==1 && var_lit[v][i].sense==current_solution[v]) item_conflict_times[v]--;
            }
            else
            {
                if (sat_count[c]==0) score[v]+=org_clause_weight[c];
                else if (sat_count[c]==1 && var_lit[v][i].sense==current_solution[v]) score[v]-=org_clause_weight[c];
            }
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