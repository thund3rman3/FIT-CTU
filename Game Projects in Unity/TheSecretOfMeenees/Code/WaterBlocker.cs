using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class WaterBlocker : MonoBehaviour
{
    public TextMeshProUGUI Warning;
    public bool is_exit = false;
    // Start is called before the first frame update
    private void Start()
    {
        if (!PersistData.playerData.brokenLegs && !is_exit) this.gameObject.SetActive(false);
    }
    private void OnTriggerStay(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            if (PersistData.playerData.EnabledElements[1])
            {
                Warning.gameObject.SetActive(false);
                this.gameObject.SetActive(false);
            }
            else Warning.gameObject.SetActive(true);
        }
    }

    private void OnTriggerExit(Collider other)
    {
        Warning.gameObject.SetActive(false);
        if (PersistData.playerData.EnabledElements[1]) this.gameObject.SetActive(false);
    }
}
