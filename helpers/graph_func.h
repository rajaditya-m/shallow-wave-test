#ifndef __GRAPHALGORITHMS_H_
#define __GRAPHALGORITHMS_H_

#include <queue>
#include <vector>

void breadth_first_search(int number_node,int** graph,int start_v)
{
	std::vector<int> visited (number_node,-1);
	// std::fill(visited.begin(),visited.end(),-1);
	std::vector<int> connected_component;
	std::queue<int> q;
	int cur_v;
	visited[start_v]=0;
	q.push(start_v);
	while(!q.empty())
	{
		cur_v = q.front();
		//connected_component.push_back(cur_v);
		for(int j=0;j<number_node;j++)
		{
			if(graph[cur_v][j])
			{
				if(visited[j]==-1)
				{
					q.push(j);
					visited[j] = visited[cur_v]+1;	
				}
			}				
		}
	}
	for(int i=0;i<number_node;i++)
	{
		if(visited[i]!=-1)
			std::cout << i << "\n";
	}
}

#endif