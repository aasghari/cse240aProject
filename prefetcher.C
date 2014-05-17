/*
 *
 * File: prefetcher.C
 * Author: Sat Garcia (sat@cs)
 * Description: This simple prefetcher waits until there is a D-cache miss then 
 * requests location (addr + 16), where addr is the address that just missed 
 * in the cache.
 *
 */

#include "prefetcher.h"
#include <stdio.h>
#include <stdlib.h>
#include <climits>

Prefetcher::Prefetcher():
    nextReqAddr(),lastReqAddr(),lastHit(),_ready()
{
}

u_int32_t Prefetcher::blockStartAddr(u_int32_t addr,int size)
{
    return addr/size*size;
}
// should return true if a request is ready for this cycle

bool Prefetcher::hasRequest(u_int32_t cycle)
{
    return _ready;
}

// request a desired address be brought in
/*
 * NOTES: u_int32_t cycle will always be the same as it was in hasRequest
 */

Request Prefetcher::getRequest(u_int32_t cycle)
{
    Request nextReq={};
    nextReq.addr=nextReqAddr;
    return nextReq;
}

// this function is called whenever the last prefetcher request was successfully sent to the L2
//only doesn't get sent if the L2 queue is full!
void Prefetcher::completeRequest(u_int32_t cycle)
{
    _ready = false;
    lastHit++;
    this->lastReqAddr=this->nextReqAddr;
}

/*
 * This function is called whenever the CPU references memory.
 * Note that only the addr, pc, load, fromCPU, issuedAt, and HitL1 should be considered valid data
 */
void Prefetcher::cpuRequest(Request req)
{
    const int L1_STEP_VALUE=16;
    const int L2_STEP_VALUE=32;
    if(_ready || !req.fromCPU)
	return;


    //If the CPU didn't hit in L1 we have mispredicted
    if(!req.HitL1)
    {
	u_int32_t reqAddrBlock= blockStartAddr(req.addr,L1_STEP_VALUE);
	nextReqAddr = reqAddrBlock + L2_STEP_VALUE;
	lastHit=0;
	_ready = true;
    }
    else if(lastHit<10) //all comes down to scaling how far ahead we fetch
    {
	 u_int32_t lastReqAddrBlock=blockStartAddr(this->lastReqAddr,L1_STEP_VALUE);
	nextReqAddr=lastReqAddrBlock+L2_STEP_VALUE;
	_ready = true;
    }



}
