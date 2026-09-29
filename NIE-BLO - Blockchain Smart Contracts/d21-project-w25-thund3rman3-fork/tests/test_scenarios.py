"""
Wake docs: https://ackee.xyz/wake/docs/latest/
"""
import logging
from wake.testing import *
from pytypes.contracts.D21 import D21 
from tests.utils import end_voting

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG) # logger.info(f"Swapped {amount}")


@default_chain.connect()
def test_no_subject():
    owner, acc1, acc2 = default_chain.accounts[0:3]
    contract = D21.deploy(from_=owner)
    contract.addVoter(acc1.address, from_=owner)
    contract.addVoter(acc2.address, from_=owner)
    # No subjects added yet
    # Attempt to start voting should revert
    with must_revert(D21.SubjectDoesNotExist):
        contract.votePositive(acc2.address, from_=acc1)

    # Start voting
    contract.startVoting(from_=owner)
    # Voting with no subjects should revert
    with must_revert(D21.SubjectDoesNotExist):
        contract.votePositive(acc1.address, from_=acc2)

    end_voting(contract=contract)
    
    # Get results
    res = contract.getResults(from_=acc1)
    assert res == [], "Results should be empty with no subjects"
    
    # Voting with no subjects should revert
    with must_revert(D21.SubjectDoesNotExist):
        contract.votePositive(acc2.address, from_=acc1)
       
    subjs = contract.getSubjects()
    assert subjs == [], "There should be no subjects"


@default_chain.connect()
def test_one_subject():
    owner, acc1 = default_chain.accounts[0:2]
    contract = D21.deploy(from_=owner)
    contract.addVoter(acc1, from_=owner)
    contract.addVoter(owner, from_=owner)
    contract.addSubject("Gamer1",from_=owner)
    
    # Attempt to start voting should revert
    with must_revert(D21.VotingNotStarted):
        contract.votePositive(owner, from_=acc1)

    # Start voting
    contract.startVoting(from_=owner)
    contract.votePositive(owner, from_=acc1)
    
    # End voting
    end_voting(contract)
    # Get results
    res = contract.getResults()
    logger.info(f"Res {res}")
    
    # Voting with no subjects should revert
    with must_revert(D21.VotingEnded):
        contract.votePositive(owner, from_=acc1)
        
    subjs = contract.getSubjects()
    assert subjs == [owner.address], "There should be one subject"


@default_chain.connect()
def test_six_subjects():
    owner, acc1, acc2, acc3, acc4,acc5 = default_chain.accounts[0:6]
    contract = D21.deploy(from_=owner)
    contract.addVoter(acc1, from_=owner)
    contract.addVoter(acc2, from_=owner)
    contract.addVoter(owner, from_=owner)
    contract.addVoter(acc3, from_=owner)
    contract.addVoter(acc4, from_=owner)
    
    contract.addSubject("Gamer1",from_=owner)
    contract.addSubject("Gamer2",from_=acc1)
    contract.addSubject("Gamer3",from_=acc2)
    contract.addSubject("Gamer4", from_=acc3)
    contract.addSubject("Gamer5", from_=acc4)
    contract.addSubject("Gamer6",from_=acc5)
    
    assert len(contract.getSubjects()) == 6, "Other length"
    a1,a2,a3,a4,a5,a6 = contract.getSubjects()
    arr = [a1,a2,a3,a4,a5,a6]
    i = 0;
    for a in arr:
        logger.info(f"Address of gamer {i}: {a}")
        i+=1
    # Start voting
    contract.startVoting(from_=owner)
    contract.votePositive(a1, from_=acc1)
    contract.votePositive(a3, from_=acc1)
    contract.votePositive(a4, from_=acc1)
    contract.voteNegative(a5, from_=acc1)
    
    contract.voteBatch([a3,a4,a5,a6], [True, True, False, True], from_=owner)
    
    contract.votePositive(a1, from_=acc2)
    # Did not use 2 positive votes before one negative
    with must_revert(D21.NeedsTwoPositiveBeforeNegative):
        contract.voteNegative(a2, from_=acc2)
    contract.votePositive(a2, from_=acc2)
    contract.voteNegative(a5, from_=acc2)
    contract.votePositive(a4, from_=acc2)
    
    contract.voteBatch([a1,a5,a6], [True, True, False], from_=acc3)
    
    contract.votePositive(a3, from_=acc4)
    contract.votePositive(a6, from_=acc4)
    contract.votePositive(a1, from_=acc4)
    # Already used 3 positive votes
    with must_revert(D21.MaxPositiveVotesReached):
        contract.votePositive(a2, from_=acc4)

    # End voting
    end_voting(contract)
    # Get results
    res = contract.getResults()
    for r in res:
        logger.info(f"{r.name}: {r.votes}")
       
    # Right soted array descendently 
    assert res[0].votes >= res[1].votes
    assert res[1].votes >= res[2].votes
    assert res[2].votes >= res[3].votes
    assert res[3].votes >= res[4].votes
        
    subjs = contract.getSubjects()
    logger.info(f"subs: {subjs}")


