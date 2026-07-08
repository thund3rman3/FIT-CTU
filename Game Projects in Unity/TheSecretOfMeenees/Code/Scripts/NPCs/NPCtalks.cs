using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;
using static UnityEngine.GraphicsBuffer;

public class NPCtalks : MonoBehaviour
{
    public ConversationSO[] possibleTalks;
    public bool[] unskippable;
    private Animator animator;
    public PlayerDataSO PDSO;
    private MeeneeDataSO MDSO;
    private TextMeshProUGUI pressE;

    private Camera playerCamera;
    private Conversation playerDialogueUI;

    public LayerMask mask;

    public float TalkingSpeed;
    public float TalkingPitch;

    private bool _talking;

    private Vector3 playerPos;

    public int MeeneeIndex;
    public string meeneeName;

    // Start is called before the first frame update
    void Start()
    {
        //Debug.Log("pressE:Start");
        //This gets the Main Camera from the Scene
        if (Camera.main != null)
        {
            playerCamera = Camera.main;
            //This enables Main Camera
            playerCamera.enabled = true;
        }
        playerDialogueUI = GameObject.FindWithTag("Conversation").GetComponent<Conversation>();
        pressE = GameObject.FindWithTag("UIControl").GetComponent<UI_Control>().PressE.GetComponent<TextMeshProUGUI>();
        //Debug.Log("pressE:" + pressE);
        animator = this.GetComponent<Animator>();
        _talking = false;
        if(TalkingSpeed == 0)
        {
            MDSO = gameObject.GetComponent<MeeneeBT>().meeneeData;
            TalkingSpeed = MDSO.talkingSpeed;
            TalkingPitch = MDSO.talkingPitch;
        }
    }

    // Update is called once per frame
    void Update()
    {
        if(pressE.gameObject.activeInHierarchy)
        {
            if (Physics.Raycast(playerCamera.transform.position, playerCamera.transform.forward, out var hit, Mathf.Infinity, mask))
            {
                var obj = hit.collider.gameObject;
                if(obj == this.gameObject && Input.GetKeyDown(PDSO.pickItemKey))
                {
                    Debug.Log($"TALKING at {obj.name}", this);
                    playerPos = playerCamera.transform.position;
                    pressE.gameObject.SetActive(false);
                    StartTalking();
                }

                Debug.Log($"looking at {obj.name}", this);
            }
        }
        if(_talking)
        {
            var lookDir = playerPos - transform.position;
            lookDir.y = 0; // keep only the horizontal direction
            transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(lookDir), Time.deltaTime);
        }
        if (!playerDialogueUI.inConversation) _talking = false;
    }

    public void StartTalking()
    {
        _talking = true;
        animator.SetBool("Talking", true);
        playerDialogueUI.SetMeeneeData(this.gameObject);
        playerDialogueUI.SetSpeedAndPitch(TalkingSpeed, TalkingPitch);
        playerDialogueUI.StartEndConvo(true);
    }

    private void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            Start();
            pressE.gameObject.SetActive(true);
            _talking = false;
        }
    }

    private void OnTriggerExit(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            pressE.gameObject.SetActive(false);
            _talking = false;
            animator.SetBool("Talking", false);
        }
    }
}
