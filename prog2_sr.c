#include <stdio.h>

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional or bidirectional
   data transfer protocols (from A to B. Bidirectional transfer of data
   is for extra credit and is not required).  Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

#define BIDIRECTIONAL 0   /* change to 1 for bidirectional */
                           /* and write a routine called B_output */

/* a "msg" is the data unit passed from layer 5 (teachers code) to layer  */
/* 4 (students' code).  It contains the data (characters) to be delivered */
/* to layer 5 via the students transport level protocol entities.         */



struct msg {
  char data[20];
};

/* a packet is the data unit passed from layer 4 (students code) to layer */
/* 3 (teachers code).  Note the pre-defined packet structure, which all   */
/* students must follow. */
struct pkt {
   int seqnum;
   int acknum;
   int checksum;
   char payload[20];
};

void stoptimer(int AorB);
void starttimer(int AorB, float increment);
void tolayer3(int AorB, struct pkt packet);
void tolayer5(int AorB, char datasent[20]);

/*
Global variables
*/
int MAX_BUFFER = 1500;
const int WINDOW_SIZE = 8;
struct pkt queue[50000];
struct pkt receiver_buffer[WINDOW_SIZE];
int seqnums_A;
int base_A;
// int seqnums_B;
struct pkt data_packet_A;
// struct pkt data_packet_B;
int last_received_B;
float timeout_value;
int from_app_A=0;
int from_transport_A=0;
int to_transport_B=0;
int to_app_B=0;
// int inTransition_A;
// int inTransition_B; 
// int state_A;
// int state_B;

int get_checksum(int seqnum, int acknum, char payload[20])
{
  int checksum = 0;
  checksum += seqnum + acknum;
  if (payload != NULL) {
    for (int i=0; i < 20; i++)
    {
      checksum += (int)payload[i];
    }
  }
  return checksum;
}

struct pkt create_pkt(int seqnum, int acknum, char payload[20]) {
  struct pkt packet;
  packet.seqnum = seqnum;
  packet.acknum = acknum;
  if (payload != NULL) {
    strncpy(packet.payload, payload, 20);
  }
  packet.checksum = get_checksum(seqnum, acknum, packet.payload);
  return packet;
}

void log_packet(char calling_entity, struct pkt packet) {
  printf("packet sent from %c with message %.*s and seq number %d\n", calling_entity, (int)sizeof(packet.payload), packet.payload, packet.seqnum);
}

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
  from_app_A++;
  if (seqnums_A - base_A >= MAX_BUFFER) {
    printf("exiting and dropping packet at A since outside the allowed MAX_BUFFER of %d\n", MAX_BUFFER);
    exit(1);
  } else {
    // inTransition_A = 1;
    data_packet_A = create_pkt(seqnums_A, -1000, message.data);
    printf("at A, created packet with sequence number %d\n", seqnums_A);
    queue[seqnums_A++] = data_packet_A;
    if (seqnums_A - base_A <= WINDOW_SIZE) {
        starttimer(seqnums_A-1, timeout_value);
        log_packet('A', data_packet_A);
        printf("checksum A is %d\n", data_packet_A.checksum);
    
        tolayer3(0, data_packet_A);
        from_transport_A++;
    }
    else {
        printf("at A, putting packet number %d into buffer since outside the window size of %d\n", seqnums_A-1, WINDOW_SIZE);
    }
    
  }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
  int checksum = packet.checksum;
  if (checksum == get_checksum(packet.seqnum, packet.acknum, packet.payload)) {
    if (packet.acknum != -1000)
    {
        printf("received acknowledgment from B with acknum %d\n", packet.acknum);
        if (queue[packet.acknum].acknum != -1) {
          stoptimer(packet.acknum);
          queue[packet.acknum].acknum = -1;
          if (base_A == packet.acknum) {
            int packet_to_sent = base_A + WINDOW_SIZE;
            base_A++;
            while (queue[base_A].acknum == -1 && base_A < seqnums_A) {
              base_A++;
            }
            printf("base_A moved to %d\n", base_A);
            while (packet_to_sent < seqnums_A && packet_to_sent < base_A + WINDOW_SIZE) {
              starttimer(packet_to_sent, timeout_value);
              log_packet('A', queue[packet_to_sent]);
              tolayer3(0, queue[packet_to_sent++]);
            }
          }
        }
        
    } else {
        printf("Something wrong: Should only receive ack packets from B since unidirectional\n");
    } 
  } else {
    printf("corrupt packet detected at A\n");
    //do nothing
  }
}

/* called when A's timer goes off */
void A_timerinterrupt(int packetType)
{
  printf("timeout happened at A for packet %d\n", packetType);
  starttimer(packetType, timeout_value);
  log_packet('A', queue[packetType]);
  tolayer3(0, queue[packetType]);
  from_transport_A++;
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  seqnums_A = 0;
  base_A = 0;
  timeout_value = 20.0;
}

/* called from layer 5, passed the data to be sent to other side */
void B_output(struct msg message)
{
}
/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
  to_transport_B++;
  int checksum = packet.checksum;
  // int ack_nack_number = last_received_B;
  if (checksum == get_checksum(packet.seqnum, packet.acknum, packet.payload)) 
  {
    if (packet.acknum != -1000)
    {
        printf("at B, something wrong acknum should be -1000 since unidirectional\n");
    } 
    else 
    {  
        printf("received packet from A with message %.*s, sequence number %d\n", (int)sizeof(packet.payload), packet.payload, packet.seqnum);
        if (packet.seqnum == last_received_B + 1) 
        {
          printf("last received B is %d\n", last_received_B);
            printf("at B, packet with message %.*s, seq number %d sent to layer 5 i.e. application layer\n", (int)sizeof(packet.payload), packet.payload,packet.seqnum);
            to_app_B++;
            tolayer5(1, packet.payload);
            int i=packet.seqnum%WINDOW_SIZE;
            receiver_buffer[i] = packet;
            receiver_buffer[i].acknum =  -1;
            i++;
            last_received_B++;
            // printf("test2\n");
            i=i%WINDOW_SIZE;
            while (receiver_buffer[i].acknum !=  -1) {
              tolayer5(1, receiver_buffer[i].payload);
              printf("at B, packet with message %.*s and sequence number %d sent to layer 5 i.e. application layer\n", (int)sizeof(receiver_buffer[i].payload), receiver_buffer[i].payload, receiver_buffer[i].seqnum);

              to_app_B++;

              receiver_buffer[i].acknum = -1;
              i++;
              i=i%WINDOW_SIZE;       
              last_received_B++;
            }     
            // printf("test3\n");       
        } 
        else 
        {
            printf("not received expected packet from A, expected packet %d, got %d\n", last_received_B + 1, packet.seqnum);
            if (packet.seqnum > last_received_B + WINDOW_SIZE) {
              printf("at B, received packet number outside buffer window so ignoring packet %d.\n", packet.seqnum);
              return;
            }
            else if (packet.seqnum > last_received_B){
              printf("at B, buffering the packet %d in window\n", packet.seqnum);
              receiver_buffer[packet.seqnum%WINDOW_SIZE] = packet;
            }
        }
        struct pkt ack_packet = create_pkt(-1000, packet.seqnum, NULL);
        printf("ack sent from B with acknum %d\n", ack_packet.acknum);
        tolayer3(1, ack_packet);
    }
  }
  else {
    printf("corrupt packet detected at B\n");
  }
}

/* called when B's timer goes off */
void B_timerinterrupt()
{
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
  last_received_B = -1;
  for (int i=0; i < WINDOW_SIZE; i++) {
    receiver_buffer[i].acknum = -1;
  }
}


/*****************************************************************
***************** NETWORK EMULATION CODE STARTS BELOW ***********
The code below emulates the layer 3 and below network environment:
  - emulates the tranmission and delivery (possibly with bit-level corruption
    and packet loss) of packets across the layer 3/4 interface
  - handles the starting/stopping of a timer, and generates timer
    interrupts (resulting in calling students timer handler).
  - generates message to be sent (passed from later 5 to 4)

THERE IS NOT REASON THAT ANY STUDENT SHOULD HAVE TO READ OR UNDERSTAND
THE CODE BELOW.  YOU SHOLD NOT TOUCH, OR REFERENCE (in your code) ANY
OF THE DATA STRUCTURES BELOW.  If you're interested in how I designed
the emulator, you're welcome to look at the code - but again, you should have
to, and you defeinitely should not have to modify
******************************************************************/

struct event {
   float evtime;           /* event time */
   int evtype;             /* event type code */
   int eventity;           /* entity where event occurs */
   struct pkt *pktptr;     /* ptr to packet (if any) assoc w/ this event */
   struct event *prev;
   struct event *next;
 };
struct event *evlist = NULL;   /* the event list */

/* possible events: */
#define  TIMER_INTERRUPT 0  
#define  FROM_LAYER5     1
#define  FROM_LAYER3     2

#define  OFF             0
#define  ON              1
#define   A    0
#define   B    1



int TRACE = 1;             /* for my debugging */
int nsim = 0;              /* number of messages from 5 to 4 so far */ 
int nsimmax = 0;           /* number of msgs to generate, then stop */
float time = 0.000;
float lossprob;            /* probability that a packet is dropped  */
float corruptprob;         /* probability that one bit is packet is flipped */
float lambda;              /* arrival rate of messages from layer 5 */   
int   ntolayer3;           /* number sent into layer 3 */
int   nlost;               /* number lost in media */
int ncorrupt;              /* number corrupted by media*/

main()
{
   struct event *eventptr;
   struct msg  msg2give;
   struct pkt  pkt2give;
   
   int i,j;
   char c; 
  
   init();
   A_init();
   B_init();
   
   while (1) {
        eventptr = evlist;            /* get next event to simulate */
        if (eventptr==NULL)
           goto terminate;
        evlist = evlist->next;        /* remove this event from event list */
        if (evlist!=NULL)
           evlist->prev=NULL;
        if (TRACE>=2) {
           printf("\nEVENT time: %f,",eventptr->evtime);
           printf("  type: %d",eventptr->evtype);
           if (eventptr->evtype==0)
         printf(", timerinterrupt  ");
             else if (eventptr->evtype==1)
               printf(", fromlayer5 ");
             else
       printf(", fromlayer3 ");
           printf(" entity: %d\n",eventptr->eventity);
           }
        time = eventptr->evtime;        /* update time to next event time */
        if (nsim==nsimmax)
    break;                        /* all done with simulation */
        if (eventptr->evtype == FROM_LAYER5 ) {
            generate_next_arrival();   /* set up future arrival */
            /* fill in msg to give with string of same letter */    
            j = nsim % 26; 
            for (i=0; i<20; i++)  
               msg2give.data[i] = 97 + j;
            if (TRACE>2) {
               printf("          MAINLOOP: data given to student: ");
                 for (i=0; i<20; i++) 
                  printf("%c", msg2give.data[i]);
               printf("\n");
       }
            nsim++;
            if (eventptr->eventity == A) 
               A_output(msg2give);  
             else
               B_output(msg2give);  
            }
          else if (eventptr->evtype ==  FROM_LAYER3) {
            pkt2give.seqnum = eventptr->pktptr->seqnum;
            pkt2give.acknum = eventptr->pktptr->acknum;
            pkt2give.checksum = eventptr->pktptr->checksum;
            for (i=0; i<20; i++)  
                pkt2give.payload[i] = eventptr->pktptr->payload[i];
      if (eventptr->eventity ==A)      /* deliver packet by calling */
           A_input(pkt2give);            /* appropriate entity */
            else
           B_input(pkt2give);
      free(eventptr->pktptr);          /* free the memory for packet */
            }
          else if (eventptr->evtype ==  TIMER_INTERRUPT) {
            if (eventptr->eventity == A) 
         A_timerinterrupt(eventptr->eventity);
             else
         A_timerinterrupt(eventptr->eventity);
             }
          else  {
       printf("INTERNAL PANIC: unknown event type \n");
             }
        free(eventptr);
        }

terminate:
   printf(" Simulator terminated at time %f\n after sending %d msgs from layer5\n",time,nsim);


   printf("Simulation Results:\n");
   printf("Protocol: Selective Repeat\n");
   printf("%d of packets sent from the Application Layer of Sender A\n", from_app_A);
   printf("%d of packets sent from the Transport Layer of Sender A\n", from_transport_A);
   printf("%d packets received at the Transport layer\n", to_transport_B);
   printf("%d packets received at the Application layer\n", to_app_B);
   printf("Total time: %f time units\n", time);
   printf("Throughput = %f packets/time units\n", to_app_B*1.0/time);
}



init()                         /* initialize the simulator */
{
  int i;
  float sum, avg;
  float jimsrand();
  
  
   printf("-----  Stop and Wait Network Simulator Version 1.1 -------- \n\n");
   printf("Enter the number of messages to simulate: ");
   scanf("%d",&nsimmax);
   printf("Enter  packet loss probability [enter 0.0 for no loss]:");
   scanf("%f",&lossprob);
   printf("Enter packet corruption probability [0.0 for no corruption]:");
   scanf("%f",&corruptprob);
   printf("Enter average time between messages from sender's layer5 [ > 0.0]:");
   scanf("%f",&lambda);
   printf("Enter TRACE:");
   scanf("%d",&TRACE);

   srand(9999);              /* init random number generator */
   sum = 0.0;                /* test random number generator for students */
   for (i=0; i<1000; i++)
      sum=sum+jimsrand();    /* jimsrand() should be uniform in [0,1] */
   avg = sum/1000.0;
   if (avg < 0.25 || avg > 0.75) {
    printf("It is likely that random number generation on your machine\n" ); 
    printf("is different from what this emulator expects.  Please take\n");
    printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
    exit(0);
    }

   ntolayer3 = 0;
   nlost = 0;
   ncorrupt = 0;

   time=0.0;                    /* initialize time to 0.0 */
   generate_next_arrival();     /* initialize event list */
}

/****************************************************************************/
/* jimsrand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in therange [0,mmm]        */
/****************************************************************************/
float jimsrand() 
{
  double mmm = 2147483647;   /* largest int  - MACHINE DEPENDENT!!!!!!!!   */
  float x;                   /* individual students may need to change mmm */ 
  x = rand()/mmm;            /* x should be uniform in [0,1] */
  return(x);
}  

/********************* EVENT HANDLINE ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/
 
generate_next_arrival()
{
   double x,log(),ceil();
   struct event *evptr;
    char *malloc();
   float ttime;
   int tempint;

   if (TRACE>2)
       printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");
 
   x = lambda*jimsrand()*2;  /* x is uniform on [0,2*lambda] */
                             /* having mean of lambda        */
   evptr = (struct event *)malloc(sizeof(struct event));
   evptr->evtime =  time + x;
   evptr->evtype =  FROM_LAYER5;
   if (BIDIRECTIONAL && (jimsrand()>0.5) )
      evptr->eventity = B;
    else
      evptr->eventity = A;
   insertevent(evptr);
} 


insertevent(p)
   struct event *p;
{
   struct event *q,*qold;

   if (TRACE>2) {
      printf("            INSERTEVENT: time is %lf\n",time);
      printf("            INSERTEVENT: future time will be %lf\n",p->evtime); 
      }
   q = evlist;     /* q points to header of list in which p struct inserted */
   if (q==NULL) {   /* list is empty */
        evlist=p;
        p->next=NULL;
        p->prev=NULL;
        }
     else {
        for (qold = q; q !=NULL && p->evtime > q->evtime; q=q->next)
              qold=q; 
        if (q==NULL) {   /* end of list */
             qold->next = p;
             p->prev = qold;
             p->next = NULL;
             }
           else if (q==evlist) { /* front of list */
             p->next=evlist;
             p->prev=NULL;
             p->next->prev=p;
             evlist = p;
             }
           else {     /* middle of list */
             p->next=q;
             p->prev=q->prev;
             q->prev->next=p;
             q->prev=p;
             }
         }
}

void printevlist()
{
  struct event *q;
  int i;
  printf("--------------\nEvent List Follows:\n");
  for(q = evlist; q!=NULL; q=q->next) {
    printf("Event time: %f, type: %d entity: %d\n",q->evtime,q->evtype,q->eventity);
    }
  printf("--------------\n");
}



/********************** Student-callable ROUTINES ***********************/

/* called by students routine to cancel a previously-started timer */
/* A or B is trying to stop timer */
void stoptimer(int AorB){
 struct event *q,*qold;

 if (TRACE>2)
    printf("          STOP TIMER: stopping timer at %f\n",time);
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
 for (q=evlist; q!=NULL ; q = q->next) 
    if ( (q->evtype==TIMER_INTERRUPT  && q->eventity==AorB) ) { 
       /* remove this event */
       if (q->next==NULL && q->prev==NULL)
             evlist=NULL;         /* remove first and only event on list */
          else if (q->next==NULL) /* end of list - there is one in front */
             q->prev->next = NULL;
          else if (q==evlist) { /* front of list - there must be event after */
             q->next->prev=NULL;
             evlist = q->next;
             }
           else {     /* middle of list */
             q->next->prev = q->prev;
             q->prev->next =  q->next;
             }
       free(q);
       return;
     }
  printf("Warning: unable to cancel your timer. It wasn't running.\n");
}


void starttimer(int AorB, float increment){

 struct event *q;
 struct event *evptr;
 char *malloc();

 if (TRACE>2)
    printf("          START TIMER: starting timer at %f\n",time);
 /* be nice: check to see if timer is already started, if so, then  warn */
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
   for (q=evlist; q!=NULL ; q = q->next)  
    if ( (q->evtype==TIMER_INTERRUPT  && q->eventity==AorB) ) { 
      printf("Warning: attempt to start a timer that is already started\n");
      return;
      }
 
/* create future event for when timer goes off */
   evptr = (struct event *)malloc(sizeof(struct event));
   evptr->evtime =  time + increment;
   evptr->evtype =  TIMER_INTERRUPT;
   evptr->eventity = AorB;
   insertevent(evptr);
} 


/************************** TOLAYER3 ***************/
void tolayer3(int AorB, struct pkt packet){
 struct pkt *mypktptr;
 struct event *evptr,*q;
 char *malloc();
 float lastime, x, jimsrand();
 int i;


 ntolayer3++;

 /* simulate losses: */
 if (jimsrand() < lossprob)  {
      nlost++;
      if (TRACE>0)    
  printf("          TOLAYER3: packet being lost\n");
      return;
    }  

/* make a copy of the packet student just gave me since he/she may decide */
/* to do something with the packet after we return back to him/her */ 
 mypktptr = (struct pkt *)malloc(sizeof(struct pkt));
 mypktptr->seqnum = packet.seqnum;
 mypktptr->acknum = packet.acknum;
 mypktptr->checksum = packet.checksum;
 for (i=0; i<20; i++)
    mypktptr->payload[i] = packet.payload[i];
 if (TRACE>2)  {
   printf("          TOLAYER3: seq: %d, ack %d, check: %d ", mypktptr->seqnum,
    mypktptr->acknum,  mypktptr->checksum);
    for (i=0; i<20; i++)
        printf("%c",mypktptr->payload[i]);
    printf("\n");
   }

/* create future event for arrival of packet at the other side */
  evptr = (struct event *)malloc(sizeof(struct event));
  evptr->evtype =  FROM_LAYER3;   /* packet will pop out from layer3 */
  evptr->eventity = (AorB+1) % 2; /* event occurs at other entity */
  evptr->pktptr = mypktptr;       /* save ptr to my copy of packet */
/* finally, compute the arrival time of packet at the other end.
   medium can not reorder, so make sure packet arrives between 1 and 10
   time units after the latest arrival time of packets
   currently in the medium on their way to the destination */
 lastime = time;
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next) */
 for (q=evlist; q!=NULL ; q = q->next) 
    if ( (q->evtype==FROM_LAYER3  && q->eventity==evptr->eventity) ) 
      lastime = q->evtime;
 evptr->evtime =  lastime + 1 + 9*jimsrand();
 


 /* simulate corruption: */
 if (jimsrand() < corruptprob)  {
    ncorrupt++;
    if ( (x = jimsrand()) < .75)
       mypktptr->payload[0]='Z';   /* corrupt payload */
      else if (x < .875)
       mypktptr->seqnum = 999999;
      else
       mypktptr->acknum = 999999;
    if (TRACE>0)    
  printf("          TOLAYER3: packet being corrupted\n");
    }  

  if (TRACE>2)  
     printf("          TOLAYER3: scheduling arrival on other side\n");
  insertevent(evptr);
} 

void tolayer5(int AorB, char datasent[20])
{
  int i;  
  if (TRACE>2) {
     printf("          TOLAYER5: data received: ");
     for (i=0; i<20; i++)  
        printf("%c",datasent[i]);
     printf("\n");
   }
  
}