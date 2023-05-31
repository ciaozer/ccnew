#ifndef _PMS_H_
#define _PMS_H_

#include "build.h"

inline void uncovered(int element_index)
{
    index_in_uncovered_stack[element_index] = uncovered_stack_count;
    push(element_index, uncovered_stack);
    current_uncovered_weight += original_weight[element_index];
}

inline void covered(int element_index)
{
    int last_uncovered_element = pop(uncovered_stack);
    int index_for_last = index_in_uncovered_stack[element_index];
    uncovered_stack[index_for_last] = last_uncovered_element;
    index_in_uncovered_stack[last_uncovered_element] = index_for_last;
    current_uncovered_weight -= original_weight[element_index];
}

inline void conflict(int edge_index)
{
    index_in_conflict_edge_stack[edge_index] = conflict_edge_stack_count;
    push(edge_index,conflict_edge_stack);   
}

inline void deconflict(int edge_index)
{
    int last_conflict_edge = pop(conflict_edge_stack);
    int index_for_last = index_in_conflict_edge_stack[edge_index];
    conflict_edge_stack[index_for_last] = last_conflict_edge;
    index_in_conflict_edge_stack[last_conflict_edge] = index_for_last;
}

void flip(int flipitem)
{
	int original_item_conflict_times = item_conflict_times[flipitem];
    int original_score = score[flipitem];
	current_solution[flipitem] = 1 - current_solution[flipitem];

    //更新冲突图的情况
    for( int i=0; i<g_adj_count[flipitem]; i++ )         //遍历翻转集合的所有边
    {
        int cur_edge = g_adj[flipitem][i];
        if( current_solution[flipitem] == 0 )            //选出该集合
        {
            edge_node_count[cur_edge]++;

            if( edge_node_count[cur_edge] == 2 )             //一条边选点个数1到2
            {
                item_conflict_times[node[cur_edge]]++;
            }
            else if( edge_node_count[cur_edge] == 1 )        //一条边选点个数0到1
            {
                node[cur_edge] = flipitem;
                int item1 = edge[cur_edge][0];
                int item2 = edge[cur_edge][1];
                item_conflict_times[item1]--;
                item_conflict_times[item2]--;
                hard_cscc[item1] = 1;
                hard_cscc[item2] = 1;
                deconflict(cur_edge);
            }
        }

        else                                        //选入该集合
        {
            edge_node_count[cur_edge]--;
            if( edge_node_count[cur_edge] == 1 )         //一条边选点2到1
            {
                int item1 = edge[cur_edge][0];
                int item2 = edge[cur_edge][1];
                if( current_solution[item1] == 0 )
                {
                    item_conflict_times[item1]--;
                    node[cur_edge] = item1;
                }
                else
                {
                    item_conflict_times[item2]--;
                    node[cur_edge] = item2;
                }
            }
            else if( edge_node_count[cur_edge] == 0 )    //一条边选点1到0
            {
                int item1 = edge[cur_edge][0];
                int item2 = edge[cur_edge][1];
                item_conflict_times[item1]++;
                item_conflict_times[item2]++;
                hard_cscc[item1] = 1;
                hard_cscc[item2] = 1;
                conflict(cur_edge); 
            }
        }
    }

    //处理翻转集合覆盖的元素
	for( int i=0; i<m_item_count[flipitem]; i++ )
    {
        int cur_ele = m_item[flipitem][i];

        if( current_solution[flipitem] == 1 )       //选入该集合
        {
            element_cover_times[cur_ele]++;
            
            if( element_cover_times[cur_ele] == 2 )      //元素覆盖数1到2
            {    
                score[only_cover_item[cur_ele]] += original_weight[cur_ele];
            }
            else if( element_cover_times[cur_ele] == 1 ) //元素覆盖数0到1
            {
                only_cover_item[cur_ele] = flipitem;//record the only true lit's var
                
                for( int j=0; j<n_ele_count[cur_ele]; j++ )
                {
                    int cur_item = n_ele[cur_ele][j];
                    score[cur_item] -= original_weight[cur_ele];
                }
                covered(cur_ele);
            }
        }

        else                                    //选出该集合
        {
            element_cover_times[cur_ele]--;
            if( element_cover_times[cur_ele] == 1 )          //元素覆盖数2到1
            {
                for( int j=0; j<n_ele_count[cur_ele]; j++ )
                {
                    int cur_item = n_ele[cur_ele][j];
                    if( current_solution[cur_item] == 1 )
                    {
                        score[cur_item] -= original_weight[cur_ele];
                        only_cover_item[cur_ele] = cur_item;
                        break;
                    }
                }
            }
            else if( element_cover_times[cur_ele] == 0 )     //元素覆盖数1到0
            {
                for( int j=0; j<n_ele_count[cur_ele]; j++ )
                {
                    int cur_item = n_ele[cur_ele][j];
                    score[cur_item] += original_weight[cur_ele];
                }
                uncovered(cur_ele); 
            }   
        }
	}

	hard_cscc[flipitem] = 0;
	item_conflict_times[flipitem] = -original_item_conflict_times;
    score[flipitem] = -original_score;

	//更新gooditem
	for( int i=gooditem_stack_count-1; i>=0; i--)  //移除gooditem中不满足要求的集合
	{
		int cur_item = gooditem_stack[i];
        if( item_conflict_times[cur_item] < 0 || ( item_conflict_times[cur_item] == 0 && score[cur_item] <= 0 ) )
		{
			gooditem_stack[i] = pop(gooditem_stack);  //交换该元素和栈顶元素
			is_in_gooditem_stack[cur_item] = 0;
		}	
	}

	for( int i=0; i<neighbor_count[flipitem]; i++ )   //从邻居中寻找新的好集合
	{
		int cur_item = neighbor[flipitem][i];
		
		conf_change[cur_item] = 1;                      //邻居发生改变
		
        if( item_conflict_times[cur_item] > 0 || ( item_conflict_times[cur_item] == 0 && score[cur_item] > 0 ) )
		{
            if( is_in_gooditem_stack[cur_item] == 0 )
            {
                push(cur_item, gooditem_stack);
                is_in_gooditem_stack[cur_item] = 1;
            }
		}
	}
	
	conf_change[flipitem] = 0;
}

int pick_var()
{	
	if( (random()%MY_RAND_MAX_INT)*BASIC_SCALE < p_random_walk ) //随机游走概率
	{
		if( conflict_edge_stack_count > 0 )         //存在冲突边 
        {
            int selected_edge = conflict_edge_stack[random()%conflict_edge_stack_count];
            return edge[selected_edge][random()%2];
        }
		else        //选一个未覆盖元素
        {
            int selected_element = uncovered_stack[random()%uncovered_stack_count];
            return n_ele[selected_element][random()%n_ele_count[selected_element]];
        }
	}
	
	if( (random()%MY_RAND_MAX_INT)*BASIC_SCALE < p_without_hard_conf )
	{
		if( conflict_edge_stack_count > 0 )     //存在冲突边
        {
            int selected_edge = conflict_edge_stack[random()%conflict_edge_stack_count]; //随机选冲突边
            int item1 = edge[selected_edge][0];
            int item2 = edge[selected_edge][1];
            if( score[item1] > score[item2] )       //这里的score应该都是负值，移除一个集合，只会损失覆盖
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
		else                //不存在冲突
        {
            int selected_element = uncovered_stack[random()%uncovered_stack_count];  //随机选择一个未覆盖元素
		
            int cur_item = n_ele[selected_element][0];
            int best_array[itemnum+10];
		    best_array[0] = cur_item;
		    int best_count = 1;
		    int best_score = score[cur_item];
    
		    for( int i=1; i<n_ele_count[selected_element]; i++ )  //遍历能覆盖该元素的所有集合
		    {
		    	cur_item = n_ele[selected_element][i];
    
		    	if( score[cur_item] > best_score )      //优先选择能覆盖更多元素的
		    	{
		    		best_array[0] = cur_item;
		    		best_count = 1;
		    		best_score = score[cur_item];
		    	}
		    	else if( score[cur_item] == best_score )
		    	{
		    		best_array[best_count] = cur_item;
                    best_count++;
		    	}
		    }
    
		    return best_array[random()%best_count]; //平局随机选
        }
	}

	if( gooditem_stack_count > 0 )  //大多数时候采用的策略
	{
        int best_array[itemnum+10];
        int best_count = 0;
        
        for( int i=0; i<gooditem_stack_count; i++ )
		{
			int cur_item = gooditem_stack[i];
            
            //if( hard_cscc[cur_item] == 1 )
            if(1)
            {
                best_array[best_count] = cur_item;  //存储是好集合且集合所在边状态发生变化的集合
                best_count++;
            }
        }
        
        if( best_count > 0 )
        {
            if( best_count == 1 ) 
            {
                return best_array[0];
            }

            if( best_count > hd_count_threshold && (random()%MY_RAND_MAX_INT)*BASIC_SCALE < rdprob ) //好集合很多且按一定比例
            {
                int item1, item2;                   //随机选best_array中的两个
                if( best_count == 2 ) 
                {
                    item1 = best_array[0];
                    item2 = best_array[1];
                }
                else 
                {
                    item1 = best_array[random()%best_count];
                    item2 = best_array[random()%best_count];
                    while (item2==item1) 
                    {
                        item2 = best_array[random()%best_count];
                    }
                }
                
                int best_item = item1;
                if( item_conflict_times[item2] > item_conflict_times[item1] ) 
                {
                    best_item = item2;
                }
                else if( item_conflict_times[item2] == item_conflict_times[item1] )
                {   
                    if( score[item2] > score[item1] ) 
                    {
                        best_item = item2;
                    }
                    else if( score[item2] == score[item1] && time_stamp[item2] < time_stamp[item1] ) 
                    {
                        best_item = item2;
                    }
                }

                return best_item;
            }
            
            else
            {
                int best_item = best_array[0];
                for( int i=1; i<best_count; i++ ) 
                {
                    int cur_item = best_array[i];
                    if( item_conflict_times[cur_item] > item_conflict_times[best_item] ) //优先选减少更多冲突的
                    {
                        best_item = cur_item;
                    }
                    else if( item_conflict_times[cur_item] == item_conflict_times[best_item] )
                    {   
                        if( score[cur_item] > score[best_item]) //同样冲突选择能覆盖更多元素的
                        {
                            best_item = cur_item;
                        }
                        else if( score[cur_item] == score[best_item] && time_stamp[cur_item] < time_stamp[best_item] ) //选择更远时间之前修改的
                        {
                            best_item = cur_item;
                        }
                    } 
                }

                return best_item;
            }
        }
	}
	
	int ccmpvars_count = 0;
    int ccmpvars[itemnum+10];
	
	for( int i=0; i<conflict_edge_stack_count; i++ )    //遍历冲突边
	{
		int cur_edge = conflict_edge_stack[i];
        int item1 = edge[cur_edge][0];
        int item2 = edge[cur_edge][1];
        if( conf_change[item1] == 1 && already_in_ccmpvars[item1] != step ) //加入边中邻居变化的点
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
	
	for( int i=0; i<uncovered_stack_count; i++ )        //遍历没被覆盖的元素
	{
		int cur_ele = uncovered_stack[i];
        for( int j=0; j<n_ele_count[cur_ele]; j++ )     //遍历能覆盖该元素的集合
        {
            int cur_item = n_ele[cur_ele][j];
            if( conf_change[cur_item]==1 && already_in_ccmpvars[cur_item] != step )
			{                                           //加入邻居变化的集合
				ccmpvars[ccmpvars_count] = cur_item;
				already_in_ccmpvars[cur_item] = step;
                ccmpvars_count++;
			}
        }
	}
	
	if( ccmpvars_count > 0 )
	{
		int cur_item = ccmpvars[0];
        int best_array[itemnum+10];
		best_array[0] = cur_item;
		int best_count = 1;
		int best_conflict = item_conflict_times[cur_item];
		int best_score = score[cur_item];
		
		for( int i=1; i<ccmpvars_count; i++ )
		{
			cur_item = ccmpvars[i];
			
			if( item_conflict_times[cur_item] > best_conflict)
			{
				best_array[0] = cur_item;
				best_count = 1;
				best_conflict = item_conflict_times[cur_item];
				best_score = score[cur_item];
			}
			else if( item_conflict_times[cur_item] == best_conflict )
			{
				if( score[cur_item] > best_score)
				{
					best_array[0] = cur_item;
					best_count = 1;
					best_score = score[cur_item];
				}
				else if(score[cur_item] == best_score)
				{
					best_array[best_count] = cur_item;
                    best_count++;
				}
			}
		}
		
		return best_array[random()%best_count];
	}

	else
	{
		if( conflict_edge_stack_count > 0 ) 
        {
            int selected_edge = conflict_edge_stack[random()%conflict_edge_stack_count];
            return edge[selected_edge][random()%2];
        }
		else 
        {
            int selected_element = uncovered_stack[random()%uncovered_stack_count];
            return n_ele[selected_element][random()%n_ele_count[selected_element]];
        }
	}
}

void local_search()
{   
	for( step = 1; step < max_flips; step++ ) 
	{
		if( conflict_edge_stack_count == 0 && current_uncovered_weight < best_uncovered_weight )
        {
            best_uncovered_weight = current_uncovered_weight;
			cout << "o " << best_uncovered_weight << "    " << step << "    " << get_runtime() << endl;                 
            for( int i=1; i<=itemnum; i++ ) 
            {
                best_solution[i] = current_solution[i];
            }
            if(best_uncovered_weight==0) 
            {
                return;
            }
        }
		
		int flipitem = pick_var();
		flip(flipitem);
		time_stamp[flipitem] = step;
        
        if( step%100 == 0 )
        {
            if( get_runtime() >= cutoff_time ) 
            {
                return;
            }
        }
	}
}

#endif