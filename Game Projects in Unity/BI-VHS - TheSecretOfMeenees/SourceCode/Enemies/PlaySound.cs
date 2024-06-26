using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Audio;

public class PlaySound : MonoBehaviour
{
    public AudioClip walk;
    public AudioClip hit;
    AudioSource sourceWalk;
    AudioSource sourceHit;
    public AudioMixerGroup amg;
    // Start is called before the first frame update
    void Awake()
    {
        sourceWalk = gameObject.AddComponent<AudioSource>();
        sourceWalk.clip = walk;
        sourceWalk.playOnAwake = false;
        sourceWalk.volume = 0.2f;   
        sourceHit = gameObject.AddComponent<AudioSource>();
        sourceHit.clip = hit;
        sourceHit.playOnAwake = false;
        sourceHit.volume = 0.2f;
        sourceHit.outputAudioMixerGroup = amg;
        sourceWalk.outputAudioMixerGroup = amg;

    }

    void PlayWalk()
    {
        sourceWalk.Play();
    }

    void PlayHit()
    {
        sourceHit.Play();
    }
}
