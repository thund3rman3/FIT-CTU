using UnityEngine;
using TMPro;
using UnityEngine.UI;

public class UIManager : MonoBehaviour
{    
    [SerializeField] private GameObject m_HostMenu;
    [SerializeField] private GameObject m_JoinMenu;
    [SerializeField] private TMP_InputField m_SessionNameHostField;
    [SerializeField] private Button m_HostButton;
    [SerializeField] private Canvas m_Canvas;

    void Update()
    {
        m_HostButton.interactable = !string.IsNullOrWhiteSpace(m_SessionNameHostField.text);
        if (ConnectionManager.Instance.State == ConnectionManager.ConnectionState.Connected)
        {
            m_Canvas.enabled = false;
        }
    }

    //public void DebugJoin()
    //{
    //    _ = ConnectionManager.Instance.HostSession();
    //}

    public void ShowHostMenu()
    {
        m_JoinMenu.SetActive(false);
        m_HostMenu.SetActive(true);
    }

    public void ShowJoinMenu()
    {
        m_HostMenu.SetActive(false);
        m_JoinMenu.SetActive(true);
    }

    public void Quit()
    {
        _ = ConnectionManager.Instance.Disconnect();
        Application.Quit();
    }

    public async void CreateSession()
    {
        if(string.IsNullOrEmpty(m_SessionNameHostField.text))
        {
            Debug.LogError("Please provide a session name, to create a session.");
            return;
        }
        await ConnectionManager.Instance.CreateOrJoinSessionAsync(m_SessionNameHostField.text);
    }

    public async void LoadLastSave()
    {
        m_HostMenu.SetActive(false);
        await ConnectionManager.Instance.CreateOrJoinSessionAsync(m_SessionNameHostField.text, true);
    }
}
