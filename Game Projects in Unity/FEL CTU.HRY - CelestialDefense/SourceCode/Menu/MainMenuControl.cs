using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using TMPro;

/**
 * Script controlling the whole Main menu
 * Contain all special button reaction function
 * It load or create new level
 */
public class MainMenuControl : MonoBehaviour
{
    //
    // References to part of the UI
    //
	[SerializeField] private Button loadButton;
    [SerializeField] private GameObject leaderboard;
    [SerializeField] private int leaderboardSize = 10;
    [SerializeField] private GameObject rowPrefab;

    [SerializeField] private GameObject sidebar2;
    [SerializeField] private InputField playerNameInputField;
    [SerializeField] private InputField seedInputField;
    [SerializeField] private Button confirmButton1;

    [SerializeField] private GameObject sidebar3;
    [SerializeField] private Button warr;
    [SerializeField] private Button mage;
    [SerializeField] private Button cannon;
    [SerializeField] private Button gather;
    [SerializeField] private Button confirmButton2;

    [SerializeField] private GameObject leaderboardBoard;

    [SerializeField] private GameObject tutorialMenu;
    [SerializeField] private GameObject tutorialStory;
    [SerializeField] private GameObject tutorialControl;
    [SerializeField] private Text tutorialButtonSwitcher;
    private bool showControls = false;

    [SerializeField] GameObject fader;

    //
    //Data that Gamescene needs to start
    //
    public static int hero_number = 1; //Type of hero
	//Boost multipliers - change based on selected hero
    public static float gatheringMultiplier = 1f;
	public static float cannonMultiplier = 1f;
	public static float catapultMultiplier = 1f;
	public static float WizzardMultiplier = 1f;

	public static int seed;
    public static bool gameShouldBeLoaded = false;
    public static string playerName = "Mister No Name";
    public static int playerScore = 0;
    public static bool ShowLeaderBoards = false;

    private Color32 unselectedColor = new Color(0, 0, 0, 0);

    public void Start()
	{
        Time.timeScale = 1;
        showControls = false;

        gameShouldBeLoaded = false;
		string path = Application.persistentDataPath + "/gamesave.save";

		if (File.Exists(path))
			loadButton.interactable = true;
		else
			loadButton.interactable = false;

        if (ShowLeaderBoards)
        {
            //When coming from lost game we want to show leaderboards
            ShowLeaderBoards = false;
            ShowLeaderboard();
        }

        //On first startup we show tutorial
        string leaderboardPath = Application.persistentDataPath + "/leaderBoards.save";
        if(!File.Exists(path) && !File.Exists(leaderboardPath))
        {
            ShowTutorial();
        }

        fader.GetComponent<Animator>().Play("FadeOut");
    }

	public void ShowMenu1()
    {
        playerNameInputField.SetTextWithoutNotify("Enter a name");
        seedInputField.SetTextWithoutNotify("Enter a seed");

        HideLeaderboard();
        HideTutorial();
        sidebar2.SetActive(true);
        sidebar3.SetActive(false);
        confirmButton1.interactable = true;
	}

	public void ShowMenu2()
	{
        warr.GetComponent<Image>().color = unselectedColor;
        mage.GetComponent<Image>().color = unselectedColor;
        gather.GetComponent<Image>().color = unselectedColor;
        cannon.GetComponent<Image>().color = unselectedColor;

		confirmButton1.interactable = false;
        sidebar3.SetActive(true);
		confirmButton2.interactable = false;
	}

    public void ShowLeaderboard()
    {   
        if(leaderboardBoard.transform.childCount == 0)
        {
            string path = Application.persistentDataPath + "/leaderBoards.save";
            if(File.Exists(path)) { 
                BinaryFormatter formatter = new BinaryFormatter();
                FileStream stream = new FileStream(path, FileMode.Open);
                LeaderboardData leaderboard = formatter.Deserialize(stream) as LeaderboardData;
                stream.Close();

                UserScoreData[] datas = leaderboard.datas;
                int c = Mathf.Min(datas.Length, leaderboardSize);

                for(int i = 0; i < c; i++){
                    UserScoreData data = datas[i];
                    GameObject obj = Instantiate(rowPrefab);
                    obj.transform.GetChild(0).GetComponent<TextMeshProUGUI>().text = data.playerName;
                    obj.transform.GetChild(1).GetComponent<TextMeshProUGUI>().text = data.score.ToString();
                    obj.transform.SetParent(leaderboardBoard.transform, false);
                }

                //Add free rows
                for (int i = 0; i < 10 - c; i++)
                {
                    GameObject obj = Instantiate(rowPrefab); 
                    obj.transform.SetParent(leaderboardBoard.transform, false);
                }
            }

        }

        tutorialMenu.SetActive(false);
        sidebar2.SetActive(false);
        sidebar3.SetActive(false);
        leaderboard.SetActive(true);
    }

    public void SwitchTutorial()
    {
        if (showControls)
        {
            ShowTutorial();
        }
        else
        {
            showControls = true;
            tutorialStory.SetActive(false);
            tutorialControl.SetActive(true);
            tutorialButtonSwitcher.text = "Story";
        }
    }

    public void ShowTutorial()
    {
        showControls = false;
        tutorialMenu.SetActive(true);
        tutorialStory.SetActive(true);
        tutorialControl.SetActive(false);
        tutorialButtonSwitcher.text = "Controls";

        sidebar2.SetActive(false);
        sidebar3.SetActive(false);
        leaderboard.SetActive(false);
    }

    public void HideTutorial()
    {
        tutorialMenu.SetActive(false);
    }

    public void HideLeaderboard()
    {
        leaderboard.SetActive(false);
    }

    public void WarrClicked()
	{
		gatheringMultiplier = 1f;
		cannonMultiplier = 1f;
		catapultMultiplier = 1.25f;
		WizzardMultiplier = 1f;

		warr.GetComponent<Image>().color = Color.white;
		mage.GetComponent<Image>().color = unselectedColor;
		gather.GetComponent<Image>().color = unselectedColor;
		cannon.GetComponent<Image>().color = unselectedColor;
		confirmButton2.interactable = true;
		hero_number = 0;
		gameShouldBeLoaded = false;
	}

    public void MageClicked()
	{
		gatheringMultiplier = 1f;
		cannonMultiplier = 1f;
		catapultMultiplier = 1f;
		WizzardMultiplier = 1.25f;

		mage.GetComponent<Image>().color = Color.white;
		warr.GetComponent<Image>().color = unselectedColor;
		gather.GetComponent<Image>().color = unselectedColor;
		cannon.GetComponent<Image>().color = unselectedColor;
		confirmButton2.interactable = true;
		hero_number = 1;
		gameShouldBeLoaded = false;
	}
    public void GatherClicked()
	{
		gatheringMultiplier = 1.25f;
		cannonMultiplier = 1f;
		catapultMultiplier = 1f;
		WizzardMultiplier = 1f;

		gather.GetComponent<Image>().color = Color.white;
		mage.GetComponent<Image>().color = unselectedColor;
		warr.GetComponent<Image>().color = unselectedColor;
		cannon.GetComponent<Image>().color = unselectedColor;
		confirmButton2.interactable = true;
		hero_number = 2;
		gameShouldBeLoaded = false;
	}
    public void CannonClicked()
	{
		gatheringMultiplier = 1f;
		cannonMultiplier = 1.25f;
		catapultMultiplier = 1f;
		WizzardMultiplier = 1f;

		cannon.GetComponent<Image>().color = Color.white;
		mage.GetComponent<Image>().color = unselectedColor;
		gather.GetComponent<Image>().color = unselectedColor;
		warr.GetComponent<Image>().color = unselectedColor;
		confirmButton2.interactable = true;
		hero_number = 3;
		gameShouldBeLoaded = false;
	}

    public void StartNewGame()
	{
		gameShouldBeLoaded = false;

        if (string.IsNullOrEmpty(seedInputField.text))
            seed = Random.Range(0, 20000).ToString().GetHashCode();
        else
            seed = seedInputField.text.GetHashCode();

        if (string.IsNullOrEmpty(playerNameInputField.text) || string.IsNullOrWhiteSpace(playerNameInputField.text))
            playerName = "Mister No Name";
        else
            playerName = playerNameInputField.text;

        fader.GetComponent<Animator>().SetTrigger("FadePernament");
        
        StartCoroutine("LoadScene", "GameScene");
    }

    public void LoadPreviousGame() {

		gameShouldBeLoaded = true;

        string path = Application.persistentDataPath + "/gamesave.save";

        if (File.Exists(path))
        {
            BinaryFormatter formatter = new BinaryFormatter();
            FileStream stream = new FileStream(path, FileMode.Open);

            //Meteor - position, health
            MeteorSaveData meteorData = formatter.Deserialize(stream) as MeteorSaveData;
            MeteorSaveData.instance = meteorData;

            //Player - position, wood, stone, iron, buildings in inventory, class, cooldowns
            PlayerSaveData playerData = formatter.Deserialize(stream) as PlayerSaveData;
            PlayerSaveData.instance = playerData;

            //Wave - wave number, score, time to next wave + enemies
            SpawningData spawningData = formatter.Deserialize(stream) as SpawningData;
            SpawningData.instance = spawningData;

            //Buildings - type, health, time to fire
            BuildingSaveData buildingData = formatter.Deserialize(stream) as BuildingSaveData;
            BuildingSaveData.instance = buildingData;

            MainMenuControl.seed = spawningData.seed;
            MainMenuControl.playerName = playerData.playerName;

            Debug.Log("Save data loaded");
            stream.Close();

            fader.GetComponent<Animator>().SetTrigger("FadePernament");

            StartCoroutine("LoadScene", "GameScene");
        }
        else
        {
            loadButton.interactable = false;
            return;
        }
	}

    IEnumerator LoadScene(string sceneName)
    {
        yield return null;

        float startTime = Time.time;
        AsyncOperation asyncOperation = SceneManager.LoadSceneAsync(sceneName);
        asyncOperation.allowSceneActivation = false;

        while (!asyncOperation.isDone)
        {
            if (Time.time >= startTime + 1.0f)
            {
                asyncOperation.allowSceneActivation = true;
            }

            yield return null;
        }
    }

    public void QuitGame()
	{
		Debug.Log("Quiting");
        fader.GetComponent<Animator>().SetTrigger("FadePernament");
        Application.Quit();
	}
}
