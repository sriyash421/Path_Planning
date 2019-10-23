#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <cmath>
#include <vector>
#include <stack>
#include <fstream>

using namespace std;
using namespace cv;

struct node{
	int x;int y;
};

struct node_info
{
	node parent;
	vector <node>children;
	int cost;
};

node_info** path;
node start_point;
node end_point;
vector <node> v;
stack <node> final_path;
int step_size =20;
int bot_size=20;

Mat img,img1;
Mat black(1,1,CV_8UC3,Scalar(0,0,0));
Mat white(1,1,CV_8UC3,Scalar(255,255,255));
Mat blue(1,1,CV_8UC3,Scalar(255,0,0));
Mat gray(1,1,CV_8UC3,Scalar(100,100,100));
Mat red(1,1,CV_8UC3,Scalar(0,0,255));
Mat green(1,1,CV_8UC3,Scalar(0,255,0));

node randomPoint();
float node_dist(node,node);
bool isValid(int,int);
int Nearest(node);
bool lineIterator1(node,node);
bool lineIterator2(node,node);
void RRT();
void reached();
void Rewire(node);
void remove(node,node);
void UpdateImg();

void begin(int event,int x,int y,int flags,void*)  										//Taking input of end and start
{
	if(event==EVENT_LBUTTONDOWN && img.at<Vec3b>(y,x)!=white.at<Vec3b>(0,0) )
	{
		start_point.x=x;
		start_point.y=y;
		path[start_point.y][start_point.x].cost=0;
		rectangle(img1,Point(start_point.x-2,start_point.y-2),Point(start_point.x+2,start_point.y+2),Scalar(0,255,0),CV_FILLED,8,0);
		imshow("win",img1);
	}
	if(event==EVENT_RBUTTONDOWN && img.at<Vec3b>(y,x)!=white.at<Vec3b>(0,0) )
	{
		end_point.x=x;
		end_point.y=y;
		rectangle(img1,Point(end_point.x-2,end_point.y-2),Point(end_point.x+2,end_point.y+2),Scalar(0,0,255),CV_FILLED,8,0);
		imshow("win",img1);
		RRT();
	}
}



int main(int argc,char ** argv)
{
	namedWindow("win",0);
	img=imread(argv[1],1);
	img1=imread(argv[1],1);
	imshow("win",img1);

	path=(node_info**)malloc(sizeof(node_info*)*img.rows);
	for(int i=0;i<img.rows;i++)
	{
		path[i]=(node_info*)malloc(sizeof(node_info)*img.cols);
		for(int j=0;j<img.cols;j++)
		{
			path[i][j].parent.x=-1;
			path[i][j].parent.y=-1;
			path[i][j].cost=100000;

		}
	}
	UpdateImg();//uncomment to apply bot dimension constraints
	setMouseCallback("win",begin,NULL);  
	waitKey(0);
}

void UpdateImg()
{
	int k;

	for(int i=0;i<img.cols;i++)
	{
		for(int j=0;j<img.rows;j++)
		{
			if(img.at<Vec3b>(j,i)[0]>150 && img.at<Vec3b>(j,i)[1]>150 && img.at<Vec3b>(j,i)[2]>150 )
					img.at<Vec3b>(j,i)=white.at<Vec3b>(0,0);
			else
					img.at<Vec3b>(j,i)=black.at<Vec3b>(0,0);	
		}
	}
	
	for(int i=0;i<img.cols;i++)
	{
		for(int j=1;j<img.rows-1;j++)
		{
			if(img.at<Vec3b>(j,i)==white.at<Vec3b>(0,0) && img.at<Vec3b>(j-1,i)==black.at<Vec3b>(0,0) )
			{
				for(k=1;k<=(int)(round(bot_size/2.0));k++)
					if(isValid(i,j-k))
						img.at<Vec3b>(j-k,i)=gray.at<Vec3b>(0,0);
			}

			if(img.at<Vec3b>(j,i)==white.at<Vec3b>(0,0) && img.at<Vec3b>(j+1,i)==black.at<Vec3b>(0,0) )
			{
				for(k=1;k<=(int)(round(bot_size/2.0));k++)
					if(isValid(i,j+k))
						img.at<Vec3b>(j+k,i)=gray.at<Vec3b>(0,0);
			}

		}
	}

	for(int i=0;i<img.cols;i++)
	{
		for(int j=0;j<img.rows;j++)
		{
			if(img.at<Vec3b>(j,i)==gray.at<Vec3b>(0,0))
					img.at<Vec3b>(j,i)=white.at<Vec3b>(0,0);
		}
	}

	for(int i=1;i<img.cols-1;i++)
	{
		for(int j=0;j<img.rows;j++)
		{
			if(img.at<Vec3b>(j,i)==white.at<Vec3b>(0,0) && img.at<Vec3b>(j,i-1)==black.at<Vec3b>(0,0) )
			{
				for(k=1;k<=(int)(round(bot_size/2.0));k++)
					if(isValid(i-k,j))
						img.at<Vec3b>(j,i-k)=gray.at<Vec3b>(0,0);
			}
			if(img.at<Vec3b>(j,i)==white.at<Vec3b>(0,0) && img.at<Vec3b>(j,i+1)==black.at<Vec3b>(0,0) )
			{
				for(k=1;k<=(int)(round(bot_size/2.0));k++)
					if(isValid(i+k,j))
						img.at<Vec3b>(j,i+k)=gray.at<Vec3b>(0,0);
			}

		}
	}
	for(int i=0;i<img.cols;i++)
	{
		for(int j=0;j<img.rows;j++)
		{
			if(img.at<Vec3b>(j,i)==gray.at<Vec3b>(0,0))
					img.at<Vec3b>(j,i)=white.at<Vec3b>(0,0);
		}
	}
}

node randomPoint()
{
	node temp;
	temp.x=rand()%(img.cols);
	temp.y=rand()%(img.rows);
	while(img.at<Vec3b>(temp.y,temp.x)!=black.at<Vec3b>(0,0))
	{	
		cout <<"generating random Point"<<endl;
		temp.x=rand()%(img.cols);
		temp.y=rand()%(img.rows);
	}
	return temp;
}

float node_dist(node a,node b)
{
	return sqrt(pow(a.x-b.x,2)+pow(a.y-b.y,2));
}

bool isValid(int x,int y)
{
	return x>=0 && y>=0 && x<img.cols && y<img.rows ;
}

int Nearest(node a)
{
	long long int i;
	long long int min_dist = node_dist(v[0],a);
	long long int min_node=0;

	for(i=1;i<v.size();i++)
	{
		float dist=node_dist(v[i],a);
		
		if( min_dist > dist)
		{
			min_dist = dist;
			min_node = i;
		}	
	}
	cout << "returning nearest_node  "<< min_node <<endl;

	return min_node;

}

node generate_step_node(node nnode,node rnode)
{
	cout <<"step_node generation"<<endl;
	node interm, step;
	float magn = 0.0, x = 0.0, y = 0.0;
	interm.x = rnode.x - nnode.x;
	interm.y = rnode.y - nnode.y;
	magn = sqrt((interm.x)*(interm.x) + (interm.y)*(interm.y));
	x = (float)(interm.x / magn);
	y = (float)(interm.y / magn);
	step.x = (int)(nnode.x + step_size*x);
	step.y = (int)(nnode.y + step_size*y);
	return step;
}


bool lineIterator1(node a,node b)
{
	node p,q;
	if(a.x > b.x)
	{
		p=a;
		q=b;
	}
	else if(a.x < b.x)
	{
		p=b;
		q=a;
	}
	else
		return false;

	float slope=((double)(p.y-q.y))/((double)(p.x-q.x));

	for(int i=q.x+1;i<p.x;i++)
	{
		int j=(int)(i*slope + q.y - slope*q.x);
			if(isValid(i,j))
				if(img.at<Vec3b>(j,i)==white.at<Vec3b>(0,0))
					return false;
	}
	return true;	
}

bool lineIterator2(node a,node b)
{
	node p,q;
	if(a.y > b.y)
	{
		p=a;
		q=b;
	}
	else if(a.y < b.y)
	{
		p=b;
		q=a;
	}
	else
		return false;

	float slope=((double)(p.y-q.y))/((double)(p.x-q.x));
	for(int i=q.y+1;i<p.y;i++)
	{
		int j=(int)((float)(i-q.y+slope*q.x)/slope);
			if(isValid(j,i))
				if(img.at<Vec3b>(i,j)==white.at<Vec3b>(0,0))
					return false;
	}
	return true;	
}

void RRT()
{	

	int flag=0;
	int count=1;
	
	v.push_back(start_point);
	cout<<"start_point added"<<endl;
	waitKey(500);
	
	node random_node,step_node,nearest_node;
	int nnode;


	while(flag==0)
	{
		random_node=randomPoint();
		nnode=Nearest(random_node);
		nearest_node=v[nnode];
		step_node=generate_step_node(nearest_node,random_node);
		

		if(!(isValid(random_node.x,random_node.y)) || (path[random_node.y][random_node.x].parent.x!=-1 && path[random_node.y][random_node.x].parent.y!=-1 ))
			continue;

		cout <<"random node no. "<<count<<endl;
		//	waitKey(0);

		if( isValid(step_node.x,step_node.y)  && lineIterator1(nearest_node,step_node) && lineIterator2(nearest_node,step_node))
		{
			v.push_back(step_node);
			cout <<"added random node no. "<<count<<endl;

			path[step_node.y][step_node.x].parent.x=nearest_node.x;
			path[step_node.y][step_node.x].parent.y=nearest_node.y;
			cout<<"parent added"<<endl;


			path[step_node.y][step_node.x].cost=path[nearest_node.y][nearest_node.x].cost+node_dist(step_node,nearest_node);
			path[nearest_node.y][nearest_node.x].children.push_back(step_node);


			line(img1, Point(nearest_node.x,nearest_node.y) , Point(step_node.x,step_node.y) , Scalar(0,255,255) , 1 , 8);
			cout <<"drawing line"<<endl;
			
			Rewire(step_node);//uncomment for rrt_star
		}

		
		if(lineIterator1(step_node,end_point) && lineIterator2(step_node,end_point) && node_dist(step_node,end_point) < step_size*4)
		{
			cout << "reached"<<endl;
			line(img1, Point(end_point.x,end_point.y) , Point(step_node.x,step_node.y) , Scalar(0,255,255) , 1 , 8);
			cout <<"drawing line"<<endl;

			path[end_point.y][end_point.x].parent.x=step_node.x;
			path[end_point.y][end_point.x].parent.y=step_node.y;
			cout<<"parent added"<<endl;


			path[end_point.y][end_point.x].cost=path[step_node.y][step_node.x].cost+node_dist(step_node,end_point);
			path[step_node.y][step_node.x].children.push_back(end_point);

			reached();
			break;
		}

		count++;
		//uncomment for animation
		if (v.size()%10==0)
		{
			waitKey(4);
		}

		imshow("win",img1);
	}

}


void reached()
{
	node a,b;
	a=end_point;
	int p=0;
	while(a.x!=start_point.x || a.y!=start_point.y)
	{
		final_path.push(a);

		b=path[a.y][a.x].parent;

		line(img1,Point(a.x,a.y),Point(b.x,b.y),Scalar(255,0,0),2,8);	

		a=b;
	}

	final_path.push(a);
	
	cout<<"printing line done"<<endl;
	imshow("win",img1);
	waitKey(0);
}


void remove(node a,node b)
{
	int i=0;
	for(i=0;i<path[a.y][a.x].children.size();i++)
		if( path[a.y][a.x].children[i].x==b.x && path[a.y][a.x].children[i].y==b.y )
			break;
	path[a.y][a.x].children.erase(path[a.y][a.x].children.begin()+i);			
}

void Rewire(node a)
{
	cout <<"Rewiring"<<endl;
	int r=step_size*10;

	int min_cost=path[a.y][a.x].cost;
	
	node best_parent=path[a.y][a.x].parent;
	

	for(int i=-r/2;i<=r/2;i++)	
	{
		for(int j=-r/2;j<=r/2;j++)
		{
			node temp;
			temp.y=a.y+j;
			temp.x=a.x+i;
			
			if( isValid(temp.x,temp.y) && path[temp.y][temp.x].parent.x != -1 && path[temp.y][temp.x].parent.y != -1)
			{
				if(lineIterator1(temp,a) && lineIterator2(temp,a))
				{
					int temp_cost=path[temp.y][temp.x].cost+node_dist(temp,a);
					
					cout << "Cost: " <<temp_cost<<" Min Cost: "<<min_cost<<endl;
					if(temp_cost<min_cost)
					{
						min_cost=temp_cost;
						best_parent=temp;
					}
				}
			}
		}			
	}
	if(path[a.y][a.x].cost!=min_cost)
	{
		line(img1, Point(a.x,a.y) , Point(path[a.y][a.x].parent.x,path[a.y][a.x].parent.y) , Scalar(0,0,0) , 1 , 8);
		remove(path[a.y][a.x].parent,a);
		
		cout <<"Changing parent node:"<<endl;
		path[a.y][a.x].cost=min_cost;
		path[a.y][a.x].parent=best_parent;
		
		path[best_parent.y][best_parent.x].children.push_back(a);

		line(img1, Point(a.x,a.y) , Point(best_parent.x,best_parent.y) , Scalar(0,255,255) , 1 , 8);
	}

	for(int i=-r/2;i<=r/2;i++)	
	{
		for(int j=-r/2;j<=r/2;j++)
		{
			node temp;
			temp.y=a.y+j;
			temp.x=a.x+i;
			
			if( isValid(temp.x,temp.y) && path[temp.y][temp.x].parent.x != -1 && path[temp.y][temp.x].parent.y != -1 && path[temp.y][temp.x].children.size()==0)
			{
				if(lineIterator1(temp,a) && lineIterator2(temp,a))
				{
					int temp_cost=path[a.y][a.x].cost + node_dist(a,temp);
					if(temp_cost < path[temp.y][temp.x].cost)
					{
						line(img1, Point(temp.x,temp.y) , Point(path[temp.y][temp.x].parent.x,path[temp.y][temp.x].parent.y) , Scalar(0,0,0) , 1 , 8);
						remove(path[temp.y][temp.x].parent,temp);
						
						path[temp.y][temp.x].cost=temp_cost;
						path[temp.y][temp.x].parent=a;

						path[a.y][a.x].children.push_back(temp);
						line(img1, Point(a.x,a.y) , Point(temp.x,temp.y) , Scalar(0,255,255) , 1 , 8);	
					}
				}
			}
		}			
	}

	waitKey(1);
}
