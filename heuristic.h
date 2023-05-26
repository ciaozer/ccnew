#ifndef _PMS_H_
#define _PMS_H_

#include "build.h"

inline void unsat(int clause)
{
    if(org_clause_weight[clause]==top_clause_weight) 
    {
        index_in_conflict_edge_stack[clause] = conflict_edge_stack_count;
        push(clause,conflict_edge_stack);
    }
    else {
        index_in_softunsat_stack[clause] = uncovered_stack_count;
        push(clause,uncovered_stack);
        
        current_uncovered_weight += original_weight[clause-edgenum];
    }   
}

inline void sat(int clause)
{
	int index,last_unsat_clause;

    if (org_clause_weight[clause]==top_clause_weight) 
    {
        last_unsat_clause = pop(conflict_edge_stack);

        index = index_in_conflict_edge_stack[clause];
        conflict_edge_stack[index] = last_unsat_clause;
        index_in_conflict_edge_stack[last_unsat_clause] = index;
    }
    else {
        last_unsat_clause = pop(uncovered_stack);

        index = index_in_softunsat_stack[clause];
        uncovered_stack[index] = last_unsat_clause;
        index_in_softunsat_stack[last_unsat_clause] = index;
        
        current_uncovered_weight -= original_weight[clause-edgenum];
    }
}

void flip(int flipvar)
{
	int i,v,c;
	int index;
	lit* clause_c;

	int org_flipvar_hscore = item_conflict_times[flipvar];
    int org_flipvar_sscore = score[flipvar];
	current_solution[flipvar] = 1 - current_solution[flipvar];

    for( int i=0; i<g_adj_count[flipvar]; i++ )
    {
        int cur_edge = g_adj[flipvar][i];
        if( current_solution[flipvar] == 0 )
        {
            sat_count[cur_edge]++;

            if (sat_count[cur_edge] == 2) //sat_count from 1 to 2
                item_conflict_times[sat_var[cur_edge]] ++;
            else if (sat_count[cur_edge] == 1) // sat_count from 0 to 1
            {
                sat_var[cur_edge] = flipvar;//record the only true lit's var
                
                int item1 = edge[cur_edge][0];
                int item2 = edge[cur_edge][1];
                item_conflict_times[item1]--;
                item_conflict_times[item2]--;
                hard_cscc[item1] = 1;
                hard_cscc[item2] = 1;
                sat(cur_edge);
            }
        }
        else
        {
            sat_count[cur_edge]--;
            if (sat_count[cur_edge] == 1) //sat_count from 2 to 1
            {
                int item1 = edge[cur_edge][0];
                int item2 = edge[cur_edge][1];
                if( current_solution[item1] == 0 )
                {
                    item_conflict_times[item1]--;
                    sat_var[cur_edge] = item1;
                }
                else
                {
                    item_conflict_times[item2]--;
                    sat_var[cur_edge] = item2;
                }
            }
            else if (sat_count[cur_edge] == 0) //sat_count from 1 to 0
            {
                int item1 = edge[cur_edge][0];
                int item2 = edge[cur_edge][1];
                item_conflict_times[item1]++;
                item_conflict_times[item2]++;
                hard_cscc[item1] = 1;
                hard_cscc[item2] = 1;
                
                unsat(cur_edge); 
            }//end else if
        }
    }

	for( int i=0; i<m_item_count[flipvar]; i++ )
    {
		c = var_lit[flipvar][i+g_adj_count[flipvar]].clause_num;
		clause_c = clause_lit[c];
        int cur_ele = m_item[flipvar][i];

        if(current_solution[flipvar] == 1)
        {
            ++sat_count[cur_ele+edgenum];
            
            if (sat_count[cur_ele+edgenum] == 2) //sat_count from 1 to 2
                score[sat_var[cur_ele+edgenum]] += original_weight[cur_ele];
            else if (sat_count[cur_ele+edgenum] == 1) // sat_count from 0 to 1
            {
                sat_var[cur_ele+edgenum] = flipvar;//record the only true lit's var
                
                for( int j=0; j<n_ele_count[cur_ele]; j++ )
                {
                    int cur_item = n_ele[cur_ele][j];
                    score[cur_item] -= original_weight[cur_ele];
                }
                sat(cur_ele+edgenum);
            }
        }
        else 
        {
            --sat_count[cur_ele+edgenum];
            if (sat_count[cur_ele+edgenum] == 1) //sat_count from 2 to 1
            {
                for( int j=0; j<n_ele_count[cur_ele]; j++ )
                {
                    int cur_item = n_ele[cur_ele][j];
                    if( current_solution[cur_item] == 1 )
                    {
                        score[cur_item] -= original_weight[cur_ele];
                        sat_var[cur_ele+edgenum] = cur_item;
                        break;
                    }
                }
            }
            else if (sat_count[cur_ele+edgenum] == 0) //sat_count from 1 to 0
            {
                for( int j=0; j<n_ele_count[cur_ele]; j++ )
                {
                    int cur_item = n_ele[cur_ele][j];
                    score[cur_item] += original_weight[cur_ele];
                }
                unsat(cur_ele+edgenum); 
            }   
        }
	}

	//hcscc
	hard_cscc[flipvar] = 0;

	//update information of flipvar
	item_conflict_times[flipvar] = -org_flipvar_hscore;
    score[flipvar] = -org_flipvar_sscore;

	//remove the vars no longer goodvar in goodvar stack 
	for(index=goodvar_stack_count-1; index>=0; index--)  //移除已经不在goodvar中的变元
	{
		v = goodvar_stack[index];
		//if(item_conflict_times[v]<0 || (item_conflict_times[v]<=0 && score[v]<=0) )
        if(item_conflict_times[v]<=0 )
		{
			goodvar_stack[index] = pop(goodvar_stack);  //交换该元素和栈顶元素
			already_in_goodvar_stack[v] = 0;
		}	
	}

	//add goodvar
	for(i=0; i<var_neighbor_count[flipvar]; ++i)   //从邻居中寻找新的好变元
	{
		v = var_neighbor[flipvar][i];
		
		conf_change[v] = 1;
		
		//if ( item_conflict_times[v]>0 || (item_conflict_times[v]==0 && score[v]>0) ) 
        if ( item_conflict_times[v]>0 )
		{
            if(already_in_goodvar_stack[v] ==0)
            {
                push(v,goodvar_stack);
                already_in_goodvar_stack[v] = 1;
            }
		}
	}
	
	conf_change[flipvar] = 0;
}

int pick_var()
{
	int c,i,v;
	int best_hscore;
	int best_sscore;
	int best_score;
	int v_score;
	
	int best_var;
	
	if( (random()%MY_RAND_MAX_INT)*BASIC_SCALE< pacprob ) //随机游走概率
	{
		if (conflict_edge_stack_count>0) 
        {
            int selected_edge = conflict_edge_stack[random()%conflict_edge_stack_count];
            return edge[selected_edge][random()%2];
        }
		else 
        {
            int selected_element = uncovered_stack[random()%uncovered_stack_count]-edgenum;
            return n_ele[selected_element][random()%n_ele_count[selected_element]];
        }
	}
	
	if( (random()%MY_RAND_MAX_INT)*BASIC_SCALE< prob )
	{
		if( conflict_edge_stack_count > 0 ) 
        {
            int selected_edge = conflict_edge_stack[random()%conflict_edge_stack_count];
            int item1 = edge[selected_edge][0];
            int item2 = edge[selected_edge][1];
            if( score[item1] > score[item2] )
            {
                return item1;
            }
            else if( score[item2] > score[item1] )
            {
                return item2;
            }
            else
            {
                return edge[selected_edge][random()%2];
            }
        }
		else 
        {
            int selected_element= uncovered_stack[random()%uncovered_stack_count]-edgenum;
		
            v = n_ele[selected_element][0];
		    best_array[0] = v;
		    best_count = 1;
		    best_sscore = score[v];
    
		    for(i=1; i<n_ele_count[selected_element]; i++)
		    {
		    	v = n_ele[selected_element][i];
    
		    	if(score[v]>best_sscore)
		    	{
		    		best_array[0] = v;
		    		best_count = 1;
		    		best_sscore = score[v];
		    	}
		    	else if(score[v]==best_sscore)
		    	{
		    		best_array[best_count++] = v;
		    	}
		    }
    
		    return best_array[random()%best_count]; //平局随机选
        }
	}
	

	if(goodvar_stack_count>0 )  //大多数时候采用的策略
	{
        best_count=0;
        
        for(i=0; i<goodvar_stack_count; ++i)
		{
			v = goodvar_stack[i];
            
            // if(item_conflict_times[v]>0 && hard_cscc[v]==1)
            if(hard_cscc[v]==1)
            {
                best_array[best_count]=v;
                best_count++;
            }
        }
        
        if (best_count>0)
        {
            if (best_count==1) return best_array[0];

            if (best_count>hd_count_threshold && (random()%MY_RAND_MAX_INT)*BASIC_SCALE<rdprob)
            {
            	
                int v1,v2;
                if (best_count==2) {
                    v1 = best_array[0];
                    v2 = best_array[1];
                }
                else 
                {
                    v1 = best_array[random()%best_count];
                    v2 = best_array[random()%best_count];
                    while (v2==v1) 
                        v2 = best_array[random()%best_count];
                }
                
                best_var=v1;
                if(item_conflict_times[v2]>item_conflict_times[v1]) best_var=v2;
                else if(item_conflict_times[v2]==item_conflict_times[v1])
                {   
                    if (score[v2]>score[v1]) best_var=v2;
                    else if (score[v2]==score[v1] && time_stamp[v2]<time_stamp[v1]) best_var=v2;
                }

                return best_var;
            }
            
            else
            {
            	
                best_var = best_array[0];
                for (i=1; i<best_count; ++i) 
                {
                    v = best_array[i];
                    if (item_conflict_times[v]>item_conflict_times[best_var]) best_var=v;
                    else if (item_conflict_times[v]==item_conflict_times[best_var])
                    {   
                        if (score[v]>score[best_var]) best_var=v;
                        else if (score[v]==score[best_var] && time_stamp[v]<time_stamp[best_var]) best_var=v;
                    } 
                }
                return best_var;
            }
        }
	}
	
	ccmpvars_count = 0;
	
	for(i=0; i<conflict_edge_stack_count; i++)
	{
		int cur_edge = conflict_edge_stack[i];
        int item1 = edge[cur_edge][0];
        int item2 = edge[cur_edge][1];
        if( conf_change[item1] == 1 && already_in_ccmpvars[item1] != step )
        {
            ccmpvars[ccmpvars_count++] = item1;
			already_in_ccmpvars[item1] = step;
        }
        if( conf_change[item2] == 1 && already_in_ccmpvars[item2] != step )
        {
            ccmpvars[ccmpvars_count++] = item2;
			already_in_ccmpvars[item2] = step;
        }
	}
	
	for(i=0; i<uncovered_stack_count; i++)
	{
		int cur_ele = uncovered_stack[i]-edgenum;
        for( int j=0; j<n_ele_count[cur_ele]; j++ )
        {
            int cur_item = n_ele[cur_ele][j];
            if(conf_change[cur_item]==1 && already_in_ccmpvars[cur_item] != step)
			{
				ccmpvars[ccmpvars_count++] = cur_item;
				already_in_ccmpvars[cur_item] = step;
			}
        }
	}
	
	if(ccmpvars_count>0)
	{
		v = ccmpvars[0];
		best_array[0] = v;
		best_count = 1;
		best_hscore = item_conflict_times[v];
		best_sscore = score[v];
		
		for(i=1; i<ccmpvars_count; i++)
		{
			v = ccmpvars[i];
			
			if(item_conflict_times[v]>best_hscore)
			{
				best_array[0] = v;
				best_count = 1;
				best_hscore = item_conflict_times[v];
				best_sscore = score[v];
			}
			else if(item_conflict_times[v]==best_hscore)
			{
				if(score[v]>best_sscore)
				{
					best_array[0] = v;
					best_count = 1;
					best_sscore = score[v];
				}
				else if(score[v]==best_sscore)
				{
					best_array[best_count++] = v;
				}
			}
		}
		
		return best_array[random()%best_count];
	}
	else
	{
		if (conflict_edge_stack_count>0) 
        {
            int selected_edge = conflict_edge_stack[random()%conflict_edge_stack_count];
            return edge[selected_edge][random()%2];
        }
		else 
        {
            int selected_element = uncovered_stack[random()%uncovered_stack_count]-edgenum;
            return n_ele[selected_element][random()%n_ele_count[selected_element]];
        }
	}
}

void local_search()
{
	int flipvar;
    
	for (step = 1; step<max_flips; ++step) 
	{
		if ( conflict_edge_stack_count==0 && current_uncovered_weight<best_uncovered_weight )
        {
            best_uncovered_weight = current_uncovered_weight;
			cout << "o " << best_uncovered_weight << "    " << step << endl;                 
            for( int i=1; i<=itemnum; i++ ) 
            {
                best_solution[i] = current_solution[i];
            }
            if(best_uncovered_weight==0) 
            {
                return;
            }
        }
		
		flipvar = pick_var();
		flip(flipvar);
		time_stamp[flipvar] = step;
        
        if (step%100==0)
        {
            if( get_runtime() >= cutoff_time ) 
            {
                return;
            }
        }
	}
}

#endif