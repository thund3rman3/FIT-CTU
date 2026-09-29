"""
Wake docs: https://ackee.xyz/wake/docs/latest/
"""
import logging
from wake.testing import *
from pytypes.contracts.D21 import D21 
from tests.utils import end_voting

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG) # logger.info(f"Swapped {amount}")

# UC1 - Everyone can register a subject (e.g. political party)  
# UC2 - Everyone can list registered subjects            
@default_chain.connect()
def test_UC1_2():
    owner, acc1, acc2 = default_chain.accounts[0:3]
    contract = D21.deploy(from_=owner)
    contract.addVoter(acc1.address, from_=owner)
    
    contract.addSubject("a")
    contract.addSubject("b", from_=acc1)
    contract.addSubject("c", from_=acc2)
       
    subjs = contract.getSubjects()
    assert len(subjs) == 3, "There should be no subjects"
    contract.getSubjects(from_=acc1)
    contract.getSubjects(from_=acc2)

# UC3 - Everyone can see the subject’s result (i.e. total vote score of the subject)               
@default_chain.connect()
def test_UC3():
    owner, acc1, acc2 = default_chain.accounts[0:3]
    contract = D21.deploy(from_=owner)
    contract.addVoter(acc1.address, from_=owner)
    contract.addSubject("gg", from_=acc2)
    contract.startVoting(from_=owner)
    contract.voteBatch(contract.getSubjects(), [True], from_=acc1)
    end_voting(contract)
    contract.getResults()
    contract.getResults(from_=owner)
    contract.getResults(from_=acc1)
    contract.getResults(from_=acc2)  

# UC4 - Only the voting owner can add eligible voters (i.e. add voters to the voting)      
@default_chain.connect()
def test_UC4():
    owner, acc1, acc2 = default_chain.accounts[0:3]
    contract = D21.deploy(from_=owner)
    contract.addVoter(acc1.address, from_=owner)
    with must_revert(D21.NeedOwnerPriviledges):
        contract.addVoter(acc1.address, from_=acc2)
    with must_revert(D21.NeedOwnerPriviledges):
        contract.addVoter(acc2.address, from_=acc2)


# UC5 - Only the owner can start the voting period               
@default_chain.connect()
def test_UC5():
    owner, acc1, acc2 = default_chain.accounts[0:3]
    contract = D21.deploy(from_=owner)
    contract.addVoter(acc1.address, from_=owner)
    
    with must_revert(D21.NeedOwnerPriviledges):
        contract.startVoting(from_=acc2)
    with must_revert(D21.NeedOwnerPriviledges):
        contract.startVoting(from_=acc1)
    contract.startVoting(from_=owner)

# UC6 - Voting ends after 7 days from the voting start  
@default_chain.connect()
def test_UC6():
    owner, acc1, acc2 = default_chain.accounts[0:3]
    contract = D21.deploy(from_=owner)
    contract.addVoter(acc1.address, from_=owner)
    contract.startVoting(from_=owner)
    assert contract.getRemainingTime() == 604800, "Not 7 days"
    
    end_voting(contract)

# UC7 - Subjects can’t be registered after the voting has started
@default_chain.connect()
def test_UC7():
    owner, acc1, acc2 = default_chain.accounts[0:3]
    contract = D21.deploy(from_=owner)
    contract.addVoter(acc1.address, from_=owner)
    contract.addVoter(owner.address, from_=owner)
    
    contract.addSubject("a", from_=acc1)
    contract.startVoting(from_=owner)
    with must_revert(D21.VotingAlreadyStarted):
        contract.addSubject("b", from_=acc1)
    with must_revert(D21.VotingAlreadyStarted):
        contract.addSubject("b")

# UC8 - Every voter has 3 positive and 1 negative vote (i.e. 4 different votes in total)              
@default_chain.connect()
def test_UC8():
    owner, acc1, acc2, acc3, acc4, acc5 = default_chain.accounts[0:6]
    contract = D21.deploy(from_=owner)
    contract.addVoter(acc1, from_=owner)
    contract.addVoter(owner, from_=owner)
    contract.addVoter(acc5, from_=owner)
    
    contract.addSubject("a", from_=acc1)
    contract.addSubject("ab", from_=acc2)
    contract.addSubject("abb", from_=acc3)
    contract.addSubject("abbb", from_=acc4)
    
    contract.startVoting(from_=owner)
    with must_revert(D21.MaxPositiveVotesReached):
        contract.voteBatch(contract.getSubjects(),[True,True,True,True],from_=owner)

    contract.voteBatch(contract.getSubjects(),[True,True,False,True],from_=owner)
    contract.voteBatch(contract.getSubjects(),[True,True,False,True],from_=acc5)
    
# UC9 - Voter can not give more than 1 vote to the same subject    
@default_chain.connect()
def test_UC9():
    owner, acc1, acc2, acc3, acc4, acc5 = default_chain.accounts[0:6]
    contract = D21.deploy(from_=owner)
    contract.addVoter(acc5, from_=owner)
    contract.addVoter(owner, from_=owner)
    
    contract.addSubject("a", from_=acc1)
    contract.addSubject("ab", from_=acc2)
    contract.addSubject("abb", from_=acc3)
    contract.addSubject("abbb", from_=acc4)

    contract.startVoting(from_=owner)
    s = contract.getSubjects()
    with must_revert(D21.AlreadyVotedForSubject):
        contract.voteBatch([s[0],s[0]],[True,True],from_=owner)
    contract.voteBatch(contract.getSubjects()[0:2],[True,True],from_=owner)
    
    with must_revert(D21.AlreadyVotedForSubject):
        contract.voteBatch([s[1],s[2],s[1]],[True,True,False],from_=acc5)
    contract.voteBatch(contract.getSubjects(),[True,True,False, True],from_=acc5)
    
# UC10 - Negative vote can be used only after 2 positive vote has been used       
@default_chain.connect()
def test_UC10():
    owner, acc1, acc2, acc3, acc4, acc5 = default_chain.accounts[0:6]
    contract = D21.deploy(from_=owner)
    contract.addVoter(acc5, from_=owner)
    contract.addVoter(owner, from_=owner)
    
    contract.addSubject("a", from_=acc1)
    contract.addSubject("ab", from_=acc2)
    contract.addSubject("abb", from_=acc3)
    contract.addSubject("abbb", from_=acc4)
    
    contract.startVoting(from_=owner)
    with must_revert(D21.NeedsTwoPositiveBeforeNegative):
        contract.voteBatch(contract.getSubjects(),[False,True,True,True],from_=owner)
    with must_revert(D21.NeedsTwoPositiveBeforeNegative):
        contract.voteBatch(contract.getSubjects(),[False,False,True,True],from_=owner)
    contract.voteBatch(contract.getSubjects(),[True,True,False,True],from_=owner)
    contract.voteBatch(contract.getSubjects(),[True,True,True, False],from_=acc5)

# UC11 - Voters should be able to vote in one transaction (i.e. give up to all 4 votes in single call)  
@default_chain.connect()
def test_UC11():
    owner, acc1, acc2, acc3, acc4, acc5 = default_chain.accounts[0:6]
    contract = D21.deploy(from_=owner)
    contract.addVoter(acc5, from_=owner)
    contract.addVoter(owner, from_=owner)
    
    contract.addSubject("a", from_=acc1)
    contract.addSubject("ab", from_=acc2)
    contract.addSubject("abb", from_=acc3)
    contract.addSubject("abbb", from_=acc4)
    
    contract.startVoting(from_=owner)
    contract.voteBatch(contract.getSubjects(),[True,True,False,True],from_=owner)
    contract.voteBatch(contract.getSubjects(),[True,True,False,True],from_=acc5)
    
# UC12 - Voting to self is not allowed (i.e. subject can’t vote for itself)          
@default_chain.connect()
def test_UC12():
    owner, acc1, acc2, acc3, acc4, acc5 = default_chain.accounts[0:6]
    contract = D21.deploy(from_=owner)
    contract.addVoter(acc1, from_=owner)
    contract.addVoter(acc2, from_=owner)
    contract.addVoter(acc3, from_=owner)
    
    contract.addVoter(owner, from_=owner)
    
    contract.addSubject("a", from_=acc1)
    contract.addSubject("ab", from_=acc2)
    contract.addSubject("abb", from_=acc3)
    contract.addSubject("abbb", from_=acc4)
    
    contract.startVoting(from_=owner)
    
    with must_revert(D21.SelfVoted):
        contract.votePositive(acc1, from_=acc1)
    contract.votePositive(acc2, from_=acc1)
    contract.votePositive(acc3, from_=acc1)
    with must_revert(D21.SelfVoted):
        contract.voteNegative(acc2, from_=acc2)

    with must_revert(D21.SelfVoted):
        contract.voteBatch(contract.getSubjects(), [True,True,True,False], from_=acc3)