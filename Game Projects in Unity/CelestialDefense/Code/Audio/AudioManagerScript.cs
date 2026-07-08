using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AudioManagerScript : MonoBehaviour
{
    public static AudioManagerScript i;

    public Sound[] sounds;

    private AudioSource theme;

    public void Awake()
    {
        i = this;

        foreach (Sound snd in sounds)
        {
            snd.source = gameObject.AddComponent<AudioSource>();
            snd.source.clip = snd.clip;
            snd.source.volume = snd.volume;
            //snd.source.pitch = snd.pitch;
            snd.source.loop = snd.loop;
        }
    }
    public void Start()
    {
        switch (gameObject.scene.name)
        {
            case "MainMenu":
                PlayTheme("MenuTheme1");
                break;
            case "GameScene":
                PlayTheme("BuildPhaseTheme");
                break;
        }   
    }

    public static void Play(string name)
    {
        Sound s = System.Array.Find(i.sounds, snd => snd.name == name);

        if (s == null) return;

        s.source.Play();
    }
    
    public static void PlaySpatial(string name, Vector3 pos)
    {
        Sound s = System.Array.Find(i.sounds, snd => snd.name == name);

        if (s == null) return;

        AudioSource.PlayClipAtPoint(s.clip, pos);
    }

    public static void Stop(string name)
    {
        Sound s = System.Array.Find(i.sounds, snd => snd.name == name);

        if (s == null) return;

        s.source.Stop();
    }

    public static void PlayTheme(string name)
    {
        Sound s = System.Array.Find(i.sounds, snd => snd.name == name);

        if (s == null) return;

        if (i.theme != null) i.theme.Stop();

        i.theme = s.source;
        i.theme.Play();
    }

    public static void PauseTheme()
    {
        i.theme.Pause();
    }

    public static void UnPauseTheme()
    {
        i.theme.UnPause();
    }
}
