using System;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(menuName = "MENU/NPC/Dialogues", fileName = "Dialogues")]
public class Dialogues : ScriptableObject
{
    [Serializable]
    public class Exchange
    {
        public string elementSecond;
        [TextArea(10, 10)]
        public List<string> talky = new List<string>();
    }
    [Serializable]
    public class TalkAt
    {
        [TextArea(10, 10)]
        public List<string> talky = new List<string>();
    }
    public TalkAt[] onlyTalkAts;
    public Exchange[] dialogues;
}
