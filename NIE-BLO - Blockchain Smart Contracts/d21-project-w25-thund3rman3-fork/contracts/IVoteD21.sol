// SPDX-License-Identifier: MIT
pragma solidity ^0.8.26;

/// @notice Interface for a D21 voting system
interface IVoteD21 {

    // ------------------------------ Structures ------------------------------

    /// @notice The subject of voting – can be for example a political party.
    struct Subject {
        string name;
        int256 votes;
    }

    // -------------------------------- Events --------------------------------

    /// @notice Emmited when the owner started the voting period.
    event VotingStarted();

    /// @notice Emmited when the owner added a new voter.
    /// @param voter  Address of the added voter
    event VoterAdded(address indexed voter);

    /// @notice Emmited when a new subject was added.
    /// @param subject  Address of the subject
    /// @param name     Name of the subject
    event SubjectAdded(address indexed subject, string name);

    /// @notice Emmited when a positive vote was given.
    /// @param voter    Address of the voter
    /// @param subject  Address of the subject
    event PositiveVoted(address indexed voter, address indexed subject);

    /// @notice Emmited when a negative vote was given.
    /// @param voter    Address of the voter
    /// @param subject  Address of the subject
    event NegativeVoted(address indexed voter, address indexed subject);

    // ---------------------- Public & external functions ---------------------

    /// @notice Add a new subject into the voting system using the name.
    /// @param name  Name of the subject
    function addSubject(string memory name) external;

    /// @notice Get the subject details.
    /// @param subject  Address of the subject
    /// @return Subject details
    function getSubject(address subject) external view returns(Subject memory);

    /// @notice Get addresses of all registered subjects.
    /// @return Array of subject addresses
    function getSubjects() external view returns(address[] memory);

    /// @notice Add a new voter into the voting system.
    /// @param voter  Address of the voter
    function addVoter(address voter) external;

    /// @notice Start the voting period.
    function startVoting() external;

    /// @notice Vote positive for the subject.
    /// @param subject  Address of the subject
    function votePositive(address subject) external;

    /// @notice Vote negative for the subject.
    /// @param subject  Address of the subject
    function voteNegative(address subject) external;

    /// @notice Vote for multiple subjects.
    /// @param subjects  Array of subject addresses
    /// @param votes     Array of votes (true for positive, false for negative)
    function voteBatch(address[] calldata subjects, bool[] calldata votes) external;

    /// @notice Get the remaining time to the voting end in seconds.
    /// @return  Remaining time in seconds
    function getRemainingTime() external view returns(uint256);

    /// @notice Get the voting results, sorted descending by votes.
    /// @return  Array of subjects sorted by votes
    function getResults() external view returns(Subject[] memory);
}
