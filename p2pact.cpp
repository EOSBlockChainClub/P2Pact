#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
using namespace eosio;

/* Smart Contract Name: P2Pact
 * Purpose: Allow proposals be linked to fund raises from the crowd
 * 
 * Proposal struct: Multi index table to store the proposals
 *      prim_key(uint64): Primary key
 *      proposer(proposer/uint64): Account name and address of proposer
 *      proposal(string): The proposal
 *      timestamp(uint64): The time proposal was uploaded
 *      threshold(uint64): The amount to be raised
 *      contributors(struct contributor): The contributors to the proposal
 *      proofs(struct proof): Proofs for completion of proposal
 *      
 * 
 * Contributor struct: Multi index table to store contributors
 *      prim_key(uint64): Primary key
 *      contributor(contributor/uint64): Account name and address of contributor
 *      contributionTotal(uint64): The total contribution of user
 * 
 * Proof struct: Multi index table to store proofs
 *      prim_key(uint64): Primary key
 *      proofName(string): Name of proof
 *      proofHash(Checksum256): Hash of proof
 * 
 * Public methods:
 *  Phase 1:      
 *      isnewuser => Check if the given account name has already contributed
 *      checkThreshold => Check if the threshold has been reached
 *      deposit => Record the deposit of tokens against the threshold
 *      addContributor => Add contributor to proposal
 *          
 *  Phase 2:
 *            