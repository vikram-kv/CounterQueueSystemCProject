//Necessary header files are included
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Few Global Variables:

int NO_COUNTERS;//First line of input-No of counters maintained
int BUFF_SIZE;//Buffer size of each counter
int firsttime=1;//A constant which is used for executing a few statements only during the first run in Scheduler() function.Boolean variable
long int total_requests;//Total number of requests at any point of time which haven't finished execution.time is a static variable in scheduler.
long int queue_size;//queue_size=size of queue at any point of time/


//Definition of Data Types:


//Request has been defined as a type which contains
//RequestID=Program allocated string in the form R(number) where  number is its position in the queue initially.Eg: R1,R20,R1000 etc.
//CounterID=Program allocated ID to all counters=Counter_Name in struct Counter
struct Request
{
	char RequestID[20];
	char CounterID[20];
	long int entrytime;//The time at which it enters a counter's buffer
	long int exittime;//The time at which it leaves a counter's buffer.
	int executiontime;//Amount of execution time needed.
	long int Queuetime;//The amount of time spent in queue.
};


//RequestHolder is datatype which HOLDS-A request by means of a Request type pointer called request
//Since, a linked list of such  objects is needed , it also contains a pointer to its own type
struct RequestHolder
{
	struct Request* request;
	struct RequestHolder* next;
};


//Counter is a type which has a program defined Counter_Name which is like "Counter-(position)" where position is the position of that
//particular counter in the list of counters.
//It also HOLDS-A request by means of Counter_Request which points to the request under execution at the counter
//It also HOLDS-A bufferhead which points to the first node in its buffer.
//It also has an integer called number_at_buffer which is the number of requests in its buffer at any time.
//To create a linked list,it contains a pointer to the next counter called next.
struct Counter
{
    char Counter_Name[20];
	struct Request* Counter_Request;
	struct RequestHolder* bufferhead;
	int number_at_buffer;
	struct Counter* next;
};


//Function Prototypes:

void Initialize_Request(struct Request*);
struct Counter* CreateCounters_Buffers(int,int);
void Free_List_of_Completed_Requests(struct RequestHolder*);
void Delete_Request_And_Location(struct RequestHolder*);
struct RequestHolder* Initialize_All_Counters_And_Buffers(FILE*,struct Counter*,struct RequestHolder*);
void Print_Allocation(FILE*,struct RequestHolder*);
void Print_LogFile(FILE*,long int,long int,struct RequestHolder*,struct Counter*);
int Scheduler(struct Counter*,struct RequestHolder**,FILE*,FILE*);
struct RequestHolder* Push_Into_Buffer(struct Counter*,struct RequestHolder*,long int);
long int Size_Of_Queue(struct RequestHolder*);
struct RequestHolder* RFile(const char*);


int main()
{
    struct RequestHolder* qhead=RFile("service_request.txt");//A queue of requests with its head as qhead is created by reading service_request.txt
    FILE* afp=fopen("allocation.txt","w");//allocation.txt is opened in write mode
    FILE* lfp=fopen("qms-log.txt","w");//qms-log.txt is opened in write mode
    struct Counter *Counterhead=CreateCounters_Buffers(NO_COUNTERS,BUFF_SIZE);//The NO_COUNTERS counters with their buffers of size BUFF_SIZE are created dynamically and linked into a queue of counters.The head of this queue is pointed to by Counterhead
    total_requests=Size_Of_Queue(qhead);//total_requests pending at time = 0 is sizeof queue whose head is qhead
    queue_size=total_requests;//At time=0,queue_size is total_requests pending
    qhead=Initialize_All_Counters_And_Buffers(lfp,Counterhead,qhead);//All counters and buffers are filled with requests.qhead has now changed.
    while(Scheduler(Counterhead,&qhead,afp,lfp)==1);//while there are requests pending, Scheduler is called iteratively
    fclose(afp);//allocation.txt is closed
    fclose(lfp);//qms-log.txt is closed
	return 0;
}

//Initialize_Request initially the memory for a dynamically allocated request whose address is requestptr.
void Initialize_Request(struct Request* Requestptr)
{
	strcpy(Requestptr->RequestID,"NA");//Not Available
	strcpy(Requestptr->CounterID,"NA");//Not Available
	Requestptr->entrytime=-1;//Illegal  value for ease of later bug testing.
	Requestptr->exittime=-1;//Illegal value
	Requestptr->executiontime=-1;//Illegal value
	Requestptr->Queuetime=-1;//Illegal value
}

//Function which creates a queue dynamically from a file fname and returns the address of the first node in that linked List
//This function also obtains the values of the global variables BUFF_SIZE and NO_COUNTERS from the file
struct RequestHolder* RFile(const char* fname)
{
	static long ID=0;//The ID number of the request
	struct RequestHolder *qhead=NULL,*qtail,*qtemp;//qhead is the pointer to the first element in the queue of requestholders,qtail is the pointer to the last node and qtemp is a pointer to a newly created noed
	FILE* fp=fopen(fname,"r");//fname is opened in read mode and fp is a pointer to fname's buffer
	if(fp==NULL)//Detection of error opening file
	{
		printf("ERROR:Failed to open the file.");
		return  NULL;
	}
	fscanf(fp,"%d",&NO_COUNTERS);//Gets number of counters from the file
	fgetc(fp);//removes \n delimiter
	fscanf(fp,"%d",&BUFF_SIZE);//Gets buffer size of each counter
	fgetc(fp);//removes \n delimiter
	int Read;//Variable to store the integer value that is read from the file
	struct Request* newrequest;
	while((Read=fgetc(fp))!=EOF)//while it is not EOF
	{
		fseek(fp,-1,SEEK_CUR);//fp is moved a byte backward
		newrequest=(struct Request*)(malloc(sizeof(struct Request)));//space is allocated to store a single request
		if(newrequest==NULL)//detection of heap overflow
		{
			return qhead;
		}
		Initialize_Request(newrequest);//the values of the fields at newrequest are initialized
		qtemp=(struct RequestHolder*)(malloc(sizeof(struct RequestHolder)));//A request holder is created to hold the request in the queue
		if(qtemp==NULL)//detection of heap overflow
		{
			free(newrequest);//removal of memory allocated for newrequest
			return qhead;
		}
		ID++;//increment the total number of requests by 1.
		qtemp->request=newrequest;//qtemp holds newrequest
		qtemp->next=NULL;//qtemp's next is initialized with NULL
		if(qhead==NULL)//if queue is empty
		{
			qhead=qtail=qtemp;
		}
		else
		{//qtemp is added to the tail of the queue
			qtail->next=qtemp;
			qtail=qtemp;
		}
		sprintf((qtemp->request)->RequestID,"R%ld",ID);//The RequestID of the request at qtemp is assigned as "R(ID)"
		fscanf(fp,"%d",&((qtemp->request)->executiontime));//The executiontime of the request at qtemp is obtained from the file
		fgetc(fp);//\n delimiter is removed
	}
	fclose(fp);//file is closed
	return qhead;//head of the queue is returned
}

//Function to create a linked list of counters , each of which holds a buffer.
//The function returns the pointer to the head of the list of counters created.
struct Counter* CreateCounters_Buffers(int no_counters,int buff_size)
{
    struct Counter *counterhead=NULL,*newcounter,*countertail=NULL;//The head of the linked list of counters is counterhead,the tail is countertail and pointer to a newly created counter will be newcounter
    char countername[20];//array which be used for storing the countername temporarily
    int counter_count=1;//No of counters created will be stored in counter_count
    int buff_count=0;//number of buff nodes created.In case of sufficient heap memory, it will be equal to buffer size.
    struct RequestHolder *buffhead=NULL,*newbuffnode,*bufftail=NULL;//pointer variables which point to buffer's head, newly created buffernode and buffer's tail
    int flag=1;//Variable used for breaking out of loops in case of overflow
    while(counter_count<=no_counters)
    {
        newcounter=(struct Counter*)(malloc(sizeof(struct Counter)));//a new counter is created
        if(newcounter==NULL)//detection of overflow
        {
            printf("Failed to create the requisite number of counters.You can use %d counters.",counter_count-1);
            break;
        }
        (newcounter->bufferhead)=NULL;//Initialization of the pointer which stores the address of the first node of the buffer with NULL
        (newcounter->Counter_Request)=NULL;//No request at counter currently
        sprintf(countername,"Counter-%d",counter_count);//Assignment of value countername
        strcpy(newcounter->Counter_Name,countername);//Copying the value of countername to Counter_Name inside the new counter
        (newcounter->number_at_buffer)=0;//No of requests inside the buffer is currently 0.
        (newcounter->next)=NULL;//newcounter's next is NULL
        buff_count=1;//keeping track of number of buffer nodes created
        buffhead=bufftail=NULL;//no nodes in buffer
        while(buff_count<=buff_size)//Loop to create buff_size number of buffer nodes
        {
            //Creation and initialization of a single buffer node
            newbuffnode=(struct RequestHolder*)(malloc(sizeof(struct RequestHolder)));
            if(newbuffnode==NULL)//detection of overflow
            {
                printf("Failed to allocate space for the requisite buffer size for %s. The buff size for it will be %d",countername,buff_count-1);
                flag=0;
                break;
            }
            newbuffnode->next=NULL;
            newbuffnode->request=NULL;
            if(buffhead==NULL)//if buffer is empty
            {
                buffhead=bufftail=newbuffnode;
            }
            else
            {//Attaching the new buffer node at the tail
                bufftail->next=newbuffnode;
                bufftail=newbuffnode;
            }
            buff_count++;
        }
        newcounter->bufferhead=buffhead;//Linking the counter with its buffer
        if(flag==0)//detection of overflow
        {
            break;
        }
        if(counterhead==NULL)//if counter list is empty
        {
            counterhead=countertail=newcounter;
        }
        else
        {//attachment of new counter at the tail end.
            countertail->next=newcounter;
            countertail=newcounter;
        }
        counter_count++;
    }
    return counterhead;
}

//Function to print entries into fp's buffer.
//It takes in a linked list of completed requests as its argument
void Print_Allocation(FILE* fp,struct RequestHolder* listofcompletedrequestshead)
{
    struct Request* reqptr=NULL;//pointer to the Request which is held inside a  node of type RequestHolder
    while(listofcompletedrequestshead!=NULL)
    {
        reqptr=listofcompletedrequestshead->request;
        //statement to print the RequestID, CounterID at which the request was processed,the time at which it had entered the counter,time at which it left the counter,time spent in queue,time spent in counter's buffer
        fprintf(fp,"<%s\t%s\t%ld\t%ld\t%ld\t%ld>\n",reqptr->RequestID,reqptr->CounterID,reqptr->exittime,reqptr->exittime+reqptr->executiontime,reqptr->Queuetime,reqptr->exittime-reqptr->entrytime);
        listofcompletedrequestshead=listofcompletedrequestshead->next;
    }
    return ;
}

//Function to Print entries into fp's buffer
//It takes in the time_instant at which it is called, queue_size then,a pointer to the first node in the list of completed requests,a pointer to the first node in the list of counters
void Print_LogFile(FILE* fp,long int queue_size,long int time_instant,struct RequestHolder* listofcompletedrequestshead,struct Counter *Counterhead)
{
    //To print Line 1 at time_instant.The line has queue_size and buffer sizes at all counters
    fprintf(fp,"<%ld\t%ld\t",time_instant,queue_size);
    struct Counter* currentcounter=Counterhead;
    while(currentcounter!=NULL)
    {
        fprintf(fp,"%d\t",currentcounter->number_at_buffer);
        currentcounter=currentcounter->next;
    }
    fprintf(fp,">\n");
    //To print Line 2 at time_instant.The line consists of requests which have entered a buffer from the queue.
    currentcounter=Counterhead;
    struct RequestHolder* buffnode=NULL;//Pointer to a node in the buffer
    fprintf(fp,"<%ld\t",time_instant);
    while(currentcounter!=NULL)//Traversal across all counters
    {
        buffnode=currentcounter->bufferhead;
        while(buffnode!=NULL)//while buff node is not the last buffer node's next
        {
            if((buffnode->request)!=NULL)//if buff node has a request
                if(((buffnode->request)->entrytime)==time_instant)//if the request has entered the buffer at time_instant
                {
                    fprintf(fp,"%s\t",(buffnode->request)->RequestID);
                }
            buffnode=buffnode->next;//inner loop update
        }
        currentcounter=currentcounter->next;//outer loop update
    }
    fprintf(fp,">\n");
    //To print Line 3 at time_instant
    currentcounter=Counterhead;
    fprintf(fp,"<%ld\t",time_instant);
    struct Request* counterrequest;//pointer to the request at the counter
    while(currentcounter!=NULL)//Traversal across all counters
    {
        counterrequest=(currentcounter->Counter_Request);
        if(counterrequest!=NULL)//if there is a request at the counter
            if((counterrequest->exittime)==time_instant&&(counterrequest->exittime)!=0)//if the request has not entered the counter directly from the queue and has its exittime as current time_instant
            {
                fprintf(fp,"%s\t",counterrequest->RequestID);
            }
        currentcounter=currentcounter->next;//loop update
    }
    fprintf(fp,">\n");
    //To print Line 4 at time_instant
    struct RequestHolder* completedrequest=listofcompletedrequestshead;//pointer to a node of the list of completed requests
    fprintf(fp,"<%ld\t",time_instant);
    while(completedrequest!=NULL)//Traversal across all completed requests
    {
        fprintf(fp,"%s\t",(completedrequest->request)->RequestID);//printing the request IDs
        completedrequest=completedrequest->next;
    }
    fprintf(fp,">\n");
    return ;
}
//Function to determine the size of the queue whose head is qhead
long int Size_Of_Queue(struct RequestHolder* qhead)
{
    long int size=0;
    while(qhead!=NULL)
    {
        size++;
        qhead=qhead->next;
    }
    return size;
}

//Function to delete both the RequestHolder node and the request held by that node
void Delete_Request_And_Location(struct RequestHolder* reqptr)
{
    free(reqptr->request);
    free(reqptr);
    total_requests--;//To update the global total number of requests pending
}

//Function to delete the entire list of completed requests and their holders
void Free_List_of_Completed_Requests(struct RequestHolder* listhead)
{
    struct RequestHolder * currentlistnode;//Pointer to a request holder
    currentlistnode=listhead;
    while(listhead!=NULL)//Traversal across the entire list
    {
        currentlistnode=listhead;
        listhead=listhead->next;
        Delete_Request_And_Location(currentlistnode);
    }
    return ;
}

//Function to Push the request at qhead to an empty buffernode and delete the requestholder of it.
//Function also returns the new qhead of the queue after a pop at the head
//Function to also enter the values of the time at which it has left the queue and entered a buffer of some counter
struct RequestHolder* Push_Into_Buffer(struct Counter* counter,struct RequestHolder* qhead,long int time)
{
    struct RequestHolder* currentbuffnode;//will be a pointer to the current buffer node inside the loop
    currentbuffnode=counter->bufferhead;
    while(currentbuffnode!=NULL)//if it is not the next node of the last node in the buffer
    {
        if(currentbuffnode->request==NULL)//if current buffer node has no request
            break;
        currentbuffnode=currentbuffnode->next;
    }
    if(currentbuffnode!=NULL)//safety check
        currentbuffnode->request=qhead->request;
    qhead->request->entrytime=time;//assigning entryttime and queuetime
    qhead->request->Queuetime=time;
    struct RequestHolder* temp=NULL;//Variable for deletion purposes.
    temp=qhead;
    qhead=qhead->next;//updating qhead
    free(temp);//deleting the old RequestHolder at qhead
    queue_size--;//decrementing the queue size by 1
    counter->number_at_buffer++;//incrementing the count of request in counter's buffer by 1
    return qhead;
}
//Function to fill the counters and their buffers with requests at time=0
//It accepts a file pointer fp to print an entry into the logfile at time=0
//It also takes in the head of the queue linked list qhead and head of the counter linked list chead as parameters
struct RequestHolder* Initialize_All_Counters_And_Buffers(FILE* fp,struct Counter* chead,struct RequestHolder* qhead)
{
    struct Counter* currentcounter=NULL;//pointer to the current counter inside the loop
    currentcounter=chead;
    //Loop to fill the counters with requests from the queue directly at time=0
    while(currentcounter!=NULL)
    {
        if(queue_size>0)//if there are elements in the queue
        {
            strcpy((qhead->request)->CounterID,currentcounter->Counter_Name);//Assigning counter ID to the request
            //Assigning the time variables inside the requests their values
            qhead->request->entrytime=0;
            qhead->request->exittime=0;
            qhead->request->Queuetime=0;
            currentcounter->Counter_Request=qhead->request;//the counter now holds the request
            //statements to update the qhead and delete old qhead
            struct RequestHolder* temp=qhead;
            qhead=qhead->next;
            free(temp);
            queue_size--;//reduce the number of elements in the queue by 1.
        }
        else
        {
            break;
        }
        currentcounter=currentcounter->next;//loop update statement
    }
    //Pair of nested loops to fill up the buffers at time=0
    for(int buffpos=1;buffpos<=BUFF_SIZE&&queue_size>0;buffpos++)//buffpos=position in the buffer from the buffer head
    {
        for(int counterpos=1;counterpos<=NO_COUNTERS;counterpos++)//cuunterpos is the position of the counter from the counter head at the request needs to be inserted
        {
            currentcounter=chead;
            for(int i=1;i<counterpos;i++)//determining the counter at which the request needs to be held
                currentcounter=currentcounter->next;
            if(queue_size>0)//if queue is non-empty
            {
                qhead->request->entrytime=0;
                qhead->request->Queuetime=0;
                qhead=Push_Into_Buffer(currentcounter,qhead,0);//request is pushed from queue to buffer.qhead is updated
            }
            else
            {
                break;
            }
        }
    }
    Print_LogFile(fp,queue_size,0,NULL,chead);//Log file is printed with time =0,empty list of completed Requests
    return qhead;//returns the update head of the queue
}

//Function to Implement the request scheduling iteratively.
//afp is the file pointer to the allocation file's buffer
//lfp is the file pointer to the log file's buffer
//ptrtoahead is the pointer to the pointer which has the address to the first node in the queue
//It is needed because the function needs access to qhead
int Scheduler(struct Counter* counterhead,struct RequestHolder** ptrtoqhead,FILE* afp,FILE* lfp)
{
    static long time=1;//time is a static variable which is initialized to 1 during the first call of the Scheduler function
    struct Counter* currentcounter;//pointer to the current counter inside loops
    struct RequestHolder* currentbuffnode;//pointer to the current buffer node in loops
    struct Request* currentrequest;//pointer to the request held by currentbuffnode
    struct RequestHolder *completedrequestshead=NULL,*completedrequeststail,*newcompletedrequest;//set of pointers to create the linked list of completed requests
    if(total_requests==0)//if there are no requests
    {
        return 0;
    }
    currentcounter=counterhead;
    while(currentcounter!=NULL)//traversal across all counters
    {
        currentrequest=currentcounter->Counter_Request;
        if(currentrequest!=NULL)//if there is a request at the counter
        if((currentrequest->exittime+currentrequest->executiontime)==time)//testing for execution completion
        {
            newcompletedrequest=(struct RequestHolder*)(malloc(sizeof(struct RequestHolder)));//creation of a node of list of completed requests
            newcompletedrequest->next=NULL;
            newcompletedrequest->request=currentrequest;//current request is held at newcompleted request
            if(completedrequestshead==NULL)
            {
                completedrequestshead=completedrequeststail=newcompletedrequest;
            }
            else
            {
                completedrequeststail->next=newcompletedrequest;
                completedrequeststail=newcompletedrequest;
            }
            currentcounter->Counter_Request=NULL;//there is no request at the counter
        }
        currentcounter=(currentcounter->next);
    }
    currentcounter=counterhead;
    //Loop to fill all counter's buffers
    while(currentcounter!=NULL)//Loop to traverse across all oounters
    {
        if((currentcounter->Counter_Request)==NULL)//if there is no request at counter
        {
            if(currentcounter->number_at_buffer>0)//if there is atleast request in the counter's buffer
            {
                currentbuffnode=(currentcounter->bufferhead);
                //Statements to move a request from the first node of the buffer to the counter
                (currentcounter->Counter_Request)=(currentbuffnode->request);
                (currentbuffnode->request)->exittime=time;
                strcpy(currentbuffnode->request->CounterID,currentcounter->Counter_Name);
                (currentcounter->number_at_buffer)--;
                while((currentbuffnode->next)!=NULL)//if the current buff node is not the last buffer node
                {
                    if(((currentbuffnode->next)->request)==NULL)//if there is no request in the buffer node next to the current buffer node
                    {
                        currentbuffnode->request=NULL;
                        break;
                    }
                    (currentbuffnode->request)=(currentbuffnode->next)->request;//To shift the requests upward by 1 buffer
                    currentbuffnode=currentbuffnode->next;//inner loop update statement
                }
                if((currentbuffnode->next)==NULL&&queue_size>0)//if the currentbuffnode is the last node and queue_size is larger than 0
                {
                    currentbuffnode->request=NULL;
                    (*ptrtoqhead)=Push_Into_Buffer(currentcounter,*ptrtoqhead,time);//qhead is updated by the return value of Push_Into_Buffer
                }
            }
        }
        currentcounter=currentcounter->next;//Outer Loop Update statement
    }
    Print_Allocation(afp,completedrequestshead);//Allocation entries for all completed requests are printed
    Print_LogFile(lfp,queue_size,time,completedrequestshead,counterhead);//log file for the time instant= time is printed
    Free_List_of_Completed_Requests(completedrequestshead);//the list of completed requests is removed from the memory
    time++;//time is raised by 1
    return 1;//Returns 1 indicating that there may be more requests to complete their execution
}
