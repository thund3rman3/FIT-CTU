using System.Collections.Generic;
using Unity.Netcode;
using UnityEngine;

enum Operation
{
    AND,
    OR
}

public class CPartnerPressurePlate : NetworkBehaviour
{
    [SerializeField] Operation m_Operation = Operation.AND;
    [SerializeField] GameObject m_PressurePlate1;
    [SerializeField] GameObject m_PressurePlate2;

    [SerializeField] bool m_StayOn = false;

    [SerializeField] List<GameObject> m_Platforms;
    [System.NonSerialized]
    public NetworkVariable<bool> m_PressedDown = new NetworkVariable<bool>(false);

    CPressurePlate m_Plate1Class;
    CPressurePlate m_Plate2Class;


    void Update()
    {
        if (m_Plate2Class == null && m_Plate1Class == null)
            return;
        // check states of pressurre plates
        int pressedDown = 0;
        pressedDown += m_Plate1Class.m_IsPressed.Value ? 1 : 0;
        pressedDown += m_Plate2Class.m_IsPressed.Value ? 1 : 0;

        // platforms are on and should stay on
        if ( m_PressedDown.Value && m_StayOn ) {
            return;        
        }

        // "press down"
        if ( !m_PressedDown.Value && ( m_Operation == Operation.AND && pressedDown == 2
                              || m_Operation == Operation.OR && pressedDown >= 1 ) ) {
            TogglePlatformsServerRpc();
        }
        // "release"
        else if ( m_PressedDown.Value && ( m_Operation == Operation.AND && pressedDown != 2
                                  || m_Operation == Operation.OR && pressedDown == 0 ) ) {
            TogglePlatformsServerRpc();
        }
    }

    //Network
    //---------------------------------------------------------------------------//
    public override void OnNetworkSpawn()
    {
        m_Plate1Class = m_PressurePlate1.GetComponent<CPressurePlate>();
        m_Plate2Class = m_PressurePlate2.GetComponent<CPressurePlate>();
    }


    [Rpc(SendTo.Server)]
    void TogglePlatformsServerRpc()
    {
        m_PressedDown.Value = !m_PressedDown.Value;
        foreach ( GameObject platform in m_Platforms ) {
            platform.GetComponent<CPlatform>().Toggle();
        }   
    }

}
