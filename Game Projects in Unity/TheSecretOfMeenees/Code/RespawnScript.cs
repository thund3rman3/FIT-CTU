using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RespawnScript : MonoBehaviour
{
    public void ResetRespawn()
    {
        this.gameObject.SetActive(false);
    }

    public void SetTime()
    {
        this.gameObject.GetComponent<AudioSource>().time = 3f;
    }
}
