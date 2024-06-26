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
    [SerializeField] private JournalPageEntry[] pagesOwned;
    [SerializeField] private int currPage; //depending on the indexing we will be doing, this might change
    [SerializeField] private AudioSource soundTurn;


    // Start is called before the first frame update
    void Start()
    {
        //The actual currPage at the start might change. We will be collecting more and the tutorial pages are in the middle after all

        //REMEBER: Keep currpage = intended +1. Its a scuffed way to always curve the text
        currPage = 0;
    }

    // Update is called once per frame
    void Update()
    {
        if (gameObject.GetComponent<Canvas>().enabled)
        {
            leftB.enabled = true;
            rightB.enabled = true;

            if (currPage - 1 < 0) leftB.enabled = false;
            else if (currPage + 1 >= pagesOwned.Length) rightB.enabled = false;
        }
    }

    public void PreviousPage()
    {
        PlaySound();
        Debug.Log("Went PREVIOUS");
        if (currPage - 1 < 0) return;
        currPage--;
        SetText();
    }
    public void NextPage()
    {
        PlaySound();
        Debug.Log("Went NEXT");
        if (currPage + 1 >= pagesOwned.Length) return;
        currPage++;
        SetText();
    }

    public void SetText()
    {
        //copy from Serialized Pages into the object TMP
        leftSide.text = pagesOwned[currPage].leftPage;
        rightSide.text = pagesOwned[currPage].rightPage;
        Debug.Log("TextSet");
    }

    public void PlaySound()
    {
        soundTurn.Play();
    }
}
