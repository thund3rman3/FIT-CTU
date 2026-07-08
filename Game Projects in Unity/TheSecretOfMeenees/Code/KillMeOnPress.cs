using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class KillMeOnPress : MonoBehaviour
{
    public KeyCode killMeKey = KeyCode.Tab;

    // Update is called once per frame
    void Update()
    {
        if(Input.GetKeyDown(killMeKey))
        {
            this.gameObject.SetActive(false);
        }
    }
}
