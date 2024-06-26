using System.Collections;
using System.Collections.Generic;
using TMPro;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.UI;
using static UnityEngine.GraphicsBuffer;

public class NPCpick : MonoBehaviour
{
    public PlayerDataSO PDSO;
    public MeeneeDataSO meeneeData;
    public bool isNPC;
    private NPCtalks myTalks;

    public LayerMask mask;
    
    private Vector3 playerPos;
    private Camera m_MainCamera;
    private TextMeshProUGUI pressE;
    private Conversation playerDialogueUI;

    private bool _readyToDie;

    void Start()
    {
        //This gets the Main Camera from the Scene
        if (Camera.main != null)
        {
            m_MainCamera = Camera.main;
            //This enables Main Camera
            m_MainCamera.enabled = true;
        }
        pressE = GameObject.FindWithTag("UIControl").GetComponent<UI_Control>().PressE.GetComponent<TextMeshProUGUI>();
        myTalks = gameObject.GetComponent<NPCtalks>();
        playerDialogueUI = GameObject.FindWithTag("Conversation").GetComponent<Conversation>();
        if (myTalks != null) myTalks.enabled = false;
        _readyToDie = false;
    }
    // Update is called once per frame
    void Update()
    {
        var lookDir = m_MainCamera.transform.position - transform.position;
        lookDir.y = 0; // keep only the horizontal direction
        transform.rotation = Quaternion.LookRotation(lookDir);

        if (pressE.gameObject.activeInHierarchy)
        {
            if (Physics.Raycast(m_MainCamera.transform.position, m_MainCamera.transform.forward, out var hit, Mathf.Infinity, mask))
            {
                var obj = hit.collider.gameObject;
                if (obj == gameObject && Input.GetKeyDown(PDSO.pickItemKey))
                {
                    if (isNPC)
                    {
                        pressE.gameObject.SetActive(false);
                        //myTalks.enabled = true;
                        myTalks.StartTalking();
                        PersistData.dungeonData.PickUnique(TranslateElement());
                    }
                    else
                    {
                        PersistData.dungeonData.carriedGenericNPCs[TranslateElement()] += 1;
                    }
                    Debug.Log(PersistData.dungeonData.carriedUniqueNPCs[TranslateElement()]);
                    _readyToDie = true;

                }
            }
        }
        //if finished talking
        if((!playerDialogueUI.inConversation || myTalks == null) && _readyToDie)
        {
            //myTalks.enabled = false;
            //KILL HIM
            Debug.Log("killing him <3");
            pressE.gameObject.SetActive(false);
            Destroy(gameObject);
        }
    }
    private void OnTriggerExit(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            pressE.gameObject.SetActive(false);
        }
    }
    private void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            pressE.gameObject.SetActive(true);
        }
    }

    private int TranslateElement()
    {
        switch (meeneeData.meeneeElement)
        {
            case "Air":
                return 0;
            case "Fire":
                return 1;
            case "Earth":
                return 2;
            case "Water":
                return 3;
        }
        return 1;
    }
}
