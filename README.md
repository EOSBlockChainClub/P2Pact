# P2Pact

An open-source micro-crowdfunding platform aimed at allowing people to raise money in order to perform charitable activities

# Smart Contract Name: P2Pact
  * Purpose: Allow proposals be linked to fund raises from the crowd

  * Proposal struct: Multi index table to store the proposals
    *      prim_key(uint64): Primary key
    *      proposer(proposer/uint64): Account name and address of proposer
    *      proposal(string): The proposal
    *      threshold(uint64): The amount to be raised
    *      totalPledged(uint64): The amount pledged by others
    *      proofHashes(checksum256): Hashed proof used for validation
    *      proofNames(string): Human readable names for each hash
    *      donors(contributor): The contributors who have donated to this proposal
    *      isDone(boolean): Where the proposer indicates they have completed the task
    *      isVoteOpen(boolean): Check to see if voting phase has begun
    *      votesFor(uint32_t): Number of votes in favour of tasks being completed
    *      votesAgainst(uint): Number of votes against the tasks being complete
    *      haveVoted: Number of votes submitted in total
  * Contributor struct: Multi index table to store contributors
    *      prim_key(uint64): Primary key
    *      contributor(contributor/uint64): Account name and address of contributor
    *      contributionTotal(uint64): The total contribution of user
    
# Public methods:
  * Phase 1:      
    *      add => Create a new proposal
    *      isnewuser => Check if the given account name has already contributed
    *      checkThreshold => Check if the threshold has been reached
    *      deposit => Record the deposit of tokens against the threshold
    *      addContributor => Add contributor to proposal
    *      markDone => Indicate that this is done
    *      modify => stateful modification of proposal state
    *      getProposal => get Proposal from address
          
  * Phase 2:
    *       checkVOtingOpen => Initiates voting phase for smart contract
    *       checkVotingThreshold => Determines whether total votes cast is majority
    *       vote => Check if contributor and if so allow a vote
    
  * Phase 3:
    *      addProofHash => Add a proof hash to the associated proposal
    *      addProofName => Name the hash associated


