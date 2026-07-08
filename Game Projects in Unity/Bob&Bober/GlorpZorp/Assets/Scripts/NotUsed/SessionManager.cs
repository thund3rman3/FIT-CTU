using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Unity.Netcode;
using Unity.Services.Authentication;
using Unity.Services.Core;
using Unity.Services.Multiplayer;
using UnityEngine;
using UnityUtils;

public class SessionManager : Singleton<SessionManager>
{
    //const string playerNamePropertyKey = "playerName";

    string sessionName = "Test Session";

    NetworkManager networkManager; 

    ISession activeSession; // all about session

    ISession ActiveSession
    {
        get => activeSession;
        set
        {
            activeSession = value;
            Debug.Log($"Active session: {activeSession}");
        }
    }

    void OnSessionOwnerPromoted(ulong sessionOwnerPromoted)
    {
        if(networkManager.LocalClient.IsSessionOwner)
            Debug.Log($"Session owner promoted: {networkManager.LocalClientId}");
    }

    void OnClientConnectedCallback(ulong clientId)
    {
        if(networkManager.LocalClientId == clientId)
            Debug.Log($"Client connected: {clientId} and can spawn {nameof(NetworkObject)}s.");
    }

    async void Start()
    {
        try
        {
            networkManager = GetComponent<NetworkManager>();
            networkManager.OnClientConnectedCallback += OnClientConnectedCallback;
            networkManager.OnClientDisconnectCallback += OnSessionOwnerPromoted;

            await UnityServices.InitializeAsync(); // Initialize Unity Gaming Services SDKs.
            await AuthenticationService.Instance.SignInAnonymouslyAsync(); // Anonymously authenticate the player
            Debug.Log($"Sign in anonymously succeeded! PlayerID: {AuthenticationService.Instance.PlayerId}");

            // Start a new session as a host works for ClinetServer
            //StartSessionAsHost();

            var options = new SessionOptions
            {
                Name = sessionName,
                MaxPlayers = 2,
            }.WithDistributedAuthorityNetwork();

            ActiveSession = await MultiplayerService.Instance.CreateOrJoinSessionAsync(sessionName,options);
        }
        catch (Exception e)
        {
            Debug.LogException(e);
        }
    }

    //async Task<Dictionary<string, PlayerProperty>> GetPlayerProperties()
    //{
    //    // Custom game-specific properties that apply to an individual player, ie: name, role, skill level, etc.
    //    var playerName = await AuthenticationService.Instance.GetPlayerNameAsync();
    //    var playerNameProperty = new PlayerProperty(playerName, VisibilityPropertyOptions.Member);
    //    return new Dictionary<string, PlayerProperty> { { playerNamePropertyKey, playerNameProperty } };
    //}

    //async void StartSessionAsHost()
    //{
    //    var playerProperties = await GetPlayerProperties();

    //    var options = new SessionOptions
    //    {
    //        MaxPlayers = 2,
    //        IsLocked = false,
    //        IsPrivate = false,
    //        PlayerProperties = playerProperties
    //    }.WithRelayNetwork();

    //    ActiveSession = await MultiplayerService.Instance.CreateSessionAsync(options);
    //    Debug.Log($"Session {ActiveSession.Id} created! Join code: {ActiveSession.Code}");
    //}


    async void KickPlayer(string playerId)
    {
        if (!ActiveSession.IsHost) return;
        await ActiveSession.AsHost().RemovePlayerAsync(playerId);
    }

    async Task<IList<ISessionInfo>> QuerySessions()
    {
        var sessionQueryOptions = new QuerySessionsOptions();
        QuerySessionsResults results = await MultiplayerService.Instance.QuerySessionsAsync(sessionQueryOptions);
        return results.Sessions;
    }

    async void LeaveSession()
    {
        if (ActiveSession != null)
        {
            try
            {
                await ActiveSession.LeaveAsync();
            }
            catch
            {
                // Ignored as we are exiting the game
            }
            finally
            {
                ActiveSession = null;
            }
        }
    }
}