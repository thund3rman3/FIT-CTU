using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class SaveMenuValues : MonoBehaviour
{
    public Slider AudioMaster;
    public Slider AudioMusic;
    public Slider AudioSoundEffects;
    public Slider Sensitivity;
   

    // Update is called once per frame
    void Update()
    {
        PersistData.settings.AudioMaster = AudioMaster.value;
        PersistData.settings.AudioMusic = AudioMusic.value;
        PersistData.settings.AudioSoundEffects = AudioSoundEffects.value;
        PersistData.settings.sensitivity = Sensitivity.value;
    }

    public void LoadSettings()
    {
        AudioMaster.value = PersistData.settings.AudioMaster;
        AudioMusic.value = PersistData.settings.AudioMusic;
        AudioSoundEffects.value = PersistData.settings.AudioSoundEffects;
        Sensitivity.value = PersistData.settings.sensitivity;
    }
}
