#include <Proposals.hpp>

namespace P2Pact {
    using namespace eosio;
    using std::string;

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
    */

   class Proposals: public contract{
        using contract::contract;

        public:
            Proposals(account_name self):contract(self) {}

            //@abi action
            void add(const account_name account, string& proposalName, string& proposalDescription,
                        uint64_t threshold) {

            }

            //@abi action
            void deposit(uint64_t amount){

            }

            //@abi action
            void addContributor(const account_name account) {

            }

            //@abi action
            bool checkThreshold(uint64_t deposit) {

            }

            //@abi action
            void getProposal(const account_name account) {

            }

        private:

            //@abi table proposal i64
            struct proposal {
                uint64_t account_name;
                string proposalName;
                string proposalDescription;
                uint64_t threshold;

                uint64_t primary_key() const { return username; }

                EOSLIB_SERIALIZE(proposal, (account_name)(proposalName)(proposalDescription)(threshold))
            };
        
            
   }
}
