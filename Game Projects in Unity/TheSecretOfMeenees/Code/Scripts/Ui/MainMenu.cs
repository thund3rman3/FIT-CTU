using System.Collections;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using TMPro;


public class MainMenu : MonoBehaviour
{
    //PlayerBehavior pbScript;
    public Camera mainCam;
    [Space]
    public Canvas TopLevel;
    public Canvas PlayMenu;
    public Canvas Settings;
    public Canvas Credits;
    private Animator cameraAnimator;
    private SaveGame _attempt = null;
    [Space]
    //public Slider Master;
    //public Slider Music;
    //public Slider SoundEffects;

    public float DelayTopLevel;
    public float DelayPlayMenu;
    [Space]
    public GameObject LoadScreen;
    public Image BarFill;
    private Vector3 PlayPos;
    private Quaternion PlayRotation;
    private void Start()
    {
        cameraAnimator = mainCam.GetComponent<Animator>();
        //TODO: Play menu jumps when changing resolution 

        _attempt = SaveSystem.LoadData();
        GameObject button = PlayMenu.gameObject.transform.GetChild(1).gameObject;
        if (_attempt != null)
        {
            button.transform.GetChild(0).GetComponent<TextMeshProUGUI>().color = Color.white;
            button.GetComponent<Button>().interactable = true;

            Settings.GetComponent<SaveMenuValues>().LoadSettings();
        }
        else
        {
            button.transform.GetChild(0).GetComponent<TextMeshProUGUI>().color = Color.gray;
            button.GetComponent<Button>().interactable = false;
        }
    }
    public void Play()
    {
        Debug.Log("Play");
        cameraAnimator.SetFloat("SpeedOfJump", 1);
        TopLevel.GetComponent<Canvas>().renderMode = RenderMode.WorldSpace;
        cameraAnimator.SetTrigger("StartClicked");
        PlayPos = PlayMenu.transform.position;
        PlayRotation = PlayMenu.transform.rotation;
        PlayMenu.GetComponent<GraphicRaycaster>().enabled = true;
        StartCoroutine(SetScreenSpace(PlayMenu, DelayPlayMenu));
    }

    public void Return()
    {
        Debug.Log("Return to top");
        PlayMenu.transform.position = PlayPos;
        PlayMenu.transform.rotation = PlayRotation;
        PlayMenu.GetComponent<Canvas>().renderMode = RenderMode.WorldSpace;
        PlayMenu.GetComponent<GraphicRaycaster>().enabled = false;
        cameraAnimator.SetFloat("SpeedOfJump", -1);
        StartCoroutine(SetScreenSpace(TopLevel, DelayTopLevel));
    }
    public void StartGame()
    {
        Debug.Log("Start");
        if(_attempt != null)
        {
            for(int i = 3; i < 6; i++) PlayMenu.gameObject.transform.GetChild(i).gameObject.SetActive(true);
            PlayMenu.gameObject.transform.GetChild(0).gameObject.SetActive(false);
        }
        else
        {
            for (int i = 6; i < 9; i++) PlayMenu.gameObject.transform.GetChild(i).gameObject.SetActive(true);
            PlayMenu.gameObject.transform.GetChild(0).gameObject.SetActive(false);
        }
    }
    public void ConfirmSaveDelete()
    {
        SaveSystem.DeleteSaveData();
        //SceneManager.LoadScene("Tutorial");
        for (int i = 6; i < 9; i++) PlayMenu.gameObject.transform.GetChild(i).gameObject.SetActive(true);
        for (int i = 3; i < 6; i++) PlayMenu.gameObject.transform.GetChild(i).gameObject.SetActive(false);
    }
    public void ReturnFromSaveDelete()
    {
        for (int i = 3; i < 6; i++) PlayMenu.gameObject.transform.GetChild(i).gameObject.SetActive(false);
        PlayMenu.gameObject.transform.GetChild(0).gameObject.SetActive(true);
    }
    public void StartContinue()
    {
        _attempt.PopulatePersistData();
        //SceneManager.LoadScene("Village");
        StartCoroutine(LoadSceneAsync("Village"));
        Debug.Log("LoadAttempt");
    }
    public void SkipTutorial()
    {
        Debug.Log("Skip tutorial");
        //SceneManager.LoadScene("Village");
        PersistData.PopulateSkipTutorial();
        SaveSystem.SaveData();
        StartCoroutine(LoadSceneAsync("Village"));
    }
    public void PlayTutorial()
    {
        PersistData.PopulateOnNewGame();
        StartCoroutine(LoadSceneAsync("Tutorial"));
    }

    public void ClickSettings()
    {
        genericAnimButton("Go_left", Settings);
    }

    public void ClickCredits()
    {
        genericAnimButton("Go_right", Credits);
    }

    public void genericAnimButton(string animation, Canvas whichOne)
    {
        TopLevel.GetComponent<Canvas>().renderMode = RenderMode.WorldSpace;
        whichOne.GetComponent<Canvas>().enabled = true;
        whichOne.GetComponent<Canvas>().renderMode = RenderMode.ScreenSpaceCamera;
        TopLevel.GetComponent<Canvas>().enabled = false;
    }

    public void Back()
    {
        if(Settings.GetComponent<Canvas>().enabled)
        {
            genericBackAnim("Go_right", Settings);
        }
        else
        {
            genericBackAnim("Go_left", Credits);
        }
    }

    public void genericBackAnim(string animation, Canvas whichOne)
    {
        whichOne.GetComponent<Canvas>().renderMode = RenderMode.WorldSpace;
        TopLevel.GetComponent<Canvas>().enabled = true;
        TopLevel.GetComponent<Canvas>().renderMode = RenderMode.ScreenSpaceCamera;
        whichOne.GetComponent<Canvas>().enabled = false;
    }

    public void EndGame()
    {
        Debug.Log("Quit");
        Application.Quit();
    }
    IEnumerator SetScreenSpace(Canvas setMe, float delay)
    {
        yield return new WaitForSecondsRealtime(delay);
        setMe.GetComponent<Canvas>().renderMode = RenderMode.ScreenSpaceCamera;
    }

    IEnumerator LoadSceneAsync(string sceneName)
    {
        LoadScreen.SetActive(true);
        AsyncOperation op = SceneManager.LoadSceneAsync(sceneName);

        while(!op.isDone)
        {
            float progress = Mathf.Clamp01(op.progress / 0.9f);
            BarFill.fillAmount = progress;

            yield return null;
        }
    }
}
