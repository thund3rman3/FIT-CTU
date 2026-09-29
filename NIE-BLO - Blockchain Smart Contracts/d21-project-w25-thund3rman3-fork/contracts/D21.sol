// SPDX-License-Identifier: MIT
pragma solidity 0.8.26;

import "./IVoteD21.sol";

struct Voter {
    mapping(address => bool) votedSubjects;
    uint8 votedPositive;
    bool exists;
    bool votedNegative;
}

contract D21 is IVoteD21 {

    // --------------------------------- State ---------------------------------
    uint64 private _votingEndTime; // ==0 - has not started, !=0 - started
    address private immutable _owner;
    
    mapping(address => Subject) private _subjects;
    address[] private _subjectAddresses;
    mapping(address => Voter) private _voters;

    uint8 constant POSITIVE_VOTES = 3;
    uint256 constant DURATION = 7 days;

    // ---------------------- Custom Errors ---------------------
    error NeedOwnerPriviledges();
    // Voting period errors
    error VotingAlreadyStarted(); 
    error VotingNotStarted();
    error VotingEnded();
    // Subject errors
    error SubjectExists();
    error SubjectDoesNotExist();
    // Registration errors
    error VoterNotRegistered();
    error VoterAlreadyRegistered();
    // Voting errors
    error SelfVoted();
    error AlreadyVotedForSubject();
    error MaxPositiveVotesReached();
    error NegativeVoteAlreadyUsed();
    error NeedsTwoPositiveBeforeNegative();
    // Count mismatch
    error ArrayLengthMismatch();
    error InvalidSubjectCount();

    // ---------------------- Public & external functions ---------------------

    constructor() {
        _owner = msg.sender;
    }
    /// @inheritdoc IVoteD21
    function addSubject(string memory name) external {
        if(_votingEndTime != 0) revert VotingAlreadyStarted();
        if(bytes(_subjects[msg.sender].name).length != 0) revert SubjectExists();

        _subjects[msg.sender] = Subject(name, 0);
        _subjectAddresses.push(msg.sender);
        emit SubjectAdded(msg.sender, name);
    }

    /// @inheritdoc IVoteD21
    function getSubjects() external view returns (address[] memory) {
        return _subjectAddresses;
    }

    /// @inheritdoc IVoteD21
    function getSubject(address subject) external view returns (Subject memory) {
        if(bytes(_subjects[subject].name).length == 0) revert SubjectDoesNotExist();
        return _subjects[subject];
    }

    /// @inheritdoc IVoteD21
    function addVoter(address voter) external {
        if(msg.sender != _owner) revert NeedOwnerPriviledges();
        Voter storage tmpVoter = _voters[voter];
        if(tmpVoter.exists) revert VoterAlreadyRegistered();

        tmpVoter.exists = true;
        emit VoterAdded(voter);
    }

    /// @inheritdoc IVoteD21
    function startVoting() external {
        if(msg.sender != _owner) revert NeedOwnerPriviledges();
        if(_votingEndTime != 0) revert VotingAlreadyStarted();
        _votingEndTime = uint64(block.timestamp + DURATION);
        emit VotingStarted();
    }

    function _voteRequirements(address subject, Voter storage sender) internal view {
        if(!sender.exists) revert VoterNotRegistered();
        if(bytes(_subjects[subject].name).length == 0) revert SubjectDoesNotExist();
        if(_votingEndTime == 0) revert VotingNotStarted();
        if(block.timestamp >= _votingEndTime) revert VotingEnded();
        if(sender.votedSubjects[subject]) revert AlreadyVotedForSubject();
        if(subject == msg.sender) revert SelfVoted();
    }

    function _votePositive(address subject, Voter storage sender) internal {
        if(sender.votedPositive >= POSITIVE_VOTES) revert MaxPositiveVotesReached();

        _subjects[subject].votes += 1;
        sender.votedPositive += 1;
        sender.votedSubjects[subject] = true;
        emit PositiveVoted(msg.sender, subject);
    }

    /// @inheritdoc IVoteD21
    function votePositive(address subject) external {
        Voter storage sender = _voters[msg.sender];
        _voteRequirements(subject, sender);
        _votePositive(subject, sender);
    }

    function _voteNegative(address subject, Voter storage sender) internal {
        if(sender.votedNegative) revert NegativeVoteAlreadyUsed();
        if(sender.votedPositive < POSITIVE_VOTES - 1) revert NeedsTwoPositiveBeforeNegative();

        _subjects[subject].votes -= 1;
        sender.votedNegative = true;
        sender.votedSubjects[subject] = true;

        emit NegativeVoted(msg.sender, subject);
    }

    /// @inheritdoc IVoteD21
    function voteNegative(address subject) external {
        Voter storage sender = _voters[msg.sender];
        _voteRequirements(subject, sender);
        _voteNegative(subject, sender);
    }

    /// @inheritdoc IVoteD21
    function voteBatch(address[] calldata subjects, bool[] calldata votes) external {
        uint256 len = subjects.length;
        if(len != votes.length) revert ArrayLengthMismatch();
        if(len <= 0 || len > 4) revert InvalidSubjectCount();

        Voter storage sender = _voters[msg.sender];
        if(!sender.exists) revert VoterNotRegistered();
        if(_votingEndTime == 0) revert VotingNotStarted();
        if(block.timestamp >= _votingEndTime) revert VotingEnded();

        uint8 positive = sender.votedPositive;
        bool negative = sender.votedNegative;

        for (uint256 i = 0; i < len; ++i) {
            address subj = subjects[i];
            if(bytes(_subjects[subj].name).length == 0) revert SubjectDoesNotExist();
            if(sender.votedSubjects[subj]) revert AlreadyVotedForSubject();
            if(subj == msg.sender) revert SelfVoted();

            if(votes[i]){
                if(positive >= POSITIVE_VOTES) revert MaxPositiveVotesReached();
                _subjects[subj].votes += 1;
                positive++;
                emit PositiveVoted(msg.sender, subj);
            }
            else{
                if(negative) revert NegativeVoteAlreadyUsed();
                if(positive < POSITIVE_VOTES - 1) revert NeedsTwoPositiveBeforeNegative();

                _subjects[subj].votes -= 1;
                negative = true;
                emit NegativeVoted(msg.sender, subj);
            }
            sender.votedSubjects[subj] = true;
        }
        sender.votedPositive = positive;
        sender.votedNegative = negative;

    }

    /// @inheritdoc IVoteD21
    function getRemainingTime() external view returns (uint256) {
        uint64 endTime = _votingEndTime;
        if(endTime == 0) revert VotingNotStarted();
        return endTime > block.timestamp ? endTime - block.timestamp : 0;
    }

    /// @inheritdoc IVoteD21
    function getResults() external view returns (Subject[] memory) {
        if(_votingEndTime == 0) revert VotingNotStarted();
        if(block.timestamp < _votingEndTime) revert VotingAlreadyStarted();

        address[] memory addr_cpy = _subjectAddresses;
        uint256 len = addr_cpy.length;
        Subject[] memory results = new Subject[](len);

        for(uint256 i = 0; i < len; ++i) {
            results[i] = _subjects[addr_cpy[i]];
        }

        if(len < 2)
            return results;

        for (uint256 i = 1; i < len; i++) {
            Subject memory elem = results[i];
            uint256 j = i;

            while ((j >= 1) && (results[j - 1].votes < elem.votes)) {
                results[j] = results[j - 1];
                j--;
            }
            results[j] = elem;
        }
        return results;
    }
}