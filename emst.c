#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAX 10000000
#define C1 1.5
#define C2 1.1
/*--------------------------------------------------------------------------*/
/****Defining the required data structures****/

/****Structure to represent a point****/
typedef struct p
{
	double x,y;
}point;

/****This represents a node in the hashtable****/
/**I will store all the points in an array. 'id' represents its index in that points array**/
typedef struct n
{
	double x,y;//Coordinates
	long id;//Point id
	struct n *next;
}node;
typedef node ***hashtable;

/****Data structure for adjacency list representation****/
typedef struct adj
{
	long index;
	struct adj *next;
}adjlistnode;
typedef adjlistnode *adjlist; 

/****Representation of an edge for Kruskal****/
typedef long vertex;
typedef struct _edge
{
	vertex u,v;//2 vertices
	double cost;//Cost of the edge
}edge;

/****Representation of a treenode****/
typedef struct _tn
{
	long size;
	struct _tn *parent;//Only parent pointers are required.
}treenode;
typedef treenode *treenodeptr;
typedef treenode *set;//A Tree is identified by it's root which is reprsented by 'set'.

treenodeptr header[MAX];//Header pointers for all the points. 

/*---------------------------------------------------------*/
/****Standard functions****/

/****Standard heapify for edges****/
void heapify(edge *E,long n,long i)
{
	while(1)
	{
		long m;
		edge t;
		if(2*i+1>=n)return;
		if(2*i+2>=n)m=2*i+1;
		else m=(E[2*i+2].cost>E[2*i+1].cost)?2*i+2:2*i+1;
		if(E[m].cost<=E[i].cost)return;
		t=E[m];E[m]=E[i];E[i]=t;
		i=m;
	}	
}
/****Standard deletemin function****/
void deletemin(edge *E,long n)
{
	E[0]=E[n-1];
	heapify(E,n-1,0);
}
/****Standard makeheap function to make heap of edges on E****/
void makeheap(edge *E,long n)
{
	long i;	
	for(i=n/2-1;i>=0;--i)
	heapify(E,n,i);	
}
/****Standard initset function****/
void initset(vertex u)
{
	treenodeptr p;
	p=(treenodeptr)malloc(sizeof(treenode));
	p->size=1;
	p->parent=p;
	header[u]=p;//header for locating u in the forest.
}
/****Standard Findset function with path compression****/
treenodeptr findset(vertex u)
{
	treenodeptr r,p,q;
	//Finding root r of the given tree
	r=header[u];
	while(r!=r->parent)r=r->parent;
	
	//Path compression begins.
	p=header[u];
	while(p->parent!=r)

	{
		q=p->parent;
		p->parent=r;
		p=q;
	}
	return r;//Set of u.
}

/****Standard mergesets function****/
void mergesets(treenodeptr r1,treenodeptr r2)
{
	treenodeptr r;
	if(r1->size<=r2->size)r=r1->parent=r2;
	else r=r2->parent=r1;
	r->size=r1->size+r2->size;
	r1->size=r2->size=r->size;
}
/*----------------------------------------------------------------------*/ 

/****Returns the distance(cost) between 2 points (x1,y1)and (x2,y2)****/
double distance(double x1,double y1,double x2,double y2)
{
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

/****Returns the number of edges in the adjacency list L****/
long countedges(adjlist L,long n)
{
	long i,count=0;
	adjlistnode *ptr;
	for(i=0;i<n;++i)
	{
		ptr=L[i].next;//Since First node in any list is the dummy node.
		while(ptr!=NULL)
		{
			count++;			
			ptr=ptr->next;
		}					
	}
	return count;
}

/****Generates a graph having 'n' nodes****/
/*It puts the points in the corresponding cells in the hash table*/
/*Simultaneously it also it also puts the points in the array of points(arr)*/
hashtable input(long n,point *arr)
{
	long i,j,m;
	long s,t;//Represents the cell in the hashtable to which a point belongs.
	hashtable hash;
	node *newnode;
	m=ceil(sqrt(n));
	/*Creating Hash table*/	
	hash=(hashtable)malloc(m*sizeof(node **));
	for(i=0;i<m;++i)
	hash[i]=(node **)malloc(m*sizeof(node));

/*Initialising the 2-D array of pointers*/
	for(i=0;i<m;++i)
	for(j=0;j<m;++j)
	hash[i][j]=NULL;	 
	
	for(i=0;i<n;++i)
	{
    		/*Creating a new node*/
	       	newnode=(node *)malloc(sizeof(node));    
	     	arr[i].x=newnode->x=(double)rand()/RAND_MAX;
	   	arr[i].y=newnode->y=(double)rand()/RAND_MAX;
	       	newnode->id=i;//Now this node in the hashtable corresponds to the i-th cell in the points array(arr).
   		
		/*Finding correct cell in the hashtable*/
		s=(long)(newnode->x*m);
		t=(long)(newnode->y*m);
		
		/*Adding the newnode to the hashtable*/		
		newnode->next=hash[s][t];
		hash[s][t]=newnode;	 	
	}	
	return hash;	
}

/****This function appends more neighbours in the adjacency list L****/
/*Those points in the array arr whose x-coordinate=-1 belong to the giant component.So There adjacency list is not appended*/
/*'hash' helps us in finding the points in the neigbourhood of any point*/
/*d1,d2 are the 2 distances between which we need to look for more neighbours for any point*/
void makeadjlist(adjlist L,hashtable hash,point *arr,long n,double d1,double d2)
{
	long i,j,k,m,s,t;
	long left,right,up,down;//Boudaries in which we will find neighbours of a point.
	double dist;
	node *ptr;
	adjlistnode *newnode,*listptr;
	point p;
	m=ceil(sqrt(n));
		
	for(i=0;i<n;++i)//List for each node
	{
		p=arr[i];//The point corresponding to i in the unit square.
		
		if(p.x==-1)//This point is in the giant component.So we don't need to append in it's adjacency list.
		continue;			
		/*Correct cell in the hashtable*/		
		s=(long)(m*(p.x));
		t=(long)(m*(p.y));

		/*Finding the boundary in which we need to look for*/
		left=floor((p.x-d2)*m);right=ceil((p.x+d2)*m);
		up=floor((p.y-d2)*m);down=ceil((p.y+d2)*m);
		
		for(j=left;j<=right;++j)
		{
			for(k=up;k<=down;++k)
			{
				/*The cell should be present in the unit square.
				 So checking if the cell(j,k) does not go out of bounds*/				
				if(j>=0&&j<m&&k>=0&&k<m)//A legal cell
				{
				/*Now we'll compare with all the points in the cell(j,k)*/  					
					ptr=hash[j][k];
					while(ptr!=NULL)
					{
						dist=distance(ptr->x,ptr->y,p.x,p.y);//Cost of edge.
		/*Checking if it is not the same point and also checking if cost is between[d1,d2)*/
				/*ptr->id represents the id of this point in the array arr*/ 	
						if(ptr->id!=i&&dist>=d1&&dist<d2)
						{
		/*Now we will check that one edge(u,v) is put only once in the adjacency list.
		First time when this function is called,none of the points in arr has x-coordinate=-1.
		So then I add an edge in L only when the x-coordinate of one point is less than or equal to the other point.
		When this function is called in future,many points belong to the giant component and there adjacency list is not appended at 			all.So if any point outside giant component has an edge with a point in giant component and cost is between[d1,d2),
		then that edge always has to be	added*/  						
							if((arr[ptr->id].x!=-1&&ptr->x<p.x)||arr[ptr->id].x==-1)
							{	
								/*Making a new node*/
								newnode=(adjlistnode *)malloc(sizeof(adjlistnode));
								newnode->index=ptr->id;
								newnode->next=NULL;
								
							/*Putting the node at the beginning of list just after the dummy node*/
								listptr=L[i].next;
								L[i].next=newnode;
								newnode->next=listptr;		
							}
						}
						ptr=ptr->next;//Next point in cell(j,k)						
					}//end of while
				}				
			}
		}		
	}
}

/****Generates a forest on n nodes****/
/*It makes a heap of edges E*/
/*It returns the number of trees currently in L*/
/*It also changes maxsize suitably so that it contains the number of vertices in the largest tree*/
long kruskal(adjlist L,long edges,point *arr,long n,long *maxsize)
{
	edge *E=(edge *)malloc((edges)*sizeof(edge));//Edge array.
	long i,e,ntrees,index;
	vertex u,v;//The 2 vertices of an edge.
	set U,V;//The sets(trees) to which u,v belong. 
	adjlistnode *ptr;
		
	/*Putting all the edges in E*/
	e=0;
	for(i=0;i<n;++i)
	{
		/*Adding all the neighbours of point with index 'i' using adjacency list L.*/
		ptr=L[i].next;
		while(ptr!=NULL)
		{
			index=ptr->index;
			E[e].u=i;E[e].v=index;E[e].cost=distance(arr[i].x,arr[i].y,arr[index].x,arr[index].y);
			++e;
			ptr=ptr->next;
		}
	}
	/*Making a heap on E.*/
	makeheap(E,e);
	/*Initialising all the trees*/
	for(i=0;i<n;++i)
	initset(i);
	ntrees=n;
	
	while(ntrees>1&&e>0)
	{
		u=E[0].u;v=E[0].v;
		U=findset(u);V=findset(v);
		if(U!=V)
		{
			/*u,v belong to different trees.So an edge can be added*/
			mergesets(U,V);
			if(U->size>(*maxsize))//Updating the size of the largest tree.
			(*maxsize)=U->size;
			--ntrees;			
		}
		deletemin(E,e--);	
	}
	free(E);
	return ntrees;//Returning the number of trees
}

/**** This is the implementation of Rajasekaran's algorithm****/
/*It generates an EMST of the given points*/
void rajasekaran(hashtable hash,point *arr,long n)
{
	long i,m,edges,max,ntrees;
	double d1,d2;
	adjlist L;
	m=ceil(sqrt(n));
	L=(adjlist)malloc(n*sizeof(adjlistnode));
	
	//Initialising the adjacency list.
	for(i=0;i<n;++i)
	{
		L[i].index=-1;
		L[i].next=NULL;
	}	
		
	/*First phase of EMST*/
	printf("+++ First phase of EMST...\n");
	max=0;//Size of largest tree.
	d1=C1/m;//Distance within which edges have to be added.
	
	makeadjlist(L,hash,arr,n,0,d1);//Making the adjacency list L.

	edges=countedges(L,n);//Counting number of edges in L.
	printf("No. of edges in G is %ld\n",edges);

	ntrees=kruskal(L,edges,arr,n,&max);//Generating a forest T.	
    	printf("No. of edges in T is %ld\n",n-ntrees);
    	printf("Number of trees = %ld\n\n",ntrees);

   	if(ntrees==1)return;
	
	/*Second phase of EMST*/
	d2=C2*log(n)/m;//Increased distance in which edges have to be added.
	int flag=0;//Simply checks which phase of EMST is going on.
	while(ntrees!=1)
	{	
		if(flag==0)		
		{
		printf("+++ Second phase of EMST...\n");
		flag=1;
		}
		else//This case does not arrive at all.But still I have checked this. 
		printf("+++ Another phase of EMST...\n");

		printf("Largest tree is of size =  %ld\n",max-1);
		
		/*Those vertices which are present in the giant component,I'm making there x=coordinates as -1.
		  This is done so that the adjacency list of these verices is not appended in future*/
		for(i=0;i<n;++i)
		{
			if(findset(i)->size==max)//This vertex belongs to the giant component.
			arr[i].x=-1;
		}
	
		makeadjlist(L,hash,arr,n,d1,d2);//Appending to L.

		edges=countedges(L,n);//Counting number of edges in L.
		printf("No. of edges in G is %ld\n",edges);

		ntrees=kruskal(L,edges,arr,n,&max);//Generating the EMST.		
    		printf("No. of edges in T is %ld\n",n-ntrees);
    		printf("Number of trees = %ld\n\n",ntrees);
		
		//Preparing distnces for next pass though it does occur at all.
		d1=d2;		
		d2=2*d2;
   	}
	free(L);
}

int main()
{
	srand((unsigned int)time(NULL));
	long n;
	hashtable hash;//Hashtable.
	point *arr;//Array of points.
	printf("Enter n:");
	scanf("%ld",&n);	
	/*Generating the graph*/
	arr=(point *)malloc(n*sizeof(point));//Array to store points 
	hash=input(n,arr);
	
	//Calling the function to generate EMST.
	rajasekaran(hash,arr,n);

	free(hash);
	free(arr);
}

