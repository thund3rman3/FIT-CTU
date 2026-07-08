using UnityEngine;
using UnityEngine.UI;
using TMPro;
using Unity.Netcode;
using Unity.Cinemachine;

public class CColorPickerControl : MonoBehaviour
{
    public float m_CurrentHue = 0;
    public float m_CurrentSaturation = 0;
    public float m_CurrentValue = 0;

    [SerializeField] private RawImage m_HueImage;
    [SerializeField] private RawImage m_SaturationValueImage;
    [SerializeField] private RawImage m_OutputImage;

    [SerializeField] private Slider m_HueSlider;

    private Texture2D m_HueTexture;
    private Texture2D m_SaturationValueTexture;
    private Texture2D m_OutputTexture;

    [SerializeField] private int m_TextureSize = 16;

    private CPlayer m_Player;


    private void Start()
    {
        m_Player = transform.parent.parent.parent.parent.GetComponent<CPlayer>();

        Color playerColor = m_Player.GetColor();
        Color.RGBToHSV(playerColor, out m_CurrentHue, out m_CurrentSaturation, out m_CurrentValue);

        CreateHueImage();
        CreateSaturationValueImage();
        CreateOutputImage();

        UpdateOutputImage();
    }

    private void CreateHueImage()
    {
        m_HueTexture = new Texture2D ( 1, m_TextureSize );
        m_HueTexture.wrapMode = TextureWrapMode.Clamp;
        m_HueTexture.name = "Hue_Texture";

        // set hue values
        for ( int i = 0; i < m_TextureSize; i ++ ) {
            m_HueTexture.SetPixel ( 0, i, Color.HSVToRGB ( ( float ) i / m_TextureSize, 1, 1 ) );    
        }

        m_HueTexture.Apply();

        m_HueImage.texture = m_HueTexture;
    }

    private void CreateSaturationValueImage()
    {
        m_SaturationValueTexture = new Texture2D ( m_TextureSize, m_TextureSize );
        m_SaturationValueTexture.wrapMode = TextureWrapMode.Clamp;
        m_SaturationValueTexture.name = "Saturation_Value_Texture";

        // set saturation and value values
        for ( int y = 0; y < m_TextureSize; y ++ ) {
            for ( int x = 0; x < m_TextureSize; x ++ ) {
                m_SaturationValueTexture.SetPixel ( x, y, Color.HSVToRGB (
                                                          m_CurrentHue,
                                                          ( float ) x / m_TextureSize,
                                                          ( float ) y / m_TextureSize ) );
            }
        }

        m_SaturationValueTexture.Apply();

        m_SaturationValueImage.texture = m_SaturationValueTexture;
    }

    private void CreateOutputImage()
    {
        m_OutputTexture = new Texture2D ( 1, m_TextureSize );
        m_OutputTexture.wrapMode = TextureWrapMode.Clamp;
        m_OutputTexture.name = "Output_Texture";

        Color outputColor = Color.HSVToRGB ( m_CurrentHue, m_CurrentSaturation, m_CurrentValue );

        // set values
        for ( int i = 0; i < m_TextureSize; i ++ ) {
            m_OutputTexture.SetPixel ( 0, i, outputColor );    
        }

        m_OutputTexture.Apply();

        m_OutputImage.texture = m_OutputTexture;
    }

    private void UpdateOutputImage()
    {
        Color currentColor = Color.HSVToRGB ( m_CurrentHue, m_CurrentSaturation, m_CurrentValue );    

        // set values
        for ( int i = 0; i < m_TextureSize; i ++ ) {
            m_OutputTexture.SetPixel ( 0, i, currentColor );    
        }

        m_OutputTexture.Apply();

        m_Player.SetColor ( currentColor );
    }

    public void SetSaturationValue ( float saturation, float value )
    {
        m_CurrentSaturation = saturation;
        m_CurrentValue = value;

        UpdateOutputImage();
    }

    public void UpdateSaturationValueImage()
    {
        m_CurrentHue = m_HueSlider.value;   

        // update texture values
        for ( int y = 0; y < m_TextureSize; y ++ ) {
            for ( int x = 0; x < m_TextureSize; x ++ ) {
                m_SaturationValueTexture.SetPixel ( x, y, Color.HSVToRGB (
                                                          m_CurrentHue,
                                                          ( float ) x / m_TextureSize,
                                                          ( float ) y / m_TextureSize ) );
            }
        }

        m_SaturationValueTexture.Apply();

        UpdateOutputImage();
    }
}
