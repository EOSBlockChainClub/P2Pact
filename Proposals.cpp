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

   class Proofs {
        public:

        Proofs(){
            proofs.proofHashes = new vector<checksum256>();
            proofs.proofNames = new vector<string>();
        }

        void addProof(checksum256 proofHash, string proofName, proofs proposalProofs ) {
            proposalProofs.proofHashes.insert(proofHash);
            prosposalProofs.proofNames.insert(proofName);
        }

        private:

        struct proofs {
            vector<checksum256> proofHashes;
            vector<string> proofNames
        }
   };

   class Contributors {
        public:
            //@abi action 
            Contributors(){}

            void update(account_name _self, uint64_t _deposit) {
                require_auth(_user);

                contributorTable obj(_self, _self);

                //Create or add total deposit depending if contributor has already donated
                if(isNewUser(_user)) {
                    //Insert object
                    obj.emplace(_self,[&](auto& address) {
                        address.primary_key = obj.available_primary_key();
                        address.user = _user;
                        address.totalContribution = _deposit;
                    });
                } else {
                    //Get object by secondary key
                    auto contributions = obj.get_index<N(getbyuser)>();
                    auto &contribution = contributions.get(_user);
                    //update total contribution
                    obj.modify(contribution, _self, [&](auto& address) {
                        address.totalContribution += _deposit;
                    });
                }
            }

        private:
            //Check if the contributor already exists, and if so return them
            bool isNewUser(account_name user) {
                contributorTable contributorObj(_self, _self);
                // get contributors by secondary key
                auto contributions = contributorObj.get_index<N(getbyuser)>();
                auto contribution = contributors.find(user);

                return contributor == contributors.end();
            }

            //@abi table
            struct contributor {
                uint64_t primary_key;
                account_name user;
                uint64_t totalContribution;
                //Create primary key
                auto primary_key() const {return primary_key;}
                //Create secondary key on contributor
                account_name get_by_user() const {return user;}
            };

            typedef multi_index< N(contributor), contributor,
                indexed_by< N(getbyuser), const_mem_fun<contributor, account_name, 
                &contributor::get_by_user> >> contributorTable;


   };


   class Proposals: public contract{
        using contract::contract;

        public:
            Proposals(account_name self):contract(self) {}

            //@abi action
            void add(const account_name account, string& proposalName, string& proposalDescription,
                        uint64_t threshold) {                
                require_auth(account); //Ensure only an owned account can submit a proposal
                proposalIndex proposals(_self, _self); //Create table and pass scope
                proposals.emplace(account, [&](auto& proposal){
                    proposal.account_name = account;
                    proposal.proposalName = proposalName;
                    proposal.proposalDescription = proposalDescription;
                    proposal.threshold = threshold;
                    proposal.totalPledged = 0;
                    proposal.contributors = new Contributors();
                    proposal.proofs = new
                });

            }

            //@abi action
            void deposit(uint64_t amount){

                //TO DO With Token

            }

            //@abi action
            void addContributor(account_name account, account_name contributor, uint64_t amount) {
                proposal currProp = getProposal(account);
                if (checkThreshold(currProp, amount) == false) {
                    currProp.contributors.update(contributor, amount);
                    currProp.totalPledged += amount;
                }
            }

            //@abi action
            bool checkThreshold(proposal currProp uint64_t amount) {
                if(currProp.totalPledged += amount > threshold) {
                    return true;
                } else return false;

            }

            //@abi action
            proposal getProposal(account_name account) {
                proposalIndex proposals(_self, _self);
                auto iterator = proposals.find(account);
                eosio_assert(iterator != proposals.end(), "Address for account not found");
                auto currProp = proposals.get(account);
                return currProp;
            }

            void addProofHash(checksum256 proofHash, string& proofName, account_name account) {
                getProposal(account).proofs.add(proofHash, proofName);
            }

        private:

            //@abi table proposal i64
            struct proposal {
                uint64_t account_name;
                string proposalName;
                string proposalDescription;
                uint64_t threshold;
                uint64_t totalPledged;
                Contributors contributors;
                Proofs proofs;
                uint64_t primary_key() const { return username; }

                EOSLIB_SERIALIZE(proposal, (account_name)(proposalName)(proposalDescription)(threshold))
            };
        
            
   }
}
