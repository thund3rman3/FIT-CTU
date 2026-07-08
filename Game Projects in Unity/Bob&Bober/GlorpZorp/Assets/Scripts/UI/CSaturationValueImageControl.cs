using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class CSaturationValueImageControl : MonoBehaviour, IDragHandler, IPointerClickHandler
{
    [SerializeField] private Image m_PickerImage;

    private RawImage m_SaturationValueImage;
    private CColorPickerControl m_ColorPickerControl;
    private RectTransform m_RectTransform;
    private RectTransform m_PickerTransform;

    private void Awake()
    {
        m_SaturationValueImage = GetComponent<RawImage>();
        m_ColorPickerControl = FindAnyObjectByType<CColorPickerControl>();
        m_RectTransform = GetComponent<RectTransform>();

        m_PickerTransform = m_PickerImage.GetComponent<RectTransform>();
        m_PickerTransform.position = new Vector2 ( - ( m_RectTransform.sizeDelta.x * 0.5f ),
                                                   - ( m_RectTransform.sizeDelta.y * 0.5f )); 

    }

    void UpdateColor ( PointerEventData eventData )
    {
        Vector3 position = m_RectTransform.InverseTransformPoint ( eventData.position );
        
        float deltaX = m_RectTransform.sizeDelta.x * 0.5f;
        float deltaY = m_RectTransform.sizeDelta.y * 0.5f;

        // clamp
        if ( position.x < - deltaX )
            position.x = - deltaX;
        else if ( position.x > deltaX )
            position.x = deltaX;

        if ( position.y < - deltaY )
            position.y = - deltaY;
        else if ( position.y > deltaY )
            position.y = deltaY;

        // get normalized positions
        float x = ( position.x + deltaX ) / m_RectTransform.sizeDelta.x;
        float y = ( position.y + deltaY ) / m_RectTransform.sizeDelta.y;

        m_PickerTransform.localPosition = position;
        m_PickerImage.color = Color.HSVToRGB ( 0, 0, 1 - y );

        m_ColorPickerControl.SetSaturationValue ( x, y );
    }


    public void OnDrag ( PointerEventData eventData )
    {
        UpdateColor ( eventData );
    }

    public void OnPointerClick ( PointerEventData eventData )
    {
        UpdateColor ( eventData );
    }
}
