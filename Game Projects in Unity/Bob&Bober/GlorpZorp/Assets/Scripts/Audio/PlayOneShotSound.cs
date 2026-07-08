using UnityEngine;
using UnityEngine.UI;

public class PlayOneShotSound : MonoBehaviour
{
    [SerializeField] private AudioSource m_AudioSource;
    [SerializeField] private AudioClip m_ClickClip;

    void Awake()
    {
        GetComponent<Button>().onClick.AddListener(PlayClick);
    }

    void PlayClick()
    {
        m_AudioSource.PlayOneShot(m_ClickClip);
    }
}
