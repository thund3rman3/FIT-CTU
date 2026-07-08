using System.Collections;
using System.Collections.Generic;
using System.Linq;
using TMPro;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.UI;

public class Conversation : MonoBehaviour
{

    private bool _deciding, _typing, _newText, _hasBranch, _hasSO, _allReady, _burnBranch;
    private int _currentSO, _currentBranch, _currBranchDepth;

    //Which Meenee are we interacting with?
    private int _interactedMeenee;
    private NPCtalks _meeneeConvoData;

    public PlayerBehavior player;
    public PlayerDataSO playerDataSO;

    private float _wait;
    public string textChanging;
    public ConversationSO[] possibleTalks;
    public bool[] unskippable;
    public TMP_Text meeneeName;
    public GameObject continueButton;
    public TMP_Text dialogueSpace;
    public GameObject[] Butt;
    public GameObject convoShell;

    public float typeDelay;

    public AudioSource talk;
    public AudioClip[] clips;

    public bool inConversation;

    private void Start()
    {
        _typing = false;
        _deciding = false;
        _allReady = true;
        _newText = false;
        _hasSO = false;
        _hasBranch = false;

        dialogueSpace.text = "";
        for (int i = 0; i < 4; i++)
        {
            Butt[i].SetActive(false);
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (convoShell.activeInHierarchy)
        {
            inConversation = true;  
            if (!_hasSO && _allReady)
            {
                dialogueSpace.text = "";
                /*for(int i = 0; i < possibleTalks.Count(); i++)
                {
                    for(int j = 0; j < PersistData.allMeeneeConvos[_interactedMeenee].torchedSOs.Count; i++)
                    {
                        if()
                    }
                }  The SOs do have to be in order, since they have no clue which one goes next...
                    Therefore, we can just use the count.*/
                _currentSO = PersistData.allMeeneeConvos[_interactedMeenee].torchedSOs.Count;
                _hasSO = true;
                
            }
            if (!_hasBranch && !_deciding)
            {
                _currentBranch = 0;
                _hasBranch = true;
                _burnBranch = possibleTalks[_currentSO].tree[0].burnAfterReading;
                _currBranchDepth = 0;
                Debug.Log("_hasBranch was false");
            }
            if (!_deciding)
            {
                if (!_typing && _allReady)
                {
                    //split between continues and decision node
                    if (_currBranchDepth < possibleTalks[_currentSO].tree[_currentBranch].branch.Length)
                    {
                        //talk
                        textChanging = possibleTalks[_currentSO].tree[_currentBranch].branch[_currBranchDepth].text;
                        _newText = true;

                    }
                    else //we have hit the branch Ender part, and need to make a decision
                    {
                        //talk
                        dialogueSpace.text = possibleTalks[_currentSO].tree[_currentBranch].branchEnder.text;
                        //enable buttons
                        StartDecision();
                    }
                }

                if (!_typing && _newText)
                {
                    continueButton.SetActive(false);
                    dialogueSpace.text = "";
                    _typing = true;
                    _newText = false;
                    //ConvoData.torchedSOs[0] = true;

                    StartCoroutine(Typing(textChanging, _wait));
                }
            }
        }
    }
    public void SetSpeedAndPitch(float speed, float pitch)
    {
        talk.pitch = pitch;
        typeDelay = 1f / (speed * 10);
    }
    public void GetText(string newText, float wait)
    {
        textChanging = newText;
        _wait = wait;
        _newText = true;
    }
    IEnumerator Typing(string text, float wait)
    {
        _allReady = false;
        foreach (char letter in text.ToCharArray())
        {
            dialogueSpace.text += letter;
            if (!talk.isPlaying)
            {
                int RandomI = Random.Range(0, clips.Length - 1);
                talk.clip = clips[RandomI];
                if (letter != ' ') talk.Play();
            }
            yield return new WaitForSecondsRealtime(typeDelay);
            if (Input.anyKey)
            {
                dialogueSpace.text = text;
                break;
            }
        }
        yield return new WaitForSecondsRealtime(wait - typeDelay * text.Length);

        _typing = false;
        continueButton.SetActive(true);
    }
    public void StartEndConvo(bool startOrEnd)
    {
        convoShell.SetActive(startOrEnd);
        inConversation = startOrEnd;
        Start();
    }
    private void StartDecision()
    {
        _deciding = true;
        //maybe a dynamic layout here idk idc.
        if (possibleTalks[_currentSO].tree[_currentBranch].branchEnder.responses.Length > 0)
        {
            for (int i = 0; i < possibleTalks[_currentSO].tree[_currentBranch].branchEnder.responses.Length; i++)
            {
                Butt[i].SetActive(true);
                Butt[i].transform.GetChild(0).GetComponent<TMP_Text>().text = possibleTalks[_currentSO].tree[_currentBranch].branchEnder.responses[i];
            }
            continueButton.SetActive(false);
        }

    }

    private void EndDecision(int decReached)
    {
        _allReady = true;
        _deciding = false;
        if (decReached >= 0)
        {
            for (int i = 0; i < possibleTalks[_currentSO].tree[_currentBranch].branchEnder.responses.Length; i++)
            {
                Butt[i].SetActive(false);
            }
            continueButton.SetActive(true);

            _currentBranch = possibleTalks[_currentSO].tree[_currentBranch].branchEnder.children[decReached];
            _currBranchDepth = 0;
            if (_burnBranch)
            {
                //ConvoData.torchedBranches
            }
        }
        //if we return a negative number, we clicked continue
        else
        {
            if (possibleTalks[_currentSO].tree[_currentBranch].branchEnder.children.Length == 0 && _currBranchDepth == possibleTalks[_currentSO].tree[_currentBranch].branch.Length)
            {
                //if(PersistData.playerData.inDungeon == true) PersistData.dungeonData.tmpMeeneeConvo[_interactedMeenee].torchedSOs.Add(_currentSO);
                PersistData.allMeeneeConvos[_interactedMeenee].torchedSOs.Add(_currentSO);
                PersistData.allMeeneeConvos[_interactedMeenee].torchedBranches= new List<int>();
                _hasSO = false;
                _hasBranch = false;
                if (_interactedMeenee == 0 && _currentSO == 0) player.TakeDamage(859);
                StartEndConvo(false);
            }
        }
    }

    public void ADecision()
    {
        EndDecision(0);
    }
    public void BDecision()
    {
        EndDecision(1);
    }
    public void CDecision()
    {
        EndDecision(2);
    }
    public void DDecision()
    {
        EndDecision(3);
    }
    public void Continue()
    {
        EndDecision(-1);
        _currBranchDepth++;
    }

    public void SetMeeneeData(GameObject meenee)
    {
        _meeneeConvoData = meenee.GetComponent<NPCtalks>();
        _interactedMeenee = _meeneeConvoData.MeeneeIndex;
        possibleTalks = _meeneeConvoData.possibleTalks;
        unskippable = _meeneeConvoData.unskippable;
        meeneeName.text = _meeneeConvoData.meeneeName;
    }

    public bool IsSkippable()
    {
        if (_currentSO >= unskippable.Length) return true;
        return !unskippable[_currentSO];
    }
}
