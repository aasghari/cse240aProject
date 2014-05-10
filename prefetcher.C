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

Prefetcher::Prefetcher():_nextReq(),lastReq(),lastHit(),_ready()
{

}

u_int32_t Prefetcher::blockStartAddr(u_int32_t addr)
{
    return addr/16*16;
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
    return _nextReq;
}

// this function is called whenever the last prefetcher request was successfully sent to the L2
void Prefetcher::completeRequest(u_int32_t cycle)
{
    _ready = false;
    lastHit++;
    this->lastReq=this->_nextReq;
}

/*
 * This function is called whenever the CPU references memory.
 * Note that only the addr, pc, load, issuedAt, and HitL1 should be considered valid data
 */
void Prefetcher::cpuRequest(Request req)
{
    if(_ready)
	return;
    u_int32_t reqAddrBlock= blockStartAddr(req.addr);
    u_int32_t lastReqAddrBlock=blockStartAddr(this->lastReq.addr);
    if(!req.HitL1)
    {
	_nextReq.addr = reqAddrBlock + 16;
	lastHit=0;
	_ready = true;
    }
    else if(lastHit<7)
    {
	_nextReq.addr=lastReqAddrBlock+16;
	_ready = true;
    }



}
