using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;
using UnityEngine.U2D;
using System.Linq;
using UnityEngine.UI;

public class JournalContent : MonoBehaviour
{
    [SerializeField] private TMP_Text leftSide;
    [SerializeField] private TMP_Text rightSide;
    [Space]
    [SerializeField] private Image leftB;
    [SerializeField] private Image rightB;
    [Space]
    [Space]
    [SerializeField] private LinkedList<JournalPageEntry> pagesOwned = new LinkedList<JournalPageEntry>();
    [SerializeField] private JournalPageEntry[] pagesAll;
    [SerializeField] private LinkedListNode<JournalPageEntry> currPage; //depending on the indexing we will be doing, this might change
    [SerializeField] private AudioSource soundTurn;


    // Start is called before the first frame update
    void Start()
    {
        //The actual currPage at the start might change. We will be collecting more and the tutorial pages are in the middle after all

        //REMEBER: Keep currpage = intended +1. Its a scuffed way to always curve the text
        UpdatePages();
        currPage = pagesOwned.First;
    }

    // Update is called once per frame
    void Update()
    {
        if (gameObject.GetComponent<Canvas>().enabled)
        {
            if (PersistData.playerData.PagesOwned.Count > pagesOwned.Count)
            {
                pagesOwned.Clear();
                UpdatePages();
                currPage = pagesOwned.Last;
                SetText();
            }
            leftB.enabled = true;
            rightB.enabled = true;

            if (currPage == pagesOwned.First) leftB.enabled = false;
            else if (currPage == pagesOwned.Last) rightB.enabled = false;
        }
    }

    public void PreviousPage()
    {
        PlaySound();
        Debug.Log("Went PREVIOUS");
        if (currPage == pagesOwned.First) return;
        currPage = currPage.Previous;
        SetText();
    }
    public void NextPage()
    {
        PlaySound();
        Debug.Log("Went NEXT");
        if (currPage == pagesOwned.Last) return;
        currPage = currPage.Next;
        SetText();
    }

    public void SetText()
    {
        //copy from Serialized Pages into the object TMP
        leftSide.text = currPage.Value.leftPage;
        rightSide.text = currPage.Value.rightPage;
        Debug.Log("TextSet");
    }

    public void PlaySound()
    {
        soundTurn.Play();
    }
    public void UpdatePages()
    {
        pagesOwned.AddFirst(pagesAll[0]);
        var tmp = pagesOwned.First;
        for(int i = 1; i < PersistData.playerData.PagesOwned.Count; i++)
        {
           pagesOwned.AddAfter(tmp, pagesAll[i]);
           tmp = tmp.Next;
        }
    }
}
