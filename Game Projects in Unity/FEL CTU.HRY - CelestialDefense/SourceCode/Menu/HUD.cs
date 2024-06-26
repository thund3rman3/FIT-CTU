using UnityEngine;
using System.Collections;
using UnityEngine.UI;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using UnityEngine.SceneManagement;
using TMPro;

/**
 * Script that holds everything important about UI in game
 * It help to display remaining time till wave, score, etc.
 */
public class HUD : MonoBehaviour
{
    public static HUD instance = null;

    public GameObject craftingMenu;

    public Text waveTimer;
    public Text waveText;
    public Text scoreText;
    public int score = 0;
    private int wave = 0;
    public Text[] resources = new Text[3];
    public GameObject removingIcon;

    [SerializeField] GameObject pauseMenu;
    [SerializeField] Slider meteorHealth;
    [SerializeField] GameObject endGameMenu;
    [SerializeField] Text endGameMenuPlayerName;
    [SerializeField] Text endGameMenuWave;
    [SerializeField] Text endGameMenuScore;

    [SerializeField] GameObject fader;

    [SerializeField] TextMeshProUGUI[] counterTexts;

    //Whether the UI should update
    public bool isFreezed = false;

    public void setRemoving(bool status)
    {
        removingIcon.SetActive(status);
    }

    public void Start()
    {
        instance = this;
        setScore(score);
        HidePauseMenu();

        if (MainMenuControl.gameShouldBeLoaded) { 
            score = SpawningData.instance.score;
            setScore(score);

            wave = SpawningData.instance.waveCounter;
            setWave(wave);            
        }
    }

    public void AddScore(int addition)
    {
        if (isFreezed) return;

        score += addition;
        setScore(score);
    }

    public void setScore(int score)
    {
        if (isFreezed) return;
        scoreText.text = "Score: " + score;
    }

    public void setWave(int wave)
    {
        if (isFreezed) return;
        this.wave = wave;
        waveText.text = "Wave: " + wave;
    }

    public void setWood(int wood)
    {
        if (isFreezed) return;
        resources[0].text = wood.ToString();
    }
    public void setStone(int stone)
    {
        if (isFreezed) return;
        resources[1].text = stone.ToString();
    }
    public void setIron(int iron)
    {
        if (isFreezed) return;
        resources[2].text = iron.ToString();
    }

    public void setRemainingTime(int timeInSeconds)
    {
        if (isFreezed) return;
        if (waveTimer == null) return;
        waveTimer.text = "Next wave in: " + timeInSeconds.ToString();
    }

    //Updates the count on the indicators for number of buildings in inventory
    public void updateBuildingCount()
    {
        if (isFreezed) return;

        for (int i = 1; i < 22; i++)
        {
            counterTexts[i].text = BuildingAndInventory.instance.buildingCount[i].ToString();
        }
    }

    public void Update()
    {
        if (isFreezed) return;

        if (Input.GetKeyDown(KeyCode.Escape))
        {
            if (pauseMenu.activeSelf)
                HidePauseMenu();
            else
                ShowPauseMenu();
        }
    }

    public void ShowPauseMenu()
    {
        if (isFreezed) return;

        Time.timeScale = 0;
        pauseMenu.SetActive(true);
        AudioManagerScript.PauseTheme();
    }

    public void HidePauseMenu()
    {
        Time.timeScale = 1;
        pauseMenu.SetActive(false);
        AudioManagerScript.UnPauseTheme();
    }

    public void UpdateMeteorHP(float percentile)
    {
        if (isFreezed) return;
        meteorHealth.value = percentile;
    }

    //Saves everything into save file
    public void SaveAndExit()
    {
        string path = Application.persistentDataPath + "/gamesave.save";
        Debug.Log("Save path is: " + path);
        BinaryFormatter formatter = new BinaryFormatter();
        FileStream stream = new FileStream(path, FileMode.Create );

        //Meteor - position, health
        MeteorSaveData meteorData = new MeteorSaveData();
        formatter.Serialize(stream, meteorData);

        //Player - position, wood, stone, iron, buildings in inventory, class, cooldowns
        PlayerSaveData playerData = new PlayerSaveData();
        formatter.Serialize(stream, playerData);

        //Wave - wave number, score, time to next wave + enemies
        SpawningData spawningData = new SpawningData();
        formatter.Serialize(stream, spawningData);

        //Buildings - type, health, time to fire
        BuildingSaveData buildingData = new BuildingSaveData();
        formatter.Serialize(stream, buildingData);

        Debug.Log("Save data saved");
        stream.Close();

        fader.GetComponent<Animator>().SetTrigger("FadePernament");

        gameIsEnding = true;
        Application.Quit();
    }
    public static bool gameIsEnding = false;

    public void ShowEndGameMenu()
    {
        Time.timeScale = 0;
        isFreezed = true;
        endGameMenu.SetActive(true);

        endGameMenuPlayerName.text = "Player: " + MainMenuControl.playerName;
        endGameMenuScore.text = "Score: " + score;
        endGameMenuWave.text = "Wave: " + wave;
    }

    //Destroy safe file and make note into leaderboards
    public void SaveScoreAndGoToLeaderboards()
    {
        MainMenuControl.ShowLeaderBoards = true;

        //Delete any save file
        string path = Application.persistentDataPath + "/gamesave.save";
        if (File.Exists(path))
        {
            File.Delete(path);
        }

        string pathLeaderBoards = Application.persistentDataPath + "/leaderBoards.save";
        BinaryFormatter formatter = new BinaryFormatter();
        LeaderboardData leaderboard = new LeaderboardData();

        if (File.Exists(pathLeaderBoards))
        {
            FileStream ss = new FileStream(pathLeaderBoards, FileMode.Open);
            leaderboard = formatter.Deserialize(ss) as LeaderboardData;
            ss.Close();
            File.Delete(pathLeaderBoards);
        }

        leaderboard.addData(score, MainMenuControl.playerName);

        FileStream stream = new FileStream(pathLeaderBoards, FileMode.CreateNew);
        formatter.Serialize(stream, leaderboard);
        stream.Close();

        fader.GetComponent<Animator>().SetTrigger("FadePernament");

        SceneManager.LoadSceneAsync("MainMenu");
    }

}

/*
	 * 1  - Wooden wall
	 * 2  - Stone wall
	 * 3  - Iron wall
	 * 4  - Hole
	 * 5  - Trap
	 * 6  - Auto-miner
	 * 7  - Cannon lvl 1
	 * 8  - Cannon lvl 2
	 * 9  - Cannon lvl 3
	 * 10 - Wizard tower
	 * 11 - Ice Wizard tower
	 * 12 - Frost Wizard tower
	 * 13 - Fire Wizard tower
	 * 14 - Magma Wizard tower
	 * 15 - Crossbow lvl 1
	 * 16 - Crossbow lvl 2
	 * 17 - Crossbow lvl 3
	 * 18 - Catapult lvl 1
	 * 18 - Catapult lvl 2
	 * 20 - Catapult lvl 3
	 * 21 - Gate
	 */