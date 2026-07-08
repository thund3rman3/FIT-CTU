using TMPro;
using Unity.Services.Multiplayer;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

public class SessionItemUI : MonoBehaviour, ISelectHandler
{
    [SerializeField] TMP_Text m_SessionNameText;
    [SerializeField] TMP_Text m_PlayerCountText;

    public UnityEvent<ISessionInfo> m_OnSelected;

    ISessionInfo m_SessionInfo;
    CustomSessionList m_ParentList;

    void Awake()
    {
        m_ParentList = GetComponentInParent<CustomSessionList>();

        if (m_ParentList != null)
        {
            m_OnSelected.AddListener(m_ParentList.SessionSelected);
        }
    }

    public void SetSession(ISessionInfo info)
    {
        m_SessionInfo = info;

        m_SessionNameText.text = info.Name;

        int currentPlayers = info.MaxPlayers - info.AvailableSlots;
        m_PlayerCountText.text = $"{currentPlayers}/{info.MaxPlayers}";
    }

    public void OnSelect(BaseEventData eventData)
    {
        m_OnSelected?.Invoke(m_SessionInfo);
    }
}
