import React, { Component } from 'react';
import Eos from 'eosjs'; // https://github.com/EOSIO/eosjs

// material-ui dependencies
import { withStyles } from '@material-ui/core/styles';
import AppBar from '@material-ui/core/AppBar';
import Toolbar from '@material-ui/core/Toolbar';
import Typography from '@material-ui/core/Typography';
import Card from '@material-ui/core/Card';
import CardContent from '@material-ui/core/CardContent';
import TextField from '@material-ui/core/TextField';
import Paper from '@material-ui/core/Paper';
import Button from '@material-ui/core/Button';
import Grid from '@material-ui/core/Grid';
import Select from '@material-ui/core/Select';
import MenuItem from '@material-ui/core/MenuItem';
import InputLabel from '@material-ui/core/InputLabel';
import FormControl from '@material-ui/core/FormControl';

// NEVER store private keys in any source code in your real life development
// This is for demo purposes only!
const accounts = [
  {"name":"proposer", "privateKey":"5K7mtrinTFrVTduSxizUc5hjXJEtTjVTsqSHeBHes1Viep86FP5", "publicKey":"EOS6kYgMTCh1iqpq9XGNQbEi8Q6k5GujefN9DSs55dcjVyFAq7B6b"},
  {"name":"donorone", "privateKey":"5KLqT1UFxVnKRWkjvhFur4sECrPhciuUqsYRihc1p9rxhXQMZBg", "publicKey":"EOS78RuuHNgtmDv9jwAzhxZ9LmC6F295snyQ9eUDQ5YtVHJ1udE6p"},
  {"name":"donortwo", "privateKey":"5K2jun7wohStgiCDSDYjk3eteRH1KaxUQsZTEmTGPH4GS9vVFb7", "publicKey":"EOS5yd9aufDv7MqMquGcQdD6Bfmv6umqSuh9ru3kheDBqbi6vtJ58"},
  {"name":"donorthree", "privateKey":"5KNm1BgaopP9n5NqJDo9rbr49zJFWJTMJheLoLM5b7gjdhqAwCx", "publicKey":"EOS8LoJJUU3dhiFyJ5HmsMiAuNLGc6HMkxF4Etx6pxLRG7FU89x6X"},
  {"name":"donorfour", "privateKey":"5KE2UNPCZX5QepKcLpLXVCLdAw7dBfJFJnuCHhXUf61hPRMtUZg", "publicKey":"EOS7XPiPuL3jbgpfS3FFmjtXK62Th9n2WZdvJb6XLygAghfx1W7Nb"},
  {"name":"donorfive", "privateKey":"5KaqYiQzKsXXXxVvrG8Q3ECZdQAj2hNcvCgGEubRvvq7CU3LySK", "publicKey":"EOS5btzHW33f9zbhkwjJTYsoyRzXUNstx1Da9X2nTzk8BQztxoP3H"},
  {"name":"donorsix", "privateKey":"5KFyaxQW8L6uXFB6wSgC44EsAbzC7ideyhhQ68tiYfdKQp69xKo", "publicKey":"EOS8Du668rSVDE3KkmhwKkmAyxdBd73B51FKE7SjkKe5YERBULMrw"}
];

// set up styling classes using material-ui "withStyles"
const styles = theme => ({
  card: {
    margin: 20,
  },
  paper: {
    ...theme.mixins.gutters(),
    marginTop: theme.spacing.unit,
    paddingTop: theme.spacing.unit * 2,
    paddingBottom: theme.spacing.unit * 2,
  },
  formButton: {
    marginTop: theme.spacing.unit,
    width: "100%",
  },
  pre: {
    background: "#ccc",
    padding: 10,
    marginBottom: 0.
  },
  formControl: {
    width: "100%",
  },
});

// Index component
class Index extends Component {

  constructor(props) {
    super(props)
    this.state = {
      vote: '',
      proposal: '',
      balance: '',
      accountName: '',
      proposalName: '',
      proposalTable: [] // to store the table rows from smart contract
    };
    this.handleProposalFormEvent = this.handleProposalFormEvent.bind(this);
    this.handleBalanceFormEvent = this.handleBalanceFormEvent.bind(this);
    this.handleVerificationFormEvent = this.handleVerificationFormEvent.bind(this);
    this.handleCompletionFormEvent = this.handleCompletionFormEvent.bind(this);
    this.handleVoteFormEvent = this.handleVoteFormEvent.bind(this);
    this.handleContributionFormEvent = this.handleContributionFormEvent.bind(this);
  }

  async handleContributionFormEvent(event) {
    event.preventDefault();
    const account = event.target.account.value;
    const privateKey = event.target.privateKey.value;
    const proposal = event.target.proposal.value;
    const pledgeAmount = event.target.pledgeAmount.value;

    if (!account || !privateKey || !proposal || !pledgeAmount) {
      alert('Please complete all the fields on the form');
      return false;
    }

    const eos = Eos({keyProvider: privateKey});
    let response = await eos.transaction({
      actions: [{
        account: "eosio.token",
        name: "transfer",
        authorization: [{
          actor: account,
          permission: 'active',
        }],
        data: {
          from: account,
          to: "p2pactacc",
          quantity: pledgeAmount.concat(".0000 TOK"),
          memo: proposal,
        },
      }],
    });

    console.log(response);
  }

  async handleVoteFormEvent(event) {
    event.preventDefault();
    const account = event.target.account.value;
    const privateKey = event.target.privateKey.value;
    const vote = this.state.vote;
    const proposal = event.target.proposal.value;

    if (!account || !privateKey || !vote || !proposal) {
      alert('Please complete all the fields on the form');
      return false;
    }

    const eos = Eos({keyProvider: privateKey});
    let response = await eos.transaction({
      actions: [{
        account: "p2pactacc",
        name: "vote",
        authorization: [{
          actor: account,
          permission: 'active',
        }],
        data: {
          account: proposal, // the account name of the proposal
          voter: account,
          choice: vote,
        },
      }],
    });

    console.log(response);
  }

  async handleCompletionFormEvent(event) {
    event.preventDefault();
    const account = event.target.account.value;
    const privateKey = event.target.privateKey.value;

    if (!account || !privateKey) {
      alert('Please complete all the fields on the form');
      return false;
    }

    const eos = Eos({keyProvider: privateKey});
    let response = await eos.transaction({
      actions: [{
        account: "p2pactacc",
        name: "markdone",
        authorization: [{
          actor: account,
          permission: 'active',
        }],
        data: {
          account
        },
      }],
    });

    console.log(response);
  }

  async handleVerificationFormEvent(event) {
    event.preventDefault();
    const proposal = event.target.proposal.value;
    const hash = event.target.hash.value;
    const account = event.target.account.value; // transaction sending account
    const privateKey = event.target.privateKey.value;
    const verificationDescription = event.target.verificationDescription.value;

    if (!proposal || !hash || !account || !privateKey || !verificationDescription) {
      alert('Please complete all the fields on the form');
      return false;
    }

    const eos = Eos({keyProvider: privateKey});
    let response = await eos.transaction({
      actions: [{
        account: "p2pactacc",
        name: "addproofhash",
        authorization: [{
          actor: account,
          permission: 'active',
        }],
        data: {
          proofHash: hash,
          proofName: verificationDescription,
          account: proposal, // this is the accountName of the proposal
        },
      }],
    });

    console.log(response);
  }

  async handleBalanceFormEvent(event) {
    event.preventDefault();

    const account = event.target.account.value;

    if (!account) {
      alert('Please complete all the fields on the form');
      return false;
    }

    const eos = Eos();
    let balance = await eos.getCurrencyBalance("eosio.token", account, "TOK");

    if (balance.length === 0) {
      balance = -1;
    }

    this.setState({ balance });
  }

  // generic function to handle form events (e.g. "submit" / "reset")
  // push transactions to the blockchain by using eosjs
  async handleProposalFormEvent(event) {
    // stop default behaviour
    event.preventDefault();

    // collect form data
    let account = event.target.account.value;
    let privateKey = event.target.privateKey.value;
    let proposalName = event.target.proposalName.value;
    let proposalDescription = event.target.proposalDescription.value;
    let threshold = event.target.fundingAmount.value;

    if (!account || !privateKey || !proposalName || !proposalDescription || !threshold) {
      alert('Please complete all the fields on the form');
      return false;
    }

    // eosjs function call: connect to the blockchain
    const eos = Eos({keyProvider: privateKey});
    const result = await eos.transaction({
      actions: [{
        account: "p2pactacc",
        name: "add",
        authorization: [{
          actor: account,
          permission: 'active',
        }],
        data: {
          account,
          proposalName,
          proposalDescription,
          threshold,
        },
      }],
    });

    console.log(result);
  }

  // gets table data from the blockchain
  // and saves it into the component state: "proposalTable"
  getTable() {
    const eos = Eos();
    eos.getTableRows({
      "json": true,
      "code": "p2pactacc",   // contract who owns the table
      "scope": "p2pactacc",  // scope of the table
      "table": "proposal",    // name of the table as specified by the contract abi
      "limit": 100,
    }).then(result => this.setState({ proposalTable: result.rows }));
  }

  componentDidMount() {
    this.getTable();
  }

  handleChange = event => {
    this.setState({ [event.target.name]: event.target.value });
  };

  render() {
    const { proposalTable } = this.state;
    const { classes } = this.props;

    console.log(this.state.proposalTable);

    // generate each note as a card
    const generateCard = (key, account_name, proposalName, proposalDescription, threshold, totalPledged,
    proofHashes, proofNames, donors, isDone, isVoteOpen, votesFor, votesAgainst, haveVoted) => (
      <Card className={classes.card} key={key}>
        <CardContent>
          <Typography variant="headline" component="h2">
            <strong>Account name:</strong> {account_name}
          </Typography>
          <Typography variant="subheading">
            Proposal name: {proposalName}
          </Typography>
          <Typography variant="subheading">
            Proposal description: {proposalDescription}
          </Typography>
          <Typography variant="subheading">
            Threshold: {threshold}
          </Typography>
          <Typography variant="subheading">
            Total pledged: {totalPledged}
          </Typography>
          <Typography variant="subheading">
            Proof: 
          </Typography>
          <Typography variant="subheading">
            {proofHashes.length > 0 ?proofHashes.map((hash, i) => 
              <Typography variant="subheading"><em>{proofNames[i]} - {hash}</em></Typography>): <em>None</em>}
          </Typography>
          <Typography variant="subheading">
            Donors:
          </Typography>
          <Typography variant="subheading">
            {donors.length > 0 ? donors.map((donor) => 
              <Typography variant="subheading"><em>{donor.user} - {donor.totalContribution}</em></Typography>)
              : <em>None</em>}
          </Typography>
          <Typography variant="subheading">
            Is done: {isDone}
          </Typography>
          <Typography variant="subheading">
            Is voting open: {isVoteOpen}
          </Typography>
          <Typography variant="subheading">
            Votes for: {votesFor}
          </Typography>
          <Typography variant="subheading">
            Votes against: {votesAgainst}
          </Typography>
          <Typography variant="subheading">
            Accounts that have voted: 
          </Typography>
          <Typography variant="subheading">
            {haveVoted.length > 0 ? haveVoted.map((account) =>
              <Typography variant="subheading"><em>{account}</em></Typography>): <em>None</em>}
          </Typography>
        </CardContent>
      </Card>
    );

    const generateProposals = () => {
      return(
      <FormControl className={classes.formControl} margin="normal">
        <InputLabel htmlFor="proposals">Proposals</InputLabel>
        <Select
          value={this.state.accountName}
          onChange={this.handleChange}
          name="proposals"
          fullWidth
        >
          <MenuItem value="">
            <em>None</em>
          </MenuItem>
          {proposalTable.map((row) => {
            <MenuItem value={row.account_name}>{console.log(row.proposalName)}{row.proposalName}</MenuItem>
          })}
        </Select>
      </FormControl>
      );
    }

    let noteCards = proposalTable.map((row, i) =>
      generateCard(i, row.account_name, row.proposalName, row.proposalDescription, row.threshold, row.totalPledged,
      row.proofHashes, row.proofNames, row.donors, row.isDone, row.isVoteOpen, row.votesFor, row.votesAgainst, row.haveVoted));

    return (
      <div>
        <AppBar position="static" color="default">
          <Toolbar>
            <Typography variant="title" color="inherit">
              P2Pact
            </Typography>
          </Toolbar>
        </AppBar>
        {noteCards}
        <Grid container spacing={24}>
          <Grid item xs={6}>
            <Paper className={classes.paper}>
              <Typography variant="headline" component="h3">
                Proposal generation
              </Typography>
              <form onSubmit={this.handleProposalFormEvent}>
                <TextField
                  name="account"
                  autoComplete="off"
                  label="Account name"
                  margin="normal"
                  fullWidth
                />
                <TextField
                  name="privateKey"
                  autoComplete="off"
                  label="Private key"
                  margin="normal"
                  fullWidth
                />
                <TextField
                  name="proposalName"
                  autoComplete="off"
                  label="Proposal Name"
                  margin="normal"
                  fullWidth
                />
                <TextField
                  name="proposalDescription"
                  autoComplete="off"
                  label="Proposal Description"
                  margin="normal"
                  fullWidth
                />
                <TextField
                  name="fundingAmount"
                  autoComplete="off"
                  label="Amount required to fund proposal"
                  type="number"
                  margin="normal"
                  fullWidth
                />
                <Button
                  variant="contained"
                  color="primary"
                  className={classes.formButton}
                  type="submit">
                  Submit proposal
                </Button>
              </form>
            </Paper>
            <Paper className={classes.paper}>
              <Typography variant="headline" component="h3">
                Account balance
              </Typography>
              <form onSubmit={this.handleBalanceFormEvent}>
                <TextField
                  name="account"
                  autoComplete="off"
                  label="Account name"
                  margin="normal"
                  fullWidth
                />
                {this.state.balance && <Typography variant="subheading">
                  {this.state.balance !== -1 ? `Account balance is ${this.state.balance}` :
                  `Account does not exist!`}
                </Typography>}
                <Button
                  variant="contained"
                  color="primary"
                  className={classes.formButton}
                  type="submit">
                  Get account balance
                </Button>
              </form>
            </Paper>
            <Paper className={classes.paper}>
              <Typography variant="headline" component="h3">
                Completion of proposal task
              </Typography>
              <form onSubmit={this.handleCompletionFormEvent}>
                <TextField
                  name="account"
                  autoComplete="off"
                  label="Account name"
                  margin="normal"
                  fullWidth
                />
                <TextField
                  name="privateKey"
                  autoComplete="off"
                  label="Private key"
                  margin="normal"
                  fullWidth
                />
                <Button
                  variant="contained"
                  color="primary"
                  className={classes.formButton}
                  type="submit">
                  Announce proposal completion
                </Button>
              </form>
            </Paper>
          </Grid>
          <Grid item xs={6}>
            <Paper className={classes.paper}>
              <Typography variant="headline" component="h3">
                Contribute to proposal
              </Typography>
              <form onSubmit={this.handleContributionFormEvent}>
                <TextField
                  name="account"
                  autoComplete="off"
                  label="Account name"
                  margin="normal"
                  fullWidth
                />
                <TextField
                  name="privateKey"
                  autoComplete="off"
                  label="Private key"
                  margin="normal"
                  fullWidth
                />
                <TextField
                  name="proposal"
                  autoComplete="off"
                  label="Proposal account name"
                  margin="normal"
                  fullWidth
                />
                <TextField
                  name="pledgeAmount"
                  autoComplete="off"
                  label="Amount to pledge"
                  margin="normal"
                  fullWidth
                />
                <Button
                  variant="contained"
                  color="primary"
                  className={classes.formButton}
                  type="submit">
                  Contribute to proposal
                </Button>
              </form>
            </Paper>
            <Paper className={classes.paper}>
              <Typography variant="headline" component="h3">
                Verification hash
              </Typography>
              <form onSubmit={this.handleVerificationFormEvent}>
                <TextField
                  name="account"
                  autoComplete="off"
                  label="Account name"
                  margin="normal"
                  fullWidth
                />
                <TextField
                  name="privateKey"
                  autoComplete="off"
                  label="Private key"
                  margin="normal"
                  fullWidth
                />
                <TextField
                  name="proposal"
                  autoComplete="off"
                  label="Proposal"
                  margin="normal"
                  fullWidth
                />
                <TextField
                  name="verificationDescription"
                  autoComplete="off"
                  label="Description of verification"
                  margin="normal"
                  fullWidth
                />
                <TextField
                  name="hash"
                  autoComplete="off"
                  label="Hash of verification file"
                  margin="normal"
                  fullWidth
                />
                <Button
                  variant="contained"
                  color="primary"
                  className={classes.formButton}
                  type="submit">
                  Submit verification hash
                </Button>
              </form>
            </Paper>
            <Paper className={classes.paper}>
              <Typography variant="headline" component="h3">
                Vote on proposal completion
              </Typography>
              <form onSubmit={this.handleVoteFormEvent}>
                <TextField
                  name="account"
                  autoComplete="off"
                  label="Account name"
                  margin="normal"
                  fullWidth
                />
                <TextField
                  name="privateKey"
                  autoComplete="off"
                  label="Private key"
                  margin="normal"
                  fullWidth
                />
                <TextField
                  name="proposal"
                  autoComplete="off"
                  label="Proposal"
                  margin="normal"
                  fullWidth
                />
                <FormControl className={classes.formControl} margin="normal">
                  <InputLabel htmlFor="vote">Vote</InputLabel>
                  <Select
                    value={this.state.vote}
                    onChange={this.handleChange}
                    name="vote"
                    fullWidth
                  >
                    <MenuItem value="">
                      <em>None</em>
                    </MenuItem>
                    <MenuItem value="for">Job completed</MenuItem>
                    <MenuItem value="against">Job failed</MenuItem>
                  </Select>
                </FormControl>
                <Button
                  variant="contained"
                  color="primary"
                  className={classes.formButton}
                  type="submit">
                  Vote
                </Button>
              </form>
            </Paper>
          </Grid>
        </Grid>
        {<pre className={classes.pre}>
          Below is a list of pre-created accounts information for add/update note:
          <br/><br/>
          accounts = { JSON.stringify(accounts, null, 2) }
        </pre>}
      </div>
    );
  }

}

export default withStyles(styles)(Index);
