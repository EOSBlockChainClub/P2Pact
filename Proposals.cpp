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
    * 
    * Contributor struct: Multi index table to store contributors
    *      prim_key(uint64): Primary key
    *      contributor(contributor/uint64): Account name and address of contributor
    *      contributionTotal(uint64): The total contribution of user
    * 
    * Proof struct: Multi index table to store proofs
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
    *       checkVOtingOpen => Initiates voting phase for smart contract
    *       checkVotingThreshold => Determines whether total votes cast is majority
    *       vote => Check if contributor and if so allow a vote
    *       distributeFunds => once vote is complete, distribute funds accordingly
    * 
    *  Phase 3:
    *      addProofHash => Add a proof hash to the associated proposal
    */

   class Proposals: public contract{
        using contract::contract;

         private:

            //@abi table
            struct contributor {
                uint64_t prim_key;
                account_name user;
                uint64_t totalContribution;
                //Create primary key
                auto primary_key() const {return prim_key;}
                //Create secondary key on contributor
                account_name get_by_user() const {return user;}
            };

            typedef multi_index< N(contributor), contributor,
                indexed_by< N(getbyuser), const_mem_fun<contributor, account_name, 
                &contributor::get_by_user> >> contributorTable;

            //@abi table proposal i64
            struct proposal {
                uint64_t account_name;
                string proposalName;
                string proposalDescription;
                uint64_t threshold;
                uint64_t totalPledged;
                vector<checksum256> proofHashes;
                vector<string> proofNames;
                vector<contributor> donors;
                bool isDone;
                bool isVoteOpen;
                uint32_t votesFor;
                uint32_t votesAgainst;
                vector<uint64_t> haveVoted;
                uint64_t primary_key() const { return account_name; }

                EOSLIB_SERIALIZE(proposal, (account_name)(proposalName)(proposalDescription)(threshold)(totalPledged)(proofHashes)(proofNames)(donors)(isDone)(isVoteOpen)(votesFor)(votesAgainst)(haveVoted))
            };

            typedef multi_index<N(proposal), proposal> proposalIndex;

            bool isNewContributor(account_name user) {
                contributorTable contributorObj(_self, _self);
                // get contributors by secondary key
                auto contributions = contributorObj.get_index<N(getbyuser)>();
                auto contribution = contributions.find(user);

                return contribution == contributions.end();
            }

        public:
            Proposals(account_name self):contract(self) {}

            //@abi action
            void add(const account_name account, string& proposalName, string& proposalDescription,
                        uint64_t threshold) {                
                require_auth(account); //Ensure only an owned account can submit a proposal
                proposalIndex proposals(_self, _self); //Create table and pass scope
                auto iterator = proposals.find(account);
                eosio_assert(iterator == proposals.end(), "Proposer has already created a proposal");

                proposals.emplace(account, [&](auto& proposal){
                    proposal.account_name = account;
                    proposal.proposalName = proposalName;
                    proposal.proposalDescription = proposalDescription;
                    proposal.threshold = threshold;
                    proposal.totalPledged = 0;
                    proposal.proofHashes = vector<checksum256>();
                    proposal.proofNames = vector<string>();
                    proposal.donors = vector<contributor>();
                    proposal.isDone = false;
                    proposal.isVoteOpen = false;
                    proposal.votesFor = 0;
                    proposal.votesAgainst = 0;
                    proposal.haveVoted = vector<uint64_t>();
                });

            }
            //@abi action
            void markdone(account_name account) {

                proposalIndex proposals(_self, _self); //Create table and pass scope
                auto iterator = proposals.find(account);

                proposals.modify(iterator, _self, [&](auto& proposal) {
                    proposal.isDone = true;
                    proposal.isVoteOpen = true;
                });

            }

            //@abi action
            void addcontrib(account_name account, account_name contributor, uint64_t amount) {
                proposal currProp = getProposal(account);
                print("Account name will follow");
                print(account);
                print("Contributor will follow");
                print(contributor);
                print("Amount will follow");
                print(amount);
                if (checkThreshold(currProp, amount) == false) {
                    print("Inside update totalPledged");
                    modify(account, amount);
                    update(contributor, amount, currProp);
                } else {
                    transfer(account, account, currProp.threshold/10);
                }
            }

            void modify(account_name proposal_account, uint64_t amount) {
                proposalIndex proposals(_self, _self); //Create table and pass scope
                auto iterator = proposals.find(proposal_account);

                proposals.modify(iterator, _self, [&](auto& proposal) {
                    proposal.totalPledged += amount;
                });

            }

            bool checkThreshold(proposal currProp, uint64_t amount) {
                print("Total pledged will follow");
                print(currProp.totalPledged);
                print("Amount will follow");
                print(amount);
                if((currProp.totalPledged) > currProp.threshold) {

                    return true;
                } else return false;

            }


            proposal getProposal(account_name account) {
                proposalIndex proposals(_self, _self);
                auto iterator = proposals.find(account);
                eosio_assert(iterator != proposals.end(), "Address for account not found");
                auto currProp = proposals.get(account);
                return currProp;
            }

            //@abi action
            void addproofhash(checksum256 proofHash, string& proofName, account_name account) {

                proposalIndex proposals(_self, _self); //Create table and pass scope
                auto iterator = proposals.find(account);

                proposals.modify(iterator, _self, [&](auto& proposal) {
                    proposal.proofHashes.push_back(proofHash);
                    proposal.proofNames.push_back(proofName);
                });
            }

            //@abi action
            void update(account_name _user, uint64_t _deposit, proposal currProp) {
                require_auth(_user);

                contributorTable obj(_self, _self);

                //Create or add total deposit depending if contributor has already donated
                if(isNewContributor(_user)) {
                    //Insert object

                    obj.emplace(_self,[&](auto& address) {
                        address.prim_key = obj.available_primary_key();
                        address.user = _user;
                        address.totalContribution = _deposit;
                    });

                    auto contributions = obj.get_index<N(getbyuser)>();
                    auto &contribution = contributions.get(_user);
                    
                    proposalIndex proposals(_self, _self); //Create table and pass scope
                    auto iterator = proposals.find(currProp.account_name);

                    proposals.modify(iterator, _self, [&](auto& proposal) {
                        proposal.donors.push_back(contribution);
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

            void transfer(account_name from, account_name to, asset quantity, string memo) {
                // need to get the proposal
                account_name accountName;
                accountName = string_to_name(memo.c_str());
                addcontrib(accountName, from, quantity.amount/10000); // for some reason amount likes to be weird
            }

            void sendFunds(account_name from, account_name to, asset quantity){
                SEND_INLINE_ACTION( *this, transfer, {st.issuer,N(active)}, {from, to, quantity} );
            }
       
            //@abi action
            void vote(account_name account, account_name voter, string choice) {
                proposal currProposal = getProposal(account);
                if(currProposal.isVoteOpen) {
                    vector<contributor>::iterator it;
                    for(it = currProposal.donors.begin(); it != currProposal.donors.end(); it++) {
                        if (it->user == voter) {
                            proposalIndex proposals(_self, _self); //Create table and pass scope
                            auto iterator = proposals.find(account);

                            if (choice.compare("for") == 0) {

                                proposals.modify(iterator, _self, [&](auto& proposal) {
                                    proposal.votesFor += 1;
                                    proposal.haveVoted.push_back(voter);
                                });
                            } else {
                                proposals.modify(iterator, _self, [&](auto& proposal) {
                                    proposal.votesAgainst += 1;
                                    proposal.haveVoted.push_back(voter);
                                });
                            }

                            if (currProposal.haveVoted.size() > (currProposal.donors.size()/2)) {
                                proposals.modify(iterator, _self, [&](auto& proposal) {
                                    proposal.isVoteOpen = false;
                                });
                            }
                        }
                    }
                }
            }
        
            
   };

    //EOSIO_ABI(Proposals, (add)(addcontrib)(addproofhash)(update))



   #define EOSIO_ABI_EX( TYPE, MEMBERS ) \
    extern "C" { \
        void apply( uint64_t receiver, uint64_t code, uint64_t action ) { \
            if( action == N(onerror)) { \
                /* onerror is only valid if it is for the "eosio" code account and authorized by "eosio"'s "active permission */ \
                eosio_assert(code == N(eosio), "onerror action's are only valid from the \"eosio\" system account"); \
            } \
            auto self = receiver; \
            if( code == self || code == N(eosio.token) || action == N(onerror) ) { \
                TYPE thiscontract( self ); \
                switch( action ) { \
                    EOSIO_API( TYPE, MEMBERS ) \
                } \
                /* does not allow destructor of thiscontract to run: eosio_exit(0); */ \
            } \
        } \
    }

    EOSIO_ABI_EX(Proposals,
    // Proposal core
    (add)
    (markdone)
    (addcontrib)
    (addproofhash)
    (update)
    (vote)
    // tokens deposits
    (transfer)
    )


}
