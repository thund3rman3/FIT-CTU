using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UIElements;

[CreateAssetMenu(menuName = "MENU/NPC/Conversation", fileName = "Conversation")]
public class ConversationSO : ScriptableObject
{
    [Serializable]
    public class Branch
    {
        public bool burnAfterReading;
        public int requirements;
        public Node[] branch;
        public DecisionNode branchEnder;
    }
    [Serializable]
    public class Node
    {
        public string text;
    }
    [Serializable]
    public class DecisionNode
    {
        public string text;
        public string[] responses;
        public int[] children; //as in, the branches we can go to now
    }
    public Branch[] tree;
}