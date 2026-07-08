using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class ElementPicker : MonoBehaviour
{
    public UI_Control uiControl;
    public Image elementVisual;
    private TemplateMagic.ElementType _SelectedElement;

    public Vector3[] positions;
    public Transform[] icons;

    private int _offset;
    private AudioSource _audioSource;

    private void Start()
    {
        _offset = 2;
        _audioSource = this.gameObject.GetComponent<AudioSource>();
        SetPositions();
    }
    // Update is called once per frame
    void Update()
    {
        if(_SelectedElement != uiControl.SelectedElement)
        {
            _SelectedElement = uiControl.SelectedElement;
            elementVisual.color = uiControl.VizualizeElement();
            ChooseOffset();
            SetPositions();
            _audioSource.Play();
        }
        
        Vector3 _directionNew = new Vector3(this.transform.rotation.x, this.transform.rotation.y, Random.Range(0, 0.01f));
        this.transform.Rotate(_directionNew);
        
    }
    void ChooseOffset()
    {
        switch (_SelectedElement)
        {
            case TemplateMagic.ElementType.Air:
                _offset = 0;
                break;
            case TemplateMagic.ElementType.Earth:
                _offset = 1;
                break;
            case TemplateMagic.ElementType.Fire:
                _offset = 2;
                break;
            case TemplateMagic.ElementType.Water:
                _offset = 3;
                break;
        }
    }
    void SetPositions()
    {
        for (int i = 0; i < icons.Length; i++)
        {
            icons[i].localPosition = positions[i + _offset];
            if (_offset+ i == 3)
            {
                icons[i].localScale = new Vector3(0.8f, 0.8f, 0.8f);
                var tempColor = icons[i].gameObject.GetComponent<Image>().color;
                tempColor.a = 255;
                icons[i].gameObject.GetComponent<Image>().color = tempColor;
            }
            else
            {
                icons[i].localScale = new Vector3(0.5f, 0.5f, 0.5f);
                var tempColor = icons[i].gameObject.GetComponent<Image>().color;
                tempColor.a = 130;
                icons[i].gameObject.GetComponent<Image>().color = tempColor;
            }
        }
    }
}
