#include <iostream>
#include <fstream>
#include <string>
#include <sstream> 
#include <unordered_set>
#include<set>
#include <map>
#include<algorithm>
#include <iterator>
#include<tuple> 
#include<vector>
#include<math.h>
#include<string.h>
#include <deque>
#include <unistd.h> 
using namespace std;

typedef enum {STATE_CREATED, STATE_READY, STATE_RUNNING, STATE_BLOCKED, STATE_DONE } process_state_t;
typedef enum { TRANS_TO_READY, TRANS_TO_RUN, TRANS_TO_BLOCK, TRANS_TO_PREEMPT, TRANS_TO_DONE} transition;
int pctr = 0;
ifstream processes;
vector<int> randvals;
string process_line;
string token;
int ofs = 0;
ifstream randf;
double total_blocked = 0;
int maxprio=4;
char scheduler;
int userq=10000;

int myrandom(int burst) { 
if(ofs==randvals.size())
	{ofs = 0;}

	return 1 + (randvals[ofs++] % burst);
 }


class process {       
  public:             
    int arrival;        
    int total;  
    int cpu;
    int io;
    int act_cpu;
    int act_io;
    int static_prio;
    int dyna_prio;
    int remaining;  
    int state_time;
    int ft;
    int tt;
    int it;
    int cw;
    int pid;
    int cpu_use;
    process(int a, int b, int c, int d)
    {
    	cpu_use = 0;
    	act_io = 0;
    	act_cpu = 0;
    	pid = pctr;
    	arrival = a;
    	total = b;
    	cpu = c;
    	io = d;
    	static_prio = myrandom(maxprio);
    	dyna_prio = static_prio-1;
    	remaining = total;
    	state_time = arrival;
    	cw = 0;
    	it = 0;

    }
};

class sched{
	public:
		int quantum=10000;
		virtual void add_process(process *p){};
		virtual process* get_next_process(){};
		virtual bool test_preempt(process *p, process *p1){};

};
sched* s;

class fcfs : public sched{
public:
	// int quantum;
	deque<process*> runq;
	fcfs(int a=10000){
		quantum = a;
	}
	void add_process(process* p ){
		runq.push_back(p);
	}
	process* get_next_process(){
		if(runq.size()==0){
			return NULL;
		}
		process* p = runq.front();
		runq.pop_front();
		return p;
	}
	bool test_preempt(process *p, process *p1)
	{return false;}




};

class lcfs : public sched{
public:
	// int quantum;
	deque<process*> runq;
	lcfs(int a=10000){
		quantum = a;
	}
	void add_process(process* p ){
		
		runq.push_front(p);

	}
	process* get_next_process(){
		if(runq.size()==0){
			return NULL;
		}
		process* p = runq.front();
		runq.pop_front();
		return p;
	}
	bool test_preempt(process *p, process *p1)
	{return false;}

};


class srtf : public sched{
public:
	// int quantum;
	deque<process*> runq;
	srtf(int a=10000){
		quantum = a;
	}
	void add_process(process* p ){
		deque<process*>::iterator it = runq.begin();
    
  
			for(auto i: runq){
				if(i->remaining>p->remaining){
					runq.insert(it, p); 
					return;
				}
				it++;
			}
			runq.insert(it, p); 


	}
	process* get_next_process(){
		if(runq.size()==0){
			return NULL;
		}
		process* p = runq.front();
		runq.pop_front();
		return p;
	}
	bool test_preempt(process *p, process *p1)
	{return false;}

};

class rr : public sched{
public:
	// int quantum;
	deque<process*> runq;
	rr(int a=10000){
		quantum = a;
	}
	void add_process(process* p ){
		runq.push_back(p);
	}
	process* get_next_process(){
		if(runq.size()==0){
			return NULL;
		}
		process* p = runq.front();
		runq.pop_front();
		return p;
	}
	bool test_preempt(process *p, process *p1)
	{return false;}




};

class prio : public sched{
public:
	// int quantum;
	deque< deque<process*> > active_runq;
	deque< deque<process*> > expired_runq;

	prio(int a=10000){
		quantum = a;
		for(int i=0;i<maxprio+1;i++){

			active_runq.push_back(deque<process*>());
			expired_runq.push_back(deque<process*>());
		}

	}
	void add_process(process* p ){
		if(p->dyna_prio>=0){
		active_runq[p->dyna_prio].push_back(p);
	}
	else{
		p->dyna_prio = p->static_prio-1;
		expired_runq[p->dyna_prio].push_back(p);
	}
	}


	process* get_next_process(){
	process* p;
		for(int i=maxprio;i>=0;i--){
			if(active_runq[i].size()>0)
			{
				p = active_runq[i].front();
				active_runq[i].pop_front();
				return p;

			}
		}
			active_runq.swap(expired_runq);
			
		for(int i=maxprio;i>=0;i--){
			if(active_runq[i].size()>0)
			{
				p = active_runq[i].front();
				active_runq[i].pop_front();
				return p;

			}
		}
		return NULL;
		
	}
	bool test_preempt(process *p, process *p1)
	{return false;}




};


class preprio : public sched{
public:
	// int quantum;
	deque< deque<process*> > active_runq;
	deque< deque<process*> > expired_runq;

	preprio(int a=10000){
		quantum = a;
		for(int i=0;i<maxprio+1;i++){

			active_runq.push_back(deque<process*>());
			expired_runq.push_back(deque<process*>());
		}

	}
	void add_process(process* p ){
		if(p->dyna_prio>=0){
		active_runq[p->dyna_prio].push_back(p);
	}
	else{
		p->dyna_prio = p->static_prio-1;
		expired_runq[p->dyna_prio].push_back(p);
	}
	}


	process* get_next_process(){
	process* p;
		for(int i=maxprio;i>=0;i--){
			if(active_runq[i].size()>0)
			{
				p = active_runq[i].front();
				active_runq[i].pop_front();
				return p;

			}
		}
			active_runq.swap(expired_runq);
			
		for(int i=maxprio;i>=0;i--){
			if(active_runq[i].size()>0)
			{
				p = active_runq[i].front();
				active_runq[i].pop_front();
				return p;

			}
		}
		return NULL;
		
	}
	bool test_preempt(process *chk_proc, process *cur_run)
	{
		if(chk_proc->dyna_prio>cur_run->dyna_prio)
			{return true;}
		return false;
	}




};



deque<process*> input_preproc(string filename){
	
	deque<process*> procs;
	processes.open(filename);

	while(getline(processes,process_line))
	{	
		stringstream streamline(process_line);
		streamline>>token;
		int at = stoi(token);
		streamline>>token;
		int tc = stoi(token);
		streamline>>token;
		int cb = stoi(token);
		streamline>>token;
		int io = stoi(token);
		process* p = new process(at,tc,cb,io);
		procs.push_back(p);
		pctr++;
	}
	return procs;


}


class event{

public:
	int timestamp;
	process* proc;
	process_state_t old_state;
	process_state_t new_state;
	transition trans;
	bool proc_prempted;
	event(int a, process* b, process_state_t c, process_state_t d, transition e, bool f=false){
		timestamp = a;
		proc = b;
		old_state = c;
		new_state = d;
		trans = e;
		proc_prempted = f;

	}
};

class DES{
	public:
		deque<event*> eventq;
		DES(deque<event*> eventq_inp){
			eventq = eventq_inp;
		}

		int get_next_event_time(){
			if(eventq.size() == 0)
				{return NULL;}
			return eventq.front()->timestamp;
		}
		event* get_event(){
			return eventq.front();
		}

		bool is_empty(){
			if(eventq.size() == 0){
				return true;
			}
			else{
				return false;
			}
		}

		void rm_event(){
			eventq.pop_front();
		}
		void rm_proc_event(process* p)
		{
			deque<event*>::iterator it = eventq.begin();
    
  
			for(auto i: eventq){
				if(i->proc==p){
					eventq.erase(it); 
					return;
				}
				it++;
			}
			// eventq.insert(it, e); 


		}
		event* get_proc_event(process* p)
		{
    
  
			for(auto i: eventq){
				if(i->proc==p){
					return i;
				}
			}


		}
		bool check_if_not_force(process* p,int curtime)
		{
			deque<event*>::iterator it = eventq.begin();
    
  
			for(auto i: eventq){
				if(i->timestamp == curtime && i->proc==p){
					return true;
				}
				it++;
			}
			return false;
		}
		void put_event(event* e)
		{
			deque<event*>::iterator it = eventq.begin();
    
  
			for(auto i: eventq){
				if(i->timestamp>e->timestamp){
					eventq.insert(it, e); 
					return;
				}
				it++;
			}
			eventq.insert(it, e); 

		}


};



void Simulation(DES* d) {
 event* evt;
 process* CURRENT_RUNNING_PROCESS = NULL;
 bool CALL_SCHEDULER;
 int prev_blocked=0;

 while( (!d->is_empty())) {
				 	event* evt = d->get_event();
				 	event* e1;
				 	event* pre_evt;
				 process *proc = evt->proc; // this is the process the event works on
				 // printf("%d %d %d %d     evt data\n",evt->timestamp,evt->old_state,evt->new_state,evt->trans);
				 int CURRENT_TIME = evt->timestamp;
				 int timeInPrevState = CURRENT_TIME-proc->state_time;
				 int for_block = proc->state_time;
				 proc->state_time = CURRENT_TIME;
					 if(evt->old_state == STATE_BLOCKED){
					 	proc->it += timeInPrevState;
					 	// cout<<CURRENT_TIME - max(prev_blocked,for_block)<<endl;
					 	
					 	

					 }
					 else if(evt->old_state == STATE_READY)
					 {
					 	proc->cw += timeInPrevState;
					 }
					 else if(evt->old_state == STATE_RUNNING)
					 {
					 	proc->cpu_use += timeInPrevState;
					 }
					 d->rm_event();
				 switch(evt->trans) { // which state to transition to?
									 case TRANS_TO_READY:{
if(evt->old_state == STATE_BLOCKED)
									 					{
									 						proc->dyna_prio = proc->static_prio -1;
									 					}

									 					if(CURRENT_RUNNING_PROCESS != NULL && s->test_preempt(proc,CURRENT_RUNNING_PROCESS))
									 					{
									 							if(!d->check_if_not_force(CURRENT_RUNNING_PROCESS,CURRENT_TIME))
									 							{
									 								pre_evt = d->get_proc_event(CURRENT_RUNNING_PROCESS);	
									 								CURRENT_RUNNING_PROCESS->act_cpu += (pre_evt->timestamp - CURRENT_TIME);
									 								// cout<<CURRENT_TIME<<"   "<< CURRENT_RUNNING_PROCESS->pid<<" cur run "<<proc->pid <<"  curtime "<<CURRENT_RUNNING_PROCESS->remaining<<" old remaining  "<< CURRENT_RUNNING_PROCESS->remaining+ (pre_evt->timestamp - CURRENT_TIME)<<" new remaining  "<<pre_evt->timestamp<<endl;
									 								CURRENT_RUNNING_PROCESS->remaining += (pre_evt->timestamp - CURRENT_TIME);
									 								d->rm_proc_event(CURRENT_RUNNING_PROCESS);
									 								e1 = new event(CURRENT_TIME, CURRENT_RUNNING_PROCESS, STATE_RUNNING, STATE_READY, TRANS_TO_PREEMPT,true);

									 								d->put_event(e1);
									 							}
									 					}

									 					
													 s->add_process(proc);// must come from BLOCKED or from PREEMPTION
													 // must add to run queue
													 CALL_SCHEDULER = true; // conditional on whether something is run
													 break;}
									 case TRANS_TO_RUN:{
									 						
									 						

									 						if(proc->act_cpu <= 0){
									 						int cpu_time_alloted = min(myrandom(proc->cpu),proc->remaining);	
									 						proc->act_cpu = cpu_time_alloted;
									 						// cout<<cpu_time_alloted<<endl;

									 						}
									 						
									 			
													 	
													 	// cout<<cpu_time_alloted<<" cputime"<<endl;
													 	CURRENT_RUNNING_PROCESS = proc;
													 	// cout<<s->quantum;
												 	if(proc->act_cpu <= s->quantum){
													 	if(proc->remaining - proc->act_cpu <= 0)
													 	{ 
													 		e1 = new event(CURRENT_TIME+proc->remaining, proc, STATE_RUNNING, STATE_DONE, TRANS_TO_DONE);
													 		// cout<<CURRENT_TIME<<" "<<cpu_time_alloted<<" first"<<endl;

													 		proc->act_cpu = 0;
													 		proc->remaining = 0;

													 d->put_event(e1);
													 	}
													else{
													 e1 = new event(CURRENT_TIME+proc->act_cpu, proc, STATE_RUNNING, STATE_BLOCKED, TRANS_TO_BLOCK);
													 d->put_event(e1);
													 		// cout<<CURRENT_TIME<<" "<<cpu_time_alloted<<" second"<<endl;

													proc->remaining = proc->remaining - proc->act_cpu;
												proc->act_cpu = 0;
											}

											}
											else{
												if(proc->remaining - s->quantum <= 0)
													 	{ 
													 		e1 = new event(CURRENT_TIME+proc->remaining, proc, STATE_RUNNING, STATE_DONE, TRANS_TO_DONE);
													 		// cout<<CURRENT_TIME<<" "<<cpu_time_alloted<<" first"<<endl;

													 		proc->remaining = 0;
													 		proc->act_cpu = 0;
													 d->put_event(e1);
													 	}
													else{
														//todo: state cahnge to premept
													 e1 = new event(CURRENT_TIME+s->quantum, proc, STATE_RUNNING, STATE_BLOCKED, TRANS_TO_PREEMPT);
													 d->put_event(e1);
													 		// cout<<CURRENT_TIME<<" "<<cpu_time_alloted<<" second"<<endl;

													proc->remaining = proc->remaining - s->quantum;
												proc->act_cpu -= s->quantum;
											}

											}

													 // create event for either preemption or blocking
													 break;}
									 case TRANS_TO_BLOCK:{
									 					// total_blocked += 1;

									 				
									 					

													 	CURRENT_RUNNING_PROCESS = NULL;
													 	// if(proc->act_io ==0)
													 	// {
													 		int io = myrandom(proc->io);
													 		proc->act_io = io;
													 		// cout<<CURRENT_TIME<<" "<<CURRENT_TIME+proc->act_io<< " "<<proc->act_io<<" "<<prev_blocked <<endl;
													 		// cout<<CURRENT_TIME+proc->act_io<< "  "<<prev_blocked <<endl;

													 		if(CURRENT_TIME>prev_blocked)
													 		{total_blocked += proc->act_io;
													 			// cout<<"full\n";
													 		}
													 		else if((CURRENT_TIME+proc->act_io)>prev_blocked)
													 		{
													 			// cout<<CURRENT_TIME+proc->act_io<< " asd "<<prev_blocked <<endl;
													 			total_blocked += (proc->act_io + CURRENT_TIME - prev_blocked);
													 			// cout<<"partail\n";
													 		}
													 		else
													 		{
													 				// cout<<"complete overlap";

													 		}

													 		// cout<<total_blocked<<" total_blocked"<<endl;
													 	prev_blocked = max(CURRENT_TIME+ proc->act_io,prev_blocked);
													 	// }
													 		
													 e1 = new event(CURRENT_TIME+proc->act_io , proc, STATE_BLOCKED, STATE_READY, TRANS_TO_READY);
													 		// cout<<CURRENT_TIME<<"  "<<io<< " io "<<endl;

													 d->put_event(e1);
													 //create an event for when process becomes READY again
													 CALL_SCHEDULER = true;
													 break;}
									 case TRANS_TO_PREEMPT:{
														 CURRENT_RUNNING_PROCESS = NULL;
														 // if(!evt->proc_prempted){
														 proc->dyna_prio -= 1;//}
									 						s->add_process(proc);
														 // add to runqueue (no event is generated)
														CALL_SCHEDULER = true;
														break;}
									case TRANS_TO_DONE:{
															CURRENT_RUNNING_PROCESS = NULL;
															proc->ft = CURRENT_TIME;
															proc->tt = CURRENT_TIME - proc->arrival;
															CALL_SCHEDULER = true;
														break;}
				 }
				 // remove current event object from Memory

				// printf("%d ctime_\n",CURRENT_TIME);
				 	 

				 if(CALL_SCHEDULER) {
								//  	if(d->is_empty()){
								// 	break;
								// }
								 if (d->get_next_event_time() == CURRENT_TIME)
										 continue; //process next event from Event queue
								 CALL_SCHEDULER = false; // reset global flag
								if (CURRENT_RUNNING_PROCESS == NULL) {
											 CURRENT_RUNNING_PROCESS = s->get_next_process();
											 if (CURRENT_RUNNING_PROCESS == NULL)
													 {continue;}
											  e1 = new event(CURRENT_TIME, CURRENT_RUNNING_PROCESS, STATE_READY, STATE_RUNNING, TRANS_TO_RUN);
											 // cout<<CURRENT_TIME<<" at the end "<<endl;
											 d->put_event(e1);

								 // create event to make this process runnable for same time.
				} 
				CALL_SCHEDULER = false;} 
		// 		cout<<"EVENTQ"<<endl;
		// 		for(auto i : d->eventq)
		// 				{printf("%d %d %d %d %d \n",i->timestamp,i->old_state,i->new_state,i->trans,i->proc->remaining);}
		// 		cout<<"EVENTQ"<<endl;
		// 		cout<<"RUNQ"<<endl;
		// 		for(auto i : s->runq)
		// {printf("%d %d %d %d %d | %d %d %d %d\n",i->arrival,i->total,i->cpu,i->io, i->static_prio,i->ft,i->tt,i->it,i->cw);}

		// 		cout<<"RUNQ"<<endl;


} }






int main (int argc, char **argv) {
	int ch;
	while((ch=getopt(argc,argv,"vtes:"))!=-1)
	{
		switch(ch)
		{case 's': {sscanf(optarg,"%c%d:%d",&scheduler,&userq,&maxprio);
		break;}
			
	}
	}


	std::map<char, string> dictionary;
	dictionary.insert(pair<char,string>('F',"FCFS"));
	dictionary.insert(pair<char,string>('L',"LCFS"));
	dictionary.insert(pair<char,string>('S',"SRTF"));
	dictionary.insert(pair<char,string>('R',"RR"));
	dictionary.insert(pair<char,string>('P',"PRIO"));
	dictionary.insert(pair<char,string>('E',"PREPRIO"));
	
	// maxprio = 5;
	randf.open(argv[optind + 1]);
	string number_rands;
	getline(randf,number_rands);
	int n = stoi(number_rands);
	int temp;
	for(int i = 0; i<n; i++)
	{
		getline(randf,number_rands);
		temp = stoi(number_rands);
		randvals.push_back(temp);

	}
	// printf("%d %d %d",randvals[0],randvals[1],randvals.size());
	deque<process*> all_proc_q;
	all_proc_q = input_preproc(argv[optind]);

	deque<event*> eq;
	for(auto i: all_proc_q)
	{
		event* e_ = new event(i->arrival,i,STATE_CREATED,STATE_READY,TRANS_TO_READY);
		eq.push_back(e_);
	}
	// for(auto i : eq)
	// 	{printf("%d %d %d %d \n",i->timestamp,i->old_state,i->new_state,i->trans);}

	DES* d = new DES(eq);
	switch(scheduler){
		case 'F':{s = new fcfs();
			break;}
			case 'L':{s= new lcfs();
			break;}
			case 'S':{s = new srtf();
			break;}
			case 'R':{s = new rr(userq);
			break;}
			case 'P':{s = new prio(userq);
			break;}
			case 'E':{s = new preprio(userq);
			break;}
		
	}
	// s = new fcfs();
	// s= new lcfs();
	// s = new srtf();
	// s = new rr(5);
	// s = new prio(5);
	// s = new preprio(userq);
	Simulation(d);
	cout<<dictionary.find(scheduler)->second;
	if(userq<10000){
		cout<<" "<<userq;
	}
	cout<<endl;
	double cpu_consumption = 0;
	double avg_tt = 0;
	double ctr = 0;
	double avg_cw = 0;
	int endtime = 0;
	for(auto i : all_proc_q)
		{	
			printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",i->pid,i->arrival,i->total,i->cpu,i->io, i->static_prio,i->ft,i->tt,i->it,i->cw);
			cpu_consumption += i->cpu_use;
			ctr += 1;
			avg_tt+= i->tt;
			avg_cw += i->cw;
			endtime = max(endtime,i->ft);


		}
		// cout<<endtime<<endl;
	// cout<<cpu_consumption<<endl;
printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",endtime,(100.0*cpu_consumption/((double)(endtime))),(100*total_blocked/(double)endtime),(avg_tt/ctr),avg_cw/ctr,(double)ctr/((double)endtime/100.0) );


}
