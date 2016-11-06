#include "project3.h"

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

simulator *simulation;
Aside Aside;
Bside Bside;
char message_buffer[MESSAGE_SIZE * BUFFER_SIZE];
int buffer_index[BUFFER_SIZE];

//finds the next entry in buffer_index with the desired value
int findNextIndex(int desired){
  for(int i = 0; i < BUFFER_SIZE; i++){
    if(buffer_index[i] == desired){
      return i;
    }
  }
  return -1;
}

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
  std::cout << "A side has recieved a message from the application that should be sent to side B: " << message.data << std::endl;

    //store message into message_buffer and the sequence number of that message into the corresponding spot in message_index
    int offset = 0;
    if((offset = findNextIndex(-1)) == -1){
      std::cout << "Buffer is too small, there is no more room" << std::endl;
      exit(1);
    }
    //store message into buffer
    snprintf(&message_buffer[offset * MESSAGE_SIZE], 20, "%s", message.data);
    //store offset index into buffer_index
    buffer_index[offset] = Aside.nextseqnum;
    Aside.nextseqnum += MESSAGE_SIZE;

    A_output();
}

//used to send packets from the buffer to ensure that they are sent after an ack is received
void A_output(){
  int offset = findNextIndex(Aside.seqnum);
  if( offset == -1){
    return;
  }
  pkt pac;
  pac.seqnum = Aside.seqnum;
  pac.acknum = Aside.acknum;
  pac.checksum = 0;
    snprintf(pac.payload, 20, "%s", &message_buffer[offset * MESSAGE_SIZE]);

  simulation->tolayer3(A, pac);
}

void B_output(struct msg message)  /* need be completed only for extra credit */
{
  std::cout << "B side has recieved a message from the application that should be sent to sideA: " << message.data << std::endl;

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
  std::cout << "A side has recieved a packet sent over the network from side B:" << packet.payload << std::endl;

  if(packet.acknum == Aside.seqnum + MESSAGE_SIZE){
    // the correct packet was acked
    std::cout << "Correct Ack Received " << Aside.seqnum << std::endl;
    Aside.seqnum += MESSAGE_SIZE;
  }
  else{
    // the packet needs to be sent again
    std::cout << "Packet needs to be resent" << std::endl;
  }
  A_output();
  
}


/* called when A's timer goes off */
void A_timerinterrupt()
{

  std::cout << "Side A's timer has gone off " << std::endl;
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  Aside.seqnum = 0;
  Aside.acknum = 0;
  Aside.nextseqnum = 0;
}


/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
    std::cout << "B side has recieved a packet sent over the network from side A:" << packet.payload << std::endl;

    if(packet.seqnum != Bside.acknum){
      //this is not the packet I was looking for
      std::cout << "Not correct packet " << Bside.acknum << std::endl;
    }
    else{
      std::cout << "Correct sequence Received " << Bside.acknum << " " << packet.seqnum << std::endl;
      simulation->tolayer5(B, packet.payload);
      Bside.acknum += MESSAGE_SIZE;
    }

  pkt ack;
  ack.seqnum = Bside.seqnum;
  ack.acknum = Bside.acknum;
  ack.checksum = 0;
  snprintf(ack.payload, 20, "%s", "heck yeah bro this 1");

  simulation->tolayer3(B, ack);
    
}

/* called when B's timer goes off */
void B_timerinterrupt()
{
    std::cout << "Side B's timer has gone off " << std::endl;

}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
  Bside.seqnum = 0;
  Bside.acknum = 0;
}

int main(int argc, char **argv) {

  simulation = new simulator;
  for(int i = 0; i < BUFFER_SIZE; i++){
    buffer_index[i] = -1;
  }
  

  A_init();
  B_init();
  simulation->go();
}
