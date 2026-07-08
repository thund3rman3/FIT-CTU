using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class BillboardCanvas : MonoBehaviour
{
    // Start is called before the first frame update
    public Transform camTransform;
    private Quaternion _originalRotation;

    private bool _typing, _newText;
    private float _wait, _normDelay, _normPitch, _halfPitch, _twiceDelay;

    public string textChanging;
    public TMP_Text dialogueSpace;
    public float typeDelay;

    public AudioSource talk;
    public AudioClip[] clips;

    private void Awake()
    {
        _typing = false;
        _newText = false;
    }
    void Start()
    {
        _originalRotation = transform.rotation;
        if (Camera.main != null)
        {
            camTransform = Camera.main.transform;
        }
    }

    // Update is called once per frame
    void Update()
    {
        transform.rotation = camTransform.rotation * _originalRotation;
        if(!_typing && _newText)
        {
            dialogueSpace.text = "";
            _typing = true;
            _newText = false;
            transform.GetChild(0).gameObject.SetActive(true);
            StartCoroutine(Typing(textChanging, _wait));
        }
    }
    public void SetSpeedAndPitch(float speed, float pitch)
    {
        _normPitch = pitch;
        _normDelay = 1f / (speed * 10);
        _halfPitch = pitch / 2;
        _twiceDelay = _normDelay * 2;
    }
    public void GetText(string newText, float wait)
    {
        textChanging = newText;
        _wait = wait;
        _newText = true; 
    }
    public bool IsTalking()
    {
        return _typing;
    }
    IEnumerator Typing(string text, float wait)
    {
        foreach (char letter in text.ToCharArray())
        {
            if(Time.timeScale < 0.9f)
            {
                talk.pitch = _halfPitch;
                typeDelay = _twiceDelay;
            }
            else
            {
                talk.pitch = _normPitch;
                typeDelay = _normDelay;
            }
            dialogueSpace.text += letter;
            if (!talk.isPlaying)
            {
                int RandomI = Random.Range(0, clips.Length - 1);
                talk.clip = clips[RandomI];
                if(letter != ' ')talk.Play();
            }
            yield return new WaitForSecondsRealtime(typeDelay);
        }
        yield return new WaitForSecondsRealtime(wait-typeDelay*text.Length);
        transform.GetChild(0).gameObject.SetActive(false);
        _typing = false;
    }
}
