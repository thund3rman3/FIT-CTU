using System;
using System.Threading.Tasks;
using Unity.Netcode;
using Unity.Services.Authentication;
using Unity.Services.Core;
using Unity.Services.Multiplayer;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityUtils;

public class ConnectionManager : MonoBehaviour
{
    public static ConnectionManager Instance { get; private set; }

    [SerializeField] private int m_MaxPlayers = 10;
    ISession m_Session;
    [SerializeField] private string m_SceneToLoad = "Level 1";
    [System.NonSerialized] public string m_SessionName;
    public ConnectionState State { get; set; } = ConnectionState.Disconnected;

    public enum ConnectionState
    {
        Disconnected,
        Connecting,
        Connected,
    }

    async void Awake()
    {
        if (Instance != null && Instance != this)
        {
            Destroy(gameObject);
            return;
        }
        Instance = this;
        DontDestroyOnLoad(this.gameObject);

        await InitializeServicesAsync();

        //if (NetworkManager.Singleton != null)
        //{
        //    NetworkManager.Singleton.OnSessionOwnerPromoted += OnSessionOwnerPromoted;
        //    NetworkManager.Singleton.OnClientConnectedCallback += OnClientConnectedCallback;
        //}
        //else
        //{
        //    Debug.LogError("NetworkManager not found in scene!");
        //    return;
        //}

    }

    void OnServerStarted()
    {
        NetworkManager.Singleton.OnServerStarted -= OnServerStarted;

        //Debug.Log($"[ConnectionManager] Server started, loading {m_SceneToLoad}");

        NetworkManager.Singleton.SceneManager.LoadScene(
            m_SceneToLoad,
            LoadSceneMode.Single
        );
    }


    async Task InitializeServicesAsync()
    {
        try
        {
            if (UnityServices.State != ServicesInitializationState.Initialized)
            {
                await UnityServices.InitializeAsync();
            }

            if (!AuthenticationService.Instance.IsSignedIn)
            {
                AuthenticationService.Instance.SwitchProfile(
                    "Player_" + UnityEngine.Random.Range(1000, 99999));
                await AuthenticationService.Instance.SignInAnonymouslyAsync();
            }

            //Debug.Log($"[ConnectionManager] Signed in as {AuthenticationService.Instance.PlayerId}");
        }
        catch (Exception e)
        {
            Debug.LogException(e);
        }
    }

    public async Task JoinSessionByID(string sessionId)
    {
        State = ConnectionState.Connecting;

        int attempts = 0;
        while (attempts < 3)
        {
            try
            {
                //Debug.Log($"[ConnectionManager] Joining session {sessionId}");
                m_Session = await MultiplayerService.Instance.JoinSessionByIdAsync(sessionId);
                //Debug.Log("[ConnectionManager] Joined session (Client)");
                State = ConnectionState.Connected;
                return;
            }
            catch (SessionException e)
            {
                Debug.LogWarning($"Join session failed, retrying... ({attempts + 1}/3). Exception {e}");
                attempts++;
                await Task.Delay(500); // pause before retrying
            }
        }

        State = ConnectionState.Disconnected;
        //Debug.LogError("Failed to join session after 3 attempts.");
    }

    // Join by session name (only in catch ...)
    private async Task JoinExistingSession(string sessionName)
    {
        //Debug.Log($"Hledám session s názvem: {sessionName}...");

        // Session list
        var queryOptions = new QuerySessionsOptions();
        var queryResult = await MultiplayerService.Instance.QuerySessionsAsync(queryOptions);

        foreach (var session in queryResult.Sessions)
        {
            if (session.Name == sessionName && !session.IsLocked)
            {
                //Debug.Log($"Nalezena session '{session.Name}' s ID: {session.Id}. Pøipojuji se...");

                await JoinSessionByID(session.Id);

                return;
            }
        }
        Debug.LogError($"Session '{sessionName}' nebyla nalezena!");
        throw new Exception("Session not found");
    }

    public async Task HostSession(string sessionName = "default")
    {
        m_SessionName = sessionName;

        try
        {
            var options = new SessionOptions()
            {
                Name = sessionName,
                MaxPlayers = m_MaxPlayers,
                IsPrivate = false,
                IsLocked = false,
            }.WithRelayNetwork();

            try
            {
                NetworkManager.Singleton.OnServerStarted += OnServerStarted;
                //Debug.Log($"Zkouším vytvoøit session: {sessionName}");
                m_Session = await MultiplayerService.Instance.CreateSessionAsync(options);
                //Debug.Log($"Session '{sessionName}' vytvoøena! Jsem Creator.");
            }   
            catch (SessionException)
            {
                Debug.LogWarning($"Vytvoøení selhalo (session už asi existuje), joining.");
                await JoinExistingSession(sessionName);
            }
            State = ConnectionState.Connected;
        }
        catch (Exception e)
        {
            State = ConnectionState.Disconnected;
            Debug.LogException(e);
        }
    }

    public async Task CreateOrJoinSessionAsync(string sessionName, bool load = false)
    {
        if (load)
        {
            GameData data = SaveLoadManager.LoadGame();
            if (data.m_LastScene.IsNullOrEmpty() || data.m_SessionName.IsNullOrEmpty())
            {
                //Debug.Log($"[ConnectionManager] Loaded game data: LastScene='{data.m_LastScene}', SessionName='{data.m_SessionName}'");
                return;
            }
            m_SceneToLoad = data.m_LastScene;
            sessionName = data.m_SessionName;
        }

        State = ConnectionState.Connecting;

        await HostSession(sessionName);
    }


    public async Task Disconnect()
    {
        SaveLoadManager.SaveGame();

        if (m_Session != null)
        {
            await m_Session.LeaveAsync();
            m_Session = null;
        }

        if (NetworkManager.Singleton != null)
        {
            //NetworkManager.Singleton.OnSessionOwnerPromoted -= OnSessionOwnerPromoted;
            //NetworkManager.Singleton.OnClientConnectedCallback -= OnClientConnectedCallback;
            if (NetworkManager.Singleton.IsListening)
                NetworkManager.Singleton.Shutdown();

        }
        State = ConnectionState.Disconnected;
        //Debug.Log("[ConnectionManager] Disconnected.");
    }


    // Callbacks
    // --------------------------------------------------------------------------//
    //void OnClientConnectedCallback(ulong clientId)
    //{
    //    Debug.Log($"[NetworkManager] Client connected: {clientId}");
    //}

    //void OnSessionOwnerPromoted(ulong clientId)
    //{
    //    Debug.Log($"[NetworkManager] Client {clientId} promoted to Session Owner.");
    //}
}