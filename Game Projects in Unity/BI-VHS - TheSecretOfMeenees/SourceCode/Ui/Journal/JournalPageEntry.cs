using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(menuName ="MENU/UI/JournalPageData", fileName = "JournalPageData")]
public class JournalPageEntry : ScriptableObject
{
    public int index; //starts from 1, as normal pages 
    [TextArea(15, 20)]
    public string leftPage;
    [TextArea(15, 20)]
    public string rightPage;
}
