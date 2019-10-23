#include <bits/stdc++.h>
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <cmath>
#include <queue>
#include <vector>

using namespace std;
using namespace cv;



const int block_width=10;
const int block_height=10;

Mat img,img_path;
Mat black(1,1,CV_8UC3,Scalar(0,0,0));
Mat white(1,1,CV_8UC3,Scalar(255,255,255));
Mat blue(1,1,CV_8UC3,Scalar(255,0,0));
Mat gray(1,1,CV_8UC3,Scalar(220,220,220));
Mat red(1,1,CV_8UC3,Scalar(0,0,255));
Mat green(1,1,CV_8UC3,Scalar(0,255,0));


struct node{
	int x;int y;
	float c;
	float cc;
	//node* parent;
};

struct Comp{

    bool operator()(const node& a, const node& b)
    {
    	//if(a.c==b.c)
    	//	return a.cc > b.cc;
        return a.c > b.c;
    }
};



node start_point;		//start_point point
node end_point;		//end point
int** path_direction;
priority_queue < node , vector < node > , Comp > path;




bool isValid(int x,int y)
{
	if(img.at<Vec3b>(y,x)==gray.at<Vec3b>(0,0))
		return false;
	for(int i=-block_width/2;i<block_width/2;i++)
	{
		for(int j=-block_height/2;j<block_height/2;j++)
		{
			if((x+i <0) || (x+i>img.cols) || (y+j>img.rows) || (y+j<0) || img.at<Vec3b>(y+j,x+i)==black.at<Vec3b>(0,0))
				return false;
		}
	}

	return true;
}



void print_path(int x,int y) 	
{
	//namedWindow("path",0);
	cout << "PRINTING 1"<<endl;
	int x1=x;//end_point.x;
	int y1=y;//end_point.y;
	cout << "PRINTING 2"<<endl;

	
	//img.at<Vec3b>(y1,x1)=red.at<Vec3b>(0,0);
	cout << "PRINTING 3"<<endl;
	cout << path_direction[y1][x1] << endl; 
	while((x1!=start_point.x || y1!=start_point.y))
	{
		cout << "PRINTING 5"<<endl;


		int direction=path_direction[y1][x1];
		
		cout << x1 <<" " << y1 << " " <<start_point.x<< " "<<start_point.y<<" "<< direction <<endl;

		for(int i=-block_width/2;i<block_width/2;i++)
		{
			for(int j=-block_height/2;j<block_height/2;j++)
			{
				img.at<Vec3b>(y1+j,x1+i)=blue.at<Vec3b>(0,0);
			}
		}

		//rectangle(img_path,Point(x1-block_width/2,y1-block_height/2),Point(x1+block_width/2,y1+block_height/2),CV_FILLED,8,0);
		img.at<Vec3b>(y1,x1)=green.at<Vec3b>(0,0);


		if(direction==1)
			y1=y1+1;
		else if(direction==2)
			x1=x1+1;
		else if(direction==3)
			y1=y1-1;
		else if(direction==4)
			x1=x1-1;
		else if (direction==5)
		{
			x1=x1+1;
			y1=y1+1;
		}
		else if (direction==6)
		{
			x1=x1-1;
			y1=y1+1;
		}
		else if (direction==7)
		{
			x1=x1-1;
			y1=y1-1;
		}
		else if (direction==8)
		{
			x1=x1+1;
			y1=y1-1;
		}
		
		//imshow("win",img);
		//imshow("path",img_path);
	}
	//img.at<Vec3b>(end_point.y,end_point.x)=red.at<Vec3b>(0,0);
	//img.at<Vec3b>(y1,x1)=green.at<Vec3b>(0,0);
	x1=x;
	y1=y;

	while((x1!=start_point.x || y1!=start_point.y))
	{
		cout << "PRINTING 5"<<endl;


		int direction=path_direction[y1][x1];
		
		cout << x1 <<" " << y1 << " " <<start_point.x<< " "<<start_point.y<<" "<< direction <<endl;

		// for(int i=-block_width/2;i<block_width/2;i++)
		// {
		// 	for(int j=-block_height/2;j<block_height/2;j++)
		// 	{
		// 		img.at<Vec3b>(y1+j,x1+i)=blue.at<Vec3b>(0,0);
		// 	}
		// }

		//rectangle(img_path,Point(x1-block_width/2,y1-block_height/2),Point(x1+block_width/2,y1+block_height/2),CV_FILLED,8,0);
		img.at<Vec3b>(y1,x1)=green.at<Vec3b>(0,0);


		if(direction==1)
			y1=y1+1;
		else if(direction==2)
			x1=x1+1;
		else if(direction==3)
			y1=y1-1;
		else if(direction==4)
			x1=x1-1;
		else if (direction==5)
		{
			x1=x1+1;
			y1=y1+1;
		}
		else if (direction==6)
		{
			x1=x1-1;
			y1=y1+1;
		}
		else if (direction==7)
		{
			x1=x1-1;
			y1=y1-1;
		}
		else if (direction==8)
		{
			x1=x1+1;
			y1=y1-1;
		}
		
		//imshow("win",img);
		//imshow("path",img_path);
	}
	cout << "START " <<start_point.x<<" "<<start_point.y<<" END "<<end_point.x<<" "<<end_point.y<<endl;
	imshow("win",img);
	waitKey(0);
}

float h_distance(int x,int y)
{
	return (sqrt(pow(y-end_point.y,2)+pow(x-end_point.x,2)));
}




void astar()
{
	path.push(start_point);
	long long int cost=1,x,y;
	node temp;
	node parent_temp;
	int count=0;

	while(!path.empty())
	{
		count++;
		imshow("win",img);
		parent_temp=path.top();
		x=parent_temp.x;
		y=parent_temp.y;
		path.pop();

		if(path_direction[y][x]<5)
			cost++;
		else
			cost=cost+1.5;

		cout << x <<" " << y << " " <<end_point.x<< " "<<end_point.y<<"\n";
		if(x == end_point.x && y == end_point.y)
		{
			cout << "PRINTING"<<endl;
			cout << path_direction[y][x] << endl; 
			print_path(x,y);
			break;
		}
		else
		{
			if(isValid(x,y-1))// && img.at<Vec3b>(y-1,x)==white.at<Vec3b>(0,0) )//&& img1.at<Vec3b>(y-1,x)==255)
			{
				img.at<Vec3b>(y-1,x)=gray.at<Vec3b>(0,0);
				temp.x=x;

				temp.y=y-1;
				//temp.parent=&parent_temp;
				path_direction[temp.y][temp.x]=1;
				temp.cc=cost;
				temp.c=cost+h_distance(temp.x,temp.y);
				path.push(temp);

				//CHECK();
			}


			if(isValid(x-1,y) )//&& img.at<Vec3b>(y,x-1)==white.at<Vec3b>(0,0) )//&& img1.at<Vec3b>(y,x-1)==255)
			{
				img.at<Vec3b>(y,x-1)=gray.at<Vec3b>(0,0);
				temp.x=x-1;
				temp.y=y;
				//temp.parent=&parent_temp;
				temp.c=cost+h_distance(temp.x,temp.y);
				path_direction[temp.y][temp.x]=2;
				temp.cc=cost;
				path.push(temp);

				//CHECK();
			}
			

			if(isValid(x,y+1) )//&& img.at<Vec3b>(y+1,x)==white.at<Vec3b>(0,0) )//&& img1.at<Vec3b>(y+1,x)==255)
			{
				img.at<Vec3b>(y+1,x)=gray.at<Vec3b>(0,0);
				temp.x=x;
				temp.y=y+1;
				//temp.parent=&parent_temp;
				path_direction[temp.y][temp.x]=3;
				temp.c=cost+h_distance(temp.x,temp.y);
				temp.cc=cost;
				path.push(temp);
				//CHECK();
			}		
			

			if(isValid(x+1,y+1))// && img.at<Vec3b>(y+1,x+1)==white.at<Vec3b>(0,0) )//&& img1.at<Vec3b>(y,x+1)==255)
			{
				img.at<Vec3b>(y+1,x+1)=gray.at<Vec3b>(0,0);
				temp.x=x+1;
				temp.y=y+1;
				//temp.parent=&parent_temp;
				path_direction[temp.y][temp.x]=7;
				temp.c=cost+h_distance(temp.x,temp.y);
				temp.cc=cost;
				path.push(temp);
				//CHECK();
			}

			if(isValid(x+1,y-1) )//&& img.at<Vec3b>(y-1,x+1)==white.at<Vec3b>(0,0) )//&& img1.at<Vec3b>(y,x+1)==255)
			{
				img.at<Vec3b>(y-1,x+1)=gray.at<Vec3b>(0,0);
				temp.x=x+1;
				temp.y=y-1;
				//temp.parent=&parent_temp;
				path_direction[temp.y][temp.x]=6;
				temp.c=cost+h_distance(temp.x,temp.y);
				temp.cc=cost;
				path.push(temp);
				//CHECK();
			}
			if(isValid(x-1,y+1))// && img.at<Vec3b>(y+1,x-1)==white.at<Vec3b>(0,0) )//&& img1.at<Vec3b>(y,x+1)==255)
			{
				img.at<Vec3b>(y+1,x-1)=gray.at<Vec3b>(0,0);
				temp.x=x-1;
				temp.y=y+1;
				//temp.parent=&parent_temp;
				path_direction[temp.y][temp.x]=8;
				temp.c=cost+h_distance(temp.x,temp.y);
				temp.cc=cost;
				path.push(temp);
				//CHECK();
			}
			if(isValid(x-1,y-1))//&& img.at<Vec3b>(y-1,x-1)==white.at<Vec3b>(0,0) )//&& img1.at<Vec3b>(y,x+1)==255)
			{
				img.at<Vec3b>(y-1,x-1)=gray.at<Vec3b>(0,0);
				temp.x=x-1;
				temp.y=y-1;
				//temp.parent=&parent_temp;
				path_direction[temp.y][temp.x]=5;
				temp.c=cost+h_distance(temp.x,temp.y);
				temp.cc=cost;
				path.push(temp);
				//CHECK();
			}
			if(isValid(x+1,y))// && img.at<Vec3b>(y,x+1)==white.at<Vec3b>(0,0) )//&& img1.at<Vec3b>(y,x+1)==255)
			{
				img.at<Vec3b>(y,x+1)=gray.at<Vec3b>(0,0);
				temp.x=x+1;
				temp.y=y;
				//temp.parent=&parent_temp;
				path_direction[temp.y][temp.x]=4;
				temp.c=cost+h_distance(temp.x,temp.y);
				temp.cc=cost;
				path.push(temp);
				//CHECK();
			}
		}

		imshow("win",img);
		if(count%3000==0)
			waitKey(1);

		
		cout << cost <<"\n";
		


	}

	if(!(parent_temp.x==end_point.x && parent_temp.y==end_point.y))
		cout <<"No path";
}



void begin(int event,int x,int y,int flags,void*)  										//Taking input of end and start_point
{
	if(event==EVENT_LBUTTONDOWN && img.at<Vec3b>(y,x)!=black.at<Vec3b>(0,0) )
	{
		start_point.x=x;
		start_point.y=y;
		start_point.c=0;
		path_direction[start_point.y][start_point.x]=0;
	}
	if(event==EVENT_RBUTTONDOWN && img.at<Vec3b>(y,x)!=black.at<Vec3b>(0,0) )
	{
		end_point.x=x;
		end_point.y=y;
		astar();
	}
}


int main(int argc,char ** argv)
{
	namedWindow("win",0);
	img=imread(argv[1],1);
	img_path=img.clone();
	imshow("win",img);
	path_direction=(int **)malloc(sizeof(int*)*img.rows);
	for(int i=0;i<img.rows;i++)
	{
		path_direction[i]=(int *)malloc(sizeof(int)*img.cols);
	}
	Mat temp(img.rows,img.cols,CV_8UC1,Scalar(255));
	setMouseCallback("win",begin,NULL);   
	waitKey(0);

	/*for(int i=0;i<img.rows;i++)
	{
		for(int j=0;j<img.cols;j++)
		{
			if(img.at<Vec3b>(j,i)[0]==0 && img.at<Vec3b>(j,i)[1]==255 && img.at<Vec3b>(j,i)[2]==0)
			{
				start_point.x=j;
				start_point.y=i;
			}

			if(img.at<Vec3b>(j,i)[0]==0 && img.at<Vec3b>(j,i)[1]==0 && img.at<Vec3b>(j,i)[2]==255)
			{
				end_point.x=j;
				end_point.y=i;
			}
		}
	}*/
	// start_point.x=5;
	// start_point.y=5;
	// end_point.x=385;
	// end_point.y=379;
	// astar();
}