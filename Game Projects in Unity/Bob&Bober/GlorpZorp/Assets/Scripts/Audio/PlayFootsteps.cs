using UnityEngine;
using System.Collections;
using UnityEngine.SceneManagement;

public class PlayFootsteps : MonoBehaviour
{
    [SerializeField] private AudioSource m_Source;
    [SerializeField] private AudioClip m_StepWood1;
    [SerializeField] private AudioClip m_StepWood2;

    [SerializeField] private AudioClip m_StepGrass1;
    [SerializeField] private AudioClip m_StepGrass2;

    private AudioClip m_ActiveFootstep1;
    private AudioClip m_ActiveFootstep2;

    [SerializeField] private float m_StepInterval = 0.2f;
    [SerializeField] private float m_MinMoveSpeed = 0.1f;

    private Rigidbody2D m_RigidBody;
    private Coroutine m_StepRoutine;
    private bool m_UseStep1 = true;
    private CBober m_PlayerScript;


    void Awake()
    {
        m_RigidBody = GetComponent<Rigidbody2D>();
        if(SceneManager.GetActiveScene().name == "Level 1")
        {
            m_ActiveFootstep1 = m_StepWood1;
            m_ActiveFootstep2 = m_StepWood2; 
        }
        else
        {
            m_ActiveFootstep1 = m_StepGrass1;
            m_ActiveFootstep2 = m_StepGrass2;
        }

        m_PlayerScript = GetComponent<CBober>();
    }

    void Update()
    {   
        bool isMoving = m_RigidBody.linearVelocity.magnitude > m_MinMoveSpeed;
        if (isMoving && m_StepRoutine == null && m_PlayerScript.m_GroundCollisions.Count >= 1)
        {
            m_StepRoutine = StartCoroutine(FootstepRoutine());
        }
        else if ((!isMoving || m_PlayerScript.m_GroundCollisions.Count == 0) && m_StepRoutine != null)
        {
            StopCoroutine(m_StepRoutine);
            m_StepRoutine = null;
        }
    }

    IEnumerator FootstepRoutine()
    {
        while (true)
        {
            m_Source.PlayOneShot(m_UseStep1 ? m_ActiveFootstep1 : m_ActiveFootstep2);
            m_UseStep1 = !m_UseStep1;

            yield return new WaitForSeconds(m_StepInterval);
        }
    }
}
