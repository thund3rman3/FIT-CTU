using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class RuneControl : MonoBehaviour
{
    public Color myColor;
    public GameObject myDad;
    // Start is called before the first frame update
    void Start()
    {
        gameObject.GetComponent<Image>().color = myColor;
    }

    void Update()
    {
        Color tmp = myDad.GetComponent<Image>().color;
        myColor.a = tmp.a;
        gameObject.GetComponent<Image>().color = myColor;
    }
}
