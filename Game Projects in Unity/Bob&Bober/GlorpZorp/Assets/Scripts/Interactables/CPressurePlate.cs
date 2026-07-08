using System.Collections;
using System.Collections.Generic;
using Unity.Netcode;
using UnityEngine;

public class CPressurePlate : NetworkBehaviour
{
    public Color m_ColorLight;
    public Color m_ColorDark;

    public float m_DeactivationTime = 1f;
    private float m_Timer = 0.0f;

    [SerializeField] List<GameObject> m_Platforms;
    public NetworkVariable<bool> m_IsPressed = new NetworkVariable<bool>(false);

    private GameObject m_Light;
    private GameObject m_Dark;

    void Start()
    {
        m_Dark = transform.Find ( "Pressure_Plate_Dark" ).gameObject;
        m_Light = transform.Find ( "Pressure_Plate_Light" ).gameObject;

        // apply color to sprites
        m_Light.GetComponent<SpriteRenderer>().color = m_ColorLight;
        m_Dark.GetComponent<SpriteRenderer>().color= m_ColorDark;
    }


    private void ResetServer()
    {
        m_IsPressed.Value = false;

        PressDownClientsRpc(false);

        // toggle states of objects
        foreach (GameObject platform in m_Platforms)
        {
            platform.GetComponent<CPlatform>().Toggle();
        }
    }

    IEnumerator StartTimer()
    {
        while (m_Timer < m_DeactivationTime)
        {
            m_Timer += Time.deltaTime;
            yield return null;
        }

        ResetServer();
    }

    //Network
    //---------------------------------------------------------------------------//
    [Rpc(SendTo.Server)]
    public void ResetTimerServerRpc()
    {
        m_Timer = 0.0f;    
    }

    [Rpc(SendTo.Server)]
    public void PressDownServerRpc()
    {
        if (m_IsPressed.Value) 
            return;

        m_IsPressed.Value = true;
        m_Timer = 0.0f;

        PressDownClientsRpc(true);

        // toggle states of objects
        foreach ( GameObject platform in m_Platforms ) {
            platform.GetComponent<CPlatform>().Toggle();
        }

        StartCoroutine ( StartTimer() );
    }

    [Rpc(SendTo.ClientsAndHost)]
    public void PressDownClientsRpc(bool down)
    {
        // move pressure plate
        Vector2 position = m_Light.transform.position;
        if(down)
            position -= new Vector2 ( 0.0f, 1.0f / 12.0f );
        else
            position += new Vector2 ( 0.0f, 1.0f / 12.0f ); 
        m_Light.transform.position = position;
        m_Dark.transform.position = position;
        GetComponent<AudioSource>().PlayOneShot(GetComponent<AudioSource>().clip);
    }
}
