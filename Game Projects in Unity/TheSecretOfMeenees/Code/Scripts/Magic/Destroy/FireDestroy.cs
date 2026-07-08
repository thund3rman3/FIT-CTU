using System.Collections;
using System.Collections.Generic;
using System.IO;
using Unity.VisualScripting;
using UnityEngine;

public class FireDestroy : MonoBehaviour
{
    public float burnTimer = 1.3f;
    private float m_last = 1.1f;
    private GameObject m_flames;
    // Start is called before the first frame update
    void Start()
    {
        GameObject prefab = Resources.Load("Prefabs/Magic/Burning_Effect") as GameObject;
        m_flames = Instantiate(prefab, this.transform);
    }

    // Update is called once per frame
    void Update()
    {
        burnTimer -= Time.deltaTime;
        if (m_last - burnTimer > 0.05f)
        {
            this.transform.localScale *= 0.98f;
            m_flames.transform.localScale *= 0.95f;
            m_last = burnTimer;
        }
        if(burnTimer <= 0 ) Destroy(transform.gameObject);
    }
}
