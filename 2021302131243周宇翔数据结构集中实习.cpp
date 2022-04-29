#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <map>
#include <queue>
#include <vector>
#include <algorithm>
#include <stack>
using namespace std;
const int INF = 0x3f3f3f3f;//无穷大
struct Arcs;
typedef struct {
	bool pass = false;
	string country, city;//国家名称首都名称
	double latitude, longitude;//经纬度信息
	std::vector<Arcs*> arcs;
	int id;
} City, Vertex;
City ct[300];

typedef struct Arcs {
	string origi_city, destination_city, transport, other_information;//出发城市 目标城市 出行方式 其余信息
	double time, cost;//时间和花费
} Route;


typedef struct {
	Vertex* ver;//城市信息数组
	Arcs arcs[199][199];//路线信息
	int vexnum, arcnum;//城市的个数 路线的个数 表示 图的顶点数和边数
	string transport[200];//交通方式  表示  边的其他信息
	string other_information[200];//路径网址
} Graph;//定义邻接矩阵
Graph g;
map<string, int>city_id;//定义城市id

typedef struct
{
	int pre; //前缀
	bool vis; //标记  
	double c;//花费数据
} WAY;  //Dijkstra的路径信息



struct node {
	int x;
	double cost;
};//Dijkstra算法信息结构体


int read_city(City* ct) {//读取城市，返回城市个数
	FILE *file; int i = 1;
	fopen_s(&file, "C:\\Users\\周宇翔\\Desktop\\数据结构实习的csv文件\\cities.csv", "r");//打开cities.csv文件
	if (file) {
		char ch;//读取csv文件数据
		while (!feof(file)) {
			ch = fgetc(file); ct[i].country = ""; ct[i].city = "";
			for (; ch != ','; ch = fgetc(file)) {//读取国家
				ct[i].country += ch;
			}
			ch = fgetc(file);
			for (; ch != ','; ch = fgetc(file)) {//读取首都
				ct[i].city += ch;
			}
			fscanf_s(file, "%lf,", &ct[i].latitude);//读取首都经纬度
			fscanf_s(file, "%lf\n", &ct[i].longitude);
			city_id.emplace(ct[i].city, i);
			ct[i].id = i;
			i++;
		}
		fclose(file);
	}
	else {
		printf("ERROR1!");
		exit(0);
	}
	return i - 1;//返回城市个数，作为有向图顶点
}

int read_route(Route* rt) {//读取路线，并文件返回路线数
	FILE *file; int i = 1; int j = 1;
	fopen_s(&file, "C:\\Users\\周宇翔\\Desktop\\数据结构实习的csv文件\\routes.csv", "r");//routes.csv文件
	if (file) {
		char ch;
		while (!feof(file)) {
			//读取csv文件数据
			ch = fgetc(file);
			for (; ch != ','; ch = fgetc(file)) {//读取起始城市
				rt[i].origi_city += ch;
			}
			ch = fgetc(file);
			for (; ch != ','; ch = fgetc(file)) {//读取目的地
				rt[i].destination_city += ch;
			}
			ch = fgetc(file);
			for (; ch != ','; ch = fgetc(file)) {//读取交通方式
				rt[i].transport += ch;
			}
			fscanf_s(file, "%lf,", &rt[i].time);//读取时间
			fscanf_s(file, "%lf,", &rt[i].cost);//读取花费
			ch = fgetc(file);
			for (; ch != '\n'; ch = fgetc(file)) {//读取其他信息（网址）
				rt[i].other_information += ch;
			}
			ct[city_id[rt[i].origi_city]].arcs.push_back(&rt[i]);
			i++;
			if (i == 1975) //读取完退出循环,已知文件共有1975条数据
				break;
		}
		fclose(file);
		cout << "读取数据成功\n" << endl;
	}
	else {
		printf("ERROR2!");
		exit(0);
	}
	return i - 1;//返回路线数，作为有向图的边数
}

void Init(Graph& g, City* ct, Route* route) {//初始化图的信息
	for (int i = 1; i <= g.vexnum; i++) {
		//city_id[g.ver[i].city] = i;//赋值每个城市的编号
	}
	//初始化图的邻接矩阵
	for (int i = 0; i <= 199; i++) {
		for (int j = 0; j <= 199; j++) {
			if (i == j)
				g.arcs[i][j].cost = g.arcs[i][j].time = 0;
			else g.arcs[i][j].cost = g.arcs[i][j].time = INF;
		}
	}
	for (int i = 1; i <= g.arcnum; i++) {
		int id1 = city_id[route[i].origi_city];
		int id2 = city_id[route[i].destination_city];
		//建立花费、时间、交通方式、其他信息的双向边（有向图）
		g.arcs[id1][id2].cost = route[i].cost;
		g.arcs[id1][id2].time = route[i].time;
		g.transport[id1] = route[i].transport;
		g.other_information[id1] = route[i].other_information;

		g.arcs[id2][id1].cost = route[i].cost;
		g.arcs[id2][id1].time = route[i].time;
		g.transport[id2] = route[i].transport;
		g.other_information[id2] = route[i].other_information;
	}
	return;
}

string depart, dest, k;//分别表示起点城市、终点城市、最短路径决定基准

void Dijkstra(Graph& g, WAY way[][199], float ThatHuafei[][199]) {//最短路径算法

	cout << "输入城市起点:" << endl;
	cin >> depart;
	cout << "输入城市终点:" << endl;
	cin >> dest;
	cout << "请输入以什么为基准,即需要那种方式消耗最少（time or cost）：" << endl;
	cin >> k;
	float min = 0;
	double tm[199][199];               //新数组存放权值信息
	for (int i = 0; i < 199; i++)      //判断最短路径基准
	{
		for (int j = 0; j < 199; j++) {
			if (k == "cost") tm[i][j] = g.arcs[i][j].cost;
			else tm[i][j] = g.arcs[i][j].time;
		}
	}
	for (int v = 0; v < g.vexnum; v++) {
		g.ver[v].pass = false;       //未求得到v的最短路径
		ThatHuafei[city_id[depart]][v] = tm[city_id[depart]][v];  //初始化最短路径，初始值为直接路径
		if (ThatHuafei[city_id[depart]][v] < 20000) {
			way[v][city_id[depart]].vis = true;
			way[v][v].vis = true;      //初始化way数组
			way[v][v].pre = city_id[depart];

		}
		else {
			way[v][v].pre = -1;
		}
	}
	ThatHuafei[city_id[depart]][city_id[depart]] = 0;
	g.ver[city_id[depart]].pass = true;
	for (int i = 0; i < g.vexnum; i++) {
		int v = 0;                            //v选择一条最短路径
		float min = 20000;                                //min是当前所知道depart的最近距离，初始为无穷
		for (int w = 0; w < g.vexnum; w++) {  //检查其他所有顶点
			if (g.ver[w].pass == false) {   //未被并入最短路径
				if (ThatHuafei[city_id[depart]][w] < min) {  //比当前最短路径短
					v = w;
					min = ThatHuafei[city_id[depart]][w];//取当前路径为最短路径
				}
			}
		}
		g.ver[v].pass = true;              //最短路径顶点v并入路径

		for (int w = 0; w < g.vexnum; w++) {  //更新当前最短路径及距离
			if (g.ver[w].pass == false && ((min + tm[v][w]) < ThatHuafei[city_id[depart]][w])) {
				//depart到w的最短路径尚未找到
				//且 depart到v 加 v到w 的最短路径 小于当前 depart到w 最短路径
				ThatHuafei[city_id[depart]][w] = min + tm[v][w];             //更新depart到w的最短路径长度
				 //depart到w的最短路径的顶点更新为 depart到v 加 v到w 的顶点
				for (int k = 0; k < g.vexnum; k++) {
					way[w][k].vis = way[v][k].vis;
					way[w][k].pre = way[v][k].pre;//更新w到k点的前驱，达到路径储存的效果
				}
				way[w][w].vis = true;
				way[w][w].pre = v;                        //更新路径，储存way路径，便于输出
				min = ThatHuafei[city_id[depart]][city_id[dest]];
			}
		}
	}
	//输出所需总时间或者花费
	cout << "总" << k << "为：" << way[city_id[depart]][city_id[dest]].c << endl;

}

//void dijstra_cost(City* origin, Arcs*(&path)[210])//最短路径算法，cost为基准，修正版
//{
//#define _FROM(r) (city_id[r->origi_city])
//#define _TO(r) (city_id[r->destination_city])
//	bool solved[210] = {};
//	solved[origin->id] = true;
//	City* ct_c[210] = {};
//	int m = 0;
//	ct_c[m++] = origin;
//	float dist[210] = {};
//	for (int i = 0; i < 210; i++)
//	{
//		dist[i] = INF;
//	}
//	dist[origin->id] = 0;
//	while(true)
//	{
//		Arcs* target = NULL;
//		for (int i = 0; i < m; i++)
//		{
//			for (Arcs* arc : ct_c[i]->arcs)
//			{
//				if (dist[_TO(arc)] > dist[ct_c[i]->id] + arc->cost)
//				{
//					dist[_TO(arc)] = dist[ct_c[i]->id] + arc->cost;
//					path[_TO(arc)] = arc;
//				}
//				if (!solved[_TO(arc)])
//				{
//					if (target == NULL || dist[_TO(target)] > dist[_TO(arc)])
//					{
//						target = arc;
//					}
//				}
//			}
//			
//		}
//		if (target == NULL)
//		{
//			break;
//		}
//		solved[_TO(target)] = true;
//		ct_c[m++] = &ct[_TO(target)];
//	}
//	
//#undef _FROM
//#undef _TO
//}
//
//void dijstra_time(City* origin, Arcs*(&path)[210], City* dest)//最短路径算法，time为基准，修正版
//{
//#define _FROM(r) (city_id[r->origi_city])
//#define _TO(r) (city_id[r->destination_city])
//	bool solved[210] = {};
//	solved[origin->id] = true;
//	City* ct_c[210] = {};
//	int m = 0;
//	ct_c[m++] = origin;
//	float dist[210] = {};
//	for (int i = 0; i < 210; i++)
//	{
//		dist[i] = INF;
//	}
//	dist[origin->id] = 0;
//	while (true)
//	{
//		Arcs* target = NULL;
//		for (int i = 0; i < m; i++)
//		{
//			for (Arcs* arc : ct_c[i]->arcs)
//			{
//				if (dist[_TO(arc)] > dist[ct_c[i]->id] + arc->time)
//				{
//					dist[_TO(arc)] = dist[ct_c[i]->id] + arc->time;
//					path[_TO(arc)] = arc;
//				}
//				if (!solved[_TO(arc)])
//				{
//					if (target == NULL || dist[_TO(target)] > dist[_TO(arc)])
//					{
//						target = arc;
//					}
//				}
//			}
//
//		}
//		if (target == NULL)
//		{
//			break;
//		}
//		solved[_TO(target)] = true;
//		ct_c[m++] = &ct[_TO(target)];
//	}
//
//#undef _FROM
//#undef _TO
//}

std::vector<Route*> Dijkstra_cost(City* origin, City* dest) {//最短路径（花费）
	float dist[199] = {};//最大
	for (size_t i = 0; i < 199; i++)
	{
		dist[i] = 90000000;
	}
	Route* _final[199] = {};
	int t = 1;
	City* set[199] = { origin };
	bool known[199] = {};
	known[origin->id] = 1;
	dist[origin->id] = 0;
	City* vertex = 0;
	Route* min = 0;
	do
	{
		vertex = 0;
		min = 0;
		for (int j = 0; j < t; j++)
		{
			City* city = set[j];
			for (int i = 0; i < city->arcs.size(); i++)
			{
				if (vertex == 0 && !known[city_id[city->arcs[i]->destination_city]])
				{
					vertex = &ct[city_id[city->arcs[i]->destination_city]];
					min = city->arcs[i];
				}
				if (city->arcs[i]->cost + dist[city->id] < dist[city_id[city->arcs[i]->destination_city]])
				{
					dist[city_id[city->arcs[i]->destination_city]] = city->arcs[i]->cost + dist[city->id];
					_final[city_id[city->arcs[i]->destination_city]] = city->arcs[i];
					if (!known[city_id[city->arcs[i]->destination_city]] && dist[city_id[min->destination_city]] > dist[city_id[city->arcs[i]->destination_city]])
					{
						vertex = &ct[city_id[city->arcs[i]->destination_city]];
						min = city->arcs[i];
					}
				}
			}
		}
		if (vertex != 0)
		{
			set[t] = vertex;
			known[vertex->id] = 1;
			t++;
		}
		else
		{
			break;
		}
	} while (true);

	City *path = dest;
	float cost;
	cost = dist[path->id];
	int x = 1;
	std::stack<Route*> stack;
	while (path != origin)
	{
		if (_final[path->id])
		{
			stack.push(_final[path->id]);
			path = &ct[city_id[_final[path->id]->origi_city]];
		}
		else
		{
			break;
		}
	}
	std::vector<Route*> paths;
	while (!stack.empty())
	{
		paths.push_back(stack.top());
		stack.pop();
	}

	cout << origin->city;
	for (int i = 0; i < size(paths); i++) {

		cout << "->" << ct[city_id[paths[i]->destination_city]].city;
	}
	cout << "\n最短路径(价格)值为" << cost << "\n";
	return paths;
}
std::vector<Route*> Dijkstra_time(City* origin, City* dest) {
	float dist[199] = {};
	for (size_t i = 0; i < 199; i++)
	{
		dist[i] = 90000000;
	}
	Route* _final[199] = {};
	int t = 1;
	City* set[199] = { origin };
	bool known[199] = {};
	known[origin->id] = 1;
	dist[origin->id] = 0;
	City* vertex = 0;
	Route* min = 0;
	do
	{
		vertex = 0;
		min = 0;
		for (int j = 0; j < t; j++)
		{
			City* city = set[j];
			for (int i = 0; i < city->arcs.size(); i++)
			{
				if (vertex == 0 && !known[city_id[city->arcs[i]->destination_city]])
				{
					vertex = &ct[city_id[city->arcs[i]->destination_city]];
					min = city->arcs[i];
				}
				if (city->arcs[i]->time + dist[city->id] < dist[city_id[city->arcs[i]->destination_city]])
				{
					dist[city_id[city->arcs[i]->destination_city]] = city->arcs[i]->time + dist[city->id];
					_final[city_id[city->arcs[i]->destination_city]] = city->arcs[i];
					if (!known[city_id[city->arcs[i]->destination_city]] && dist[city_id[min->destination_city]] > dist[city_id[city->arcs[i]->destination_city]])
					{
						vertex = &ct[city_id[city->arcs[i]->destination_city]];
						min = city->arcs[i];
					}
				}
			}
		}
		if (vertex != 0)
		{
			set[t] = vertex;
			known[vertex->id] = 1;
			t++;
		}
		else
		{
			break;
		}
	} while (true);

	City* path = dest;
	float time;
	time = dist[path->id];
	int x = 1;
	std::stack<Route*> stack;
	while (path != origin)
	{
		if (_final[path->id])
		{
			stack.push(_final[path->id]);
			path = &ct[city_id[_final[path->id]->origi_city]];
		}
		else
		{
			break;
		}
	}
	std::vector<Route*> paths;
	while (!stack.empty())
	{
		paths.push_back(stack.top());
		stack.pop();
	}

	cout << origin->city;
	for (int i = 0; i < size(paths); i++) {

		cout << "->" << ct[city_id[paths[i]->origi_city]].city;
	}
	cout << "\n最短路径(时间)值为" << time << "\n";
	return paths;
}

void BFS(City& origin) {
	std::queue<City*> queue;//创建队列
	queue.push(&origin);//起点入列
	bool visited[199] = {};//判断是否以遍历
	visited[origin.id] = 1;//遍历起点
	while (!queue.empty())//不断入队出队直到完全遍历
	{
		City* current = queue.front();
		queue.pop();
		visited[current->id] = 1;
		cout << current->city << "-> ";
		for (int i = 0; i < current->arcs.size(); i++)
		{
			Route* route = current->arcs[i];
			if (!visited[city_id[route->destination_city]])
			{
				queue.push(&ct[city_id[route->destination_city]]);
				visited[city_id[route->destination_city]] = 1;
			}
		}
	}
}//利用队列进行广度优先遍历


//可视化文件创建的参考代码：
//创建一个html文件,将百度API可视化所需要的代码和信息写入到这个html文件中
void viewable(std::vector<Route*> result) {
	FILE* fp3;
	fp3 = fopen("C:\\Users\\周宇翔\\Desktop\\数据结构实习的csv文件\\graph.htm", "w");
	if (fp3 == NULL)
	{
		cout << "error3!";
		exit(0);
	}
	//HTML文件头部说明
	std::string s = "<!DOCTYPE html><html><head>\
<style type='text/css'>body, html{width: 100%;height: 100%;margin:0;font-family:'微软雅黑';}#allmap{height:100%;width:100%;}#r-result{width:100%;}</style>\
<script type='text/javascript' src='http://api.map.baidu.com/api?v=2.0&ak=nSxiPohfziUaCuONe4ViUP2N'></script>";

	fprintf(fp3, "%s<title>Shortest path from %s to %s</title></head><body>\
<div id='allmap'></div></div></body></html><script type='text/javascript'>\
var map = new BMap.Map('allmap');\
var point = new BMap.Point(0, 0);\
map.centerAndZoom(point, 2);\
map.enableScrollWheelZoom(true);", s.c_str(), ct[city_id[result[0]->origi_city]].city, ct[city_id[result[size(result)-1]->destination_city]].city);



	 int j = 0;
	for(;j<size(result);)
	{
		//路径中一条弧弧头及其信息
		fprintf(fp3, "var marker%d = new BMap.Marker(new BMap.Point(%.4f, %.4f));\
map.addOverlay(marker%d);\n\
var infoWindow%d = new BMap.InfoWindow(\"<p style = 'font-size:14px;'>country: %s<br/>city: %s</p>\");\
marker%d.addEventListener(\"click\", function(){\
this.openInfoWindow(infoWindow%d);}); ", j, ct[city_id[result[j]->origi_city]].longitude, ct[city_id[result[j]->origi_city]].latitude, j, j, ct[city_id[result[j]->origi_city]].country, ct[city_id[result[j]->origi_city]].city, j, j);



		j++;
	
		//路径中一条弧弧尾及其信息
		fprintf(fp3, "var marker%d = new BMap.Marker(new BMap.Point(%.4f, %.4f));\
map.addOverlay(marker%d);\n\
var infoWindow%d = new BMap.InfoWindow(\"<p style = 'font-size:14px;'>country: %s<br/>city: %s</p>\");\
marker%d.addEventListener(\"click\", function(){\
this.openInfoWindow(infoWindow%d);}); ", j, ct[city_id[result[j-1]->destination_city]].longitude, ct[city_id[result[j - 1]->destination_city]].latitude, j, j, ct[city_id[result[j - 1]->destination_city]].country, ct[city_id[result[j - 1]->destination_city]].city, j, j);



		//路径中一条弧的信息
		fprintf(fp3, "var contentString0%d = '%s, %s --> %s, %s (%s - %.0f hours - $%.0f - %s)';\
var path%d = new BMap.Polyline([new BMap.Point(%.4f, %.4f),new BMap.Point(%.4f, %.4f)], {strokeColor:'#18a45b', strokeWeight:8, strokeOpacity:0.8});\
map.addOverlay(path%d);\
path%d.addEventListener(\"click\", function(){\
alert(contentString0%d);});", j, ct[city_id[result[j-1]->origi_city]].city, ct[city_id[result[j - 1]->origi_city]].country, ct[city_id[result[j - 1]->destination_city]].city, ct[city_id[result[j - 1]->destination_city]].country, result[j-1]->transport, result[j - 1]->time, result[j - 1]->cost, result[j - 1]->other_information,j, ct[city_id[result[j-1]->origi_city]].longitude, ct[city_id[result[j-1]->origi_city]].latitude, ct[city_id[result[j - 1]->destination_city]].longitude, ct[city_id[result[j - 1]->destination_city]].latitude, j, j, j);


	}
	fprintf(fp3, "</script>\n");//html文件结束

	fclose(fp3);
}

int main() {
	cout << "欢迎搜索航班信息！" << endl;
	g.vexnum = read_city(ct);
	Route* route = new Route[3000];
	g.arcnum = read_route(route);
	//Init(g, ct, route);//图的初始化
	//WAY way[199][199];
	//float huafei[199][199];
	cout << "打印广度优先遍历结果,以第一个城市为起点" << endl;
	BFS(ct[1]);//打印广度优先遍历结果,以第一个城市为起点
	//Dijkstra(g, way, huafei);//打印最短路径结果
	Arcs* arcs[210] = {};
	auto origin = ct[city_id["Beijing"]];
	auto dest = ct[city_id["London"]];
	std::vector<Route*> result1 = Dijkstra_cost(&origin, &dest);
	std::vector<Route*> result2 = Dijkstra_time(&origin, &dest);
	cout << "祝您旅途愉快！" << endl; cout << endl;
	viewable(result2);//可视化
	//system("pause");
	getchar();
	return 0;
}
