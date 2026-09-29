"""
Wake docs: https://ackee.xyz/wake/docs/latest/
"""
from wake.testing import *
from pytypes.contracts.D21 import D21


@default_chain.connect()
def test_addSubject():
    chain = default_chain

    # Deploy contract
    deployer = chain.accounts[0]
    d21 = D21.deploy(from_ =deployer)

    # Add subject from an address
    subject_addr_1 = chain.accounts[1]
    name_1 = "Party1"
    tx = d21.addSubject(name=name_1, from_=subject_addr_1)

    # Check event was emitted
    assert len(tx.events) == 1
    assert isinstance(tx.events[0], D21.SubjectAdded)

    # Check that the subject was added
    subject = d21.getSubject(subject_addr_1)
    assert subject.name == name_1
    assert subject.votes == 0
    
    # Check party already registered
    name_2 = "Party2"
    with must_revert():
        d21.addSubject(name=name_2, from_=subject_addr_1)

    # check a non-existing subject
    subject_addr_2 = chain.accounts[2]
    name_2 = "Party2"
    with must_revert():
        d21.getSubject(subject_addr_2)

    d21.addSubject(name=name_2, from_=subject_addr_2)
    subject = d21.getSubject(subject_addr_2)
    assert subject.name == name_2
    assert subject.votes == 0

    # Check that all subjects are there
    subjects = d21.getSubjects()
    assert len(subjects) == 2
    assert subjects[0] == subject_addr_1._address
    assert subjects[1] == subject_addr_2._address

    d21.startVoting(from_=deployer)

    # Check cannot add party after voting started
    name_3 = "Party3"
    with must_revert():
        d21.addSubject(name_3, from_= deployer)


@default_chain.connect()
def test_poc_multiple_subjects_per_address():
    chain = default_chain
    deployer = chain.accounts[0]
    attacker = chain.accounts[1]

    d21 = D21.deploy(from_=deployer)
    d21.addSubject(name="Party1", from_=attacker)

    # Expected by spec: revert
    with must_revert():
        d21.addSubject(name="Party2", from_=attacker)

@default_chain.connect()
def test_addVoter():
    chain = default_chain

    deployer = chain.accounts[0]
    d21 = D21.deploy(from_ = deployer)

    voter_1 = chain.accounts[1]
    tx = d21.addVoter(voter=voter_1, from_=deployer)

    assert len(tx.events) == 1
    assert isinstance(tx.events[0], D21.VoterAdded)

    with must_revert():
        d21.addVoter(voter=voter_1, from_=deployer)
    
    voter_2 = chain.accounts[2]
    with must_revert():
        d21.addVoter(voter=voter_2, from_=voter_1)

@default_chain.connect()
def test_startVoting():
    chain = default_chain

    deployer = chain.accounts[0]
    d21 = D21.deploy(from_ = deployer)

    account_1 = chain.accounts[1]
    with must_revert():
        d21.startVoting(from_=account_1)

    tx = d21.startVoting(from_= deployer)
    assert len(tx.events) == 1
    assert isinstance(tx.events[0], D21.VotingStarted)

    with must_revert():
        d21.startVoting(from_=deployer)

@default_chain.connect()
def test_get_remaining_time():
    chain = default_chain
    day = 24 * 60 * 60
    week = 7 * day

    deployer = chain.accounts[0]
    d21 = D21.deploy(from_=deployer)

    d21.startVoting(from_=deployer)

    # baseline
    start_remaining = d21.getRemainingTime()
    assert 0 < start_remaining <= week

    # use latest block time as reference
    now = chain.blocks["latest"].timestamp

    # +1 day
    chain.set_next_block_timestamp(now + day)
    chain.mine()

    after_1_day = d21.getRemainingTime()
    assert 0 < after_1_day < start_remaining

    # +7 days (or more) -> should be 0
    chain.set_next_block_timestamp(now + week)
    chain.mine()

    after_week = d21.getRemainingTime()
    assert after_week == 0


@default_chain.connect()
def test_vote_positive():
    chain = default_chain

    deployer = chain.accounts[0]
    voter1 = chain.accounts[1]
    unregistered_voter = chain.accounts[2]

    # accounts used only as transaction senders
    subj_sender1 = chain.accounts[3]
    subj_sender2 = chain.accounts[4]
    subj_sender3 = chain.accounts[5]
    subj_sender4 = chain.accounts[6]
    unregistered_subject_sender = chain.accounts[7]

    d21 = D21.deploy(from_=deployer)

    # Register subjects and capture their actual identifiers from events
    tx_self = d21.addSubject(name="SelfParty", from_=voter1)
    tx1 = d21.addSubject(name="Party1", from_=subj_sender1)
    tx2 = d21.addSubject(name="Party2", from_=subj_sender2)
    tx3 = d21.addSubject(name="Party3", from_=subj_sender3)
    tx4 = d21.addSubject(name="Party4", from_=subj_sender4)

    self_subject = tx_self.events[0].subject
    subject1 = tx1.events[0].subject
    subject2 = tx2.events[0].subject
    subject3 = tx3.events[0].subject
    subject4 = tx4.events[0].subject

    # Register voter
    d21.addVoter(voter=voter1, from_=deployer)

    # cannot vote before voting starts
    with must_revert():
        d21.votePositive(subject=subject1, from_=voter1)

    # start voting
    d21.startVoting(from_=deployer)

    # unregistered voter cannot vote
    with must_revert():
        d21.votePositive(subject=subject1, from_=unregistered_voter)

    # cannot vote for unknown subject
    with must_revert():
        d21.votePositive(subject=unregistered_subject_sender.address, from_=voter1)

    # First positive vote
    tx_pos1 = d21.votePositive(subject=subject1, from_=voter1)
    assert len(tx_pos1.events) == 1
    assert isinstance(tx_pos1.events[0], D21.PositiveVoted)

    s1 = d21.getSubject(subject1)
    assert s1.votes == 1

    # Cannot vote for the same subject twice
    with must_revert():
        d21.votePositive(subject=subject1, from_=voter1)

    # Cannot vote for itself
    with must_revert():
        d21.votePositive(subject=self_subject, from_=voter1)

    # Second positive vote
    d21.votePositive(subject=subject2, from_=voter1)
    assert d21.getSubject(subject2).votes == 1

    # Third positive vote
    d21.votePositive(subject=subject3, from_=voter1)
    assert d21.getSubject(subject3).votes == 1

    # Fourth positive vote should fail
    with must_revert():
        d21.votePositive(subject=subject4, from_=voter1)

    # after voting duration, any positive vote must fail
    now = chain.blocks["latest"].timestamp
    day = 24 * 60 * 60
    voting_duration = 7 * day

    now = chain.blocks["latest"].timestamp
    chain.set_next_block_timestamp(now + voting_duration)
    chain.mine()

    with must_revert():
        d21.votePositive(subject=subject2, from_=voter1)

@default_chain.connect()
def test_vote_negative():
    chain = default_chain
    day = 24 * 60 * 60
    week = 7 * day

    deployer = chain.accounts[0]
    voter1 = chain.accounts[1]
    unregistered_voter = chain.accounts[2]

    # accounts used only as senders for subject registrations
    subj_sender1 = chain.accounts[3]
    subj_sender2 = chain.accounts[4]
    subj_sender3 = chain.accounts[5]
    subj_sender4 = chain.accounts[6]
    unregistered_subject_sender = chain.accounts[7]

    d21 = D21.deploy(from_=deployer)

    # Register subjects and capture their identifiers from events
    tx_self = d21.addSubject(name="SelfParty", from_=voter1)
    tx1 = d21.addSubject(name="Party1", from_=subj_sender1)
    tx2 = d21.addSubject(name="Party2", from_=subj_sender2)
    tx3 = d21.addSubject(name="Party3", from_=subj_sender3)
    tx4 = d21.addSubject(name="Party4", from_=subj_sender4)

    self_subject = tx_self.events[0].subject
    subject1 = tx1.events[0].subject
    subject2 = tx2.events[0].subject
    subject3 = tx3.events[0].subject
    subject4 = tx4.events[0].subject

    # register voter1
    d21.addVoter(voter=voter1, from_=deployer)

    # cannot vote negative before voting starts
    with must_revert():
        d21.voteNegative(subject=subject1, from_=voter1)

    # start voting
    d21.startVoting(from_=deployer)

    # unregistered voter cannot cast negative vote
    with must_revert():
        d21.voteNegative(subject=subject1, from_=unregistered_voter)

    # cannot vote negative for unknown subject (use an address)
    with must_revert():
        d21.voteNegative(subject=unregistered_subject_sender.address, from_=voter1)

    # cannot use negative vote before 2 positive votes
    with must_revert():
        d21.voteNegative(subject=subject1, from_=voter1)

    # give voter1 two positive votes
    d21.votePositive(subject=subject1, from_=voter1)
    d21.votePositive(subject=subject2, from_=voter1)

    # try self negative vote BEFORE any negative is used
    with must_revert() as exc:
        d21.voteNegative(subject=self_subject, from_=voter1)

    # sanity
    assert d21.getSubject(subject1).votes == 1
    assert d21.getSubject(subject2).votes == 1

    # first negative vote: happy path
    tx_neg = d21.voteNegative(subject=subject3, from_=voter1)
    assert len(tx_neg.events) == 1
    assert isinstance(tx_neg.events[0], D21.NegativeVoted)
    assert d21.getSubject(subject3).votes == -1

    # second negative vote must fail
    with must_revert():
        d21.voteNegative(subject=subject4, from_=voter1)

    # after voting duration, any negative vote must fail (7 days per spec)
    now = chain.blocks["latest"].timestamp
    chain.set_next_block_timestamp(now + week + 1)
    chain.mine()

    with must_revert():
        d21.voteNegative(subject=subject1, from_=voter1)


@default_chain.connect()
def test_vote_batch():
    chain = default_chain
    day = 24 * 60 * 60
    week = 7 * day

    deployer = chain.accounts[0]
    voter1 = chain.accounts[1]
    voter2 = chain.accounts[8]
    unregistered_voter = chain.accounts[2]

    # only used as senders for registrations
    subj_sender1 = chain.accounts[3]
    subj_sender2 = chain.accounts[4]
    subj_sender3 = chain.accounts[5]
    subj_sender4 = chain.accounts[6]
    unregistered_subject_sender = chain.accounts[7]

    d21 = D21.deploy(from_=deployer)

    # register subjects and capture identifiers
    tx1 = d21.addSubject(name="S1", from_=subj_sender1)
    tx2 = d21.addSubject(name="S2", from_=subj_sender2)
    tx3 = d21.addSubject(name="S3", from_=subj_sender3)
    tx4 = d21.addSubject(name="S4", from_=subj_sender4)

    subject1 = tx1.events[0].subject
    subject2 = tx2.events[0].subject
    subject3 = tx3.events[0].subject
    subject4 = tx4.events[0].subject

    # register voters
    d21.addVoter(voter=voter2, from_=deployer)
    d21.addVoter(voter=voter1, from_=deployer)

    # register voter1's own subject (for self-vote tests / results)
    tx_self = d21.addSubject(name="Self", from_=voter1)
    self_subject = tx_self.events[0].subject

    # cannot batch vote before start
    with must_revert():
        d21.voteBatch([subject1], [True], from_=voter1)

    # start voting
    d21.startVoting(from_=deployer)

    # length mismatch
    with must_revert():
        d21.voteBatch([subject1, subject2], [True], from_=voter1)

    # unregistered voter
    with must_revert():
        d21.voteBatch([subject1], [True], from_=unregistered_voter)

    # unknown subject (use an address that is not registered)
    with must_revert():
        d21.voteBatch([unregistered_subject_sender.address], [True], from_=voter1)

    # self vote (UC12) - should revert
    with must_revert():
        d21.voteBatch([self_subject], [True], from_=voter1)

    # batch votes: 2 positive + 1 negative (valid)
    d21.voteBatch(
        [subject1, subject2, subject3],
        [True, True, False],
        from_=voter1
    )

    assert d21.getSubject(subject1).votes == 1
    assert d21.getSubject(subject2).votes == 1
    assert d21.getSubject(subject3).votes == -1

    # double-voting in same batch is forbidden
    with must_revert():
        d21.voteBatch([subject1, subject1], [True, True], from_=voter1)

    # max positives = 3 (per spec). voter2 tries 4 positives in one batch -> should revert
    with must_revert():
        d21.voteBatch(
            [subject1, subject2, subject3, subject4],
            [True, True, True, True],
            from_=voter2,
        )

    # after voting duration, voting fails (7 days per spec)
    now = chain.blocks["latest"].timestamp
    chain.set_next_block_timestamp(now + week + 1)
    chain.mine()

    with must_revert():
        d21.voteBatch([subject4], [True], from_=voter1)

    # test getResults()
    results = d21.getResults()
    assert len(results) == 5
    name_to_votes = {subj.name: subj.votes for subj in results}

    assert name_to_votes["S1"] == 1
    assert name_to_votes["S2"] == 1
    assert name_to_votes["S3"] == -1
    assert name_to_votes["S4"] == 0
    assert name_to_votes["Self"] == 0