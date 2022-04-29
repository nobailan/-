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
const int INF = 0x3f3f3f3f;//�����
struct Arcs;
typedef struct {
	bool pass = false;
	string country, city;//���������׶�����
	double latitude, longitude;//��γ����Ϣ
	std::vector<Arcs*> arcs;
	int id;
} City, Vertex;
City ct[300];

typedef struct Arcs {
	string origi_city, destination_city, transport, other_information;//�������� Ŀ����� ���з�ʽ ������Ϣ
	double time, cost;//ʱ��ͻ���
} Route;


typedef struct {
	Vertex* ver;//������Ϣ����
	Arcs arcs[199][199];//·����Ϣ
	int vexnum, arcnum;//���еĸ��� ·�ߵĸ��� ��ʾ ͼ�Ķ������ͱ���
	string transport[200];//��ͨ��ʽ  ��ʾ  �ߵ�������Ϣ
	string other_information[200];//·����ַ
} Graph;//�����ڽӾ���
Graph g;
map<string, int>city_id;//�������id

typedef struct
{
	int pre; //ǰ׺
	bool vis; //���  
	double c;//��������
} WAY;  //Dijkstra��·����Ϣ



struct node {
	int x;
	double cost;
};//Dijkstra�㷨��Ϣ�ṹ��


int read_city(City* ct) {//��ȡ���У����س��и���
	FILE *file; int i = 1;
	fopen_s(&file, "C:\\Users\\������\\Desktop\\���ݽṹʵϰ��csv�ļ�\\cities.csv", "r");//��cities.csv�ļ�
	if (file) {
		char ch;//��ȡcsv�ļ�����
		while (!feof(file)) {
			ch = fgetc(file); ct[i].country = ""; ct[i].city = "";
			for (; ch != ','; ch = fgetc(file)) {//��ȡ����
				ct[i].country += ch;
			}
			ch = fgetc(file);
			for (; ch != ','; ch = fgetc(file)) {//��ȡ�׶�
				ct[i].city += ch;
			}
			fscanf_s(file, "%lf,", &ct[i].latitude);//��ȡ�׶���γ��
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
	return i - 1;//���س��и�������Ϊ����ͼ����
}

int read_route(Route* rt) {//��ȡ·�ߣ����ļ�����·����
	FILE *file; int i = 1; int j = 1;
	fopen_s(&file, "C:\\Users\\������\\Desktop\\���ݽṹʵϰ��csv�ļ�\\routes.csv", "r");//routes.csv�ļ�
	if (file) {
		char ch;
		while (!feof(file)) {
			//��ȡcsv�ļ�����
			ch = fgetc(file);
			for (; ch != ','; ch = fgetc(file)) {//��ȡ��ʼ����
				rt[i].origi_city += ch;
			}
			ch = fgetc(file);
			for (; ch != ','; ch = fgetc(file)) {//��ȡĿ�ĵ�
				rt[i].destination_city += ch;
			}
			ch = fgetc(file);
			for (; ch != ','; ch = fgetc(file)) {//��ȡ��ͨ��ʽ
				rt[i].transport += ch;
			}
			fscanf_s(file, "%lf,", &rt[i].time);//��ȡʱ��
			fscanf_s(file, "%lf,", &rt[i].cost);//��ȡ����
			ch = fgetc(file);
			for (; ch != '\n'; ch = fgetc(file)) {//��ȡ������Ϣ����ַ��
				rt[i].other_information += ch;
			}
			ct[city_id[rt[i].origi_city]].arcs.push_back(&rt[i]);
			i++;
			if (i == 1975) //��ȡ���˳�ѭ��,��֪�ļ�����1975������
				break;
		}
		fclose(file);
		cout << "��ȡ���ݳɹ�\n" << endl;
	}
	else {
		printf("ERROR2!");
		exit(0);
	}
	return i - 1;//����·��������Ϊ����ͼ�ı���
}

void Init(Graph& g, City* ct, Route* route) {//��ʼ��ͼ����Ϣ
	for (int i = 1; i <= g.vexnum; i++) {
		//city_id[g.ver[i].city] = i;//��ֵÿ�����еı��
	}
	//��ʼ��ͼ���ڽӾ���
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
		//�������ѡ�ʱ�䡢��ͨ��ʽ��������Ϣ��˫��ߣ�����ͼ��
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

string depart, dest, k;//�ֱ��ʾ�����С��յ���С����·��������׼

void Dijkstra(Graph& g, WAY way[][199], float ThatHuafei[][199]) {//���·���㷨

	cout << "����������:" << endl;
	cin >> depart;
	cout << "��������յ�:" << endl;
	cin >> dest;
	cout << "��������ʲôΪ��׼,����Ҫ���ַ�ʽ�������٣�time or cost����" << endl;
	cin >> k;
	float min = 0;
	double tm[199][199];               //��������Ȩֵ��Ϣ
	for (int i = 0; i < 199; i++)      //�ж����·����׼
	{
		for (int j = 0; j < 199; j++) {
			if (k == "cost") tm[i][j] = g.arcs[i][j].cost;
			else tm[i][j] = g.arcs[i][j].time;
		}
	}
	for (int v = 0; v < g.vexnum; v++) {
		g.ver[v].pass = false;       //δ��õ�v�����·��
		ThatHuafei[city_id[depart]][v] = tm[city_id[depart]][v];  //��ʼ�����·������ʼֵΪֱ��·��
		if (ThatHuafei[city_id[depart]][v] < 20000) {
			way[v][city_id[depart]].vis = true;
			way[v][v].vis = true;      //��ʼ��way����
			way[v][v].pre = city_id[depart];

		}
		else {
			way[v][v].pre = -1;
		}
	}
	ThatHuafei[city_id[depart]][city_id[depart]] = 0;
	g.ver[city_id[depart]].pass = true;
	for (int i = 0; i < g.vexnum; i++) {
		int v = 0;                            //vѡ��һ�����·��
		float min = 20000;                                //min�ǵ�ǰ��֪��depart��������룬��ʼΪ����
		for (int w = 0; w < g.vexnum; w++) {  //����������ж���
			if (g.ver[w].pass == false) {   //δ���������·��
				if (ThatHuafei[city_id[depart]][w] < min) {  //�ȵ�ǰ���·����
					v = w;
					min = ThatHuafei[city_id[depart]][w];//ȡ��ǰ·��Ϊ���·��
				}
			}
		}
		g.ver[v].pass = true;              //���·������v����·��

		for (int w = 0; w < g.vexnum; w++) {  //���µ�ǰ���·��������
			if (g.ver[w].pass == false && ((min + tm[v][w]) < ThatHuafei[city_id[depart]][w])) {
				//depart��w�����·����δ�ҵ�
				//�� depart��v �� v��w �����·�� С�ڵ�ǰ depart��w ���·��
				ThatHuafei[city_id[depart]][w] = min + tm[v][w];             //����depart��w�����·������
				 //depart��w�����·���Ķ������Ϊ depart��v �� v��w �Ķ���
				for (int k = 0; k < g.vexnum; k++) {
					way[w][k].vis = way[v][k].vis;
					way[w][k].pre = way[v][k].pre;//����w��k���ǰ�����ﵽ·�������Ч��
				}
				way[w][w].vis = true;
				way[w][w].pre = v;                        //����·��������way·�����������
				min = ThatHuafei[city_id[depart]][city_id[dest]];
			}
		}
	}
	//���������ʱ����߻���
	cout << "��" << k << "Ϊ��" << way[city_id[depart]][city_id[dest]].c << endl;

}

//void dijstra_cost(City* origin, Arcs*(&path)[210])//���·���㷨��costΪ��׼��������
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
//void dijstra_time(City* origin, Arcs*(&path)[210], City* dest)//���·���㷨��timeΪ��׼��������
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

std::vector<Route*> Dijkstra_cost(City* origin, City* dest) {//���·�������ѣ�
	float dist[199] = {};//���
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
	cout << "\n���·��(�۸�)ֵΪ" << cost << "\n";
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
	cout << "\n���·��(ʱ��)ֵΪ" << time << "\n";
	return paths;
}

void BFS(City& origin) {
	std::queue<City*> queue;//��������
	queue.push(&origin);//�������
	bool visited[199] = {};//�ж��Ƿ��Ա���
	visited[origin.id] = 1;//�������
	while (!queue.empty())//������ӳ���ֱ����ȫ����
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
}//���ö��н��й�����ȱ���


//���ӻ��ļ������Ĳο����룺
//����һ��html�ļ�,���ٶ�API���ӻ�����Ҫ�Ĵ������Ϣд�뵽���html�ļ���
void viewable(std::vector<Route*> result) {
	FILE* fp3;
	fp3 = fopen("C:\\Users\\������\\Desktop\\���ݽṹʵϰ��csv�ļ�\\graph.htm", "w");
	if (fp3 == NULL)
	{
		cout << "error3!";
		exit(0);
	}
	//HTML�ļ�ͷ��˵��
	std::string s = "<!DOCTYPE html><html><head>\
<style type='text/css'>body, html{width: 100%;height: 100%;margin:0;font-family:'΢���ź�';}#allmap{height:100%;width:100%;}#r-result{width:100%;}</style>\
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
		//·����һ������ͷ������Ϣ
		fprintf(fp3, "var marker%d = new BMap.Marker(new BMap.Point(%.4f, %.4f));\
map.addOverlay(marker%d);\n\
var infoWindow%d = new BMap.InfoWindow(\"<p style = 'font-size:14px;'>country: %s<br/>city: %s</p>\");\
marker%d.addEventListener(\"click\", function(){\
this.openInfoWindow(infoWindow%d);}); ", j, ct[city_id[result[j]->origi_city]].longitude, ct[city_id[result[j]->origi_city]].latitude, j, j, ct[city_id[result[j]->origi_city]].country, ct[city_id[result[j]->origi_city]].city, j, j);



		j++;
	
		//·����һ������β������Ϣ
		fprintf(fp3, "var marker%d = new BMap.Marker(new BMap.Point(%.4f, %.4f));\
map.addOverlay(marker%d);\n\
var infoWindow%d = new BMap.InfoWindow(\"<p style = 'font-size:14px;'>country: %s<br/>city: %s</p>\");\
marker%d.addEventListener(\"click\", function(){\
this.openInfoWindow(infoWindow%d);}); ", j, ct[city_id[result[j-1]->destination_city]].longitude, ct[city_id[result[j - 1]->destination_city]].latitude, j, j, ct[city_id[result[j - 1]->destination_city]].country, ct[city_id[result[j - 1]->destination_city]].city, j, j);



		//·����һ��������Ϣ
		fprintf(fp3, "var contentString0%d = '%s, %s --> %s, %s (%s - %.0f hours - $%.0f - %s)';\
var path%d = new BMap.Polyline([new BMap.Point(%.4f, %.4f),new BMap.Point(%.4f, %.4f)], {strokeColor:'#18a45b', strokeWeight:8, strokeOpacity:0.8});\
map.addOverlay(path%d);\
path%d.addEventListener(\"click\", function(){\
alert(contentString0%d);});", j, ct[city_id[result[j-1]->origi_city]].city, ct[city_id[result[j - 1]->origi_city]].country, ct[city_id[result[j - 1]->destination_city]].city, ct[city_id[result[j - 1]->destination_city]].country, result[j-1]->transport, result[j - 1]->time, result[j - 1]->cost, result[j - 1]->other_information,j, ct[city_id[result[j-1]->origi_city]].longitude, ct[city_id[result[j-1]->origi_city]].latitude, ct[city_id[result[j - 1]->destination_city]].longitude, ct[city_id[result[j - 1]->destination_city]].latitude, j, j, j);


	}
	fprintf(fp3, "</script>\n");//html�ļ�����

	fclose(fp3);
}

int main() {
	cout << "��ӭ����������Ϣ��" << endl;
	g.vexnum = read_city(ct);
	Route* route = new Route[3000];
	g.arcnum = read_route(route);
	//Init(g, ct, route);//ͼ�ĳ�ʼ��
	//WAY way[199][199];
	//float huafei[199][199];
	cout << "��ӡ������ȱ������,�Ե�һ������Ϊ���" << endl;
	BFS(ct[1]);//��ӡ������ȱ������,�Ե�һ������Ϊ���
	//Dijkstra(g, way, huafei);//��ӡ���·�����
	Arcs* arcs[210] = {};
	auto origin = ct[city_id["Beijing"]];
	auto dest = ct[city_id["London"]];
	std::vector<Route*> result1 = Dijkstra_cost(&origin, &dest);
	std::vector<Route*> result2 = Dijkstra_time(&origin, &dest);
	cout << "ף����;��죡" << endl; cout << endl;
	viewable(result2);//���ӻ�
	//system("pause");
	getchar();
	return 0;
}
