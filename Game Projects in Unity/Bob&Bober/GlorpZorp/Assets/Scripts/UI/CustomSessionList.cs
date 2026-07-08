using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Unity.Services.Authentication;
using Unity.Services.Core;
using Unity.Services.Multiplayer;
using UnityEngine;
using UnityEngine.UI;

public class CustomSessionList : MonoBehaviour
{
    [Header("UI References")]
    [SerializeField] private GameObject m_SessionItemPrefab;
    [SerializeField] private Transform m_ContentParent;
    [SerializeField] private Button m_RefreshButton;
    [SerializeField] private Button m_JoinButton;

    private List<GameObject> listItems = new List<GameObject>();
    private ISessionInfo selectedSession;

    async private void Start()
    {
        m_RefreshButton?.onClick.AddListener(() => _ = RefreshSessionList());
        m_JoinButton?.onClick.AddListener(OnJoinClicked);

        m_JoinButton.interactable = false;

        await RefreshSessionList();
    }

    public async Task RefreshSessionList()
    {
        foreach (var item in listItems)
            Destroy(item);

        listItems.Clear();
        selectedSession = null;
        m_JoinButton.interactable = false;

        try
        {
            var result = await MultiplayerService.Instance.QuerySessionsAsync(new QuerySessionsOptions());

            foreach (var sessionInfo in result.Sessions)
            {
                var itemGO = Instantiate(m_SessionItemPrefab, m_ContentParent);

                if (itemGO.TryGetComponent<SessionItemUI>(out var itemUI))
                {
                    itemUI.SetSession(sessionInfo);
                    itemUI.m_OnSelected.AddListener(SessionSelected);
                }

                listItems.Add(itemGO);
            }
        }
        catch (Exception e)
        {
            Debug.LogError($"Chyba pøi naèítání session: {e}");
        }
    }

    public void SessionSelected(ISessionInfo info)
    {
        selectedSession = info;
        m_JoinButton.interactable = true;

        //Debug.Log($"Vybrána session: {info.Name} ({info.Id})");
    }

    private async void OnJoinClicked()
    {
        if (selectedSession == null) 
            return;

        try
        {
            await ConnectionManager.Instance.JoinSessionByID(selectedSession.Id);
        }
        catch (Exception ex)
        {
            Debug.LogError($"Join selhal: {ex}");
        }
    }
}
