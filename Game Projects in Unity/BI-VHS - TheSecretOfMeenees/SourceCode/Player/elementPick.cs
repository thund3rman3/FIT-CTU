using UnityEngine;
using UnityEngine.UI;
using TMPro;
using System.Collections;

public class ElementPick : MonoBehaviour
{
    public PlayerDataSO PDSO;
    public int type;
    public TextMeshProUGUI pressE;
    public GameObject elemStack;

    public GameObject overlay = null;
    bool pickable = false;

    public void Start()
    {
        
    }


    private void OnTriggerStay(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            pressE.gameObject.SetActive(true);
            if (Input.GetKey(PDSO.pickItemKey))
            {
                if (type == 5)
                {
                    Debug.Log("journal picked");
                    PersistData.playerData.haveJournal = true;
                    PersistData.playerData.EnabledSpellCast = true;
                    PersistData.playerData.EnabledElements[2] = true;
                    this.gameObject.SetActive(false);
                    pressE.gameObject.SetActive(false);
                    overlay.gameObject.SetActive(true);
                }
                else if (type < 5)
                {
                    Debug.Log("elem picked" );
                    PersistData.playerData.EnabledElements[type] = true;
                    this.gameObject.SetActive(false);
                    pressE.gameObject.SetActive(false);
                    elemStack.transform.GetChild(type).gameObject.SetActive(true);
                }
            }

        }
    }

    private void OnTriggerExit(Collider other)
    {
        pressE.gameObject.SetActive(false);
    }
}
