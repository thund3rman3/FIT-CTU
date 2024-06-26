using BehaviorTree;
using System.Collections;
using UnityEngine;

public class Rest : MonoBehaviour
{
    private float _restTick;
    private int _energy;
    private int _maxEnergy;

    public Rest() { _restTick = 0.1f; }
    public void StartSleep(int energy, int maxEnergy)
    {
        _energy = energy;   
        _maxEnergy = maxEnergy;
        
        StartCoroutine(sleep());
    }
    public bool CheckRested()
    {
        return _energy == _maxEnergy;
    }
    public int getEnergy()
    {
        return _energy;
    }

    IEnumerator sleep()
    {
        Debug.Log("We sleeping bois");
        while (_energy != _maxEnergy)
        {
            Debug.Log("ZZZ");
            _energy++;
            yield return new WaitForSecondsRealtime(_restTick);
        }
    }
}

public class TaskRest : Node
{
    private bool _startRest;
    private GameObject _me;
    private Rest _sleepyBoy;

    public TaskRest(GameObject elementalMe)
    {
        _startRest = true;
        _me = elementalMe;
        _sleepyBoy = _me.AddComponent<Rest>();
    }

    public override NodeState Evaluate()
    {
        Debug.Log("Resting");
        int y = (int)GetData("Energy");
        if (y < 50 && _startRest) //TODO: parametrize the 50 here, OPEN IT in MeeneeBT
        {
            _startRest = false;
            state = NodeState.RUNNING;
           int bleh = ((MeeneeDataSO)GetData("meeneeData")).maxEnergy;
            //disable in hiearchy
            _me.GetComponentInChildren<SkinnedMeshRenderer>().enabled = false;
            //_me.GetComponent<MeeneeBT>().ShutItOffOtto();
            _sleepyBoy.StartSleep( y, bleh );
            return state;
        }
        else if(_sleepyBoy.CheckRested())
        {
            parent.parent.SetData("Energy", _sleepyBoy.getEnergy());
            ClearData("ConversationNon");
            ClearData("ConversationPartner");
            _me.GetComponentInChildren<SkinnedMeshRenderer>().enabled = true;
            //_me.GetComponent<MeeneeBT>().TurnOn();
            state = NodeState.SUCCESS;
            return state;
        }
        state = NodeState.RUNNING;
        return state;
    }

    
}
