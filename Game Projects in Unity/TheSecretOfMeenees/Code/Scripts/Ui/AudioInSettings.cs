using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.Audio;
using UnityEngine.UI;

public class AudioInSettings : MonoBehaviour
{

    public AudioMixer mine;
    public Slider me;
    public TMP_Text theText;
    private bool _muted = false;
    private float _lastVal;

    //sound settings
    public void ClickSound()
    {
        if (_muted)
        {
            mine.SetFloat("MyExposedParam", _lastVal);
            //mine.FindSnapshot("Default").TransitionTo(0.02f);
            _muted = false;
            me.GetComponent<Slider>().enabled = true;
            theText.color = Color.white;
        }
        else
        {
            _lastVal = me.value;
            mine.SetFloat("MyExposedParam", -80.0f);
            me.GetComponent<Slider>().enabled = false;
            //mine.FindSnapshot("Mute").TransitionTo(0.02f);
            theText.color = Color.red;
            _muted = true;
        }
    }

    public void ChangeSound()
    {
        mine.SetFloat("MyExposedParam", me.value);
    }
}
