/* An implementation of the Chandy Misra solution to the Dining Philosophers Problem */

// good thing i read the paper , as the solution provided is distributed.
// each process takes a decision based on local state (partial info of H)!
// https://www.cs.rochester.edu/~scott/456/local_only/Philosophers.pdf
// so that means this solution must not have central locks either
//
//
// Precedence graph : u precedes v (i.e u has higher precedence) (i.e H has edge u->v) if
// 1) fork is clean and is with u
// 2) fork is dirty and with v
// 3) fork is in transit from v to u
//
// process with lower precedence must yield to process with higher precedence.
// so in the above example, v must eventually relinquish fork
//
// initial condition: ensure H is acyclic
// E.g. Initializing the system so that philosophers with lower IDs have dirty
// forks ensures the graph is initially acyclic.
//
// drinking philosophers is an extension over dining. drinker's uses
// the precedence graph from H (based on forks) as part of the solution.
// i think they use it to improve local state. As mentioned in the paper
// drinker's cannot infer precedence from just the state of u as it
// needs to know who holds the fork as well (to know who should own bottle
// when a request is made as those are the primary resources)..
// so the goal seems to be to make a situation so that fork's current owner
// is known as well.
//
// the conflict resolution rule says, u sends a bottle to v, in response
// to a request, iff
//    u doesn't need the bottle or [u is not drinking && does not hold fork (u,v)]
//
// so for bottle that is actually shared, the fork is used as a means to
// ensure eventual release of the bottle. To do this, u requests for the fork
// held by v as well as request of the bottle.
// since v releases the fork in finite time, it will evetually
// release the bottle as well
//
// Question: how about distributed locking. As a generalization, is it
// same as implementing this dining philosopher's where each process is
// connected to every other process and running one round?
//
//
// wonder what happens if sending a req token or fork fails
// should more messages be added to ensure state restores to prev state?
// e.g. fork was sent from u -> v but v never got it. v times out
// and sends a DID_NOT_RECEIVE note to u. if u concurrently requests
// for fork, it must observe DID_NOT_RECEIVE first and can take action
// accordingly.. but what about if the DID_NOT_RECEIVE fails too!??
// is a solution possible?
