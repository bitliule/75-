#include<cstdio>
#include<iostream>
#include<queue>
#include<cstring>
using namespace std;

const int MAX_RESOURCES_NUM = 100;      //系统资源的最大种类
const int MAX_PROCESS_NUM = 100;        //进程的最大数量
queue<int> SecuritySequence;        //保存安全序列的队列

struct SYSTEM                           //定义系统状态，关系为：Need[i][j] = Max[i][j] - Allocation[i][j]
{
	int Finish[MAX_PROCESS_NUM];               //标记进程是否被完成
	int Available[MAX_RESOURCES_NUM];           //可利用资源向量，第i种资源的数量为Available[i]
	int Max[MAX_PROCESS_NUM][MAX_RESOURCES_NUM];         //最大需求矩阵，第i个进程对第j个资源的最大需求为Max[i][j]
	int Allocation[MAX_PROCESS_NUM][MAX_RESOURCES_NUM];  //分配矩阵,第i个进程已获得第j个资源的数量为Allocation[i][j]
	int Need[MAX_PROCESS_NUM][MAX_RESOURCES_NUM];        //需求矩阵，第i个进程还需要第j个资源的数量为Need[i][j]
};

//安全性算法，判断系统是否存在安全序列。已决定是否分配资源给进程,传入参数为当前系统的状态。进程数，资源数
int IsSafe(SYSTEM status, int process_num, int resources_num)
{
	//由于需要不断循环的寻找可以分配资源的进程，需要找一个循环的最大圈数
	//分析：每一次循环都要找到一个可分配资源的进程，否则下一次循环也找不到可分配资源的进程
	//即最大的圈数为资源的种类数
	while (!SecuritySequence.empty())          //置空安全序列
		SecuritySequence.pop();
	for (int i = 0; i < process_num; i++)
	{
		int have_process = 0;               //标记该趟是否可以找到可调度的进程。
		for (int j = 0; j < process_num; j++)
		{
			if (status.Finish[j] == 0)       //进程程j未被完成
			{
				int success = 1;            //由于标记系统可分配的各类资源是否满足进程j的需求
				for (int k = 0; k < resources_num; k++)      //检查系统可分配的各类资源是否满足进程j的需求
				{
					if (status.Available[k] < status.Need[j][k])     //系统有一类资源不满足进程需求
					{
						success = 0;
						break;
					}
				}
				if (success == 1)             //系统所有类资源都满足进程需求,调度进程 j ，运行完成 j ，并回收资源。
				{
					have_process = 1;                           //已找到可调度的进程
					status.Finish[j] = 1;                          //标记进程 j 已完成
					for (int l = 0; l < resources_num; l++)      //回收各类资源
					{
						status.Available[l] += status.Allocation[j][l];
					}
					SecuritySequence.push(j);                   //进程 j 入队保存
				}
			}
		}
		if (have_process == 0)                                   //没有可调度的进程跳出；
		{
			break;
		}
	}
	int is_safe = 1;
	for (int i = 0; i < process_num; i++)                //判断所有进程是否以完成，如果已完成则存在安全序列，否则，不存在；
	{
		if (status.Finish[i] == 0)                       //有进程未被调度，所有不存在安全序列；
			is_safe = 0;
	}
	return is_safe;
}

//银行家算法
int  BankerAlgorithm(SYSTEM status, int process_num, int resources_num, int process, int Request[])
{
	for (int i = 0; i < resources_num; i++)          //判断需求资源是否大于最大需求
	{
		if (Request[i] > status.Need[process][i])    //需求资源大于最大需求,出错
		{
			printf("需求资源大于最大需求\n");
			return 0;
		}
	}
	for (int i = 0; i < resources_num; i++)             //需求资源是否大于可分配资源
	{
		if (Request[i] > status.Available[i])       //需求资源大于可分配资源，无法调度
		{
			printf("需求资源大于可分配资源\n");
			return 0;
		}
	}
	//满足分配要求。试探性分配，并判断是否存在安全序列
	for (int i = 0; i < resources_num; i++)
	{
		//试探性分配
		status.Available[i] -= Request[i];
		status.Allocation[process][i] += Request[i];
		status.Need[process][i] -= Request[i];
	}
	//判断是否存在安全序列
	if (IsSafe(status, process_num, resources_num))
	{
		printf("存在安全序列：\n");
		while (!SecuritySequence.empty())            //输出安全序列
		{
			printf("%d ", SecuritySequence.front());
			SecuritySequence.pop();
		}
		return 1;
	}
	else                        //没有安全序列，试探性分配作废
	{
		printf("不存在安全序列，不分配资源\n");
		for (int i = 0; i < resources_num; i++)
		{
			status.Available[i] += Request[i];
			status.Allocation[process][i] -= Request[i];
			status.Need[process][i] += Request[i];
			return 0;
		}
	}
}

//输出系统状态
void display(SYSTEM status, int process_num, int resources_num)
{
	//系统状态Available
	printf("系统状态Available：\n");
	for (int i = 0; i < resources_num; i++)
		printf("%d ", status.Available[i]);
	putchar('\n');
	//系统状态最大需求矩阵Max
	printf("请初始化系统状态最大需求矩阵Max：\n");
	for (int i = 0; i < process_num; i++)
	{
		for (int j = 0; j < resources_num; j++)
		{
			printf("%d ", status.Max[i][j]);
		}
		putchar('\n');
	}
	//系统状态最大需求矩阵Allocation
	printf("可利用资源向量Allocation：\n");
	for (int i = 0; i < process_num; i++)
	{
		for (int j = 0; j < resources_num; j++)
		{
			printf("%d ", status.Allocation[i][j]);
		}
		putchar('\n');
	}
	//需求矩阵Need
	printf("需求矩阵Need\n");
	for (int i = 0; i < process_num; i++)
	{
		for (int j = 0; j < resources_num; j++)
		{
			printf("%d ", status.Need[i][j]);
		}
		putchar('\n');
	}
}

int main(int argc, char* argv[])
{
	//    //重定义输出输入流至文件
	//    freopen("data.in","r",stdin);
	//    freopen("data.out","w",stdout);
	SYSTEM status;      //定义系统状态
	int resources_num, process_num;      //定义资源数，进程数
	//始化进程数，和资源种数
	printf("请初始化进程数，和资源种数：\n");
	scanf_s("%d %d", &process_num, &resources_num);
	//始化系统状态Available
	printf("请初始化系统状态Available：\n");
	for (int i = 0; i < resources_num; i++)
		scanf_s("%d", &status.Available[i]);
	//始化系统状态最大需求矩阵Max
	printf("请初始化系统状态最大需求矩阵Max：\n");
	for (int i = 0; i < process_num; i++)
	{
		for (int j = 0; j < resources_num; j++)
		{
			scanf_s("%d", &status.Max[i][j]);
		}
	}
	//始化系统状态最大需求矩阵Allocation
	printf("请初始化可利用资源向量Allocation：\n");
	for (int i = 0; i < process_num; i++)
	{
		for (int j = 0; j < resources_num; j++)
		{
			scanf_s("%d", &status.Allocation[i][j]);
		}
	}
	//自动处理需求矩阵Need
	for (int i = 0; i < process_num; i++)
	{
		for (int j = 0; j < resources_num; j++)
		{
			status.Need[i][j] = status.Max[i][j] - status.Allocation[i][j];
		}
	}
	int process;                        //记录资源请求的进程的号
	int Request[MAX_RESOURCES_NUM];     //记录该进程对各资源请求的数量
	if (IsSafe(status, process_num, resources_num) == 0)
	{
		printf("初始状态不存在安全序列\n");
		return 0;
	}
	else
	{
		printf("初始状态安全\n");
		printf("存在安全序列：\n");
		while (!SecuritySequence.empty())            //输出安全序列
		{
			printf("%d ", SecuritySequence.front());
			SecuritySequence.pop();
		}
		putchar('\n');
	}
	while (1)        //模拟系统运行
	{
		display(status, process_num, resources_num);
		printf("请输入资源请求：process\n");
		int process;
		int Request[MAX_RESOURCES_NUM];
		scanf_s("%d", &process);
		if (process == -1)
			break;
		printf("请输入资源请求：Request[](the length of the Request is that of resources_num)\n");
		for (int i = 0; i < resources_num; i++)
		{
			scanf_s("%d", &Request[i]);
		}
		if (BankerAlgorithm(status, process_num, resources_num, process, Request) == 1)
		{
			int process_end = 1;        //判断进程是否执行完
			for (int i = 0; i < resources_num; i++)
			{
				status.Available[i] -= Request[i];
				status.Allocation[process][i] += Request[i];
				status.Need[process][i] -= Request[i];
				if (status.Need[process][i] != 0)
					process_end = 0;
			}
			printf("资源分配成功\n");
			//进程执行完，回收资源
			if (process_end)
			{
				printf("进程P%d执行完,释放资源\n", process);
				for (int i = 0; i < resources_num; i++)
				{
					status.Available[i] += status.Allocation[process][i];
				}
			}
		}
	}
	return 0;
}
