"""
Wake docs: https://ackee.xyz/wake/docs/latest/
"""
from tests.utils import end_voting
import logging
from wake.testing import *
from pytypes.contracts.D21 import D21 


logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG) # logger.info(f"Swapped {amount}")

@default_chain.connect()
def test_time():
    owner, acc1 = default_chain.accounts[0:2]
    contract = D21.deploy(from_=owner)
    contract.addVoter(acc1, from_=owner)
    contract.addVoter(owner, from_=owner)
    contract.addSubject("Gamer1", from_=owner)
    
    # Can not get results because voting did not start
    with must_revert(D21.VotingNotStarted):
        contract.getResults()
    
    # Can not vote + or - before start
    with must_revert(D21.VotingNotStarted):
        contract.votePositive(owner, from_=acc1)
    with must_revert(D21.VotingNotStarted):
        contract.voteNegative(owner, from_=acc1)
    with must_revert(D21.VotingNotStarted):
        contract.voteBatch([owner],[True], from_=acc1)
    # Can not get time before start
    with must_revert(D21.VotingNotStarted):
        contract.getRemainingTime()
    
    # Start voting
    contract.startVoting(from_=owner)
    
    #Can not start voting - it already started
    with must_revert(D21.VotingAlreadyStarted):
        contract.startVoting(from_=owner)
    
    # Can not get results because voting is still going
    with must_revert(D21.VotingAlreadyStarted):
        contract.getResults()
    
    # Can not add subject - voting started
    with must_revert(D21.VotingAlreadyStarted):
        contract.addSubject("Bob", from_=acc1)
    
    contract.votePositive(owner, from_=acc1)
    with must_revert(D21.SelfVoted):
        contract.votePositive(owner, from_=owner)
    
    # End voting
    end_voting(contract)
    # Get results
    res = contract.getResults()
    logger.info(f"Res {res}")
    
    # Can not vote + or - after end
    with must_revert(D21.VotingEnded):
        contract.votePositive(owner, from_=acc1)
    with must_revert(D21.VotingEnded):
        contract.voteNegative(owner, from_=acc1)
    with must_revert(D21.VotingEnded):
        contract.voteBatch([owner],[True], from_=acc1)

@default_chain.connect()
def test_results():
    acc = default_chain.accounts[0:5]
    owner = acc[0] 
    contract = D21.deploy(from_=owner)
    contract.addVoter(acc[1], from_=owner)
    contract.addVoter(acc[2], from_=owner)
    contract.addVoter(owner, from_=owner)
    contract.addVoter(acc[3], from_=owner)
    contract.addVoter(acc[4], from_=owner)
    
    contract.addSubject("Gamer1",from_=owner)
    contract.addSubject("Gamer2",from_=acc[1])
    contract.addSubject("Gamer3",from_=acc[2])
    contract.addSubject("Gamer4",from_=acc[3])
    contract.addSubject("Gamer5",from_=acc[4])
    
    assert len(contract.getSubjects()) == 5, "Other length"
    subjects = contract.getSubjects()
    i = 0;
    for a in subjects:
        logger.info(f"Address of subject {i}: {a}")
        i+=1
        
    # Start voting
    contract.startVoting(from_=owner)
    
    for i in range(0, len(acc)):
        rnd4 = random.sample(range(len(acc)), 4)
        first = rnd4.pop()
        second = rnd4.pop()
        third = rnd4.pop()
        if first == i:
            continue
        contract.votePositive(acc[first], from_=acc[i])
        if second == i:
            continue
        contract.votePositive(acc[second], from_=acc[i])
        if third == i:
            continue
        contract.votePositive(acc[third], from_=acc[i])
        if rnd4[0] == i:
                continue
        contract.voteNegative(acc[rnd4[0]], from_=acc[i])

        
    
    # End voting
    end_voting(contract)
    # Get results
    res = contract.getResults()
    for r in res:
        logger.info(f"{r.name}: {r.votes}")
       
    # Right soted array descendently 
    for i in range(0, len(res) - 1):
        assert res[i].votes >= res[i+1].votes
      
@default_chain.connect()
def test_negative_vote():
    acc = default_chain.accounts[0:5]
    owner = acc[0]
    acc_unregistred = acc[2]
    
    contract = D21.deploy(from_=owner)
    contract.addVoter(acc[1], from_=owner)
    contract.addVoter(owner, from_=owner)
    
    contract.addSubject("Gamer1", from_=owner)
    contract.addSubject("Gamer2",from_=acc[1])
    contract.addSubject("Gamer3",from_=acc[2])
    contract.addSubject("Gamer4",from_=acc[3])
    contract.addSubject("Gamer5", from_=acc[4])
        
    # Start voting
    contract.startVoting(from_=owner)
    
    with must_revert(D21.VoterNotRegistered):
        contract.voteNegative(acc[0], from_=acc_unregistred)
    with must_revert(D21.SubjectDoesNotExist):
        contract.voteNegative(default_chain.accounts[8], from_=owner)
    
    with must_revert(D21.NeedsTwoPositiveBeforeNegative):
        contract.voteNegative(acc[2], from_=acc[1])
    contract.votePositive(acc[0], from_=acc[1])
    contract.votePositive(acc[3], from_=acc[1])
    contract.voteNegative(acc[2], from_=acc[1])
    with must_revert(D21.AlreadyVotedForSubject):
        contract.voteNegative(acc[2], from_=acc[1])
    with must_revert(D21.SelfVoted):
        contract.voteNegative(acc[1], from_=acc[1])
    with must_revert(D21.NegativeVoteAlreadyUsed):
        contract.voteNegative(acc[4], from_=acc[1])
    
    # End voting
    end_voting(contract)
    # Get results
    res = contract.getResults()
    for r in res:
        logger.info(f"{r.name}: {r.votes}")


@default_chain.connect()
def test_positive_vote():
    acc = default_chain.accounts[0:6]
    owner = acc[0]
    acc_unregistred = acc[2]
    
    contract = D21.deploy(from_=owner)
    contract.addVoter(acc[1], from_=owner)
    contract.addVoter(owner, from_=owner)
    
    contract.addSubject("Gamer1",from_=owner)
    contract.addSubject("Gamer2",from_=acc[1])
    contract.addSubject("Gamer3",from_=acc[2])
    contract.addSubject("Gamer4", from_=acc[3])
    contract.addSubject("Gamer5", from_=acc[4])
    contract.addSubject("Gamer6", from_=acc[5])
    
        
    # Start voting
    contract.startVoting(from_=owner)
    
    with must_revert(D21.VoterNotRegistered):
        contract.voteNegative(acc[0], from_=acc_unregistred)
    with must_revert(D21.SubjectDoesNotExist):
        contract.voteNegative(default_chain.accounts[8], from_=owner)
    
    contract.votePositive(acc[0], from_=acc[1])
    contract.votePositive(acc[3], from_=acc[1])
    contract.voteNegative(acc[2], from_=acc[1])
    with must_revert(D21.AlreadyVotedForSubject):
        contract.votePositive(acc[3], from_=acc[1])
    with must_revert(D21.SelfVoted):
        contract.votePositive(owner, from_=owner)
        
    contract.votePositive(acc[4], from_=acc[1])
    with must_revert(D21.MaxPositiveVotesReached):
        contract.votePositive(acc[5], from_=acc[1])
    
    # End voting
    end_voting(contract)
    # Get results
    res = contract.getResults()
    for r in res:
        logger.info(f"{r.name}: {r.votes}")


@default_chain.connect()
def test_vote_batch():
    acc = default_chain.accounts[0:6]
    owner = acc[0]
    user1 = acc[1]
    unregistred = acc[2]
    
    contract = D21.deploy(from_=owner)
    contract.addVoter(acc[1], from_=owner)
    contract.addVoter(owner, from_=owner)
    
    contract.addSubject("Gamer1",from_=owner)
    contract.addSubject("Gamer2",from_=acc[1])
    contract.addSubject("Gamer3",from_=acc[2])
    contract.addSubject("Gamer4",from_=acc[3])
    contract.addSubject("Gamer5",from_=acc[4])
    

    subjects = contract.getSubjects()
    
    # Start voting
    contract.startVoting(from_=owner)
    
    with must_revert(D21.ArrayLengthMismatch):
        contract.voteBatch(subjects,[True, False])
    with must_revert(D21.InvalidSubjectCount): # 5 votes for five subjects
        contract.voteBatch([subjects[0],subjects[1],subjects[2], subjects[2],subjects[2]],[True,True,True,True,True], from_=owner )
    
    with must_revert(D21.VoterNotRegistered):
        contract.voteBatch([acc[0],acc[1],acc[3],acc[4]], [True,True,False,True], from_=unregistred)
        
    with must_revert(D21.SubjectDoesNotExist):
        contract.voteBatch([acc[5]],[True], from_=user1)
    with must_revert(D21.AlreadyVotedForSubject):
        contract.voteBatch([subjects[1],subjects[1], subjects[2],subjects[2]],[True,True,True,False], from_=owner )
    with must_revert(D21.SelfVoted):
        contract.voteBatch([acc[0],acc[1],acc[3],acc[4]], [True,True,True,False], from_=acc[1])
    with must_revert(D21.MaxPositiveVotesReached):
        contract.voteBatch([acc[0],acc[2],acc[3],acc[4]], [True,True,True,True], from_=user1)
    with must_revert(D21.NegativeVoteAlreadyUsed):
        contract.voteBatch([acc[0],acc[2],acc[3],acc[4]], [True,True,False,False], from_=user1)
    with must_revert(D21.NeedsTwoPositiveBeforeNegative):
        contract.voteBatch([acc[0],acc[2],acc[3],acc[4]], [False,True,False,False], from_=user1)
  
    for i in range(0, len(acc)-4): 
        voter = acc[i]
        
        own_subject_addr = subjects[i] 
        
        valid_subjects = [s for s in subjects if s != own_subject_addr]
        
        max_votes = min(4, len(valid_subjects))
        rnd_len = random.randint(1, max_votes)
        
        voted_subjects = random.sample(valid_subjects, rnd_len)
        
        bool_arr = []
        match rnd_len:
            case 1: 
                bool_arr = [True]
            case 2: 
                bool_arr = [True, True]
            case 3: 
                bool_arr = [True, True, False] 
            case 4: 
                bool_arr = [True, True, True, False] 
        
        if not voted_subjects:
            continue

        contract.voteBatch(voted_subjects, bool_arr, from_=voter)     
        logger.info(f"account {i} voted for subjects: {voted_subjects} and votes: {bool_arr}")
    
    # End voting
    end_voting(contract)
    # Get results
    res = contract.getResults()
    for r in res:
        logger.info(f"{r.name}: {r.votes}")


