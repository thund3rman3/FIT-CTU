using System.Collections.Generic;
using Unity.Netcode;
using UnityEngine;

public class CLever : NetworkBehaviour
{
    public Color m_ColorLight;
    public Color m_ColorDark;

    [SerializeField] List<GameObject> m_Platforms;
    public NetworkVariable<bool> m_IsOn = new NetworkVariable<bool>(false);

    // Parts
    private GameObject m_OnDark;
    private GameObject m_OnLight;
    private GameObject m_OffDark;
    private GameObject m_OffLight;

    public void Toggle()
    {
        if (!IsServer) return;

        m_IsOn.Value = !m_IsOn.Value;

        foreach (GameObject platform in m_Platforms)
        {
            platform.GetComponent<CPlatform>().Toggle();
        }
    }

    //Network
    //---------------------------------------------------------------------------//
    public override void OnNetworkSpawn()
    {
        m_OnDark = transform.Find("Lever_On_Dark").gameObject;
        m_OnLight = transform.Find("Lever_On_Light").gameObject;
        m_OffDark = transform.Find("Lever_Off_Dark").gameObject;
        m_OffLight = transform.Find("Lever_Off_Light").gameObject;

        m_OnDark.GetComponent<SpriteRenderer>().color = m_ColorDark;
        m_OnLight.GetComponent<SpriteRenderer>().color = m_ColorLight;
        m_OffDark.GetComponent<SpriteRenderer>().color = m_ColorDark;
        m_OffLight.GetComponent<SpriteRenderer>().color = m_ColorLight;

        UpdateVisuals(m_IsOn.Value);

        m_IsOn.OnValueChanged += OnStateChanged;
    }

    public override void OnNetworkDespawn()
    {
        m_IsOn.OnValueChanged -= OnStateChanged;
    }


    //Callbacks
    //---------------------------------------------------------------------------//
    private void OnStateChanged(bool previous, bool current)
    {
        UpdateVisuals(current);
        GetComponent<AudioSource>().PlayOneShot(GetComponent<AudioSource>().clip);
    }

    private void UpdateVisuals(bool isOn)
    {
        m_OnDark.SetActive(isOn);
        m_OnLight.SetActive(isOn);
        m_OffDark.SetActive(!isOn);
        m_OffLight.SetActive(!isOn);
    }

}