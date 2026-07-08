using NUnit.Framework;
using System.Collections.Generic;
using Unity.Mathematics;
using Unity.Netcode;
using UnityEngine;

public class CMovingPlatform : CPlatform
{
    private Vector3 m_StartPosition;
    public Vector3 m_EndPosition;

    public float m_Speed = 0.01f;

    private Transform m_Platform;
    private Transform m_Path;

    // if the platform is currently travelling towards the end or start position
    private bool m_TowardsEnd = true;


    private void Start() {
        m_Platform = transform.Find ( "Platform" );
        m_Path = transform.Find ( "Path" );

        m_StartPosition = m_Platform.position;
    }



    void Update() {
        if ( m_IsOn.Value ) {
            if ( m_TowardsEnd ) { // platform is going 'there'
                float distanceX = m_EndPosition.x - m_Platform.position.x;
                float distanceY = m_EndPosition.y - m_Platform.position.y;
                float distanceToEnd = math.sqrt ( distanceX * distanceX + distanceY * distanceY );

                if ( distanceToEnd >= m_Speed * Time.deltaTime ) { // we have not reached the end yet, continue along path
                    Vector3 orientation = m_EndPosition - m_Platform.position;

                    m_Platform.position = m_Platform.position + orientation.normalized * m_Speed * Time.deltaTime;
                }
                else { // we will reach the end in this step, turn around
                    m_Platform.position = m_EndPosition;

                    m_TowardsEnd = false;
                }
            }
            else { // platform is going 'back' to start position
                float distanceX = m_StartPosition.x - m_Platform.position.x;
                float distanceY = m_StartPosition.y - m_Platform.position.y;
                float distanceToStart = math.sqrt ( distanceX * distanceX + distanceY * distanceY );

                if ( distanceToStart >= m_Speed * Time.deltaTime ) { // we have not reached the start yet, continue along path
                    Vector3 orientation = m_StartPosition - m_Platform.position;

                    m_Platform.position = m_Platform.position + orientation.normalized * m_Speed * Time.deltaTime;
                }
                else { // we will reach the start in this step, turn around
                    m_Platform.position = m_StartPosition;

                    m_TowardsEnd = true;
                }
            }
        }
    }
    private void Animate() {
        foreach ( Transform piece in m_Path ) {
            piece.GetComponent<CLoopAnimation>().m_Animate = m_IsOn.Value;
        }    
    }


    public override void Toggle() {
        m_IsOn.Value = !m_IsOn.Value;
        
        Animate();
    }
}
