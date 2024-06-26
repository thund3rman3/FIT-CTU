using UnityEngine;

public class UI_Control: MonoBehaviour
{
    [Header("Player")]
    public GameObject Player;
    public float TimeScale;
    [Header("Cursor")]
    [SerializeField] private Texture2D[] CursorImage = new Texture2D[4];
    //[SerializeField] private GameObject trail;
    [Space]
    [Header("Journal and Spellcasting control")]
    [SerializeField] private GameObject Journal;
    [SerializeField] private GameObject SpellCastingWheel;
    [SerializeField] private GameObject Overlay;
    [SerializeField] private GameObject Conversation;
    [SerializeField] public GameObject PressE;
    [Space]
    [Header("Keybinds")]
    [SerializeField] public KeyCode journalKey = KeyCode.Tab;
    [SerializeField] public KeyCode spellcastKey = KeyCode.Mouse1;
    [SerializeField] public KeyCode spellConfirmKey = KeyCode.Mouse0;
    [Header("States")]
    [SerializeField] bool JournalActive = false;
    [SerializeField] bool SpellCastActive = false;
    [SerializeField] bool ConvoActive = false;
    [SerializeField] bool SpellCasted = false;
    [SerializeField] bool ClearTrail = false;
    [SerializeField] bool SpellReset = false;
    [SerializeField] bool BlankScreen = true;
    [Header("Magic")]
    public TemplateMagic.ElementType SelectedElement = TemplateMagic.ElementType.Earth;
    [SerializeField] float CastTimeLimit = 4;
    [SerializeField] float CastCoolDownIs = 4;
    [SerializeField] float CastActiveTime = 0;
    [SerializeField] float CastCoolDownTime = 0;
    public Color Air;
    public Color Earth;
    public Color Fire;
    public Color Water;

    private Canvas RefCanvas;
    private PlayerCam RefPlayerCam;
    private PlayerBehavior RefPlayerBeh;
    private SpellCast_Control RefSpellCastControll;
    private CanvasGroup RefCanvasAlphaControl;
    private Conversation RefConversation;
    private TrailRenderer RefTrailRenderer;

    // Start is called before the first frame update
    void Start()
    {
        RefCanvas = Journal.GetComponent<Canvas>();
        RefPlayerCam = Camera.main.GetComponent<PlayerCam>();
        RefPlayerBeh = Player.GetComponent<PlayerBehavior>();
        RefConversation = Conversation.GetComponent<Conversation>();
        RefSpellCastControll = transform.GetChild(1).GetComponent<SpellCast_Control>();
        RefCanvasAlphaControl = Overlay.GetComponent<CanvasGroup>();
        RefTrailRenderer = transform.GetChild(0).GetComponent<TrailRenderer>();
        Cursor.visible = false;
        Cursor.SetCursor(CursorImage[0], new Vector2(64f, 64f), CursorMode.Auto);
        RefTrailRenderer.startColor = VizualizeElement();
        RefTrailRenderer.enabled = false;
        //RefTrailRenderer.enabled = true;
        //UpdateTrail();
    }

    // Update is called once per frame
    void Update()
    {
        if (RefPlayerBeh.alive)
        {

            if (Input.mouseScrollDelta.y != 0) //ItemScroll
            {
                if (Input.mouseScrollDelta.y == 1)
                {
                    if (TryElementUp())
                    {
                        if (SpellCastActive) SpellReset = true;
                        RefTrailRenderer.startColor = VizualizeElement();
                    }
                }
                else if (Input.mouseScrollDelta.y == -1)
                {
                    if (TryElementDown())
                    {
                        if (SpellCastActive) SpellReset = true;
                        RefTrailRenderer.startColor = VizualizeElement();
                    }
                }
            }
            if (Input.GetKeyDown(journalKey)) //Journal Switch
            {
                if (BlankScreen)
                {
                    RefPlayerCam.enabled = Player.GetComponent<PlayerMove>().enabled = BlankScreen = false;
                    Cursor.visible = RefCanvas.enabled = JournalActive = true;
                    Cursor.lockState = CursorLockMode.None;
                    //Cursor.lockState = CursorLockMode.Locked;
                    //trail.SetActive(false);
                    //scuffed way to avoid the initial text not curving
                    Journal.GetComponent<JournalContent>().SetText();
                    RefCanvasAlphaControl.alpha = 0.1f;
                }
                else if (JournalActive)
                {
                    Cursor.lockState = CursorLockMode.Locked;
                    RefPlayerCam.enabled = Player.GetComponent<PlayerMove>().enabled = BlankScreen = true;
                    Cursor.visible = RefCanvas.enabled = JournalActive = false;
                    RefCanvasAlphaControl.alpha = 1f;
                }
            }//talking
            if (RefConversation.inConversation)
            {
                if (BlankScreen)
                {
                    Debug.Log("UI_Control starting convo");
                    RefPlayerCam.enabled = Player.GetComponent<PlayerMove>().enabled = BlankScreen = false;
                    Cursor.visible = ConvoActive = true;
                    Cursor.lockState = CursorLockMode.None;
                    RefCanvasAlphaControl.alpha = 0.1f;
                }
            }
            if ((!RefConversation.inConversation && ConvoActive) || ( Input.GetKeyDown(KeyCode.E) && (RefConversation.IsSkippable())) && RefConversation.inConversation)
            {
                Debug.Log("UI_Control ending convo" + RefConversation.inConversation);
                Cursor.lockState = CursorLockMode.Locked;
                RefPlayerCam.enabled = Player.GetComponent<PlayerMove>().enabled = BlankScreen = true;
                Cursor.visible = ConvoActive = false;
                RefConversation.StartEndConvo(false);
                RefCanvasAlphaControl.alpha = 1f;
            }
            if (SpellCastActive)
            {
                CastActiveTime += Time.deltaTime * (1 / TimeScale);
                UpdateTrail();
            }
            if (Input.GetKeyDown(spellConfirmKey) && SpellCasted) //Fire Spell
            {
                if (RefSpellCastControll.ConfirmSpell())
                {
                    SpellCasted = false;
                    BlankScreen = true;
                }
            }
            if (CastCoolDownTime > 0) CastCoolDownTime -= Time.deltaTime;
            else if ((Input.GetKeyDown(spellcastKey) || SpellReset) && (BlankScreen || SpellCasted) && PersistData.playerData.EnabledSpellCast)
            {
                BlankScreen = SpellReset = SpellCasted = false;
                SetSpellCast(true);
                ClearTrail = true;
                UpdateTrail();
                RefTrailRenderer.Clear();
            }
            if (((Input.GetKeyUp(spellcastKey) || SpellReset) && SpellCastActive) || CastActiveTime > CastTimeLimit)
            {
                if (!SpellReset)
                {
                    SpellCasted = RefSpellCastControll.FinishCast(SelectedElement, VizualizeElement());
                    if (!SpellCasted) CastCoolDownTime = CastCoolDownIs;
                }
                BlankScreen = !SpellCasted;
                SetSpellCast(false);
            }
            //Debug.Log(Time.timeScale);
        }
        else 
        {
            RefCanvasAlphaControl.alpha = 1f;
            if (Input.anyKeyDown) RefPlayerBeh.FinishDeath();
        }
    }
    void SetSpellCast(bool activate){
        CastActiveTime = 0;
        SpellCastingWheel.SetActive(RefTrailRenderer.enabled = Cursor.visible = SpellCastActive = activate);
        RefPlayerCam.enabled = !activate;
        Time.timeScale = activate ? TimeScale : 1.0f;
        Cursor.lockState = activate ? CursorLockMode.None : CursorLockMode.Locked;
    }
    void UpdateTrail(){
            var mousePos = Input.mousePosition;
            mousePos.z = 0.6f;
            RefTrailRenderer.transform.position = Camera.main.ScreenToWorldPoint(mousePos);
    }
    bool TryElementUp(){
        for (int i = (int)SelectedElement + 1; i < 4; i++){
            if (PersistData.playerData.EnabledElements[i]){
                SelectedElement = (TemplateMagic.ElementType)i;
                return true;
            }
        }
        return false;
    }
    bool TryElementDown(){
        for (int i = (int)SelectedElement - 1; i >= 0; i--){
            if (PersistData.playerData.EnabledElements[i]){
                SelectedElement = (TemplateMagic.ElementType)i;
                return true;
            }
        }
        return false;
    }
    public Color VizualizeElement(){
        switch(SelectedElement){
            case TemplateMagic.ElementType.Air:
                return Air;
            case TemplateMagic.ElementType.Earth:
                return Earth;
            case TemplateMagic.ElementType.Fire:
                return Fire;
            case TemplateMagic.ElementType.Water:
                return Water;                
        }
        return Color.black;
        //Cursor.SetCursor(CursorImage[(int)SelectedElement], new Vector2(64f ,64f), CursorMode.Auto);
    }


}
