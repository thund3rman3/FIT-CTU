"""
Wake fuzzing tests (property-based).
"""
from wake.testing import *
from wake.testing.fuzzing import *
from pytypes.contracts.D21 import D21


class D21Fuzz(FuzzTest):
    def pre_sequence(self):
        self.chain = default_chain
        self.owner = self.chain.accounts[0]
        self.d21 = D21.deploy(from_=self.owner)

        self.probe_voter = self.chain.accounts[9]
        self.pool = list(self.chain.accounts[1:9])

        self.subjects: list[str] = []
        self.voters: list[str] = []
        self.started = False

        # Ensure probe voter is registered
        self._try(lambda: self.d21.addVoter(voter=self.probe_voter, from_=self.owner))
        self.voters.append(self.probe_voter.address)

        # Ensure at least one subject exists for invariants
        def add_probe_subject():
            tx = self.d21.addSubject(name="PROBE_SUBJECT", from_=self.chain.accounts[1])
            self.subjects.append(tx.events[0].subject)
        self._try(add_probe_subject)

    # --- helpers ---

    def _try(self, fn):
        """Execute a call that may revert"""
        try:
            fn()
        except Exception:
            pass

    def _any_account(self):
        return random.choice(self.pool)

    def _any_subject(self):
        if not self.subjects:
            return None
        return random.choice(self.subjects)

    def _any_voter(self):
        if not self.voters:
            return None
        cands = [v for v in self.voters if v != self.probe_voter.address]
        return random.choice(cands) if cands else None

    # --- flows (random actions) --- 

    @flow()
    def flow_add_subject(self):
        # Only meaningful before voting starts, but fuzz can try anytime.
        sender = self._any_account()
        name = f"Party-{random.randint(0, 10_000)}"

        def call():
            tx = self.d21.addSubject(name=name, from_=sender)
            # capture subject id from event
            subj = tx.events[0].subject
            self.subjects.append(subj)

        self._try(call)

    @flow()
    def flow_add_voter(self):
        # Only owner should succeed.
        voter_acc = self._any_account()
        caller = random.choice([self.owner, self._any_account()])

        def call():
            self.d21.addVoter(voter=voter_acc, from_=caller)
            self.voters.append(voter_acc.address)

        self._try(call)

    @flow()
    def flow_start_voting(self):
        # Owner may start once.
        caller = random.choice([self.owner, self._any_account()])

        def call():
            self.d21.startVoting(from_=caller)
            self.started = True

        self._try(call)

    @flow()
    def flow_vote_positive(self):
        voter = self._any_voter()
        subj = self._any_subject()
        if voter is None or subj is None:
            return

        # Find the account object for this voter address
        voter_acc = next(a for a in self.chain.accounts if a.address == voter)

        self._try(lambda: self.d21.votePositive(subject=subj, from_=voter_acc))

    @flow()
    def flow_vote_negative(self):
        voter = self._any_voter()
        subj = self._any_subject()
        if voter is None or subj is None:
            return

        voter_acc = next(a for a in self.chain.accounts if a.address == voter)
        self._try(lambda: self.d21.voteNegative(subject=subj, from_=voter_acc))

    @flow()
    def flow_vote_batch(self):
        voter = self._any_voter()
        if voter is None or not self.subjects:
            return

        voter_acc = next(a for a in self.chain.accounts if a.address == voter)

        # random batch length 1..4, random subjects, random +/- votes
        k = random.randint(1, 4)
        subs = [random.choice(self.subjects) for _ in range(k)]
        vs = [random.choice([True, False]) for _ in range(k)]

        self._try(lambda: self.d21.voteBatch(subs, vs, from_=voter_acc))

    @flow()
    def flow_time_warp(self):
        # Randomly move time forward by up to 9 days
        now = self.chain.blocks["latest"].timestamp
        delta = random.randint(0, 9 * 24 * 60 * 60)
        self.chain.set_next_block_timestamp(now + delta)
        self.chain.mine()

    # --- invariants ---

    @invariant(period=5)
    def invariant_no_negative_before_two_positives(self):
        if not self.started or not self.subjects:
            return

        subj = random.choice(self.subjects)

        with must_revert():
            self.d21.voteNegative(subject=subj, from_=self.probe_voter)

    @invariant(period=5)
    def invariant_voting_window_enforced(self):
        # If voting hasn't started, positive voting should revert.
        if not self.voters or not self.subjects:
            return
        voter_addr = random.choice(self.voters)
        voter_acc = next(a for a in self.chain.accounts if a.address == voter_addr)
        subj = random.choice(self.subjects)

        if not self.started:
            with must_revert():
                self.d21.votePositive(subject=subj, from_=voter_acc)

    @invariant(period=5)
    def invariant_list_subjects_is_public_and_consistent(self):
        # Everyone can list registered subjects
        caller = random.choice(self.chain.accounts)

        # Must not revert for any caller in any state
        subjects = self.d21.getSubjects(from_=caller)

        # Basic consistency checks
        # no duplicates
        assert len(subjects) == len(set(subjects))

        # every returned subject must exist / be queryable
        for s in subjects:
            _ = self.d21.getSubject(s)  # should not revert

    @invariant(period=5)
    def invariant_only_owner_can_start_voting(self):
        # Pick a random non-owner
        non_owner = random.choice([a for a in self.chain.accounts if a.address != self.owner.address])

        # Non-owner must never be able to start voting (regardless of current state)
        with must_revert():
            self.d21.startVoting(from_=non_owner)

        # Once started, owner should also be unable to start again (start is one-time)
        if self.started:
            with must_revert():
                self.d21.startVoting(from_=self.owner)

    @invariant(period=5)
    def invariant_uc7_no_subject_registration_after_start(self):
        # Subjects can’t be registered after voting has started
        if not self.started:
            return

        caller = random.choice(self.chain.accounts)
        name = f"LateParty-{random.randint(0, 10_000)}"

        with must_revert():
            self.d21.addSubject(name=name, from_=caller)


@default_chain.connect()
def test_fuzz_d21():
    D21Fuzz().run(sequences_count=30, flows_count=80)
